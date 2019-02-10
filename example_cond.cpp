/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/

#include <unistd.h> //unix标准头文件
#include <stdio.h>
#include <stdlib.h> //标准库头文件
#include <queue> //队列
#include "co_routine.h"
using namespace std;
struct stTask_t
{
	int id;//id,使用整型保存
};
struct stEnv_t
{   //持有条件变量
	stCoCond_t* cond;
	queue<stTask_t*> task_queue;//任务队列，模板参数是一个任务结构体指针
};
void* Producer(void* args)
{
	co_enable_hook_sys();//允许钩子系统函数
	stEnv_t* env=  (stEnv_t*)args;
	int id = 0;//id变量
	while (true)
	{   
        //创建一个任务结构体，使用calloc函数分配内存
		stTask_t* task = (stTask_t*)calloc(1, sizeof(stTask_t));
		//任务对象，id变量+1
		task->id = id++;
		
		env->task_queue.push(task);//将创建的任务句柄插入任务队列
		printf("%s:%d produce task %d\n", __func__, __LINE__, task->id);
		co_cond_signal(env->cond);//唤醒，条件变量
		//直接调用poll函数
		poll(NULL, 0, 1000);
	}
	return NULL;
}
//消费者函数
void* Consumer(void* args)
{   
	co_enable_hook_sys();//允许钩子函数系统调用
	stEnv_t* env = (stEnv_t*)args;
	while (true)
	{
		if (env->task_queue.empty())//如果任务队列已经为空
		{   //条件变量，定时等待
		 	co_cond_timedwait(env->cond, -1);
			continue;//如果超时则继续下一次循环
		}
		
		stTask_t* task = env->task_queue.front();//从任务队列中获取队头任务指针
		env->task_queue.pop();//pop出队头指针
		//打印任务信息
		printf("%s:%d consume task %d\n", __func__, __LINE__, task->id);
		//直接free任务所在的内存块
		free(task);
	}
	return NULL;
}
int main()
{
	stEnv_t* env = new stEnv_t;//创建一个环境变量
	env->cond = co_cond_alloc();//动态创建条件变量，设置环境变量指针

	stCoRoutine_t* consumer_routine;//消费者协程，协程结构体指针
	//调用create函数创建协程
	co_create(&consumer_routine, NULL, Consumer, env);//env是传给协程函数的参数
	//唤醒消费者协程
	co_resume(consumer_routine);

	stCoRoutine_t* producer_routine;//
	co_create(&producer_routine, NULL, Producer, env);//创建生产者线程
	co_resume(producer_routine);//唤醒生产者协程
	
	//创建事件循环
	co_eventloop(co_get_epoll_ct(), NULL, NULL);
	return 0;
}
