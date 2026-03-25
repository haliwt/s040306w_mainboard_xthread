#ifndef __INTERRUPT_MANAGER_H_
#define __INTERRUPT_MANAGER_H_
#include "main.h"



//void tim17_isr_callback_handler(void);

// 定义 TIM17 回调函数类型
//typedef void (*Tim17Callback)(void);

void callback_register_fun(void);


// 注册接口
//void tim17_register_callback(Tim17Callback cb);

// ISR 调用的触发函数
void tim17_invoke_callback(void);



#endif 
