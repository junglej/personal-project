#include "common.h"
 #include "led.h"   
void TIM2_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIM7更新中断标志    
		TIM_Cmd(TIM2, DISABLE);  //关闭TIM7 
	}	    
}

void TIM2_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//TIM7时钟使能    

	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	TIM_Cmd(TIM2,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}
	 
u8* atk_8266_check_cmd(u8 *str)
{
	
	char *strx=0;
	if(USART_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART_RX_BUF[USART_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
u8 res=0; 
USART_RX_STA=0;
u1_printf("%s\r\n",cmd);	//发送命令
if(ack&&waittime)		//需要等待应答
{
	while(--waittime)	//等待倒计时
	{
		delay_ms(10);
		if(USART_RX_STA&0X8000)//接收到期待的应答结果
		{
			if(atk_8266_check_cmd(ack))
			{
//				printf("ack:%s\r\n",(u8*)ack);
				break;//得到有效数据 
			}
				USART_RX_STA=0;
		} 
	}
	if(waittime==0)res=1; 
}
return res;
} 
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
		u8 res=0; 
		USART_RX_STA=0;
		u1_printf("%s",data);	//发送命令
		if(ack&&waittime)		//需要等待应答
		{
			while(--waittime)	//等待倒计时
			{
				delay_ms(10);
				if(USART_RX_STA&0X8000)//接收到期待的应答结果
				{
					if(atk_8266_check_cmd(ack))break;//得到有效数据 
					USART_RX_STA=0;
				} 
			}
			if(waittime==0)res=1; 
		}
		return res;
}

//获取Client ip地址
//ipbuf:ip地址输出缓存区
void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
	if(atk_8266_send_cmd("AT+CIFSR","OK",50))//获取WAN IP地址失败
	{
		ipbuf[0]=0;
		return;
	}		
	p=atk_8266_check_cmd("\"");
	p1=(u8*)strstr((const char*)(p+1),"\"");
	*p1=0;
	sprintf((char*)ipbuf,"%s",p+1);	
}


//ATK-ESP8266模块测试主函数
void atk_8266_test(void)
	{ 
	u8 *p;	
	while(atk_8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
			atk_8266_send_cmd("AT+CIPMODE=0","OK",200); //关闭透传模式	
	while(atk_8266_send_cmd("ATE0","OK",20));//关闭回显
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);   //0：单连接，1：多连接
	sprintf((char*)p,"AT+CIPSERVER=1,%s","8080");
	atk_8266_send_cmd(p,"OK",20);     //开启Server模式，端口号为8080		
	USART_RX_STA=0;
}



