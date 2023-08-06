#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "misc.h"

#include "i2c_slave.h"
#include "I2C_Control.h"
#include "Wire.h"
uint8_t flag = 1;
#define Soft_i2C
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

		return read_buf[adr];
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
extern "C"
{
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
}
/*******************************************************************/

/*******************************************************************/
extern "C"
{
void I2C1_EV_IRQHandler(void)
{

  __IO uint32_t SR1Register =0;
  __IO uint32_t SR2Register =0;

  SR1Register = I2C1->SR1;
  SR2Register = I2C1->SR2;

    /* I2C1????(MSL = 0) */
  if((SR2Register &0x0001) != 0x0001)
  {
    /* ????????????????????????(ADDR = 1: EV1(????????????)) */
    if((SR1Register & 0x0002) == 0x0002)
    {
        /* ??????? */
        SR1Register = 0;
        SR2Register = 0;

        Rx_Idx_IIC1=0;
        Tx_Idx_IIC1=0;
    }

    /* ????????(RXNE = 1: EV2) */
    if((SR1Register & 0x0040) == 0x0040)
    {

        Buffer_Rx_IIC1[Rx_Idx_IIC1++] = I2C1->DR;
        SR1Register = 0;
        SR2Register = 0;		
			if(Buffer_Rx_IIC1[0] == 0xAE)
			{
						Response_Message[0] = 0x26;				
			}
			else if(Buffer_Rx_IIC1[0] == 0xAF | Buffer_Rx_IIC1[0] == 0x10)
			{
				for(int i = 0; i < 40; i++)
				{
					
						Response_Message[i] = 0x00;
				}
										if(read_buf[2] == 0x02)		
										{
											flag++;
											if(flag == 0x01)
											{
					  //Finger 0 event info (touch / event type / hover / palm / event id[0~3])
						Response_Message[0] = 0xA2;
																  //Finger 0 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
						Response_Message[1] = (read_buf[13] << 4) | (read_buf[11] & 0x0F);
					
					  //Finger 0 x coordinate (bit 7 ~ bit 0)
						Response_Message[2] = read_buf[10] ;
					
					  //Finger 0 y coordinate (bit 7 ~ bit 0)
						Response_Message[3] = read_buf[12] ;

					  //Finger 0 z (strength)
					  Response_Message[4] = read_buf[14] ;
											}
											else
											{
																	  //Finger 0 event info (touch / event type / hover / palm / event id[0~3])
						Response_Message[0] =  0xA1;									
									
					  //Finger 0 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
						Response_Message[1] = (read_buf[7] << 4) | (read_buf[5] & 0x0F);
					
					  //Finger 0 x coordinate (bit 7 ~ bit 0)
						Response_Message[2] = read_buf[4] ;
					
					  //Finger 0 y coordinate (bit 7 ~ bit 0)
						Response_Message[3] = read_buf[6] ;

					  //Finger 0 z (strength)
					  Response_Message[4] = read_buf[8] ;
												flag = 0;
											}
										}
										else{
						  //Finger 0 event info (touch / event type / hover / palm / event id[0~3])
						Response_Message[0] = (read_buf[1]<<4 | 0x7F) & 0xA1;									
									
					  //Finger 0 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
						Response_Message[1] = (read_buf[7] << 4) | (read_buf[5] & 0x0F);
					
					  //Finger 0 x coordinate (bit 7 ~ bit 0)
						Response_Message[2] = read_buf[4] ;
					
					  //Finger 0 y coordinate (bit 7 ~ bit 0)
						Response_Message[3] = read_buf[6] ;

					  //Finger 0 z (strength)
					  Response_Message[4] = read_buf[8] ;
            	}
					  //Finger 1 event info (touch / event type / hover / palm / event id[0~3])		
            if(read_buf[2] == 0x02)		
						{							
						  Response_Message[8] = 0xA2;
						}
						else
						{
						  Response_Message[8] = 0x22;						
						}
					  //Finger 1 xy coordinate (high)  y coordinate (bit 11 ~ bit 8) x coordinate (bit 11 ~ bit 8)
						Response_Message[9] = (read_buf[13] << 4) | (read_buf[11] & 0x0F);					

					  //Finger 1 x coordinate (bit 7 ~ bit 0)
						Response_Message[10] = read_buf[10] ;
					
					  //Finger 1 y coordinate (bit 7 ~ bit 0)
						Response_Message[11] = read_buf[12] ;

					  //Finger 1 z (strength)
					  Response_Message[12] = read_buf[14] ;
						
				

			}
			else if(Buffer_Rx_IIC1[0] == 0x0F)
			{
//					if(read_buf[2] == 0x02)		
//					{
//	          Response_Message[0] = 0x10;
//					}
//					else
//					{
			      Response_Message[0] = 0x08;

//					}

			}
			else if(Buffer_Rx_IIC1[0] == 0xE1)
			{
						Response_Message[0] = 0x04;
						Response_Message[1] = 0x82;
						Response_Message[2] = 0x07;
						Response_Message[3] = 0x00;
			}
			
			else if(Buffer_Rx_IIC1[0] == 0xF6)
			{
						Response_Message[0] = 0x55;
						Response_Message[1] = 0x56;
						Response_Message[2] = 0x32;
						Response_Message[3] = 0x00;
						Response_Message[4] = 0x00;
						Response_Message[5] = 0x00;
						Response_Message[6] = 0x00;
						Response_Message[7] = 0x00;
						Response_Message[8] = 0x11;
						Response_Message[9] = 0x08;

			}
			else if(Buffer_Rx_IIC1[0] == 0x02)
			{
						Response_Message[0] = 0x54;
						Response_Message[1] = 0x00;
						Response_Message[2] = 0x00;
						Response_Message[3] = 0x00;
			}
			else if(Buffer_Rx_IIC1[0] == 0x0B)
			{
						Response_Message[0] = 0x13;
						Response_Message[1] = 0x0C;
						Response_Message[2] = 0x00;
			}
			
			else if(Buffer_Rx_IIC1[0] == 0xE1)
			{
						Response_Message[0] = 0x04;
						Response_Message[1] = 0x82;
						Response_Message[2] = 0x07;
						Response_Message[2] = 0x00;
			}
			else if(Buffer_Rx_IIC1[0] == 0xE2)
			{
						Response_Message[0] = 0x82;
			}
			else if(Buffer_Rx_IIC1[0] == 0xE3)
			{
						Response_Message[0] = 0x07;
			}
//		  else
//			{
//						Response_Message[0] = 0x01;
//						Response_Message[1] = 0x02;
//			}

    }
    /* ?????????(STOPF =1: EV4) */
    if(( SR1Register & 0x0010) == 0x0010)
    {
        I2C1->CR1 |= 0x0001;
        SR1Register = 0;
        SR2Register = 0;
        Flag_RcvOK_IIC1 = 1;     
	
    }
    /* ????????(TxE = 1: EV3??EV3-1) */
    if((SR1Register & 0x0080) == 0x0080)
    {
        I2C1->DR = Response_Message[Tx_Idx_IIC1++]; 
        SR1Register = 0;
        SR2Register = 0;
    }
    /* ????????(AF =1: EV3-2) */
    if(( SR1Register & 0x0400) == 0x0400)
    {
        I2C1->SR1 &= 0xFDFF;
        SR1Register = 0;
        SR2Register = 0;     
		
    }       
  }

}
}
/*******************************************************************/
extern "C"
{
void I2C1_ER_IRQHandler(void) {
//  if (I2C_GetITStatus(I2C1, I2C_IT_AF)) {
//    I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
//  }
	  __IO uint32_t SR1Register =0;
  __IO uint32_t SR2Register =0;
  SR1Register = I2C1->SR1;
  SR2Register = I2C1->SR2;

    if(I2C_GetITStatus(I2C1,        I2C_IT_SMBALERT)) {
			        I2C_ClearITPendingBit(I2C1, I2C_IT_SMBALERT);

    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_TIMEOUT)) {
						        I2C_ClearITPendingBit(I2C1, I2C_IT_TIMEOUT);

    } 
    else if(I2C_GetITStatus(I2C1, I2C_IT_PECERR)) {
									        I2C_ClearITPendingBit(I2C1, I2C_IT_PECERR);

    } 
    else if(I2C_GetITStatus(I2C1, I2C_IT_OVR)) {
			
									        I2C_ClearITPendingBit(I2C1, I2C_IT_OVR);

    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_AF)) {

        I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_ARLO)) {
        I2C_ClearITPendingBit(I2C1, I2C_IT_ARLO);

    }
    else if(I2C_GetITStatus(I2C1, I2C_IT_BERR)) {
        I2C_ClearITPendingBit(I2C1, I2C_IT_BERR);

    }
        I2C1->CR1 |= 0x0001;
        SR1Register = 0;
        SR2Register = 0;  

}
}
/*******************************************************************/
