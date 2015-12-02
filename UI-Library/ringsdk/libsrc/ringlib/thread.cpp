/**********************************************************************
//
//
//        ##########                          ######  #########   # ######    #
//      #############                      ########### ######### #########  ###
//     ######## # ###  ##                 ############# ##    ## #####  # ####
//     ####  ##    ## ###                 ###     ### # #      #####   #####
//          #     ###  #            #     ##       ##  ##      ##     ###
//         ## ###### ##      ##  ####    ####          #       #     ##
//       #########  ###  ## ### #######   ######      ##      ##    ###
//      ######      ##  ######  ##  ##       ####     #      ##     ####
//     #######     ##   ###### ##  ###          ##   ##     ###    ######
//     #########   ##  ###### ## ######         ### ##    ###      #  #####
//    ##    ###### ####### ### #### ##  ## #######  ########      ##    ####
//    ##      #### ###  #  ### ### ##  ##########   ######       ##      ####
//   ##         ##                ##   #########    ####         #         ##
//               #              ###
//                              ##
//                             ###
//                             ##
//
//
//							RingSDK类库 ringlib.lib
//作者：临风
//
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：thread.cpp
//
//说明：线程类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB

#include "ringlib.h"

RingThread::RingThread()
{
	m_handle = NULL;
	m_info = NULL;
	m_bQuit = FALSE;
	m_event = NULL;
	m_dwId = 0;
	m_dwExitCode = 0xFFFFFFFF;
}

RingThread::~RingThread()
{
	//等待30秒给线程自动退出，否则强行退出。	
	Quit();
	if(m_event)
		CloseHandle(m_event);
}
	
BOOL RingThread::Start(THREADPROC proc,LONG param)
{
	if(m_handle)
		return TRUE;	//已有线程运行
	if(proc)
		m_handle = NewThread((THREADPROC)proc,(LPVOID)param);
	return (BOOL)m_handle;
}

BOOL RingThread::CanContinue(DWORD dwMilSec/* = INFINITE*/)
{
	if(m_event == NULL)
		m_event = CreateEvent(NULL,TRUE,TRUE,NULL);

	return (WAIT_OBJECT_0 == WaitForSingleObject(m_event,dwMilSec));
}

void RingThread::Continue()
{
	m_crl.Enter();
	
	m_nEvCnt ++;
	
	if(m_event)
		SetEvent(m_event);
	else
		m_event = CreateEvent(NULL,TRUE,TRUE,NULL);

	m_crl.Leave();
}

void RingThread::Wait(BOOL bForceWait/*=FALSE*/)
{
	m_crl.Enter();

	if(bForceWait)
		m_nEvCnt = 0;
	else
		m_nEvCnt --;
	
	if(m_nEvCnt <= 0)
	{
		m_nEvCnt = 0;
		
		if(m_event)
			ResetEvent(m_event);
		else
			m_event = CreateEvent(NULL,TRUE,FALSE,NULL);
	}

	m_crl.Leave();
}

void RingThread::Pause()
{
	if(m_handle)
		SuspendThread(m_handle);
}

void RingThread::Resume()
{
	if(m_handle)
		ResumeThread(m_handle);
}

BOOL RingThread::SetPriority(int nPriority)
{
	if(m_handle)
		return SetThreadPriority(m_handle,nPriority);
	else
		return FALSE;
}

BOOL RingThread::PostMessage(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return PostThreadMessage(m_dwId,uMsg,wParam,lParam);
}

void RingThread::Quit(int nWaitSecond/*=30*/)
{
	if(m_handle == NULL)
		return;

	m_nWaitSecond = nWaitSecond;
	WaitQuit();
}

void RingThread::ThreadQuit(int nWaitSecond/*=30*/)
{
	if(m_bQuit || m_handle == NULL)	//m_bQuit = TRUE表示该退出线程已启动
		return;

	//启动退出线程，如果不创建退出线程，一旦退出代码在需要退出的线程中调用，会导致
	//暂时死锁，延续至等待线程退出的时间。
	m_nWaitSecond = nWaitSecond;
	HANDLE handle = NewThread((THREADPROC)QuitProc,this);
	CloseHandle(handle);
}

DWORD RingThread::NotifyThreadQuit()
{
	m_crl.Enter();
	try
	{
		if(m_handle)
			CloseHandle(m_handle);

		//重置参数，以备下一次启动	
		m_handle = NULL;
		delete m_info;
		m_info = NULL;
		m_bQuit = FALSE;
		m_nEvCnt = 0;
		
		if(m_event)
			SetEvent(m_event);
	}
	catch(...)
	{
	}
	m_crl.Leave();
	return m_dwExitCode;
}

DWORD WINAPI RingThread::ThreadProc(RingThInfo* obj)
{	
	if(obj)
	{
		try
		{
			obj->m_thread->m_dwId = GetCurrentThreadId();
			obj->m_thread->m_dwExitCode = 0xFFFFFFFF;
			try
			{
				obj->m_thread->m_dwExitCode = (obj->m_obj->*(obj->m_lpthreadFunc))(obj->m_param);
			}
			catch(...)
			{
			}
			return obj->m_thread->NotifyThreadQuit();			
		}
		catch(...)
		{
		}
	}
	return 0;
}

DWORD WINAPI RingThread::QuitProc(RingThread* obj)
{
	if(obj)
		return obj->WaitQuit();
	return 0;
}

DWORD RingThread::GetExitCode()
{
	if(m_handle == NULL)
		return m_dwExitCode;
	else
	{
		DWORD Code;
		if(GetExitCodeThread(m_handle,&Code))
			return Code;
		else
			return 0xFFFFFFFF;
	}	
}

DWORD RingThread::WaitQuit()
{
	try
	{
		DWORD code = STILL_ACTIVE;
		int cnt = 0;
		int max = m_nWaitSecond * 1000;	//毫秒
		//BOOL bAlive;

		do
		{
			if(code != STILL_ACTIVE)
				break;

			m_bQuit = TRUE;
			
			if(m_event)
				SetEvent(m_event);

			Sleep(1);
			cnt ++;
			if(cnt > max)
			{
				TerminateThread(m_handle,0xFFFFFFFF);
				break;
			}
			//m_crl.Enter();
			//bAlive = (m_handle && GetExitCodeThread(m_handle,&code));
			//m_crl.Leave();
		}while(m_handle && GetExitCodeThread(m_handle,&code));
		
		return NotifyThreadQuit();		
	}
	catch(...)
	{
	}
	return 0;
}
