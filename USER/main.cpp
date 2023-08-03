#include "Arduino.h"
#include "I2C_Control.h"
#include "i2c_slave.h"

#define LED_Pin PC13
#define KEY_Pin PA1

uint8_t cmd_buf[10];

void LED_Toogle()
{
    Serial.println("KEY is pressed!");
    togglePin(LED_Pin);
}
void setup() {
	
		// put your setup code here, to run once:
		pinMode(LED_Pin, OUTPUT);
    pinMode(KEY_Pin, INPUT_PULLUP);
    attachInterrupt(KEY_Pin, LED_Toogle, FALLING);
       digitalWrite(LED_Pin, HIGH);
        delay(10);
        digitalWrite(LED_Pin, LOW);
        delay(100);
	       digitalWrite(LED_Pin, HIGH);
        delay(10);
		I2C_Config();
		I2C1_Slave_init();

		cmd_buf[0] = 0x01;
		cmd_buf[1] = 0x00;	
		I2C_Write_nByte(0x20,0x00C0,cmd_buf,2);
		delay(10);
//		
//		I2C_Read_nByte(0x20, 0x00CC,read_buf, 2);
//		delay(10);			
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


}

void loop() {
    // put your main code here, to run repeatedly:
//		  I2C_Write_nByte(0x20,0xCC,cmd_buf,3);
//      I2C_Read_nByte(0x20, 0x8000,read_buf, 40);
//	      I2C_Write_nByte(0x20, 0x0300,NULL, 0);
//        delay(16);

//	    for(int i = 0; i < 5; i++)
//    {
//        digitalWrite(LED_Pin, HIGH);
//        delay(1000);
//        digitalWrite(LED_Pin, LOW);
//        delay(1000);
//    }
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
