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
//							RingSDK 演示程序(ISee图象浏览器)
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
//说明：MDI子窗口，图象显示部分代码
//
**********************************************************************/
#include "global.h"

BEGIN_REGMSG(child)
	REGMSG(WM_PAINT)
	REGMSG(WM_ERASEBKGND)
	REGMSG(WM_SIZE)
	REGMSG(WM_CREATE)
END_REGMSG

RINGMSG(child,WM_CREATE)
{
	char szPic[MAX_PATH];
	GetWindowText(m_hWnd,szPic,MAX_PATH);
	m_dib.Load(szPic);
	m_dib.SetTarget(m_hWnd);
	
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_APP)));
	return DefaultProc(param);
}

RINGMSG(child,WM_SIZE)
{
	InvalidateRect(m_hWnd,NULL,TRUE);
	return DefaultProc(param);
}

RINGMSG(child,WM_ERASEBKGND)
{
	return TRUE;
}

RINGMSG(child,WM_PAINT)
{
	int w,h;
	RECT rc,rcImage;
	PAINTSTRUCT ps;
	
	GetClientRect(&rc);
	//计算图象显示宽高
	if(rc.right < m_dib.Width())
	{
		//拉伸显示
		h = rc.right*m_dib.Height()/m_dib.Width();
		if(h > rc.bottom)
		{
			h = rc.bottom;
			w = h*m_dib.Width()/m_dib.Height();
		}
		else
			w = rc.right;
	}
	else if(rc.bottom < m_dib.Height())
	{
		//拉伸显示
		w = rc.bottom*m_dib.Width()/m_dib.Height();
		if(w > rc.right)
		{
			w = rc.right;
			h = w*m_dib.Height()/m_dib.Width();
		}
		else
			h = rc.bottom;
	}
	else
	{
		w = m_dib.Width();
		h = m_dib.Height();
	}
	rcImage.left = (rc.right - w)/2;
	rcImage.right = rcImage.left + w;
	rcImage.top = (rc.bottom - h)/2;
	rcImage.bottom = rcImage.top + h;
	
	//绘制图象
	BeginPaint(m_hWnd,&ps);
	m_dib.Draw(ps.hdc,rcImage.left,rcImage.top,0,0,w,h);
	//排除图象区域绘制背景，这样不会产生闪烁
	ExcludeClipRect(ps.hdc,rcImage.left,rcImage.top,rcImage.right,rcImage.bottom);
	FillRect(ps.hdc,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
	EndPaint(m_hWnd,&ps);
	return 0;
}
