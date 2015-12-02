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
#define IDC_NAVBAR				12003
#define IDC_NAVPANE				12004
#define IDC_PROGRESS				12005
#define CM_UPPATH					13000

//查看工具
#define CM_ZOOMIN					12010
#define CM_ZOOMOUT				12011
#define CM_NEXTPIC				12012
#define CM_PREVPIC				12013
//文件导航工具
#define CM_FILTER					12020
#define CM_SORTER					12021
#define CM_MODE					12022
//文件过滤标志
#define FILTER_FOLDER         FILE_ATTRIBUTE_DIRECTORY
#define FILTER_ALBUM          0x10000000
#define FILTER_PIC            0x20000000
#define FILTER_MEDIA          0x40000000
#define FILTER_OTHER          0x80000000
#define FILTER_HIDDEN			FILE_ATTRIBUTE_HIDDEN

#define THUMB_MASK				0x10000000
#define SIZE_MASK					0x0FFFFFFF

//自定义控件
class RingTabEx:public RingTab
{
public:
	RingTabEx(){}
	~RingTabEx(){}

protected:
	//重载虚函数，使背景与主窗口一致
	BOOL OnDrawBkg(HDC);
};

//主窗口设定,普通窗口
BEGIN_SETUPMAINWINDOW(IS_WINDOW)
	BEGIN_DEF_VAR(NO_INIT)
		RingStatusBar* m_statbar;
		RingDirTree* m_dirtree;
		RingListView* m_fileview;
		RingTab* m_navbar;
		RingTabEx *m_tab;
		RingProgress* m_progress;
		int m_nMarkCol,m_nOrder,m_nMarkUp,m_nMarkDown;
		DWORD m_dwFilter;
		HICON m_iconFile;
	END_DEF_VAR
	
	void ListFile();
	void SetStatusFilePart(LPCTSTR lpszFile);

	BEGIN_DEFMSG
		DEFMSG(WM_CREATE)
		DEFMSG(WM_DESTROY)
		DEFMSG(WM_DRAWITEM)
		DEFMSG(WM_MENUSELECT)
		DEFMSG(WM_ACTIVATEAPP)
		DEFNOTIFY(TVN_SELCHANGED)
		DEFNOTIFY(TCN_SELCHANGE)		
		DEFCMD(OnOpen)
		DEFCMD(OnExit)
		DEFCMD(OnHelp)
		DEFCMD(OnSetViewMode)
		DEFCMD(OnSetFileFilter)
		DEFCMD(OnSetColumn)
		DEFCMD(OnSortColumn)
	END_DEFMAINMSG
END_SETUPMAINWINDOW

#define VIEW_FITSIZE		9999	//图象适应窗口显示

//预览子窗口设定
BEGIN_SETUPWINDOW(child,IS_DIALOG)
	BINDID(IDD_ABOUT)					//绑定对话框资源ID
	BEGIN_DEF_VAR(NO_INIT)
		RingDIB m_dib;
		BOOL m_bFullScreen;
		RECT m_rcFloat;
		int m_nRadio;		//放大/缩小倍率
		POINT m_ptMouse;
		HCURSOR m_hCursor,m_hCurHand,m_hCurGrab,m_hCurZoom,m_hCurArr;
	END_DEF_VAR

	void SetCursor(HCURSOR hCur = NULL);
	BOOL View(LPCTSTR lpszPic);
	void SetFullScreen(BOOL bFullScreen);
	void UpdatePicShow();
	int CalcRadio();
	void SetImgDest();
	BOOL MoveImg(int nDir);
	void MoveImg(int nOffX,int nOffY);
	void ZoomIn();
	void ZoomOut();
	void ViewNextImg(BOOL bNext=TRUE);

	BEGIN_DEFMSG
		DEFMSG(WM_INITDIALOG)
		DEFMSG(WM_PAINT)
		DEFMSG(WM_ERASEBKGND)
		DEFMSG(WM_SIZE)
		DEFMSG(WM_LBUTTONDOWN)
		DEFMSG(WM_MOUSEMOVE)
		DEFMSG(WM_LBUTTONUP)
		DEFMSG(WM_KEYDOWN)
		DEFMSG(WM_LBUTTONDBLCLK)
	END_DEFMSG(child)
END_SETUPWINDOW(child)

//导航子窗口设定
BEGIN_SETUPWINDOW(NavPane,IS_DIALOG)
	BINDID(IDD_ABOUT)					//绑定对话框资源ID
	BEGIN_DEF_VAR(NO_INIT)
		RingToolBar* m_toolbar;
		RingToolBar* m_navtool;
		RingListView* m_fileview;
		RingTab* m_tab;
		int m_nViewMode,m_nFileMode;
		int m_nMarkCol,m_nOrder,m_nMarkUp,m_nMarkDown;
		RECT m_RectViewer;
		int m_stateViewer,m_nBarBorder;		
		RingImageList* m_img;
		HIMAGELIST m_hSysImgL,m_hSysImgS,m_hHeaderImg;
		RingThread* m_thread;
		RingDataGuard m_guard;
		//HBRUSH m_br1,m_br2,m_brline,m_brsel;
		//COLORREF m_crSel;
	END_DEF_VAR

	void SetViewMode(int iFlag);
	void SetFullScreen(BOOL bFullScreen);
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	static int CALLBACK SortFileProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	DWORD ViewThumbProc(LONG);
	void UpdateFileList();

	BEGIN_DEFMSG
		DEFMSG(WM_INITDIALOG)
		DEFMSG(WM_SIZE)
		DEFMSG(WM_DESTROY)
		DEFNOTIFY(NM_DBLCLK)
		DEFNOTIFY(LVN_COLUMNCLICK)
		DEFNOTIFY(LVN_ITEMCHANGED)
		DEFNOTIFY(TBN_DROPDOWN)
		DEFNOTIFY(NM_RETURN)
		DEFNOTIFY(LVN_SHOWCOLUMN)
		DEFCMD(OnUpParentPath)
		DEFCMD(OnZoomIn)
		DEFCMD(OnZoomOut)
		DEFCMD(OnNextImg)
		DEFCMD(OnPrevImg)
		DEFCMD(OnSetFileFilter)
		DEFCMD(OnSortColumn)
		DEFCMD(OnSetViewMode)
	END_DEFMSG(NavPane)
END_SETUPWINDOW(NavPane)

//版权信息对话框设定
BEGIN_SETUPWINDOW(DlgAbout,IS_DIALOG)
	BINDID(IDD_ABOUT)					//绑定对话框资源ID
	BEGIN_DEF_VAR(NO_INIT)		
		RingDIB* m_dib,*m_ripple;
		short *m_buf1,*m_buf2;
		RingThread* m_thread;
	END_DEF_VAR

	DWORD RippleThread(LONG);
	void RippleSpread();
	void RenderRipple();
	void DropStone(int x,int y,int stonesize,int stoneweight);

	BEGIN_DEFMSG
		DEFMSG(WM_INITDIALOG)
		DEFMSG(WM_LBUTTONDOWN)
		DEFMSG(WM_MOUSEMOVE)
		DEFMSG(WM_CLOSE)
	END_DEFMSG(DlgAbout)
END_SETUPWINDOW(DlgAbout)

#endif
