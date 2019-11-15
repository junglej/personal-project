#include "uart2.h"
#include "usart.h"
#include "led.h"
#include "common.h" 
#include "malloc.h"
Bme bme={0,0,0,0,0};
u8 *p2;

void uart2init(uint32_t bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		 
		//USART1_RX	  GPIOA.10初始化
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure); 

		//Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		 //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//串口波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
		USART_Init(USART2, &USART_InitStructure); //初始化串口1
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
		USART_Cmd(USART2, ENABLE);                    //使能串口1 
		p2=mymalloc(SRAMIN,64);							//申请32字节内存
}
//////////////
void USART2_IRQHandler(void)
{
	
	static uint8_t i=0,rebuf[20]={0};
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//判断接收标志
	{
		rebuf[i++]=USART_ReceiveData(USART2);//读取串口数据，同时清接收标志
		if (rebuf[0]!=0x5A)i=0;	
		if ((i==2)&&(rebuf[1]!=0x5A))i=0;
		if(i>2)//i等于4时，已经接收到数据量字节rebuf[3]
		{				
			if(i!=(rebuf[3]+4))return;	//判断是否接收一帧数据完毕
			//for(j=0;j<(rebuf[3]);j++)sum+=rebuf[j+4];
			//	USART_SendData(USART1,rebuf[8]);//向串口1发送数据
				//USART_SendData(USART1,sum);//向串口1发送数据
			switch(rebuf[2])//接收完毕后处理
			{
				case 0x15:
							bme.Lux=((rebuf[4]<<24)|(rebuf[5]<<16)|(rebuf[6]<<8)|rebuf[7])/100;
					
					break;
				case 0x45:
							bme.Temp=((rebuf[4]<<8)|rebuf[5])/100-6;
							bme.P=(rebuf[6]<<24|rebuf[7]<<16|rebuf[8]<<8|rebuf[9])/100+430;
							bme.Hum=(rebuf[10]<<8|rebuf[11])/100;
							bme.Alt=(rebuf[12]<<8|rebuf[13]-24);
							sprintf((char*)p2,"%03d%03d%03d%04d%06f%04d",bme.Lux,bme.Temp,bme.Hum,bme.Alt,bme.P,2000);//测试数据
							atk_8266_send_cmd("AT+CIPSEND=0,24","OK",24);  //发送指定长度的数据
							delay_ms(100);
							atk_8266_send_data(p2,"OK",100);  //发送指定长度的数据
				break;//原始数据接收，可模仿0x45的方式
				case 0x35:break;
			}	
				
//			printf("  %d",bme.Lux);
//			printf("  %d",bme.Temp);
//			printf("  %d",bme.P);
//			printf("  %d",bme.Hum);
//  		printf("  %d",bme.Alt);
//			printf("\r\n\r\n");
			
//			USART_SendData(USART1,bme.Lux);//向串口1发送数据
//			USART_SendData(USART1,bme.Temp);//向串口1发送数据
//			USART_SendData(USART1,bme.P);//向串口1发送数据
//			USART_SendData(USART1,bme.Hum);//向串口1发送数据
//			USART_SendData(USART1,bme.Alt);//向串口1发送数据				
			i=0;//缓存清0
		}
	}
}
