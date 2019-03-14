#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "key.h"
#include "exti.h"
#include "can.h"
#include "timer.h"

int main(void)
{
	u8 M01[2]={0x80,0x01};  
	u8 M02[2]={0x01,0x01};  
	u8 M1[1];               //SYNC
	u16 ID=0x000;
//	u8 res=0;               //can发送标识
//	u8 sen=0;               //can接收标识
//	u8 canbuf[8];           //can缓存
	double count1,count2=0.0f;
	double angle1,angle2=0.0f;
	

	
    Cache_Enable();                 //打开L1-Cache
    HAL_Init();				        //初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    delay_init(216);                //延时初始化
	uart_init(115200);		        //串口初始化
	LED_Init();                     //LED初始化
	KEY_Init();                     //初始化按键
	//波特率=Fpclk1/((tbs1+tbs2+1)*brp)=54/(6+11+1)/3=1Mbps
	EXTI_Init();                    //外部中断初始化
	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_6TQ,CAN_BS1_11TQ,3,CAN_MODE_NORMAL);//CAN初始化正常模式,波特率1Mbps               
	TIM5_Encoder_Init(0xFFFFFFFF-1,1-1);	
	TIM2_Encoder_Init(0xFFFFFFFF-1,1-1);
    TIM3_CH2_CAP_Init(1080-1,1-1);  //溢出时间为10us
	
	//NMT网络配置
	CAN1_Send_Msg(M01,2,ID);        //进入预操作状态
	delay_ms(1);
	CAN1_Send_Msg(M02,2,ID);        //启动远程节点1进入操作状态 
		
	while(1)
	{
		       
		//发送SYNC报文
		ID=0x080;                       
		CAN1_Send_Msg(M1,0,ID);        //发送0个字节 
		
//		if(res)printf("N\r\n");		//提示发送失败
//		else printf("Y\r\n");		    //提示发送成功
//		sen=CAN1_Receive_Msg(canbuf);
//		if(sen)printf("\r\n can接收\r\n");
		
//		count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
//		count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
//		angle1=count1/20000*360;
//		angle2=count2/320000*360;
//		printf("ANGLE1:%lf,ANGLE2:%lf\r\n\r\n",angle1,angle2);
		
		delay_ms(10);
	}
}

