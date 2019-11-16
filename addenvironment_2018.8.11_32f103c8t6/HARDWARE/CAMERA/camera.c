#include "camera.h"
#include "usart.h"
#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum
{
	waitForStart,
	waitForData,
	waitForChksum,
	waitForEnd
} WifilinkRxState;
static u8 rawWifiData[8];
  
u8 dataIndex=0;
WifilinkRxState rxState=waitForStart;

static u8 wifiDataCrc(u8 *data)
{
	u8 temp=(data[1]^data[2]^data[3]^data[4]^data[5])&0xff;
	if(temp==data[6])
		return 1;
	return 0;
}

static void wifiDataHandle(u8 *data)          //51-128-198(33-80-CD)
{																	    
	
	TIM_SetCompare4(TIM3,data[2]*50/77+(200-205*50/77));    
																							/*
																								100-150-200(64-96-C8) 100——200
																								电调50Hz  TIM3 CH4
																								100————1ms速度最大
																								150————1.5ms速度为0
																								200————2ms速度最大
																							*/
//	USART_SendData(USART1, data[1]/1.54+0x43);
	TIM_SetCompare4(TIM4,data[1]*100/77+(250-205*100/77));	
																							/*
																								50——150——250(32-96-FA) 50——250
																								舵机50Hz  	TIM4 CH4
																								50————0度  
																								150————90度
																								250————180度  
																							*/
//	USART_SendData(USART1, data[2]*1.2987-0x10);
}

void Cam_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA端口时钟
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化
 GPIO_SetBits(GPIOB,GPIO_Pin_10);						 // 输出
}

void Camera_Init(u32 bound)
{  	 
	  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能UGPIOB时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
  
  //USART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//收发模式
  USART_Init(USART3, &USART_InitStructure);     //初始化串口3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
	
	USART_ClearFlag(USART3, USART_FLAG_RXNE);
  USART_Cmd(USART3, ENABLE);                    //使能串口3 

	Cam_Init();
}

void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{	  	
		u8 c;
		c= USART_ReceiveData(USART3)& 0x00FF; 	
			switch(rxState)
			{
				case waitForStart:			
					if(c == 0x66)					/*起始符正确*/
					{		
						dataIndex=1;
						rawWifiData[0] = c;
						rxState = waitForData;
					} else							/*起始符错误*/
					{
						rxState = waitForStart;
					}
					break;				
				case waitForData:
					rawWifiData[dataIndex] = c;
					dataIndex++;
					if (dataIndex == 6)				/*数据接收完成，校验*/
					{
						rxState = waitForChksum;
					}
					break;
				case waitForChksum:
					rawWifiData[6] = c;
					if (wifiDataCrc(rawWifiData))	/*校验正确，判断结束符*/
					{
						rxState = waitForEnd;
					} else
					{
						rxState = waitForStart;		/*校验错误*/
					}
					break;
				case waitForEnd:
					if (c == 0x99)					/*结束符正确*/
					{
						rawWifiData[7] = c;
						wifiDataHandle(rawWifiData);/*处理接收到的数据*/
					} else
					{
						rxState = waitForStart;		/*结束符错误*/
					}
					rxState = waitForStart;
					break;
				default:
					break;
			}
		}
		else	/*超时处理*/
		{
			rxState = waitForStart;
		}
}



