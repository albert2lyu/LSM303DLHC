#include "I2C.h"

void I2C_Configuration(void)
{
		 I2C_InitTypeDef  I2C_InitStructure;
		 GPIO_InitTypeDef  GPIO_InitStructure; 

		 RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
		
		 GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
		 GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
		
		 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_9;
		 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		 GPIO_Init(GPIOB, &GPIO_InitStructure);
			
		 I2C_DeInit(I2C1);
		 I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
		 I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
		 I2C_InitStructure.I2C_OwnAddress1 = 0x43;
		 I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		 I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		 I2C_InitStructure.I2C_ClockSpeed = 100000;
			
		 I2C_Cmd(I2C1, ENABLE);
		 I2C_Init(I2C1, &I2C_InitStructure);

		 I2C_AcknowledgeConfig(I2C1, ENABLE);
}



static void I2C_delay(uint16_t cnt)
{
	while(cnt--);
}


static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE);
    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
  I2C_GenerateSTOP(I2Cx, ENABLE);
}

uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
  if(num==0)
	return 1;
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
  I2C_GenerateSTART(I2Cx, ENABLE);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

  I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter);
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2Cx, addr);
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));	
  while (num)
 {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);
    		I2C_GenerateSTOP(I2Cx, ENABLE);
		}
	    
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)); 
	    *buf = I2C_ReceiveData(I2Cx);
	    buf++;
	    num--;
    }

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	return 0;
}	


uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
  	I2C_GenerateSTART(I2Cx, ENABLE);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  	I2C_SendData(I2Cx, addr);
	  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  	I2C_SendData(I2Cx, value); 
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	I2C_GenerateSTOP(I2Cx, ENABLE);
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);
	  I2C_delay(1000);
	  return 0;
}

uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
	uint8_t err=0;
	while(num--)
	{
		if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf++))
		{
			err++;
		}
	}
	if(err)
		return 1;
	else 
		return 0;	
}

