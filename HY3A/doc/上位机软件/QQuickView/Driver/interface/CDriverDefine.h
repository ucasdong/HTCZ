
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <process.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include "../common/cJSON.h"

#define		THREAD_AFFINITY

#define		MAIN_PROCESSING_CPU				1
#define		MAIN_GRAB_BASE_THREAD1			1<<1
#define		MAIN_GRAB_BASE_THREAD2			1<<2
#define		MAIN_NOTIFY_THREAD				1<<3
#define		SAVE_ORIG_THREAD				1<<4 
#define		SAVE_DATA_TRANS_THREAD			1<<5
#define		PLAYBACK_THREAD					1<<6
#define		STATUS_THREAD					1<<7

	
#define		MAIN_UI							1<<12
#define		WINDOW_THREAD1					1<<13
#define		WINDOW_THREAD2					1<<14
#define		WINDOW_THREAD3					1<<15
#define		WINDOW_THREAD4					1<<16
#define		WINDOW_THREAD5					1<<17
#define		WINDOW_THREAD6					1<<18
#define		WINDOW_THREAD7					1<<19
#define		WINDOW_THREAD8					1<<20

