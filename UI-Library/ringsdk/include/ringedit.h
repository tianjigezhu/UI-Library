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
//原文件名：ringedit.h
//
//本文件说明：RingEdit类声明
//
//类库功能：Edit控件封装
//
**********************************************************************/
#ifndef _INC_RINGEDIT
#define _INC_RINGEDIT

class RingEditDrawer:public RingOwnerDrawer
{
public:
	RingEditDrawer(){};
	virtual ~RingEditDrawer(){};
	
	virtual void OnNcPaint(HDC,LPRECT){}
	virtual void OnMouseHover(HDC,LPRECT){}
	virtual void OnMouseLeave(HDC,LPRECT){}
	virtual void OnSetFocus(HDC,LPRECT){}
	virtual void OnKillFocus(HDC,LPRECT){}
};

class RingEdit:public RingControls
{
public:
	RingEdit();
   ~RingEdit();
	
   void LimitText(int nChars = 0);
   void InsertLine(LPCTSTR lpText,BOOL bFirstLine=FALSE);
	
	//void SetOwnerDraw(){};
	
	BOOL SetReadOnly(BOOL bReadOnly);
	BOOL IsReadOnly();

	LPCTSTR WndClassName(){return "Edit";}

	//消息封装
	void SetPasswordChar(char c){SendMessage(m_hWnd,EM_SETPASSWORDCHAR,(WPARAM)c,0);}
	void SetSel(int start=0,int end=-1){SendMessage(m_hWnd,EM_SETSEL,start,end);}
	DWORD GetSel(LPDWORD lpStart,LPDWORD lpEnd)
			{return SendMessage(m_hWnd,EM_GETSEL,(WPARAM)lpStart,(LPARAM)lpEnd);}
	void Copy(){SendMessage(m_hWnd,WM_COPY,0,0);}
	void Cut(){SendMessage(m_hWnd,WM_CUT,0,0);}
	BOOL DelCurSel(){SendMessage(m_hWnd,WM_CLEAR,0,0);return TRUE;}
	BOOL Clear(){return SetValue("");}
	void Paste(){SendMessage(m_hWnd,WM_PASTE,0,0);InvalidateRect(m_hWnd,NULL,TRUE);}

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	//void OnParentNotify(HWND,UINT,UINT,LONG);
	LRESULT OnBkColor(UINT,HDC);
		
//private:
//	CAN_OWNER_DRAW();	
};
#endif
