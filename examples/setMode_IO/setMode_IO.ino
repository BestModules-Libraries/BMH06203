/*****************************************************************
File:         setMode_IO.ino
Description:  1.I2C(50k) is used to communicate with BMS31M002.
              2.hardware Serial (BAUDRATE 9600) is used to communicate with Serial port monitor.            
connection method： 
Note:
******************************************************************/
#include <BMH06203.h>

 BMH06203 mytherm(&Wire); //Please uncomment out this line of code if you use Wire on BMduino
//BMH06203 mytherm(&Wire1);//Please uncomment out this line of code if you use Wire1 on BMduino
// BMH06203 mytherm(&Wire2);//Please uncomment out this line of code if you use Wire2 on BMduino

void setup() 
{
   Serial.begin(9600);     // start serial for output
   mytherm.begin(); 
   mytherm.setMode(IO_MODE1);
   mytherm.setIOParam(30);
}

void loop()
{
    Serial.println("After running the program, power cycle to switch to IO mode");
}
