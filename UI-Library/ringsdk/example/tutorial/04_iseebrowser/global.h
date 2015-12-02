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
//说明：程序头文件
//
//本程序在上一个演示程序基础上演示：
//		创建XP样式的菜单
//		创建侧边栏
//		创建文件浏览树
//		ListView控件使用演示
//		帮助信息对话框演示
//
**********************************************************************/
#ifndef _INC_GLOBAL_H
#define _INC_GLOBAL_H
#include "ringmain.h"
#include "ringdib.h"
#include "resource.h"

#define IDC_FOLDERTREE			12001
#define IDC_FILEVIEW				12002

//MDIFrame主窗口设定
BEGIN_SETUPMAINWINDOW(IS_MDIFRAME)
	BEGIN_DEF_VAR(NO_INIT)
		RingStatusBar* m_statbar;
		RingDirTree* m_dirtree;
		RingListView* m_fileview;
	END_DEF_VAR
	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_DESTROY)
		DEFMSG(WM_MENUSELECT)
		DEFNOTIFY(TVN_SELCHANGED)
		DEFNOTIFY(NM_DBLCLK)
		DEFCMD(OnOpen)
		DEFCMD(OnExit)
		DEFCMD(OnHelp)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

//MDI子窗口设定
BEGIN_SETUPWINDOW(child,IS_MDICHILD)
	BEGIN_DEF_VAR(NO_INIT)
		RingDIB m_dib;
	END_DEF_VAR

	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_PAINT)
		DEFMSG(WM_ERASEBKGND)
		DEFMSG(WM_SIZE)		
	END_DEFMSG(child)
END_SETUPWINDOW(child)
#endif