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
//原文件名：ringchecklist.h
//
//本文件说明：RingProgress类声明
//
//类库功能：进度条控件封装。
//
**********************************************************************/
#ifndef _INC_RINGPROGRESS
#define _INC_RINGPROGRESS

class RingProgress:public RingControls
{
public:
	RingProgress(){InitCommonControls();}
	virtual ~RingProgress(){}

	LPCTSTR WndClassName(){return PROGRESS_CLASS;}

	//虚函数重载
	int GetInt(int index=INDEX_CURSEL){return GetPos();}
	BOOL SetValue(int val,int index=-1){return SetProgress(val);}
	BOOL SetBkColor(COLORREF crColor){SendMessage(m_hWnd,PBM_SETBKCOLOR,0,crColor);return TRUE;}
	BOOL Clear(){SetProgress(0);return TRUE;}

	//消息封装
	int DeltaPos(int nIncrement){return SendMessage(m_hWnd,PBM_DELTAPOS,nIncrement,0);}
	int GetPos(){return SendMessage(m_hWnd,PBM_GETPOS,0,0);}
	int GetRange(BOOL bLowLimit,PPBRANGE ppb=NULL);
	COLORREF SetBarColor(COLORREF crColor){return SendMessage(m_hWnd,PBM_SETBARCOLOR,0,crColor);}
	int SetProgress(int nPos){return SendMessage(m_hWnd,PBM_SETPOS,nPos,0);}
	DWORD SetRange(int nMin,int nMax){return SendMessage(m_hWnd,PBM_SETRANGE,0,MAKELONG(nMin,nMax));}
	DWORD SetRange32(int nMin,int nMax){return SendMessage(m_hWnd,PBM_SETRANGE,nMin,nMax);}
	int SetStep(int nStep){return SendMessage(m_hWnd,PBM_SETSTEP,nStep,0);}
	int StepIt(){return SendMessage(m_hWnd,PBM_STEPIT,0,0);}

};

#endif