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
//							RingSDK类库 ringmain.lib
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
//原文件名：ringdowapp.cpp
//
//说明：对WinMain的封装实现，程序界面库将基于本类库
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringmain.h"

RingdowApp RingApp;

IDLEPROC RingdowApp::OnIdleProc = (IDLEPROC)RingdowApp::OnIdle;
int RingdowApp::m_CmdShow = SW_SHOW;
LPSTR RingdowApp::m_szCommand = NULL;
HACCEL RingdowApp::m_hAccel = NULL;
RingBaseWnd* RingdowApp::m_window = NULL;
int RingdowApp::m_Result = 0;

RingdowApp::~RingdowApp()
{
	try
	{
		delete m_window;
		m_window = NULL;
	}
	catch(...)
	{
	}
}

BOOL RingdowApp::SetAccelTable(LPCTSTR lpTableName)
{
	m_hAccel = LoadAccelerators(GetInstance(),lpTableName);
   return (BOOL)m_hAccel;
}

int RingdowApp::Run(BOOL bShow)
{
   if(m_window)
   {
      if(m_window->m_windowType == ITIS_DIALOG)
			return m_Result;
		else
		{
			MSG msg;
			HWND hwndMDIClient = NULL;
			int nRet;
			
			if(bShow)
				ShowWindow(m_window->m_hWnd,m_CmdShow);
			else
				ShowWindow(m_window->m_hWnd,SW_HIDE);
			
			UpdateWindow(m_window->m_hWnd);
			
			if(m_window->m_windowType == ITIS_MDIFRAME)
			{
				hwndMDIClient = ((RingMDIFrameWnd*)m_window)->GetClient();
				((RingMDIFrameWnd*)m_window)->SetMonExtClientWnd(&hwndMDIClient);
			}

			while(TRUE)
			{
      		if(!PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
      			OnIdleProc();
            
            nRet = GetMessage(&msg,NULL,0,0);
				if(nRet && nRet != -1)
				{				
					if((hwndMDIClient == NULL || !TranslateMDISysAccel(hwndMDIClient, &msg))
						&&	!TranslateAccelerator(msg.hwnd,m_hAccel,&msg))
	        		{
   	        		TranslateMessage(&msg);  // Translates virtual key codes
      				DispatchMessage(&msg);   // Dispatches message to window
  		   		}
				}
   			else
					return msg.wParam;
			}
		}
   }
   else
   	return 0;
}

LRESULT RingdowApp::OnIdle(void)
{
   return 0;
}

void RingdowApp::SetIdleProc(IDLEPROC IdleProc)
{
	OnIdleProc = IdleProc;
}

