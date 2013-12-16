// Do not remove the include below
#include "SmartCAR_Bluetooth.h"

MPU6050 accelgyro;
int16_t MPU6050_data[6];

union DATA_DB {
	int16_t value;
	unsigned char buff[2];
};

union DATA_DB MPU;

int Motor[6] = { 22, 23, 24, 25, 4, 5 };
int data = 0, flag = 0;
int dealy_time = 400, RX_flag = 1, PWM_value1, PWM_value2;

unsigned char RX_buf2[7];
unsigned char TX_buff[5] = { 0x76, 0x00, 0xF0, 0x00, 0xF0 };
unsigned char TX_bufs[5] = { 0x76, 0x00, 0x0F, 0x00, 0x0F };
char RX_buf[7], RX_ultra[17];
unsigned char TX_buf[7] = { 0x76, 0x00, 0x21, 0x00, }, TX_buf_infrared[17] = {
		0x76, 0x00, 0x3C, 0x00, }, TX_buf_sensor[22] =
		{ 0x76, 0x00, 0x33, 0x00, };
unsigned int ENCODER_CNT_L = 0, ENCODER_CNT_R = 0, Encoder_value_L,
		Encoder_value_R, sensor_flag1 = 0, sensor_flag2 = 0, Ultra_EN = 0,
		sensor_read = 0;
unsigned char Timer_flag = 0;
unsigned char program_flag = 0;

unsigned char left_max = 0;
unsigned char right_max = 0;
unsigned char left_speed = 0;
unsigned char right_speed = 0;

//적외선센서.
int S_DIN = 42, S_SCLK = 43, S_SYNCN = 44, IN_SEN_EN = 26;
int SensorA[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };
int SensorD[8] = { 30, 31, 32, 33, 34, 35, 36, 37 };
unsigned int Buff_A[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned int ADC_MAX[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned int ADC_MIN[8] = { 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023 };
unsigned int ADC_MEAN[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char Sensor_data = 0;
int DAC_data = 0;
//
//The setup function is called once at startup of the sketch
void setup() {
	int z;
	DEBUG_BEGIN(115200);
	Serial.begin(115200);
	Serial1.begin(115200);
	for (z = 0; z < 6; z++) {
		pinMode(Motor[z], OUTPUT);
		digitalWrite(Motor[z], LOW);
	}
	Wire.begin();
	accelgyro.initialize();

	Timer1::set(200000, Timer1_ISR);
	Timer1::start();

	//적외선센서

	pinMode(IN_SEN_EN, OUTPUT);
	pinMode(S_DIN, OUTPUT);
	pinMode(S_SCLK, OUTPUT);
	pinMode(S_SYNCN, OUTPUT);
	digitalWrite(S_SCLK, LOW);
	digitalWrite(S_SYNCN, HIGH);
	digitalWrite(IN_SEN_EN, HIGH);
	for (z = 0; z < 6; z++) {
		pinMode(Motor[z], OUTPUT);
		digitalWrite(Motor[z], LOW);
	}
	for (z = 0; z < 8; z++) {
		pinMode(SensorD[z], INPUT);
	}
	Serial.begin(115200);
	DAC_setting(0x9000);
	for (z = 0; z < 8; z++) {
		DAC_CH_Write(z, 255);
	}
//	infrared_init();
	//
}

// The loop function is called in an endless loop
void loop() {
	Motor_Speed_Control();
	Motor_Control('L', left_speed);
	Motor_Control('R', right_speed);

	delay(50);

	int z;
	for(z=0;z<8;z++)
	{
		Buff_A[z] = analogRead(SensorA[z]);
	}
	for(z=4;z<12;z++)
	{
		if(Buff_A[z-4]>400)
		{
			TX_buf_infrared[z] = 1;
		}
		else
		{
			TX_buf_infrared[z] = 0;
		}
	}
	Serial.write(TX_buf_infrared, 17);
}

void serialEvent() {
	unsigned char z, tmp = 0;
	Serial.readBytes(RX_buf, 7);
	if ((RX_buf[0] == 0x76) && (RX_buf[1] == 0)) {
		for (z = 2; z < 6; z++)
			tmp += (unsigned char) RX_buf[z];
		tmp &= 0xFF;
		if ((unsigned char) RX_buf[6] == tmp) {
			switch (RX_buf[2]) {
			case 0x20:
				switch (RX_buf[4]) {
				case FORWARD:
					Motor_mode(0x09);
					setMaxSpeed(RX_buf[5]);
					break;
				case LEFT:
					Motor_mode(0x0A);
					setMaxSpeed(RX_buf[5]);
					break;
				case LIGHT:
					Motor_mode(0x05);
					setMaxSpeed(RX_buf[5]);
					break;
				default:
					break;
				}
				TX_buf[6] = TX_buf[2];
				for (z = 4; z < 6; z++) {
					TX_buf[z] = (unsigned char) RX_buf[z];
					TX_buf[6] += TX_buf[z];
				}
				for (z = 0; z < 7; z++) {
					Serial.write(TX_buf[z]);
					DEBUG_PRINTF(TX_buf[z],HEX); DEBUG_PRINT(" ");
					delay(5);
				}
				break;
			case 0x30:
				sensor_flag2 = ((unsigned int) (RX_buf[4] << 8)
						| (unsigned char) RX_buf[5]);
				break;
			case 0xF0:
				delay(500);
				asm("jmp 0");
				break;
			}
		}
	}
}

void serialEvent1() {
	unsigned char z, tmp = 0;
	Serial1.readBytes(RX_ultra, 17);
	if ((RX_ultra[0] == 0x76) && (RX_ultra[1] == 0)) {
		for (z = 2; z < 16; z++)
			tmp += (unsigned char) RX_ultra[z];
		tmp = tmp & 0xFF;
		if ((unsigned char) RX_ultra[16] == tmp) {
			TX_buf_infrared[16] = TX_buf_infrared[2];
			for (z = 4; z < 16; z++) {
				TX_buf_infrared[z] = (unsigned char) RX_ultra[z];
				TX_buf_infrared[16] += TX_buf_infrared[z];
			}
			if (Ultra_EN)
				Serial.write(TX_buf_infrared, 17);
		}
	} else {
		for (z = 1; z < 17; z++) {
			if (RX_ultra[z] == 0x76) {
				if (z != 16) {
					if (RX_ultra[z + 1] == 0)
						tmp = z;
				} else {
					tmp = z;
				}
			}
		}
		Serial1.readBytes(RX_ultra, tmp);
	}
}

void Motor_mode(int da) {
	int z;
	for (z = 0; z < 4; z++)
		digitalWrite(Motor[z], (da >> z) & 0x01);
}

void Motor_Control(char da, unsigned int OC_value) {
	switch (da) {
	case 'L':
		analogWrite(Motor[4], OC_value);
		break;
	case 'R':
		analogWrite(Motor[5], OC_value);
		break;
	case 'A':
		analogWrite(Motor[4], OC_value);
		analogWrite(Motor[5], OC_value);
		break;
	}
}

void Motor_Speed_Control()
{
	if(left_speed != right_speed)
	{
		left_speed = left_max*15;
		right_speed = right_max*15;
	}
	else if(left_max == right_max)
	{
		Motor_Speed_Left(left_max);
		Motor_Speed_Right(right_max);
	}
	else
	{
		left_speed = left_max*15;
		right_speed = right_max*15;
	}
}
void Motor_Speed_Left(char maxSpeed) {
	if (left_speed == maxSpeed * 15) {
		return;
	} else if (left_speed < maxSpeed * 15) {
		left_speed+=5;
	} else if (left_speed > maxSpeed * 15) {
		left_speed-=5;
	}
}

void Motor_Speed_Right(char maxSpeed) {
	if (right_speed == maxSpeed * 15) {
		return;
	} else if (right_speed < maxSpeed * 15) {
		right_speed+=5;
	} else if (right_speed > maxSpeed * 15) {
		right_speed-=5;
	}
}
void setMaxSpeed(char received) {
	right_max = received&15;
	left_max = received&240;
	left_max = left_max>>4;
}

void Encoder_count_L() {
	ENCODER_CNT_L++;
}

void Encoder_count_R() {
	ENCODER_CNT_R++;
}

void Timer1_ISR() {
	Timer_flag = 1;
}


///적외선센서
void ADC_Compare(void)
{
	int z;
	for(z=0;z<8;z++)
	{
		if(ADC_MAX[z] < Buff_A[z])
			ADC_MAX[z] = Buff_A[z];
		if(ADC_MIN[z] > Buff_A[z])
			ADC_MIN[z] = Buff_A[z];
	}
}
unsigned char SensorD_read(void)
{
	unsigned char data=0,z;
	for(z=0;z<8;z++)
	{
		data >>= 1;
		if(digitalRead(SensorD[z]))
			data |= 0x80;
	}
	return data;
}

void SensorA_read(void)
{
	int z;
	for(z=0;z<8;z++)
		Buff_A[z] = analogRead(SensorA[z]);
}

void DAC_CH_Write(unsigned int ch, unsigned int da)
{
	unsigned int data = ((ch<<12)&0x7000) | ((da<<4) & 0x0FF0);
	DAC_setting(data);
}

void DAC_setting(unsigned int data)
{
	int z;

	digitalWrite(S_SCLK,HIGH);
	delayMicroseconds(1);
	digitalWrite(S_SCLK,LOW);
	delayMicroseconds(1);
	digitalWrite(S_SYNCN,LOW);
	delayMicroseconds(1);
	for(z=16;z>0;z--)
	{
		digitalWrite(S_DIN,(data>>(z-1))&0x1);
		digitalWrite(S_SCLK,HIGH);
		delayMicroseconds(1);
		digitalWrite(S_SCLK,LOW);
		delayMicroseconds(1);
	}
	digitalWrite(S_SYNCN,HIGH);
}

