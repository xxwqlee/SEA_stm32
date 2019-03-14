#include "exti.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "can.h"
#include "usart.h"
#include "timer.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//外部中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
static double count1,count2,count1_0=0.0f,count2_0=0.0f;
static double dangle1,dangle2=0.0f;
//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();               //开启GPIOA时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();               //开启GPIOC时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();               //开启GPIOH时钟
	__HAL_RCC_CAN1_CLK_ENABLE();                //使能CAN1时钟
    
    GPIO_Initure.Pin=GPIO_PIN_0;                //PA0
    GPIO_Initure.Mode=GPIO_MODE_IT_RISING;      //上升沿触发
    GPIO_Initure.Pull=GPIO_PULLDOWN;			//下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_13;               //PC13
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;     //下降沿触发
    GPIO_Initure.Pull=GPIO_PULLUP;				//上拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_3;     //PH2,3  下降沿触发，上拉
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //中断线0
    HAL_NVIC_SetPriority(EXTI0_IRQn,1,0);       //抢占优先级为1子优先级为0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);             //使能中断线0
    
    //中断线2
    HAL_NVIC_SetPriority(EXTI2_IRQn,1,1);       //抢占优先级为1，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);             //使能中断线2
    
    //中断线3
    HAL_NVIC_SetPriority(EXTI3_IRQn,1,2);       //抢占优先级为1，子优先级为2
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);             //使能中断线2
	
	//中断线5
	HAL_NVIC_SetPriority(EXTI9_5_IRQn,0,2);       //抢占优先级为0，子优先级为2
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);             //使能中断线5
    
    //中断线13
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,1,3);   //抢占优先级为1，子优先级为3
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);         //使能中断线15  
}


//中断服务函数
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);   //调用中断处理公用函数
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);   //调用中断处理公用函数
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);   //调用中断处理公用函数
}

void EXTI5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);   //调用中断处理公用函数
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);  //调用中断处理公用函数
}

//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static u8 led0sta=1,led1sta=1;
	u16 ID;
    u8 res;
	
	//SDO速度环控制
	u8 M11[8]={0x2f,0x60,0x60,0x00,0x03};                //启动速度环
	u8 M12[8]={0x2b,0x40,0x60,0x00,0x05,0x00};           //控制器disable
	u8 M13[8]={0x23,0xff,0x60,0x00,0x40,0x0D,0x03,0x00}; //设置速度
	u8 M14[8]={0x23,0x00,0x21,0x00,0xa0,0x86,0x01,0x00}; //设置加速度
	u8 M15[8]={0x2b,0x40,0x60,0x00,0x0f,0x00};           //运行速度环
	
	//TxPDO2 映射
	u8 M21[8]={0x2f,0x01,0x1A,0x00,0x00};                		//映射Disable
	u8 M22[8]={0x2f,0x01,0x18,0x02,0x01};                		//设置通讯参数为同步（1个SYNC信号）
	u8 M23[8]={0x23,0x01,0x1A,0x01,0x10,0x00,0x60,0x22}; 		//电机相位角
	u8 M24[8]={0x23,0x01,0x1A,0x02,0x10,0x00,0x63,0x22}; 		//编码器角
	u8 M25[8]={0x23,0x01,0x1A,0x03,0x20,0x00,0x69,0x60};		//电机转速
	u8 M26[8]={0x2f,0x01,0x1A,0x00,0x03};                		//映射Enable
	
	//PDO速度控制
//	u8 M3[6]={0x00,0xF6,0xA0,0x86,0x01,0x00};			//设置速度
	u8 M3[6]={0x00,0xF6,0xA0,0x86,0x00,0x00};			//设置速度
	
	//PDO速度控制
	u8 M2[6]={0x00,0xF6,0xE0,0x93,0x04,0x00};			//设置速度
		
	//	//Heartbeat配置
	//	u8 M20[8]={0x2b,0x17,0x10,0x00,0xE8,0x03};//配置为1000ms
    delay_ms(50);      //消抖
	
    switch(GPIO_Pin)
    {
        case GPIO_PIN_0:
            if(WK_UP==1)	
            {
				//SDO速度环控制			
				ID=0x601;		
				
				res=CAN1_Send_Msg(M11,8,ID);        
				delay_ms(1);  
				res=CAN1_Send_Msg(M12,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M13,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M14,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M15,8,ID);        
				delay_ms(1);    
				
			    //控制LED0,LED1互斥点亮
                led1sta=!led1sta;
                led0sta=!led1sta;
                LED1(led1sta);
                LED0(led0sta);
            }
            break;
        case GPIO_PIN_2:
            if(KEY1==0) 	
            {
				//PDO速度控制
				ID=0x501;
				
				res=CAN1_Send_Msg(M3,6,ID);        
				
				//控制LED1翻转	
                led1sta=!led1sta;
                LED1(led1sta);					  
            };
            break;
        case GPIO_PIN_3:
            if(KEY0==0)  	
            {
				//PDO速度控制
				ID=0x501;
				
				res=CAN1_Send_Msg(M2,6,ID);  
				
				//同时控制LED0,LED1翻转 
                led1sta=!led1sta;
                led0sta=!led0sta;
                LED1(led1sta);
                LED0(led0sta); 			  
            }
            break;
		case GPIO_PIN_5:
            count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
			count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
			dangle1=(count1-count1_0)/20000*360;
			dangle2=(count2-count2_0)/320000*360;
			printf("%f    %f\r\n",dangle1,dangle2);

			count1_0=count1;
			count2_0=count2;
            break;
        case GPIO_PIN_13:
            if(KEY2==0)  	
            {
				//TxPDO2 映射
                ID=0x601;	
				
				res=CAN1_Send_Msg(M21,8,ID);        
				delay_ms(1);    
				res=CAN1_Send_Msg(M22,8,ID);       
				delay_ms(1);    
				res=CAN1_Send_Msg(M23,8,ID);         
				delay_ms(1);    
				res=CAN1_Send_Msg(M24,8,ID);        
				delay_ms(1);    
				res=CAN1_Send_Msg(M25,8,ID);        
				delay_ms(1);    
				res=CAN1_Send_Msg(M26,8,ID);        
				delay_ms(1);   
				
				//控制LED0翻转
				led0sta=!led0sta;
                LED0(led0sta);
            }
            break;
    }
}
