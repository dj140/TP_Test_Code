#ifndef _I2C_CONTROL_H
#define _I2C_CONTROL_H

#include "stm32f10x.h"
#ifdef __cplusplus
extern "C" {
#endif
//extern u8 read_buf[];

/*************宏定义操作区*************/
//#define I2C1D//硬件I2C1
//#define I2C1R//硬件I2C1重映射
#define I2C2D//硬件I2C2
//#define I2CS//软件模拟I2C
/*************************************/

#ifdef I2C1D//若定义硬件I2C1
	#define I2Cx             I2C1
	#define I2Cx_Clock       RCC_APB1Periph_I2C1//I2C时钟
	#define I2Cx_SCL_PIN     GPIO_Pin_6//I2C SCL GPIO引脚
	#define I2Cx_SDA_PIN     GPIO_Pin_7//I2C SDA GPIO引脚
#endif

#ifdef I2C1R//若定义硬件I2C1重映射
	#define I2Cx             I2C1
	#define I2Cx_Clock       RCC_APB1Periph_I2C1//I2C时钟
	#define I2Cx_SCL_PIN     GPIO_Pin_8//I2C SCL GPIO引脚
	#define I2Cx_SDA_PIN     GPIO_Pin_9//I2C SDA GPIO引脚
#endif

#ifdef I2C2D//若定义硬件I2C2
	#define I2Cx             I2C2
	#define I2Cx_Clock       RCC_APB1Periph_I2C2//I2C时钟
	#define I2Cx_SCL_PIN     GPIO_Pin_10//I2C SCL GPIO引脚
	#define I2Cx_SDA_PIN     GPIO_Pin_11//I2C SDA GPIO引脚
#endif


	//配置宏定义
#define I2C_Speed        400000//快速模式400k
#define I2Cx_MCU_Addr_7  0X55//单片机本身的7位I2C地址
#define I2Cx_GPIO_Clock  RCC_APB2Periph_GPIOB//I2C GPIO时钟
#define I2Cx_SCL_PORT    GPIOB//I2C SCL GPIO端口
#define I2Cx_SDA_PORT    GPIOB//I2C SDA GPIO端口
//操作函数	
u8	I2C_Write_1Byte(u8 DeviceAddr,u8 WriteAddr,u8 Data);//硬件I2C写1字节
u8	I2C_Read_1Byte(u8 DeviceAddr,u8 ReadAddr);//硬件I2C读1字节
u8	I2C_Write_nByte(u8 DeviceAddr,u16 WriteAddr,u8 *DataArray,u8 n);//硬件I2C写n字节
u8	I2C_Read_nByte(u8 DeviceAddr,u16 ReadAddr,u8 *DataArray,u8 n);//硬件I2C读n字节
void	I2C_Config(void);//I2C初始化
void	I2C_ERROR(u8 errNum);//I2C错误处理函数
#ifdef __cplusplus
}
#endif

#endif
