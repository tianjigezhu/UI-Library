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
//原文件名：ringstatus.h
//
//本文件说明：状态栏封装
//
**********************************************************************/
#ifndef _INC_RINGSTATUS
#define _INC_RINGSTATUS
#include <commctrl.h>

//状态栏格类型
//宽度类型
#define SBPS_STATICW		0x0				//固定宽度
#define SBPS_FITTEXT		0x1				//自适应文字宽度
#define SBPS_STRETCH		0x2				//变长，适应窗口宽度
//#define SBPS_MENUHELP	0x3				//变长，显示菜单帮助
//状态
#define SBPF_UPDATE		0x00010000		//需要更新文字
//显示类型
#define SBTS_BORDER		0x0				//有边框
#define SBTS_NOBORDER	0x1000			//无边框(SBT_NOBORDERS<<4)
#define SBTS_POPOUT		0x2000			//突起显示(SBT_POPOUT<<4)
#define SBTS_RTL			0x4000			//反向阅读顺序(SBT_RTLREADING<<4)

//#define SPS_TEXT			0x10		  //文字
//#define SPS_PROGRESS		0x40		  //进度栏（RINGSDK扩展）
//#define SPS_TIMER			0x80		  //时间栏（RINGSDK扩展）
//宽度类型
#define SBPW_FITTEXT		0x0FFE
#define SBPW_STRETCH		0x0FFF
//设置分隔栏时生成分隔栏信息，
//strid:要显示文字的资源ID，
//type:必须是SBTS_XXX值之一
//width:分隔栏宽度，不可超过4095，或为SBPW_XXX值之一
#define MAKESBPINT(strid,type,width) ((strid << 16) | type | (width&SBPW_STRETCH))
//取MAKESBPINT的type参数，返回以下值之一：SBT_NOBORDERS，SBT_POPOUT，SBT_RTLREADING
#define SBPWSTYLE(val) ((val & 0xF000) >> 4)

typedef struct _tagRingStatusPart
{
	int m_nIndex;
	UINT m_uStyle;
	int m_nWidth;
	ringStr* m_str;
	RingBaseWnd* m_child;
/*	union
	{
		int m_nStep;
		RingThread* m_Timer;
	};
*/
}RINGSTATUSPANE,*LPRINGSTATUSPANE;

class RingStatusBar:public RingControls
{
public:
	RingStatusBar();
	virtual ~RingStatusBar();
	
	BOOL Create(DWORD dwStyle = SBARS_SIZEGRIP);

	LPCTSTR WndClassName(){return STATUSCLASSNAME;}

	BOOL SetParts(int parts,LPINT lpArr = NULL);
	UINT GetPartStyle(int nPart);
	//BOOL SetPartStyle(int index,UINT uStyle,LPCTSTR lpszText=NULL,int nWidth=0);
	//BOOL SetTimerFormat(int index,LPCTSTR lpszFormat);
	//BOOL SetProgress(int index,int nStep);	
	virtual BOOL SetValue(int nPart,LPCTSTR lpszVal,int uStyle=-1,int nWidth=0);
	int OnMenuHelp(RINGPARAMS &param,HMENU hMenu);
	BOOL InsertItem(RingBaseWnd* wnd,int iPart);

	//重载的虚函数
	BOOL SetValue(int val,int index=INDEX_CURSEL);
	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);
	BOOL SetBkColor(COLORREF crColor){return ::SendMessage(m_hWnd,SB_SETBKCOLOR,0,(LPARAM)crColor);}

	int GetPartWidth(int nPart);

	//消息封装
	int GetParts(int nPart,LPINT lpCoord=NULL)
	{return lpCoord==NULL?m_nPartCount:(::SendMessage(m_hWnd,SB_GETPARTS,nPart,(LPARAM)lpCoord));}
	HICON GetIcon(int nPart){return (HICON)::SendMessage(m_hWnd,SB_GETICON,nPart,0);}
	BOOL SetIcon(int nPart,HICON hIcon)
	{return ::SendMessage(m_hWnd,SB_SETICON,nPart,(LPARAM)hIcon);}
	BOOL GetBorders(LPINT lpInt){return ::SendMessage(m_hWnd,SB_GETBORDERS,0,(LPARAM)lpInt);}
	BOOL GetRect(int nPart,LPRECT lpRect)
	{return ::SendMessage(m_hWnd,SB_GETRECT,nPart,(LPARAM)lpRect);}
	int GetTextLength(int nPart)
	{return LOWORD(::SendMessage(m_hWnd,SB_GETTEXTLENGTH,nPart,0));}
	UINT GetBorderStyle(int nPart)
	{return HIWORD(::SendMessage(m_hWnd,SB_GETTEXTLENGTH,nPart,0));}
	BOOL IsSimple(){return ::SendMessage(m_hWnd,SB_ISSIMPLE,0,0);}
	void SetSimple(BOOL bSimple){::SendMessage(m_hWnd,SB_SIMPLE,bSimple,1);}
	void ToggleSimple(){SetSimple(!IsSimple());}

	//DWORD m_TimerProc(LPRINGSTATUSPANE);
	
protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	void UpdateAllParts();
	LPRINGSTATUSPANE GetPanePtr(int index);
	int GetItemText(int index,LPTSTR lpBuf,int size);

private:	
	BOOL CreatePaneInfo(int nParts);
	void FreeAllPart();
	//BOOL SetTimerFormat(LPRINGSTATUSPANE lpPart,LPCTSTR lpszFormat);	
	BOOL SetPaneText(LPRINGSTATUSPANE lpPart,LPCTSTR lpszVal,UINT uStyle);
	//void DestroyTimerPart(LPRINGSTATUSPANE lpPart);

protected:
	RingDockBar* m_dockbar;

private:
	LPRINGSTATUSPANE m_pane;
	int m_nPartCount;
};

#endif
