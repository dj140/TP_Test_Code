#ifndef _I2C_CONTROL_H
#define _I2C_CONTROL_H

#include "stm32f10x.h"
#ifdef __cplusplus
extern "C" {
#endif
//extern u8 read_buf[];

/*************�궨�������*************/
//#define I2C1D//Ӳ��I2C1
//#define I2C1R//Ӳ��I2C1��ӳ��
#define I2C2D//Ӳ��I2C2
//#define I2CS//���ģ��I2C
/*************************************/

#ifdef I2C1D//������Ӳ��I2C1
	#define I2Cx             I2C1
	#define I2Cx_Clock       RCC_APB1Periph_I2C1//I2Cʱ��
	#define I2Cx_SCL_PIN     GPIO_Pin_6//I2C SCL GPIO����
	#define I2Cx_SDA_PIN     GPIO_Pin_7//I2C SDA GPIO����
#endif

#ifdef I2C1R//������Ӳ��I2C1��ӳ��
	#define I2Cx             I2C1
	#define I2Cx_Clock       RCC_APB1Periph_I2C1//I2Cʱ��
	#define I2Cx_SCL_PIN     GPIO_Pin_8//I2C SCL GPIO����
	#define I2Cx_SDA_PIN     GPIO_Pin_9//I2C SDA GPIO����
#endif

#ifdef I2C2D//������Ӳ��I2C2
	#define I2Cx             I2C2
	#define I2Cx_Clock       RCC_APB1Periph_I2C2//I2Cʱ��
	#define I2Cx_SCL_PIN     GPIO_Pin_10//I2C SCL GPIO����
	#define I2Cx_SDA_PIN     GPIO_Pin_11//I2C SDA GPIO����
#endif


	//���ú궨��
#define I2C_Speed        400000//����ģʽ400k
#define I2Cx_MCU_Addr_7  0X55//��Ƭ�������7λI2C��ַ
#define I2Cx_GPIO_Clock  RCC_APB2Periph_GPIOB//I2C GPIOʱ��
#define I2Cx_SCL_PORT    GPIOB//I2C SCL GPIO�˿�
#define I2Cx_SDA_PORT    GPIOB//I2C SDA GPIO�˿�
//��������	
u8	I2C_Write_1Byte(u8 DeviceAddr,u8 WriteAddr,u8 Data);//Ӳ��I2Cд1�ֽ�
u8	I2C_Read_1Byte(u8 DeviceAddr,u8 ReadAddr);//Ӳ��I2C��1�ֽ�
u8	I2C_Write_nByte(u8 DeviceAddr,u16 WriteAddr,u8 *DataArray,u8 n);//Ӳ��I2Cдn�ֽ�
u8	I2C_Read_nByte(u8 DeviceAddr,u16 ReadAddr,u8 *DataArray,u8 n);//Ӳ��I2C��n�ֽ�
void	I2C_Config(void);//I2C��ʼ��
void	I2C_ERROR(u8 errNum);//I2C��������
#ifdef __cplusplus
}
#endif

#endif
