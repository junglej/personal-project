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
																								100-150-200(64-96-C8) 100����200
																								���50Hz  TIM3 CH4
																								100��������1ms�ٶ����
																								150��������1.5ms�ٶ�Ϊ0
																								200��������2ms�ٶ����
																							*/
//	USART_SendData(USART1, data[1]/1.54+0x43);
	TIM_SetCompare4(TIM4,data[1]*100/77+(250-205*100/77));	
																							/*
																								50����150����250(32-96-FA) 50����250
																								���50Hz  	TIM4 CH4
																								50��������0��  
																								150��������90��
																								250��������180��  
																							*/
//	USART_SendData(USART1, data[2]*1.2987-0x10);
}

void Cam_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				 // �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��
 GPIO_SetBits(GPIOB,GPIO_Pin_10);						 // ���
}

void Camera_Init(u32 bound)
{  	 
	  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��UGPIOBʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
  
  //USART3_RX	  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //Usart3 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure);     //��ʼ������3
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	
	USART_ClearFlag(USART3, USART_FLAG_RXNE);
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3 

	Cam_Init();
}

void USART3_IRQHandler(void)
{	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
	{	  	
		u8 c;
		c= USART_ReceiveData(USART3)& 0x00FF; 	
			switch(rxState)
			{
				case waitForStart:			
					if(c == 0x66)					/*��ʼ����ȷ*/
					{		
						dataIndex=1;
						rawWifiData[0] = c;
						rxState = waitForData;
					} else							/*��ʼ������*/
					{
						rxState = waitForStart;
					}
					break;				
				case waitForData:
					rawWifiData[dataIndex] = c;
					dataIndex++;
					if (dataIndex == 6)				/*���ݽ�����ɣ�У��*/
					{
						rxState = waitForChksum;
					}
					break;
				case waitForChksum:
					rawWifiData[6] = c;
					if (wifiDataCrc(rawWifiData))	/*У����ȷ���жϽ�����*/
					{
						rxState = waitForEnd;
					} else
					{
						rxState = waitForStart;		/*У�����*/
					}
					break;
				case waitForEnd:
					if (c == 0x99)					/*��������ȷ*/
					{
						rawWifiData[7] = c;
						wifiDataHandle(rawWifiData);/*������յ�������*/
					} else
					{
						rxState = waitForStart;		/*����������*/
					}
					rxState = waitForStart;
					break;
				default:
					break;
			}
		}
		else	/*��ʱ����*/
		{
			rxState = waitForStart;
		}
}



