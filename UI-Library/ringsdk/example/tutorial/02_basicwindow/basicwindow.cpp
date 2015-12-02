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
//							RingSDK 演示程序
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
//说明：
//
//本程序演示最简单的SDI窗口，演示如何设置窗口背景图片及显示一个包含图标
//的版权信息消息框
//
**********************************************************************/
#include "ringmain.h"
#include "resource.h"

//主窗口设置
BEGIN_SETUPMAINWINDOW(IS_WINDOW)
	BEGIN_DEFMSG
		//定义命令处理函数
		DEFCMD(OnExit)
		DEFCMD(OnHelp)
		//定义消息处理函数
		DEFMSG(WM_CREATE)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

//主窗口消息处理关联
BEGIN_REGMAINMSG
	REGMSG(WM_CREATE)
	//关联命令ID和消息处理函数
	REGCMD(CM_EXIT,OnExit)
	REGCMD(CM_ABOUT,OnHelp)
END_REGMSG

//类库主函数，相当于WinMain
int RingMain()
{
	//创建主窗口，参数：窗口标题，窗口类名，窗口类型，菜单资源
	APP_CREATEWINDOW("RingSDK教程二","RingSDKApp",WS_OVERLAPPEDWINDOW,MAKEINTRESOURCE(IDM_MENU));
   //进入消息循环，RingApp为类库内置对象
	return RingApp.Run();
}

//消息处理函数
RINGMAINMSG(WM_CREATE)
{
	//设置窗口图标
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_APP)));
	//设置窗口背景图片（背景图片为平铺显示）
	SetBkgBitmap(IDB_BITMAP1);
	//返回默认处理，param为本函数的参数，RINGMAINMSG(WM_CREATE)宏展开为：
	//LRESULT RingWnd_window::On_WM_CREATE(HWND hWnd,RINGPARAMS& param)
	return DefaultProc(param);
}

//命令处理函数，无返回值
RINGMAINCMD(OnExit)
{
	SendExit();   
}

RINGMAINCMD(OnHelp)
{
	//显示带图标版权信息消息框
	ShowHelpInfo("RingSDK 演示程序\n\n(C)Copyright 2007     ","RingSDK教程二",MAKEINTRESOURCE(IDI_APP));
}
