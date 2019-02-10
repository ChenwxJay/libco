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

#ifndef __CO_EPOLL_H__
#define __CO_EPOLL_H__
#include <stdint.h> //中断头文件
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> //时间处理头文件
//#include <time.h>

#if !defined( __APPLE__ ) && !defined( __FreeBSD__ )
//指定平台，包含epoll.h头文件
#include <sys/epoll.h>

struct co_epoll_res //对epoll_event的封装
{   
	int size;//指定大小字段
	struct epoll_event *events;//指向epoll_event的指针
	struct kevent *eventlist;//事件链表
};
int 	co_epoll_wait( int epfd,struct co_epoll_res *events,int maxevents,int timeout );
int 	co_epoll_ctl( int epfd,int op,int fd,struct epoll_event * );
int 	co_epoll_create( int size );
struct 	co_epoll_res *co_epoll_res_alloc( int n );
void 	co_epoll_res_free( struct co_epoll_res * );

#else
//另一种平台
#include <sys/event.h>//事件头文件包含
enum EPOLL_EVENTS
{
	EPOLLIN = 0X001,//输入事件
	EPOLLPRI = 0X002,//
	EPOLLOUT = 0X004,

	EPOLLERR = 0X008,//err，错误事件
	EPOLLHUP = 0X010,//正常事件

    EPOLLRDNORM = 0x40,
    EPOLLWRNORM = 0x004,
};
#define EPOLL_CTL_ADD 1//epoll操作
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3
typedef union epoll_data
{
	void *ptr;//底层指针，保存数据
	int fd;//描述符
	uint32_t u32;
	uint64_t u64;
} epoll_data_t; //类型重定义，结构体类型

struct epoll_event
{
	uint32_t events;//events,用来表示事件
	epoll_data_t data;//存储数据
};

struct co_epoll_res
{
	int size;//大小
	struct epoll_event *events;//epoll事件指针
	//
	struct kevent *eventlist;
};
int 	co_epoll_wait( int epfd,struct co_epoll_res *events,int maxevents,int timeout );
int 	co_epoll_ctl( int epfd,int op,int fd,struct epoll_event * );
int 	co_epoll_create( int size );//封装epoll_create函数

//动态分配epoll_res结构体，返回一个指针
struct 	co_epoll_res *co_epoll_res_alloc( int n );
//内存释放
void 	co_epoll_res_free( struct co_epoll_res * );

#endif
#endif


