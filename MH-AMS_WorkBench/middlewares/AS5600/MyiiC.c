#include "MyiiC.h"
#include "delay.h"

void SDA_IN(void)
{
	IIC_SDA_GPIO->cfgr  &= (uint32_t)~(0x03 << (sda_index * 2));
  IIC_SDA_GPIO->cfgr  |= (uint32_t)(0x00 << (sda_index * 2));
}

void SDA_OUT(void)
{
	IIC_SDA_GPIO->cfgr  &= (uint32_t)~(0x03 << (sda_index * 2));
  IIC_SDA_GPIO->cfgr  |= (uint32_t)(0x01 << (sda_index * 2));
}

void scl_bits_write(uint8_t bits)
{
	if(bits==1) gpio_bits_write(IIC_SCL_GPIO, IIC_SCL_PIN, TRUE);
	else gpio_bits_write(IIC_SCL_GPIO, IIC_SCL_PIN, FALSE);
}

void sda_bits_write(uint8_t bits)
{
	if(bits==1) gpio_bits_write(IIC_SDA_GPIO, IIC_SDA_PIN, TRUE);
	else gpio_bits_write(IIC_SDA_GPIO, IIC_SDA_PIN, FALSE);
}

uint8_t sda_bits_read(void)
{
	if(gpio_input_data_bit_read(IIC_SDA_GPIO, IIC_SDA_PIN)==RESET) return 0;
	else return 1;
}

//初始化IIC
void IIC_Init(void)
{			
//  gpio_init_type gpio_init_struct;
//	/* enable the led clock */
//	crm_periph_clock_enable(IIC_CRM, TRUE);
//	/* set default parameter */
//	gpio_default_para_init(&gpio_init_struct);
//	/* configure the led gpio */
//	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
//	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
//	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
//	gpio_init_struct.gpio_pins =  IIC_SCL_PIN|IIC_SDA_PIN;
//	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
//	gpio_init(IIC_GPIO, &gpio_init_struct);
	scl_bits_write(1);
	sda_bits_write(1);
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	sda_bits_write(1);	  	  
	scl_bits_write(1);
	delay_us(40);
 	sda_bits_write(0);//START:when CLK is high,DATA change form high to low 
	delay_us(40);
	scl_bits_write(0);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	scl_bits_write(0);
	sda_bits_write(0);//STOP:when CLK is high DATA change form low to high
 	delay_us(40);
	scl_bits_write(1);
	sda_bits_write(1);//发送I2C总线结束信号
	delay_us(40);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint16_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	sda_bits_write(1);delay_us(10);	   
	scl_bits_write(1);delay_us(10);	 
	while(sda_bits_read())
	{
		ucErrTime++;
		if(ucErrTime>2500)
		{
			IIC_Stop();
			return 1;
		}
	}
	scl_bits_write(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	scl_bits_write(0);
	SDA_OUT();
	sda_bits_write(0);
	delay_us(20);
	scl_bits_write(1);
	delay_us(20);
	scl_bits_write(0);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	scl_bits_write(0);
	SDA_OUT();
	sda_bits_write(1);
	delay_us(20);
	scl_bits_write(1);
	delay_us(20);
	scl_bits_write(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
  uint8_t t;   
	SDA_OUT(); 	    
  scl_bits_write(0);//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    sda_bits_write((txd&0x80)>>7);
    txd<<=1; 	  
		delay_us(20);   //对TEA5767这三个延时都是必须的
		scl_bits_write(1);
		delay_us(20); 
		scl_bits_write(0);
		delay_us(20);
  }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    scl_bits_write(0);;
    delay_us(20);
		scl_bits_write(1);
    receive<<=1;
    if(sda_bits_read())receive++;   
		delay_us(10); 
  }					 
  if (!ack)
      IIC_NAck();//发送nACK
  else
      IIC_Ack(); //发送ACK   
  return receive;
}


