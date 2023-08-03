#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "misc.h"

#include "i2c_slave.h"

/*******************************************************************/
extern volatile uint8_t ADCBuffer[16];

unsigned char i2c1_mode=I2C1_MODE_WAITING;
/*---------IIC1---------------*/
uint8_t  Buffer_Rx_IIC1[40];//????
uint8_t  Rx_Idx_IIC1=0;//????
uint8_t  Flag_RcvOK_IIC1 = 0;// ?????? 
uint8_t  Tx_Idx_IIC1=0;//????
u8 Response_Message[40];//????
uint8_t i2c1_ram_adr=0;
uint8_t i2c1_send_adr=0;

uint8_t i2c1_ram[I2C1_RAM_SIZE+1];

/*******************************************************************/
uint8_t get_i2c1_ram(uint8_t adr) {
	//ADC data
//	if ((ADC_ADDR_START <= adr) & (adr < ADC_ADDR_START + ADC_CHANNELS*2)) {
//		return ADCBuffer[adr - ADC_ADDR_START];
//	}
//	else {
//		// Other addresses
//	uint8_t buffer[10];
//	buffer[0] = 0x22;

//		buffer[1] = 0x22;
//		buffer[2] = 0xE6;
//		buffer[3] = 0x22;

		return i2c1_ram[adr];
//	}
}

void set_i2c1_ram(uint8_t adr, uint8_t val) {
	i2c1_ram[adr] = val;
	return;
}


/*******************************************************************/
void I2C1_Slave_init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* Configure I2C_EE pins: SCL and SDA */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure the I2C event priority */
    NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Configure I2C error interrupt to have the higher priority.
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    /* I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = I2CSLAVE_ADDR;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2C1_CLOCK_FRQ;

    /* I2C Peripheral Enable */
    I2C_Cmd(I2C1, ENABLE);
    /* Apply I2C configuration after enabling it */
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE); //Part of the STM32 I2C driver
    I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE); //Part of the STM32 I2C driver
}
/*******************************************************************/

/*******************************************************************/
void I2C1_ClearFlag(void) {
  // ADDR-Flag clear
  while((I2C1->SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR) {
    I2C1->SR1;
    I2C1->SR2;
  }

  // STOPF Flag clear
  while((I2C1->SR1&I2C_SR1_STOPF) == I2C_SR1_STOPF) {
    I2C1->SR1;
    I2C1->CR1 |= 0x1;
  }
}
/*******************************************************************/

/*******************************************************************/
void I2C1_EV_IRQHandler(void)
{

  __IO uint32_t SR1Register =0;
  __IO uint32_t SR2Register =0;

  SR1Register = I2C1->SR1;
  SR2Register = I2C1->SR2;

    /* I2C1是从机(MSL = 0) */
  if((SR2Register &0x0001) != 0x0001)
  {
    /* 主机已发送地址，地址为被置位·(ADDR = 1: EV1(包括发送和接收)) */
    if((SR1Register & 0x0002) == 0x0002)
    {
        /* 清除标志位 */
        SR1Register = 0;
        SR2Register = 0;

        Rx_Idx_IIC1=0;
        Tx_Idx_IIC1=0;
    }



    /* 接收数据(RXNE = 1: EV2) */
    if((SR1Register & 0x0040) == 0x0040)
    {
        Buffer_Rx_IIC1[Rx_Idx_IIC1++] = I2C1->DR;
        SR1Register = 0;
        SR2Register = 0;		
			if(Buffer_Rx_IIC1[1] == 0xCC | Buffer_Rx_IIC1[1] == 0x20)
			{
						Response_Message[0] = 0x48;
						Response_Message[1] = 0xE5;
			}
			else if(Buffer_Rx_IIC1[0] == 0x80)
			{
						Response_Message[0] = 0x04;
						Response_Message[1] = 0x08;
			}
			else if(Buffer_Rx_IIC1[0] == 0x2C)
			{
						Response_Message[0] = 0xAA;
						Response_Message[1] = 0x55;
			}
//		  else
//			{
//						Response_Message[0] = 0x01;
//						Response_Message[1] = 0x02;
//			}
    }
    /* 检测到停止条件(STOPF =1: EV4) */
    if(( SR1Register & 0x0010) == 0x0010)
    {
        I2C1->CR1 |= 0x0001;
        SR1Register = 0;
        SR2Register = 0;
        Flag_RcvOK_IIC1 = 1;            
    }




    /* 发送数据(TxE = 1: EV3、EV3-1) */
    if((SR1Register & 0x0080) == 0x0080)
    {

//			Response_Message[0] = 0x01;
//						Response_Message[1] = 0x02;
				Response_Message[2] = 0x03;
				Response_Message[3] = 0x04;
				Response_Message[4] = 0x05;
				Response_Message[5] = 0x06;
				Response_Message[6] = 0x07;
				Response_Message[7] = 0x08;
				Response_Message[8] = 0x09;
				Response_Message[9] = 0x10;
				Response_Message[10] = 0x11;
				Response_Message[11] = 0x12;
				Response_Message[12] = 0x13;
				Response_Message[13] = 0x14;
				Response_Message[14] = 0x15;
				Response_Message[15] = 0x16;


        I2C1->DR = Response_Message[Tx_Idx_IIC1++]; 
        SR1Register = 0;
        SR2Register = 0;
    }
    /* 检测到非应答(AF =1: EV3-2) */
    if(( SR1Register & 0x0400) == 0x0400)
    {
        I2C1->SR1 &= 0xFDFF;
        SR1Register = 0;
        SR2Register = 0;        
    }       
  }

}
/*******************************************************************/
void I2C1_ER_IRQHandler(void) {
//  if (I2C_GetITStatus(I2C1, I2C_IT_AF)) {
//    I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
//  }
	  __IO uint32_t SR1Register =0;
  __IO uint32_t SR2Register =0;
  SR1Register = I2C1->SR1;
  SR2Register = I2C1->SR2;

    if(I2C_GetITStatus(I2C1,        I2C_IT_SMBALERT)) {
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT)) {
    } 
    else if(I2C_GetITStatus(I2C1, I2C_IT_PECERR)) {
    } 
    else if(I2C_GetITStatus(I2C1, I2C_IT_OVR)) {

    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_AF)) {

        I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_ARLO)) {

    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_BERR)) {

    }
        I2C1->CR1 |= 0x0001;
        SR1Register = 0;
        SR2Register = 0;  

}
/*******************************************************************/
