#ifndef __MYIIC_H
#define __MYIIC_H

#include "main.h"

#define IIC_CRM					CRM_GPIOF_PERIPH_CLOCK
#define IIC_SCL_GPIO		GPIOA
#define IIC_SCL_PIN			GPIO_PINS_15
#define IIC_SDA_GPIO		GPIOB
#define IIC_SDA_PIN			GPIO_PINS_3
#define sda_index				3

void SDA_IN(void);
void SDA_OUT(void);
void scl_bits_write(uint8_t bits);
void sda_bits_write(uint8_t bits);
uint8_t sda_bits_read(void);

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);
#endif
















