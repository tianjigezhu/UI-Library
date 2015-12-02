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
//							临风程序界面类库 ringdows.lib
//作者：临风
//
//版本：0.2
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringmdichild.cpp
//
//说明：对MDI子窗口的封装实现。
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdows.h"

//ringFile rfc = "child.txt";
//char cc[80];
HWND RingMDIChildWnd::Create(LPCTSTR szTitle,LPCTSTR szClass,
									  DWORD dwStyle,DWORD dwExStyle,
									  int x,int y,int width,int height,
									  HWND hParentWnd/*=NULL*/,HMENU hMenu/*=NULL*/,
									  LPARAM lParam/*=0*/)
{
   if(!Register(szClass,
		(HBRUSH)(COLOR_WINDOW + 1),
		m_windowInfo.lpszMenu,
		m_windowInfo.hIconBig,
		m_windowInfo.hCursor,
		m_windowInfo.hIconSmall,
		ITIS_MDICHILD))
		return NULL;
	
   if(m_parent->GetWindowType() == ITIS_MDIFRAME)
		((RingMDIFrameWnd*)m_parent)->CreateChild(szTitle,this,FALSE,dwStyle | WS_CHILD,
																lParam,(LPSTR)szClass);

   return m_hWnd;
}

LRESULT RingMDIChildWnd::RingdowProc(HWND hWnd,RINGPARAMS param)
{
   LRESULT res;
	//无需改变WM_CREATE的参数，InitChildProc里已改变
	if(param.uMsg == WM_MOUSEACTIVATE)//WM_ACTIVATE && param.wParam == WA_CLICKACTIVE)
	{
		res = DispMessage((RINGVCLASS*)this,m_msgs,hWnd,param);
		RingCoolMenu* rm = ((RingMDIFrameWnd*)m_parent)->GetCoolMenu();
		if(m_parent->GetMenu() == NULL && rm)
		{
			RingMDIClient* client = ((RingMDIFrameWnd*)m_parent)->GetClientObj();
			client->RefreshMenu(rm,m_hWnd);
			client->UpdateMenuBar(m_hWnd);
		}
		return res;
	}
	else if(param.uMsg == WM_SIZE)
	{
		if(param.wParam == SIZE_MAXIMIZED 
			|| param.wParam == SIZE_RESTORED 
			|| param.wParam == SIZE_MINIMIZED)
		{
			//LRESULT res = DispMessage((RINGVCLASS*)this,m_msgs,hWnd,param);
			RingCoolMenu* rm = ((RingMDIFrameWnd*)m_parent)->GetCoolMenu();
			if(m_parent->GetMenu() == NULL && rm)
			{
				RingMDIClient* client = ((RingMDIFrameWnd*)m_parent)->GetClientObj();
				client->UpdateMenuBar(m_hWnd);
			}
			//return res;
		}
	}

	NotifyChildProc(hWnd,param.uMsg,param.wParam,param.lParam);
   return DispMessage((RINGVCLASS*)this,m_msgs,hWnd,param);	
}


