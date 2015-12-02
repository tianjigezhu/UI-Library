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
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringtab.h
//
//本文件说明：RingTab类声明
//
//类库功能：Tab控件封装
//
**********************************************************************/
#ifndef _INC_RINGTAB
#define _INC_RINGTAB

typedef struct _tagRingTabItemData
{
	UINT uType;
	RingBaseWnd* obj;
}RINGTABITEMDATA,*LPRINGTABITEMDATA;

class RingTab:public RingControls
{
public:
	RingTab();
	virtual ~RingTab();

	//创建/关联操作
   BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);

	LPCTSTR WndClassName(){return WC_TABCONTROL;}
	void SetExStyle(DWORD dwExStyle);

	BOOL InsertTab(LPCTSTR szText,int subindex,UINT uidIcon=-1);
	//插入多个tab,szTabs格式："firstname|secondname|..."
	BOOL InsertTabs(LPCTSTR szTabs);
	BOOL DeleteTab(int iCol);
	
	int AddIcon(UINT uidIcon,HINSTANCE hInst=GetInstance());
	int AddIcon(HICON hicon);
	BOOL SetIcon(int indexTab,int indexIcon);
	
	//绑定TAB到指定窗口
	BOOL SetTabToWnd(int iCol,RingBaseWnd* wnd);

	int GetItemCount(){return TabCtrl_GetItemCount(m_hWnd);}
	//得到指定TAB绑定的数据
	LONG GetItemData(int iCol);
	int GetCurSel(){return TabCtrl_GetCurSel(m_hWnd);}
	int Select(int iCol);

	BOOL GetClientRect(LPRECT lprc);

	virtual LRESULT OnTabSelect(RINGPARAMS& param);

	//虚函数重载
	HDWP DefDeferWindowPos(HDWP,int,int,int,int,UINT uAppendFlag=0);	
	BOOL Attach(UINT uId,RingBaseWnd* parent=NULL){return RingControls::Attach(uId,parent);}
	BOOL Attach(HWND);
	RingDockBar* EnableDock(LPCTSTR szTitle,UINT uDockStyle = TBS_EX_DOCKABLE,
		UINT uDockFlag = TBS_FLAG_DOCKTOP,LPRECT lprcPos = NULL);
	RingDockBar* EnableDockSize(LPCTSTR szTitle,int nWidth,int nHeight,
		UINT uDockFlag = TBS_FLAG_DOCKLEFT,UINT uDockStyle = TBS_EX_DOCKABLE);

	//消息封装
	int GetRowCount(){return TabCtrl_GetRowCount(m_hWnd);}
	int GetCount(){return TabCtrl_GetItemCount(m_hWnd);}
	BOOL GetItemRect(int nIndex,LPRECT lprc);

protected:
	//LRESULT DefaultProc(RINGPARAMS&);
	LRESULT RingdowProc(HWND,RINGPARAMS);
	LRESULT OnDestroy(RINGPARAMS& param);
	virtual LRESULT OnPaint(RINGPARAMS& param);
	LRESULT OnDefNotify(RingBaseWnd* w,RINGPARAMS& param);
	virtual BOOL OnDrawBkg(HDC hDC);
	//释放内部绑定的itemdata
	void DelBindWndData();

	int GetItemText(int index,LPTSTR lpBuf,int size);
	
	void CheckReposChild();

protected:
	HIMAGELIST m_hImage;
	//当前显示的窗口（BindWndToTab后有效）
	RingBaseWnd* m_CurrTabWindow;
	RingDockBar* m_dockbar;
	int m_nCheckPos;
};
#endif
