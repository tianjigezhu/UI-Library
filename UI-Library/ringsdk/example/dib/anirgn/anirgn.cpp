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
//							RingSDK演示程序
//作者：临风
//
//e-mail:ringphone@sina.com
//
//原文件：anirgn.cpp
//
//演示动画GIF的显示及创建异型窗口
//
**********************************************************************/
#include "ringmain.h"
#include "ringdib.h"

BEGIN_SETUPMAINWINDOW(IS_WINDOW)
	BEGIN_DEF_VAR(NO_INIT)
		RingDIB* m_dib;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_LBUTTONDOWN)
		DEFMSG(WM_RBUTTONDOWN)
		DEFMSG(WM_DESTROY)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

BEGIN_REGMAINMSG
	REGMSG(WM_CREATE)
	REGMSG(WM_LBUTTONDOWN)
	REGMSG(WM_RBUTTONDOWN)
	REGMSG(WM_DESTROY)
END_REGMSG

int RingMain()
{
	APP_CREATEWINDOW("RingDIB演示程序","RINGSDK_DIB_DEMO",WS_POPUP,(LPCTSTR)NULL);
	return RingApp.Run();
}

RINGMAINMSG(WM_CREATE)
{
	ringStr str(RSTR_CURRPATH);
	str += "\\BALL_ANI.GIF";

	DIBREGFORMAT(GIF);
	m_dib = new RingDIB;
	m_dib->SetTarget(m_hWnd);
	if(m_dib->Load(str.string()))
		m_dib->CreateCoolWindow();

	CenterWindow();
	return DefaultProc(param);
}

RINGMAINMSG(WM_LBUTTONDOWN)
{
	EasyMove(m_hWnd);
	return 0;
}

RINGMAINMSG(WM_RBUTTONDOWN)
{
	if(Confirm("确定要退出吗？"))
		Close();
	return 0;
}

RINGMAINMSG(WM_DESTROY)
{
	delete m_dib;
	return Quit();
}
