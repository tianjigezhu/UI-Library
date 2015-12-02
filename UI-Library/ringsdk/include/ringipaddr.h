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
//原文件名：ringipaddr.h
//
//本文件说明：RingIPAddr类声明
//
//类库功能：IP地址控件封装
//
**********************************************************************/
#ifndef _INC_RINGIPADDR
#define _INC_RINGIPADDR
#include <commctrl.h>

class RingIPAddr:public RingControls
{
public:
	RingIPAddr();
	virtual ~RingIPAddr(){}
	
   BOOL Create(UINT uId,int x,int y,int width,int height);
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);

	LPCTSTR WndClassName(){return WC_IPADDRESS;}

	//读取,设置数据
	int GetInt(int index=-1);
	//本函数不可重载，其内部调用GetItemText，因此派生类可重载GetItemText改变该函数行为。
	BOOL SetValue(int val,int index=-1){SendMessage(m_hWnd,IPM_SETADDRESS,0,(DWORD)val);return TRUE;}
	BOOL SetValue(LPCTSTR lpszVal,int index=-1);
	BOOL Clear(){SendMessage(m_hWnd,IPM_CLEARADDRESS,0,0);return TRUE;}
	BOOL IsBlank(){return SendMessage(m_hWnd,IPM_ISBLANK,0,0);}
	BOOL SetRange(int nField,BYTE bLow,BYTE bHigh)
	{return SendMessage(m_hWnd,IPM_SETRANGE,nField,MAKEIPRANGE(bLow,bHigh));}

protected:
	int GetItemText(int index,LPTSTR lpBuf,int size);
};

#endif
