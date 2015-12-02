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
//							RingSDK程序界面类库 ringdows.lib
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
//原文件名：ringspinbutton.h
//
//说明：对up-down控件的封装
//
**********************************************************************/
#ifndef _INC_RINGSPINBUTTON
#define _INC_RINGSPINBUTTON

class RingSpinButton:public RingControls
{
public:
	RingSpinButton(){}
	virtual ~RingSpinButton(){}
		
	LPCTSTR WndClassName(){return UPDOWN_CLASS;}
	
	virtual BOOL Create(DWORD dwStyle,UINT uId,RingControls* objbuddy,int nUpper,int nLower,int nPos,
								int x,int y,int cx,int cy)
	{
		if(m_hWnd && IsWindow())
			return TRUE;
		
		m_hWnd = CreateUpDownControl(dwStyle,x,y,cx,cy,m_parent->Handle(),uId,GetInstance(),
							objbuddy->Handle(),nUpper,nLower,nPos);
		RingBaseWnd::Attach(m_hWnd);
		m_ID = uId;
		return (BOOL)m_hWnd;
	}
	//虚函数重载
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height)
	{
		return RingControls::Create(dwStyle,dwExStyle,szInitText,uId,x,y,width,height);
	}
	int GetInt(int index=INDEX_CURSEL)
	{
		DWORD res = ::SendMessage(m_hWnd,UDM_GETPOS,0,0);
		if(HIWORD(res) == 0)
			return LOWORD(res);
		else
			return 0;
	}
	BOOL SetValue(int val,int index=INDEX_CURSEL)
	{
		return ::SendMessage(m_hWnd,UDM_SETPOS,0,MAKELONG((short)val,0));
	}
	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL)
	{
		if(lpszVal)
			return SetValue(atoi(lpszVal));
		else
			return FALSE;
	}
		
	//消息封装
	int GetAccel(int nAccel=0,LPUDACCEL paAccels=NULL)
	{
		return ::SendMessage(m_hWnd,UDM_GETACCEL,nAccel,(LPARAM)paAccels);
	}
	BOOL SetAccel(int nAccel,LPUDACCEL paAccels)
	{
		return ::SendMessage(m_hWnd,UDM_SETACCEL,nAccel,(LPARAM)paAccels);
	}
	//10或16进制
	int GetBase(){return ::SendMessage(m_hWnd,UDM_GETBASE,0,0);}
	int SetBase(int nBase){return ::SendMessage(m_hWnd,UDM_SETBASE,nBase,0);}
	HWND GetBuddy(){return (HWND)::SendMessage(m_hWnd,UDM_GETBUDDY,0,0);}
	RingControls* GetBuddyObj();
	HWND SetBuddy(HWND hwnd){return (HWND)::SendMessage(m_hWnd,UDM_SETBUDDY,(WPARAM)hwnd,0);}
	DWORD GetRange(){return (DWORD)::SendMessage(m_hWnd,UDM_GETRANGE,0,0);}
	void SetRange(int nMin,int nMax)
	{::SendMessage(m_hWnd,UDM_SETRANGE,0,MAKELONG((short)nMax,(short)nMin));}
	void GetRange32(int* pMin,int* pMax){::SendMessage(m_hWnd,UDM_GETRANGE32,(WPARAM)pMin,(LPARAM)pMax);}
	void SetRange32(int nMin,int nMax){::SendMessage(m_hWnd,UDM_SETRANGE32,nMin,nMax);}
	int GetPos32()
	{
		BOOL bErr = FALSE;
		int nPos = ::SendMessage(m_hWnd,UDM_GETPOS32,0,(LPARAM)&bErr);
		if(!bErr)
			return nPos;
		else
			return 0;
	}
	int SetPos32(int nPos){return ::SendMessage(m_hWnd,UDM_SETPOS32,0,nPos);}

protected:
	int GetItemText(int index,LPTSTR lpBuf,int size){return 0;}
};

#endif
