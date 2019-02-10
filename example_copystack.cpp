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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "coctx.h"
#include "co_routine.h"
#include "co_routine_inner.h"

void* RoutineFunc(void* args)//协程函数定义
{   
	co_enable_hook_sys();//使能系统提供的钩子函数配置
	int* routineid = (int*)args; //协程id
	while (true)
	{
		char sBuff[128];//字符缓冲区，用于存储数据
		sprintf(sBuff, "from routineid %d stack addr %p\n", *routineid, sBuff);

		printf("%s", sBuff);//打印缓冲区数据
		//进行poll
		poll(NULL, 0, 1000); //sleep 1s
	}
	return NULL;
}

int main()
{   //构建一个共享栈，使用alloc函数
	stShareStack_t* share_stack= co_alloc_sharestack(1, 1024 * 128);
	//协程属性
	stCoRoutineAttr_t attr;
	attr.stack_size = 0;//栈大小，设置为0
	attr.share_stack = share_stack; //填充属性结构体的栈字段

	stCoRoutine_t* co[2];//协程结构体指针数组
	int routineid[2];
	for (int i = 0; i < 2; i++)//循环处理循环数组
	{   //
		routineid[i] = i;//协程id
		co_create(&co[i], &attr, RoutineFunc, routineid + i);//最后一个参数是传给协程函数的
		co_resume(co[i]);//唤醒协程
	}
	//第一个参数，使用get函数获取当前线程的epoll fd
	co_eventloop(co_get_epoll_ct(), NULL, NULL);
	return 0;
}
