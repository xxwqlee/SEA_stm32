#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/11/27
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
extern TIM_HandleTypeDef TIM3_Handler;      //定时器3句柄 
extern TIM_IC_InitTypeDef TIM3_CH1Config;   
//extern TIM_OC_InitTypeDef TIM3_CH4Handler;  //定时器3通道4句柄

extern TIM_HandleTypeDef TIM5_Handler;      //定时器5句柄
extern TIM_Encoder_InitTypeDef TIM5_Encoder_Config; //编码器接口

extern TIM_HandleTypeDef TIM2_Handler;              //定时器2句柄 
extern TIM_Encoder_InitTypeDef TIM2_Encoder_Config; //编码器接口

//void TIM3_Init(u16 arr,u16 psc);
//void TIM3_PWM_Init(u16 arr,u16 psc);
//void TIM_SetTIM3Compare4(u32 compare);
void TIM3_CH2_CAP_Init(u16 arr,u16 psc);

void TIM5_Encoder_Init(u32 arr,u16 psc); //TIM5 编码器模式初始化
void TIM2_Encoder_Init(u32 arr,u16 psc);
#endif

