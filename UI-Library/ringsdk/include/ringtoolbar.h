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
//原文件名：ringtoolbar.h
//
//本文件说明：工具栏定义
//
**********************************************************************/
#ifndef _INC_RINGTOOLBAR
#define _INC_RINGTOOLBAR
#include <commctrl.h>

#define SET_RINGTOOLTIP(lparam,str)	((LPTOOLTIPTEXT)lparam)->lpszText = str
#define BARTOOLY (GetSystemMetrics(SM_CYCAPTION))// + GetSystemMetrics(SM_CYBORDER))
#define BARTOOLX (GetSystemMetrics(SM_CXBORDER)*2)

#ifndef RT_TOOLBAR
#define RT_TOOLBAR	MAKEINTRESOURCE(241)
#endif

class RingCoolBar;

////////////////////////////////////////////////////////////////
//
//工具栏使用IMAGELIST显示按钮图象，因此一些有关BITMAP的消息和操作
//均已在内部处理转换为IMAGELIST的操作，对调用者透明
//
////////////////////////////////////////////////////////////////
class RingToolBar:public RingControls
{
public:
	RingToolBar();
	virtual ~RingToolBar();
	
	//注意：以下Create要用到的BITMAP只能在本模块资源中
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	virtual BOOL Create(int idBitmap,DWORD dwStyle=0,DWORD dwExStyle=0);

	LPCTSTR WndClassName(){return TOOLBARCLASSNAME;}
		
	void SetExStyle(DWORD dwExStyle);
	RingDockBar* EnableDock(LPCTSTR szTitle,UINT uDockStyle = TBS_EX_DOCKABLE,
		UINT uDockFlag = TBS_FLAG_DOCKTOP,LPRECT lprcPos = NULL);

	BOOL LoadToolBar(LPCTSTR lpszResourceName,DWORD dwStyle=0,DWORD dwExStyle=0,HINSTANCE hInst=GetInstance());
	BOOL LoadToolBar(UINT uid,DWORD dwStyle=0,DWORD dwExStyle=0,HINSTANCE hInst=GetInstance());

	virtual void WrapToolBar(int nRows,LPRECT lprc);
	
	BOOL AddButtons(int num,LPCTBBUTTON lptb);
   BOOL InsertButton(int index,LPCTBBUTTON lptb);
   BOOL InsertButton(int index,int iBitmap,int idCmd,
		LPCTSTR szText = NULL,
		BYTE btStyle = TBSTYLE_BUTTON,
		BYTE btState = TBSTATE_ENABLED,
		DWORD dwData = 0);
	BOOL InsertText(int index,LPCTSTR szText);
	BOOL InsertSep(int index,int width = 0,int id = 0);
	//插入控件，如控件未创建，则创建控件
	RingControls* InsertItem(int index,RingControls* item,DWORD dwStyle=0,UINT uId=0,int width=0,int height=0);
	//得到控件对象
	RingControls* GetContrlolItem(UINT uId);

	int AddImage(UINT id,int cx=16,int cy=15,COLORREF crMask=CLR_DEFAULT,HINSTANCE hInst=GetInstance());
	int AddImage(HBITMAP hbm,int cx=16,int cy=15,COLORREF crMask=CLR_DEFAULT);
	int AddHotImage(UINT id,int cx=16,int cy=15,COLORREF crMask=CLR_DEFAULT,HINSTANCE hInst=GetInstance());
	int AddHotImage(HBITMAP hbm,int cx=16,int cy=15,COLORREF crMask=CLR_DEFAULT);

	//重载虚函数
	DWORD GetStyle(){return SendMessage(m_hWnd,TB_GETSTYLE,0,0);}
	void SetStyle(DWORD dwStyle){SendMessage(m_hWnd,TB_SETSTYLE,0,dwStyle);}
	void ReplaceStyle(DWORD dwsRemove,DWORD dwsReplace);

	//消息封装
	int AddString(UINT uID,HINSTANCE hInst=GetInstance()){return SendMessage(m_hWnd,TB_ADDSTRING,(WPARAM)hInst,MAKELPARAM(uID,0));}
	int AddString(LPCTSTR str){return SendMessage(m_hWnd,TB_ADDSTRING,0,MAKELONG(str, 0));}
	int GetButtonCount(){return SendMessage(m_hWnd,TB_BUTTONCOUNT,0,0);}
	BOOL GetButtonRect(int nIndex,LPRECT pRc){return SendMessage(m_hWnd,TB_GETITEMRECT,(WPARAM)nIndex,(LPARAM)pRc);}
	DWORD GetButtonSize(){return SendMessage(m_hWnd,TB_GETBUTTONSIZE,0,0);}
	HIMAGELIST GetImageList(){return (HIMAGELIST)SendMessage(m_hWnd,TB_GETIMAGELIST,0,0);}
	HIMAGELIST GetHotImageList(){return (HIMAGELIST)SendMessage(m_hWnd,TB_GETHOTIMAGELIST,0,0);}
	BOOL SetButtonSize(int cx,int cy){return SendMessage(m_hWnd,TB_SETBUTTONSIZE,0,MAKELPARAM(cx,cy));}
	void PressButton(UINT uID,BOOL bPress){SendMessage(m_hWnd,TB_PRESSBUTTON,(WPARAM)uID,MAKELPARAM(bPress,0));}
	void EnableButton(UINT uID,BOOL bEnable=TRUE){SendMessage(m_hWnd,TB_ENABLEBUTTON,(WPARAM)uID,MAKELPARAM(bEnable,0));}
	int GetHotItem(){return SendMessage(m_hWnd,TB_GETHOTITEM,0,0);}
	UINT GetButtonState(UINT uID){return SendMessage(m_hWnd,TB_GETSTATE,(WPARAM)uID,0);}
	BOOL GetButton(int nIndex,LPTBBUTTON lptb){return SendMessage(m_hWnd,TB_GETBUTTON,nIndex,(LPARAM)lptb);}
	BOOL SetButtonInfo(int nIndex,LPTBBUTTON lptb){return SendMessage(m_hWnd,TB_SETBUTTONINFO,nIndex,(LPARAM)lptb);}
	HIMAGELIST SetImageList(HIMAGELIST himl,int nIndex=0){return (HIMAGELIST)SendMessage(m_hWnd,TB_SETIMAGELIST,nIndex,(LPARAM)himl);}
	HIMAGELIST SetHotImageList(HIMAGELIST himl){return (HIMAGELIST)SendMessage(m_hWnd,TB_SETHOTIMAGELIST,0,(LPARAM)himl);}
	BOOL CheckButton(int index,BOOL bChecked){return SendMessage(m_hWnd,TB_CHECKBUTTON,index,MAKELONG(bChecked,0));}
	BOOL IsButtonChecked(UINT uId){return SendMessage(m_hWnd,TB_ISBUTTONCHECKED,uId,0);}
	BOOL IsButtonPressed(UINT uId){return SendMessage(m_hWnd,TB_ISBUTTONPRESSED,uId,0);}
	BOOL DeleteButton(int index){return SendMessage(m_hWnd,TB_DELETEBUTTON,index,0);}
	void DeleteAllButtons();
	
protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	//LRESULT OnNotify(RINGPARAMS param);
	//LRESULT OnEraseBkg(RINGPARAMS param);
	LRESULT OnSizing(RINGPARAMS& param);
	
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);

	virtual void Update(int nOrgButtonCnt,LPRECT lprc=NULL,BOOL bUpdateFloatRect=TRUE,BOOL bSetPos=TRUE);
	virtual void CalcSize(LPSIZE lpsize=NULL);
	virtual void UpdateItems();

	void ChangeState(int state,RingDockSite* docksite=NULL);
	
	//停靠状态，响应停靠状态改变事件，返回-1：未改变，TRUE：已处理，0：未处理
	BOOL OnDockStateChange(int stateOld,int stateNew);
	BOOL OnGetDragRects(int state,LPRECT lprc);

private:
	int AddInterImage(HBITMAP hbm,int cx,int cy,COLORREF crMask,HIMAGELIST& imgInter,
							UINT uMsgGet,UINT uMsgSet,BOOL bSetButtonSize);

protected:
	//最大按钮（如加入的下拉框）的宽高及竖向，
	//横向状态的宽度，高度,浮动状态的宽高
	SIZE m_btnSize,m_BarSize,m_fSize;
	int m_nMaxRow,m_nfRows;
	RingDockBar* m_dockbar;
	HIMAGELIST m_img,m_imgHot;
};

#endif
