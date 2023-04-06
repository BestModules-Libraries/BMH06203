/*************************************************
File:       		BMH06203.h
Author:             BESTMODULE
Description:        Define classes and required variables
History：			
V1.0.1	 -- initial version；2023-02-09；Arduino IDE :v1.8.13

**************************************************/
#ifndef _BMH06203_H__
#define _BMH06203_H__

#include <Arduino.h>
#include <Wire.h>

#define BMH06203_ADDR 0x28
#define CHECK_OK       0x01
#define CHECK_ERROR    0x02
#define TIMEOUT_ERROR  0x03

#define IIC_MODE  0x00
#define PWM_MODE  0x01
#define IO_MODE1  0x02//≥ temperature threshold IO_C output low level，＜temperature threshold IO_C output high level
#define IO_MODE2 0x06//≥temperature threshold IO_C output high level，＜temperature threshold IO_C output low level

#define AMB_TEMP  0x08  // Environment temperature(unit ℃)
#define OBJ_TEMP 0x09  // Surface temperature(unit ℃)
#define BODY_TEMP 0x0A // Body temperature(unit ℃)

class BMH06203
{
public:
    BMH06203(TwoWire *theWire = &Wire);
    void begin(uint8_t i2c_addr = BMH06203_ADDR);
    float readTemperature(uint8_t TYPE);
    void sleep();
    void writeEEPROM(uint8_t addr, uint16_t data);
    uint16_t readEEPROM(uint8_t addr);
    void setMode(uint8_t Mode);
    void setPWMParam(uint8_t min,uint8_t max);
    void setIOParam(uint8_t threshold);
      
private:
    void writeBytes(uint8_t wbuf[], uint8_t wlen);
    uint8_t readBytes(uint8_t rbuf[], uint8_t rlen);
    TwoWire *_wire = NULL;
    uint8_t _i2cAddr;
    uint32_t _sclPin;
    uint32_t _sdaPin;
};

#endif
