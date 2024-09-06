/*****************************************************************
File:         BMH06203.cpp
Author:       BEST MODULES CORP.
Description:  I2C communication with the BMH06203 and obtain the corresponding value 
History：      V1.0.2  -- 2024-08-20
******************************************************************/
#include "BMH06203.h"

/*************************************************
Description:Constructor
Parameters: *theWire: Wire object if your board has more than one I2C interface            
Return:             
Others:             
*************************************************/
BMH06203::BMH06203(TwoWire *theWire)
{
  _wire = theWire;
}
/*************************************************
Description:  Module Initial
Parameters:   i2c_addr：Module IIC address       
Return:       void         
Others:            
*************************************************/
void BMH06203::begin(uint8_t  i2c_addr)
{
    if (_wire != NULL)
    {
        _wire->setClock(100000);
        _wire->begin();
    }
    _i2cAddr = i2c_addr;
}

/*************************************************
Description:  Get temperature data
Parameters:   TYPE:
                0x08(AMB_TEMP):ambient temperature 
                0x09(OBJ_TEMP):Surface temperature of object 
                0x0a(BODY_TEMP):Human body temperature 
Return:       Temperature data,unit ℃
Others:
*************************************************/
float BMH06203::readTemperature(uint8_t TYPE)
{

    uint8_t check,Tempbuf[3];
    float data;
    uint8_t type[1]={TYPE};
    writeBytes(type,1);
    check = readBytes(Tempbuf,3);
    delay(10);
    if(check == CHECK_OK)
    {
        data = float(Tempbuf[1] << 8 | Tempbuf[0]) / 10;
        return data;
    }
    return 0;    
}
/*************************************************
Description:  Enter halt mode
Parameters:
Return:       void
Others:
*************************************************/
void BMH06203::sleep()
{
    uint8_t checksum = (0xFF + 0x34 + 0x12) & 0xff;
    uint8_t haltbuf[4] = {0xFF,0x34,0x12,checksum};
    writeBytes(haltbuf,4);
}
/*************************************************
Description:  Write EEPROM
Parameters:   addr:EEPROM address,write value  set 08H @see BMH06203.h MODE_:
              data:Data written into EEPROM
Return:       void
Others:       When continuously writing data to the EEPROM,
              the interval for each stroke must be 10ms
*************************************************/
void BMH06203::writeEEPROM(uint8_t addr, uint16_t data)
{
    uint8_t command = 0x20 + addr;
    uint8_t checksum = (command + lowByte(data) + highByte(data)) & 0xff;
    uint8_t EEPROMPWD[4] = {command,lowByte(data),highByte(data),checksum};
    writeBytes(EEPROMPWD,4);
}
/*************************************************
Description:  Read EEPROM value
Parameters:   addr:EEPROM addres
Return:       EEPROM address value
Others:       Read data immediately after writing data to the EEPROM at an interval of 10ms
*************************************************/
uint16_t BMH06203::readEEPROM(uint8_t addr)
{
    uint8_t command = 0x20+addr;
    uint8_t readcommand[1] = {command};
    uint16_t databuf[1]={0};
    writeBytes(readcommand,1);
    uint8_t readbuf[3] = {0};
    uint8_t check = readBytes(readbuf,3);
    delay(10);
    if(check == CHECK_OK)
    {
        databuf[0] = readbuf[1] << 8 | readbuf[0];
        return databuf[0];
    }
    else
    {
        return 0;
    }
}
/*************************************************
Description:  Set the output mode
Parameters:   Mode:
                IIC_MODE = 0x00,
                PWM_MODE = 0x01,
                IO_MODE1 = 0x02,
                IO_MODE2 = 0x06,      
Return:       void          
Others:            
*************************************************/
void BMH06203::setMode(uint8_t Mode)
{
    if(_wire == &Wire)
    {
      pinMode(19,OUTPUT); 
      digitalWrite(19,LOW);
      delay(70);//Wait 50ms and then power cycle up
      digitalWrite(19,HIGH); 
    }
    #if defined(ARDUINO_HT32_USB)
    if(_wire == &Wire1) 
    {
      pinMode(21,OUTPUT); 
      digitalWrite(21,LOW); 
      delay(70);//Wait 50ms and then power cycle up
      digitalWrite(21,HIGH); 
    }
    if(_wire == &Wire2)
    {
      pinMode(24,OUTPUT); 
      digitalWrite(24,LOW); 
      delay(70);//Wait 50ms and then power cycle up
      digitalWrite(24,HIGH); 
    }
    #endif
    begin();
    writeEEPROM(0x08,Mode);
}
/*************************************************
Description:  Set PWM mode, temperature range  
Parameters:   min:  Set the minimum temperature 
              max:  Set the maximum temperatur
Return:       void      
Others:       
*************************************************/
void BMH06203::setPWMParam(float min,float max)
{
  uint16_t _min = min *10;
  uint16_t _max = max *10;
  writeEEPROM(0x0A,_min); 
  writeEEPROM(0x0B,_max); 
}
/*************************************************
Description:Set IO mode, temperature trigger threshold  
Parameters: threshold: Set the temperature threshold      
Return:     void     
Others:       
*************************************************/
void BMH06203::setIOParam(float threshold)
{
  uint16_t _threshold = threshold * 10;
  writeEEPROM(0x0C,_threshold); 
}
/**********************************************************
Description: Write data
Parameters: wbuf:The array for storing Data to be sent
            wlen:Length of data sent
Return:     void
Others: 
**********************************************************/
void BMH06203::writeBytes(uint8_t wbuf[], uint8_t wlen)
{
    if (_wire != NULL)
    {
        while (_wire->available() > 0)
        {
            _wire->read();
        }
        _wire->beginTransmission(_i2cAddr);
        _wire->write(wbuf, wlen);
        _wire->endTransmission();
    }
    delay(10);
}
/**********************************************************
Description: Read data
Parameters: rbuf: Used to store received data
            rlen: Length of data to be read
Return:     0:OK
            1:CHECK_ERROR
            2: timeout error
Others: 
**********************************************************/
uint8_t BMH06203::readBytes(uint8_t rbuf[], uint8_t rlen)
{
    uint8_t i = 0, checkSum = 0;
    _wire->requestFrom(_i2cAddr, rlen);
    if (_wire->available() == rlen)
    {
        for (i = 0; i < rlen; i++)
        {
            rbuf[i] = _wire->read();
        }
    }
    else
    {
        return TIMEOUT_ERROR;
    }

    /* Check Sum */
    for (i = 0; i < (rlen - 1); i++)
    {
        checkSum += rbuf[i];
    }
    checkSum = checkSum & 0xFF;
    if (checkSum == rbuf[rlen - 1])
    {
        return CHECK_OK; // Check correct
    }
    else
    {
        return CHECK_ERROR; // Check error
    }
}
