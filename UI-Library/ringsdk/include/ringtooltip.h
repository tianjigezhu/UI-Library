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
//原文件名：ringtooltip.h
//
//本文件说明：提示框类定义
//
**********************************************************************/
#ifndef _INC_RINGTOOLTIP
#define _INC_RINGTOOLTIP
#include <commctrl.h>

typedef struct tagRingTipText
{
	int m_id;
	LPCTSTR lpszTip;
	int m_nSize;
	tagRingTipText* m_next;
}RINGTIPTEXT,*LPRINGTIPTEXT;

class RingToolTip:public RingControls
{
public:
	RingToolTip();
	virtual ~RingToolTip();
	
	virtual BOOL Create(DWORD dwStyle=TTS_NOPREFIX|TTS_ALWAYSTIP);

	LPCTSTR WndClassName(){return TOOLTIPS_CLASS;}
	
	void SetTipStyle(DWORD dwStyle){m_dwTipStyle = dwStyle;}
	DWORD GetTipStyle(){return m_dwTipStyle;}

	BOOL AddTool(HWND hWnd,LPCTSTR lpszTip,LPRECT lprc=NULL);
	BOOL AddTool(UINT uId,LPCTSTR lpszTip,LPRECT lprc=NULL);
	LPCTSTR AddTipText(UINT uId,LPCTSTR lpszText);

	//虚函数重载
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	//这里的Attach不是Attach自身，而是指定要显示提示信息的目标窗口，可调用多次指定多个目标窗口
	BOOL Attach(HWND hWnd);
	BOOL Show(int nCmdShow=SW_SHOW){return ::SendMessage(m_hWnd,TTM_ACTIVATE,(nCmdShow==SW_SHOW),0);}
	BOOL SetPos(int x,int y,int width=-999,int height=-999,HWND hPos = NULL,UINT uFlags = SWP_NOZORDER);
	BOOL SetValue(int val,int index=INDEX_CURSEL);
	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);
	
protected:
	//LRESULT RingdowProc(HWND,RINGPARAMS);
	//LRESULT OnNotify(RINGPARAMS param);
	//LRESULT OnEraseBkg(RINGPARAMS param);
		
private:
	DWORD m_dwTipStyle;
	LPRINGTIPTEXT m_tiptext;
};

#endif
