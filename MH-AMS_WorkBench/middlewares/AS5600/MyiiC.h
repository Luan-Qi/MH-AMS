#ifndef __MYIIC_H
#define __MYIIC_H

#include "main.h"

#define IIC1_SCL_GPIO			GPIOA
#define IIC1_SCL_PIN			GPIO_PINS_15
#define IIC1_SDA_GPIO			GPIOB
#define IIC1_SDA_PIN			GPIO_PINS_3

#define IIC2_SCL_GPIO			GPIOF
#define IIC2_SCL_PIN			GPIO_PINS_1
#define IIC2_SDA_GPIO			GPIOF
#define IIC2_SDA_PIN			GPIO_PINS_0

#define IIC_NOP		  			delay_IIC(100) 

class IIC
{
public:
	IIC(gpio_type * SCL_GPIO, uint16_t SCL_PIN, gpio_type * SDA_GPIO, uint16_t SDA_PIN);
	void SDA_IN(void);
	void SDA_OUT(void);
	void scl_bits_write(uint8_t bits);
	void sda_bits_write(uint8_t bits);
	uint8_t sda_bits_read(void);

	//IIC���в�������
	void IIC_Init(void);                //��ʼ��IIC��IO��
	void IIC_Start(void);				//����IIC��ʼ�ź�
	void IIC_Stop(void);	  			//����IICֹͣ�ź�
	void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
	uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
	uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
	void IIC_Ack(void);					//IIC����ACK�ź�
	void IIC_NAck(void);				//IIC������ACK�ź�
protected:
	gpio_type * IIC_SCL_GPIO = GPIOA;
	uint16_t IIC_SCL_PIN = GPIO_PINS_15;
	gpio_type * IIC_SDA_GPIO = GPIOB;
	uint16_t IIC_SDA_PIN = GPIO_PINS_3;
	uint8_t sda_index = 3;
};

#endif
















