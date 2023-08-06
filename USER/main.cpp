#include "Arduino.h"
#include "I2C_Control.h"
#include "i2c_slave.h"
#include "Wire.h"

#define LED_Pin PC13
#define TP_INT_Pin PA2
#define KEY_Pin PA1
#define PWM_Pin PA0

#define Soft_i2C

TwoWire Wire(SCL_Pin, SDA_Pin, SOFT_FAST);

uint8_t read_buf[40];
uint8_t cmd_buf[10];

void LED_Toogle()
{
			  #ifdef HW_i2C			
			  I2C_Read_nByte(0x20, 0x8000,read_buf, 40);
	      I2C_Write_nByte(0x20, 0x0300, 0, 0);	
			  #endif
			
        #ifdef Soft_i2C			
				Wire.beginTransmission(0x20); 		
				Wire.write(0x80);        		
				Wire.write(0x00);        		    		
				Wire.endTransmission(); 
				delay_us(50);
			
				Wire.requestFrom(0x20, 40);    
			
				while (Wire.available()) 
				{ 
						for(uint8_t i = 0; i < 40; i++)
					{
					  read_buf[i] = Wire.read(); 
					}
				}	
				
				Wire.beginTransmission(0x20); 		
				Wire.write(0x03);        		
				Wire.write(0x00);        		    		
				Wire.endTransmission(); 

			  #endif		
				digitalWrite(TP_INT_Pin, LOW);
        delay_us(1000);
			  digitalWrite(TP_INT_Pin, HIGH);

}
void setup() {
	   uint8_t read_buf[2];
    /*PWM DutyCycle: 0~1000 -> 0~100%*/
    /*PWM Frequency: 10KHz*/
    PWM_Init(PWM_Pin, 1000, 60);
	  pwmWrite(PWM_Pin, 500);
		// put your setup code here, to run once:
		pinMode(LED_Pin, OUTPUT);
		pinMode(TP_INT_Pin, OUTPUT);
		digitalWrite(TP_INT_Pin, HIGH);

    pinMode(KEY_Pin, INPUT_PULLUP);
    attachInterrupt(KEY_Pin, LED_Toogle, FALLING);
	
	  digitalWrite(LED_Pin, HIGH);
		delay(10);
		digitalWrite(LED_Pin, LOW);
		delay(100);
		digitalWrite(LED_Pin, HIGH);
		delay(10);
	
		I2C1_Slave_init();
	  #ifdef Soft_i2C
	  Wire.begin();
	
		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;
		Wire.beginTransmission(0x20); 		
		Wire.write(0x00);        		
		Wire.write(0xC0);        		
		Wire.write(cmd_buf, 2);        		
		Wire.endTransmission(); 
		delay(10);

		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;
		Wire.beginTransmission(0x20); 		
		Wire.write(0x02);        		
		Wire.write(0xC0);        		
		Wire.write(cmd_buf, 2);        		
		Wire.endTransmission(); 
		delay(10);
		

		Wire.beginTransmission(0x20); 		
		Wire.write(0x04);        		
		Wire.write(0xC0);        		
		Wire.endTransmission();
		delay(10);
		
		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;
		Wire.beginTransmission(0x20); 		
		Wire.write(0x01);        		
		Wire.write(0xC0);        		
		Wire.write(cmd_buf, 2);        		
		Wire.endTransmission(); 
		delay(10);
		#endif
		
		#ifdef HW_i2C
		
		I2C_Config();

		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;	
		I2C_Write_nByte(0x20,0x00C0,cmd_buf,2);
		delay(10);
		
		I2C_Read_nByte(0x20, 0x00CC,read_buf, 2);
		delay(10);			
//		
		I2C_Write_nByte(0x20, 0x04C0, cmd_buf, 0);
		delay(10);						

		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;		
		I2C_Write_nByte(0x20, 0x02C0,cmd_buf, 2);						
		delay(2);
		
		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;		
		I2C_Write_nByte(0x20, 0x01C0,cmd_buf, 2);						
		delay(2);
//		
//		cmd_buf[0] = 0x01;
//		cmd_buf[1] = 0x00;		
//		I2C_Write_nByte(0x20, 0x1900,  cmd_buf,2);						
//		delay(1);	
//		
//		cmd_buf[0] = 0x01;
//		cmd_buf[1] = 0x00;		
//		I2C_Write_nByte(0x20, 0x1000, cmd_buf, 2);						
//		delay(1);	

//		cmd_buf[0] = 0x01;
//		cmd_buf[1] = 0x00;		
//		I2C_Write_nByte(0x20, 0x1500,cmd_buf,  2);						
//		delay(1);	
//		
//		cmd_buf[0] = 0xE3;
//		cmd_buf[1] = 0x00;		
//		I2C_Write_nByte(0x20,  0xC000,cmd_buf, 2);						
//		delay(1);	
//		
//		cmd_buf[0] = 0xCB;
//		cmd_buf[1] = 0x01;		
//		I2C_Write_nByte(0x20, 0xC100, cmd_buf, 2);						
//		delay(1);	
//		
//		I2C_Write_nByte(0x20, 0x0600, NULL, 0);
//		delay(1);	
//		
//		cmd_buf[0] = 0x1F;
//		cmd_buf[1] = 0x00;		
//		I2C_Write_nByte(0x20, 0xF000, cmd_buf, 2);						
//		delay(1);		

//		cmd_buf[0] = 0x00;
//		cmd_buf[1] = 0x00;		
//		I2C_Write_nByte(0x20, 0x2601,cmd_buf,  2);						
//		delay(1);	
      #endif

}

void loop() {

    // put your main code here, to run repeatedly:


}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    GPIO_JTAG_Disable();
//  SysClock_Init(F_CPU_128MHz);
    Delay_Init();
    ADCx_Init(ADC1);
    setup();
    for(;;)loop();
}
