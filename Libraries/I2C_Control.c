#include "I2C_Control.h"

#define  I2C_Wait_Short 0x1000//Ӳ��I2C�̵ȴ�ʱ��
#define  I2C_Wait_Long  0xA000//Ӳ��I2C���ȴ�ʱ��

uint32_t I2C_Wait = 0;//Ӳ��I2C�ȴ�����
u8 I2C_ERROR_Count = 0;//I2C�����������

void I2C_Config(void)//I2C��ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO��ʼ���ṹ��
	
	#if (defined I2C1D)||(defined I2C1R)||(defined I2C2D)//������Ӳ��I2C
		I2C_InitTypeDef  I2C_InitStructure;//����I2C��ʼ���ṹ��
		RCC_APB1PeriphClockCmd( I2Cx_Clock,ENABLE );//ʹ����I2C�йص�ʱ��
		RCC_APB2PeriphClockCmd( I2Cx_GPIO_Clock,ENABLE );//ʹ����I2C GPIO�йص�ʱ��
		
		#ifdef I2C1R//������Ӳ��I2C1��ӳ��
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,ENABLE );//ʹ����ӳ��ʱ��,ע��֮�󲻵�ȡ��ʹ��
			GPIO_PinRemapConfig( GPIO_Remap_I2C1,ENABLE );//ʹ����ӳ��
		#endif
		
		GPIO_InitStructure.GPIO_Pin = I2Cx_SCL_PIN;//SCL����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//��©���
		GPIO_Init( I2Cx_SCL_PORT,&GPIO_InitStructure );//GPIO��ʼ��
		
		GPIO_InitStructure.GPIO_Pin = I2Cx_SDA_PIN;//SDA����
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//��©���
		GPIO_Init( I2Cx_SDA_PORT,&GPIO_InitStructure );//GPIO��ʼ��
		
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//����Ϊ��ͨI2Cģʽ
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;//�ߵ�ƽ�����ȶ�,�͵�ƽ���ݱ仯SCLʱ���ߵ�ռ�ձ�
		I2C_InitStructure.I2C_OwnAddress1 =I2Cx_MCU_Addr_7;//��Ƭ�������7λ��10λI2C��ַ
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//ʹ��Ӧ��
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//7λ��10λѰַģʽ
		I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;//ͨ������
		I2C_Init(I2Cx, &I2C_InitStructure);//I2C��ʼ��
		I2C_Cmd(I2Cx, ENABLE);//ʹ��I2C
	#endif
	

}

#if (defined I2C1D)||(defined I2C1R)||(defined I2C2D)//������Ӳ��I2C
u8 I2C_Write_1Byte(u8 DeviceAddr,u8 WriteAddr,u8 Data)//Ӳ��I2Cд1�ֽ�
{
	I2C_GenerateSTART( I2Cx,ENABLE );//������ʼ��
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))//���EV5�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(1);return 0;	}	}
	
	I2C_Send7bitAddress( I2Cx,DeviceAddr << 1,I2C_Direction_Transmitter );//�����豸��ַ(8λ�Լ�д)
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//���EV6(д)�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(2);return 0;	}	}
	
	I2C_SendData( I2Cx,WriteAddr );//д8λ�Ĵ�����ַ
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(3);return 0;	}	}
	
	I2C_SendData( I2Cx,Data );//д8λ����
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(4);return 0;	}	}
	
	I2C_GenerateSTOP( I2Cx,ENABLE );//���ͽ�����
	return 1;
}

u8 I2C_Read_1Byte(u8 DeviceAddr,u8 ReadAddr)//Ӳ��I2C��1�ֽ�
{
	I2C_Wait = I2C_Wait_Long;//�ȴ�ʱ�临λ
	while(I2C_GetFlagStatus( I2Cx,I2C_FLAG_BUSY ))
	{	if((I2C_Wait--) == 0){	I2C_ERROR(21);return 0;	}	}
	
	I2C_GenerateSTART( I2Cx,ENABLE );//������ʼ��
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))//���EV5�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(22);return 0;	}	}
	
	I2C_Send7bitAddress( I2Cx,DeviceAddr,I2C_Direction_Transmitter );//�����豸��ַ(8λ�Լ�д)
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//���EV6(д)�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(23);return 0;	}	}
	
	I2C_Cmd( I2Cx,ENABLE );//Clear EV6 by setting again the PE bit
	I2C_SendData( I2Cx,ReadAddr );//д8λ�Ĵ�����ַ
	I2C_Wait=I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(24);return 0;	}	}
	
	I2C_GenerateSTART( I2Cx,ENABLE );//���η�����ʼ��
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))//���EV5�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(25);return 0;	}	}
	
	I2C_Send7bitAddress( I2Cx,DeviceAddr,I2C_Direction_Receiver );//�����豸��ַ(8λ�Լ���)
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))//���EV6(��)�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(26);return 0;	}	}
	
	I2C_Wait = I2C_Wait_Long;//�ȴ�ʱ�临λ
	I2C_AcknowledgeConfig( I2Cx,DISABLE );//��ʹ��Ӧ��
	I2C_GenerateSTOP( I2Cx,ENABLE );//���ͽ�����
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))//���EV7�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(27);return 0;	}	}
	
	I2C_AcknowledgeConfig( I2Cx,ENABLE );//Ϊ֮���ͨѶʹ��Ӧ��
	return I2C_ReceiveData(I2Cx);//ȡ������
}

u8 I2C_Write_nByte(u8 DeviceAddr,u16 WriteAddr,u8 *DataArray,u8 n)//Ӳ��I2Cдn�ֽ�
{
	I2C_Wait = I2C_Wait_Long;//�ȴ�ʱ�临λ
	while(I2C_GetFlagStatus( I2Cx,I2C_FLAG_BUSY ))
	{	if((I2C_Wait--) == 0){	I2C_ERROR(41);return 0;	}	}
	
	I2C_GenerateSTART( I2Cx,ENABLE );//������ʼ��
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))//���EV5�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(42);return 0;	}	}
	
	I2C_Send7bitAddress( I2Cx,DeviceAddr << 1,I2C_Direction_Transmitter );//�����豸��ַ(8λ�Լ�д)
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//���EV6(д)�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(43);return 0;	}	}
	
	I2C_Cmd( I2Cx,ENABLE );//Clear EV6 by setting again the PE bit
	
	I2C_SendData( I2Cx,WriteAddr >> 8);//д8λ�Ĵ�����ַ
	I2C_SendData( I2Cx,WriteAddr );//д8λ�Ĵ�����ַ
	
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(44);return 0;	}	}
	
	for(; n>0; n--)
	{
		I2C_SendData( I2Cx,*DataArray );//д8λ����
		I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�����
		{	if((I2C_Wait--) == 0){	I2C_ERROR(45);return 0;	}	}
		
		DataArray++;
	}
	I2C_GenerateSTOP( I2Cx,ENABLE );//���ͽ�����
	return 1;
}

u8 I2C_Read_nByte(u8 DeviceAddr,u16 ReadAddr,u8 *DataArray,u8 n)//Ӳ��I2C��n�ֽ�
{
	I2C_Wait = I2C_Wait_Long;//�ȴ�ʱ�临λ
	while(I2C_GetFlagStatus( I2Cx,I2C_FLAG_BUSY ))
	{	if((I2C_Wait--) == 0){	I2C_ERROR(61);return 0;	}	}
	
	I2C_GenerateSTART( I2Cx,ENABLE );//������ʼ��
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))//���EV5�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(62);return 0;	}	}
	
	I2C_Send7bitAddress( I2Cx,DeviceAddr << 1,I2C_Direction_Transmitter );//�����豸��ַ(8λ�Լ�д)
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))//���EV6(д)�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(63);return 0;	}	}
	
	I2C_Cmd( I2Cx,ENABLE );//Clear EV6 by setting again the PE bit
	
	I2C_SendData( I2Cx,ReadAddr >> 8 );//д8λ�Ĵ�����ַ
	I2C_SendData( I2Cx,ReadAddr );//д8λ�Ĵ�����ַ
	
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))//���EV8�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(64);return 0;	}	}
	
	I2C_GenerateSTART( I2Cx,ENABLE );//���η�����ʼ��
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))//���EV5�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(65);return 0;	}	}
	
	I2C_Send7bitAddress( I2Cx,DeviceAddr << 1,I2C_Direction_Receiver );//�����豸��ַ(8λ�Լ���)
	I2C_Wait = I2C_Wait_Short;//�ȴ�ʱ�临λ
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))//���EV6(��)�����
	{	if((I2C_Wait--) == 0){	I2C_ERROR(66);return 0;	}	}
	
	for(; n>0; n--)
	{
		if(n == 1)
		{
			I2C_AcknowledgeConfig( I2Cx,DISABLE );//��ʹ��Ӧ��
			I2C_GenerateSTOP( I2Cx,ENABLE );//���ͽ�����
		}
		I2C_Wait = I2C_Wait_Long;//�ȴ�ʱ�临λ
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))//���EV7�����
		{	if((I2C_Wait--) == 0){	I2C_ERROR(67);return 0;	}	}
		
		*DataArray = I2C_ReceiveData(I2Cx);
		DataArray++;
	}
	I2C_AcknowledgeConfig( I2Cx,ENABLE );//Ϊ֮���ͨѶʹ��Ӧ��
	return 1;
}
#endif


void I2C_ERROR(u8 errNum)//I2C��������
{
//	printf("I2C_err%d\n", errNum);//���ڴ�ӡ������Ϣ
	//PCout(13) = 0;//��������LED
	#if (defined I2C1D)||(defined I2C1R)||(defined I2C2D)//������Ӳ��I2C
	I2C_ERROR_Count++;
	
	if(((u8)(errNum / 10) % 2) == 0){
		I2C_GenerateSTOP( I2Cx,ENABLE );//������Ӳ��I2C��������Ӳ��I2C������
	}
	
	if(I2C_ERROR_Count >= 8){//��δ���������I2C����
		I2C_ERROR_Count = 0;
		I2C_DeInit(I2Cx);
		I2C_Config();
//		printf("I2C_RESTART\n");//���ڴ�ӡI2C������Ϣ
	}
	#endif	
}
