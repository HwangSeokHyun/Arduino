// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef SmartCAR_Bluetooth_H_
#define SmartCAR_Bluetooth_H_
#include "Arduino.h"
//add your includes for the project SmartCAR_Bluetooth here
#include "Timer1.h""
#include "Timer2.h""
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"


//end of add your includes here
//#define DEBUG
#ifdef DEBUG
    #define DEBUG_BEGIN(x) Serial3.begin(x)
    #define DEBUG_PRINT(x) Serial3.print(x)
    #define DEBUG_PRINTF(x, y) Serial3.print(x, y)
    #define DEBUG_PRINTLN(x) Serial3.println(x)
    #define DEBUG_PRINTLNF(x, y) Serial3.println(x, y)
    #define DEBUG_END()  Serial3.end()
#else
    #define DEBUG_BEGIN(x)
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTF(x, y)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTLNF(x, y)
    #define DEBUG_END()
#endif

#define FORWARD  0x09
#define BACKWARD  0x06
#define LEFT_U  0x0A
#define LIGHT_U 0x05
#define LEFT    0x08
#define LIGHT   0x01
#define LEFT_B    0x04
#define LIGHT_B   0x02
#define STOP    0x00

#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project SmartCAR_Bluetooth here
void Motor_mode(int da);
void Motor_Control(char da, unsigned int OC_value);
void Motor_Speed_Left(char maxSpeed);
void Motor_Speed_Right(char maxSpeed);
void setMaxSpeed(char received);
void Encoder_count_L();
void Encoder_count_R();
void Timer1_ISR();
void Motor_Speed_Control();

void ADC_Compare(void);
unsigned char SensorD_read(void);
void SensorA_read(void);
void DAC_CH_Write(unsigned int ch, unsigned int da);
void DAC_setting(unsigned int data);




//Do not add code below this line
#endif /* SmartCAR_Bluetooth_H_ */
