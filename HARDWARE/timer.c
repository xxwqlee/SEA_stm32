#include "timer.h"
#include "led.h"
#include "can.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//定时器中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//修改说明
//V1.1 20160711
//新增TIM3_PWM_Init函数,用于PWM输出
////////////////////////////////////////////////////////////////////////////////// 	 

TIM_HandleTypeDef TIM3_Handler;              //定时器3句柄 
TIM_IC_InitTypeDef TIM3_CH1Config; 
//TIM_OC_InitTypeDef TIM3_CH4Handler;          //定时器3通道4句柄

TIM_HandleTypeDef TIM5_Handler;              //定时器5句柄 
TIM_Encoder_InitTypeDef TIM5_Encoder_Config; //编码器接口

TIM_HandleTypeDef TIM2_Handler;              //定时器2句柄 
TIM_Encoder_InitTypeDef TIM2_Encoder_Config; //编码器接口
static double count1,count2,count1_0=0.0f,count2_0=0.0f;
static double dangle1,dangle2=0.0f;
//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_CH2_CAP_Init(u16 arr,u16 psc)
{  
     
    
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_IC_Init(&TIM3_Handler);
    
    TIM3_CH1Config.ICPolarity=TIM_ICPOLARITY_FALLING;    //上升沿捕获
    TIM3_CH1Config.ICSelection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM3_CH1Config.ICPrescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM3_CH1Config.ICFilter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_IC_ConfigChannel(&TIM3_Handler,&TIM3_CH1Config,TIM_CHANNEL_2);//配置TIM5通道2
    HAL_TIM_IC_Start_IT(&TIM3_Handler,TIM_CHANNEL_2);   //开始捕获TIM3的通道2
    __HAL_TIM_ENABLE_IT(&TIM3_Handler,TIM_IT_UPDATE);   //使能更新中断
}

//定时器3底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_IC_Init()调用
//htim:定时器3句柄
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	
    GPIO_Initure.Pin=GPIO_PIN_7;            //PA7
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;      //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    GPIO_Initure.Alternate=GPIO_AF2_TIM3;   //PA7复用为TIM3通道2
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    HAL_NVIC_SetPriority(TIM3_IRQn,1,0);    //设置中断优先级，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
}



//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM3_Handler);//定时器共用处理函数
}
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//更新中断（溢出）发生时执行Tout= ((arr+1)*(psc+1))/Tclk
{
//	double count1,count2,angle1_b=0.0f,angle2_b=0.0f;
//	
//	double speed1,speed2;
//	count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
//	count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
//	angle1_b=count1/20000*360;
//	angle2_b=count2/320000*360;
//	speed1=(angle1_b-angle1_a)*100000;
//	speed2=(angle2_b-angle2_a)*100000;
////	printf("ANGLE1:%lf,ANGLE2:%lf\r\nSPEED1:%f,SPEED2:%f\r\n",angle1_b,angle2_b,speed1,speed2);	
//	angle1_a=angle1_b;
//	angle2_a=angle2_b; 
}
//定时器输入捕获中断处理回调函数，该函数在HAL_TIM_IRQHandler中会被调用
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)//捕获中断发生时执行
{
	
	count1=__HAL_TIM_GET_COUNTER(&TIM5_Handler);
	count2=__HAL_TIM_GET_COUNTER(&TIM2_Handler);
	dangle1=(count1-count1_0)/20000*360;
	dangle2=(count2-count2_0)/320000*360;
	printf("%f    %f\r\n",dangle1,dangle2);
	
	count1_0=count1;
	count2_0=count2;
}


//TIM5 编码器模式初始化

void TIM5_Encoder_Init(u32 arr,u16 psc) 
{	 
	TIM5_Handler.Instance=TIM5;                               //定时器5
    TIM5_Handler.Init.Prescaler=psc;                          //定时器分频
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;         //向上计数模式
    TIM5_Handler.Init.Period=arr;                             //自动重装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	TIM5_Encoder_Config.EncoderMode=TIM_ENCODERMODE_TI12;     //TI1和TI2均计数
	TIM5_Encoder_Config.IC1Polarity=TIM_ICPOLARITY_FALLING;    //双边沿捕获
    TIM5_Encoder_Config.IC1Selection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM5_Encoder_Config.IC1Prescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM5_Encoder_Config.IC1Filter=0;                          //配置输入滤波器，不滤波
	TIM5_Encoder_Config.IC2Polarity=TIM_ICPOLARITY_FALLING;    //双边沿捕获
    TIM5_Encoder_Config.IC2Selection=TIM_ICSELECTION_DIRECTTI;//映射到TI2上
    TIM5_Encoder_Config.IC2Prescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM5_Encoder_Config.IC2Filter=0;                          //配置输入滤波器，不滤波 
    HAL_TIM_Encoder_Init(&TIM5_Handler,&TIM5_Encoder_Config); //初始化TIM5编码器接口	
	HAL_TIM_Encoder_Start(&TIM5_Handler,TIM_CHANNEL_ALL);     //打开TIM5的通道
}

//TIM2 编码器模式初始化
void TIM2_Encoder_Init(u32 arr,u16 psc) 
{	 
	TIM2_Handler.Instance=TIM2;                               //定时器2
    TIM2_Handler.Init.Prescaler=psc;                          //定时器分频
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;         //向上计数模式
    TIM2_Handler.Init.Period=arr;                             //自动重装载值
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	TIM2_Encoder_Config.EncoderMode=TIM_ENCODERMODE_TI12;     //TI1和TI2均计数
	TIM2_Encoder_Config.IC1Polarity=TIM_ICPOLARITY_FALLING;    //双边沿捕获
    TIM2_Encoder_Config.IC1Selection=TIM_ICSELECTION_DIRECTTI;//映射到TI1上
    TIM2_Encoder_Config.IC1Prescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM2_Encoder_Config.IC1Filter=0;                          //配置输入滤波器，不滤波
	TIM2_Encoder_Config.IC2Polarity=TIM_ICPOLARITY_FALLING;    //双边沿捕获
    TIM2_Encoder_Config.IC2Selection=TIM_ICSELECTION_DIRECTTI;//映射到TI2上
    TIM2_Encoder_Config.IC2Prescaler=TIM_ICPSC_DIV1;          //配置输入分频，不分频
    TIM2_Encoder_Config.IC2Filter=0;                          //配置输入滤波器，不滤波
    HAL_TIM_Encoder_Init(&TIM2_Handler,&TIM2_Encoder_Config); //初始化TIM2编码器接口	
	HAL_TIM_Encoder_Start(&TIM2_Handler,TIM_CHANNEL_ALL);     //打开TIM2的通道
}

//编码器模式初始化回调函数
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
	
  GPIO_InitTypeDef GPIO_Initure;                       //初始化IO
  if(htim->Instance==TIM5)
  {
	__HAL_RCC_TIM5_CLK_ENABLE();                    //使能定时器5
	__HAL_RCC_GPIOH_CLK_ENABLE();		            //开启GPIOH时钟
	
	GPIO_Initure.Pin=GPIO_PIN_10|GPIO_PIN_11;       //PH10,PH11
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	        //复用推挽
    GPIO_Initure.Pull=GPIO_PULLUP;        		   	//上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     		//高速
	GPIO_Initure.Alternate=GPIO_AF2_TIM5;			//PH10,11复用为TIM5_CH1,2
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
  }
  else if(htim->Instance==TIM2)
  {
	__HAL_RCC_TIM2_CLK_ENABLE();                    //使能定时器2
	__HAL_RCC_GPIOA_CLK_ENABLE();		            //开启GPIOA时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();                   //开启GPIOB时钟
	
	GPIO_Initure.Pin=GPIO_PIN_3;                    //PB3
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;  	        //复用推挽
    GPIO_Initure.Pull=GPIO_PULLUP;        		    //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     	    //高速
	GPIO_Initure.Alternate=GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	  
	GPIO_Initure.Pin=GPIO_PIN_15;          		    //PA15
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
  }
}
