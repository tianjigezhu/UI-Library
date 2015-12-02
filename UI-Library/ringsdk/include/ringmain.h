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
//原文件名：ringmain.h
//
//说明：对WinMain的封装，程序界面库将基于本类库，包含本文件无须实现
//      WinMain，主函数为RingMain，必须自己实现。
//
**********************************************************************/
#ifndef _INC_RINGMAIN
#define _INC_RINGMAIN
#include "ringdows.h"

class RingdowApp
{
public:
	RingdowApp():m_bIsService(FALSE){}
	~RingdowApp();

   static BOOL SetAccelTable(LPCTSTR lpTableName);
   static int Run(BOOL bShow=TRUE);

   static void SetIdleProc(IDLEPROC IdleProc);
   static LRESULT OnIdle(void);

	BOOL IsService(){return m_bIsService;}
	int GetShowCmd(){return m_CmdShow;}
	void SetShowCmd(int nCmd){m_CmdShow = nCmd;}
	LPSTR GetCmdLine(){return m_szCommand;}

private:
   static IDLEPROC OnIdleProc;
	BOOL m_bIsService;
	static int m_CmdShow;
	friend class RingService;

public:
   static LPSTR m_szCommand;				//程序接收的启动命令行
	static HACCEL m_hAccel;
   static RingBaseWnd* m_window;
   static int m_Result;
};

#define IS_WINDOW		RingWnd
#define IS_MDIFRAME	RingMDIFrameWnd
#define IS_MDICHILD	RingMDIChildWnd
#define IS_DIALOG		RingDialog
#define IS_PROPPAGE	RingPropPage
#define IS_PROPSHEET RingPropSheet

//m_windowType+=ITIS_MAINWINDOW:标记为主窗口,OnCreate里会检测并还原原始窗口类型
#define BEGIN_SETUPMAINWINDOW(baseclass)	\
class RingWnd_window:public baseclass\
{\
public:\
	RingWnd_window(RingBaseWnd* parent)\
   {\
   	SetParentObj(parent);\
      m_msgs = (RingMsgs*)RingSDK_winmsg;\
      m_windowType += ITIS_MAINWINDOW;\
		InitRingSDKWindow();\
   }\

#define BEGIN_DEFMAINMSG
#define END_DEFMAINMSG	END_DEFMSG(window)

#define BEGIN_REGMAINMSG	\
RingWnd_window* window = NULL;\
RingdowMsg<RingWnd_window> RingWnd_window::RingSDK_winmsg[] = {

//这里不能define为RINGXXX(window,notify)，因为如果
//RINGMAINNOTIFY(NM_DBLCLK),NM_DBLCLK已定义，转到
//RINGNOTIFY(window,notify)时NM_DBLCLK被转换掉了
#define RINGMAINMSG(msgname) LRESULT RingWnd_window::On_##msgname(HWND hWnd,RINGPARAMS& param)
#define RINGMAINMSGEX(msgname) LRESULT RingWnd_window::msgname(HWND hWnd,RINGPARAMS& param)
#define RINGMAINCMD(funcname) void RingWnd_window::funcname(UINT event)
#define RINGMAINNOTIFY(notify)	LRESULT RingWnd_window::On_##notify(HWND hWnd,RINGPARAMS& param)

#define RINGMAINCLASS			RINGCLASS(window)
#define RINGMAINMETHOD(func)	RINGMETHOD(window,func)
#define RINGMAINVAR				RINGVAR(window)
#define END_SETUPMAINWINDOW	END_SETUPWINDOW(window)

#define INITMAINWINDOW  INITWINDOW(window)

#define NEW_MAINWINDOW()	\
try\
{\
	if(window == NULL)\
		window = new RingWnd_window(NULL);\
	RingdowApp::m_window = window;\
}\
catch(...)\
{\
	abort();\
}

#define APP_CREATEWINDOW(szTitle,szClass,dwStyle,lpszMenu)	\
NEW_MAINWINDOW()\
window->SetMenu(lpszMenu);\
InitRingLib(window->Create(szTitle,szClass,dwStyle),GetInstance())

#define APP_CREATEWINDOWEX(szTitle,szClass,dwStyle,dwExStyle,lpszMenu)	\
NEW_MAINWINDOW()\
window->SetMenu(lpszMenu);\
InitRingLib(window->Create(szTitle,szClass,dwStyle,dwExStyle),GetInstance())

#define APP_CREATEDIALOG(szTemplate)	\
NEW_MAINWINDOW()\
RingdowApp::m_Result = window->DoModal(szTemplate);

//#define REG_CHILDWND(szClass,dwStyle,dwExStyle)	RingWnd::RegClass(szClass,dwStyle,dwExStyle)

extern RingdowApp RingApp;

#ifndef MAKE_SELF_LIB
#ifdef _DEBUG
#pragma comment(lib, "ringmaind.lib")
#else
#pragma comment(lib, "ringmain.lib")
#endif
#endif

#endif
