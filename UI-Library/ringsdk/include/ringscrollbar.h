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
//原文件名：ringscrollbar.h
//
//本文件说明：RingScrollBar类声明
//
//类库功能：滚动条控件封装
//
**********************************************************************/
#ifndef _INC_RINGSCROLL
#define _INC_RINGSCROLL

class RingScrollBar:public RingControls
{
public:
	RingScrollBar();
   virtual ~RingScrollBar(){}
	
   LPCTSTR WndClassName(){return "SCROLLBAR";}

	//消息封装
	int GetScrollPos(){return ::GetScrollPos(m_hWnd,SB_CTL);}
	int SetScrollPos(int nPos,BOOL bRedraw=TRUE){return ::SetScrollPos(m_hWnd,SB_CTL,nPos,bRedraw);}
	BOOL GetScrollRange(LPSIZE lpSize);
	BOOL SetScrollRange(int nPosMin,int nPosMax,BOOL bRedraw=TRUE)
	{
		return ::SetScrollRange(m_hWnd,SB_CTL,nPosMin,nPosMax,bRedraw);
	}	
	BOOL EnableScrollBar(UINT wArrows){return ::EnableScrollBar(m_hWnd,SB_CTL,wArrows);}
	BOOL GetScrollInfo(LPSCROLLINFO lpsi);
	BOOL SetScrollInfo(LPSCROLLINFO lpsi,BOOL bRedraw=TRUE);
	BOOL SetScrollInfo(int nMask,int nPosMin,int nPosMax,int nPage,int nPos,int nTrackPos,
							BOOL bRedraw=TRUE);	
	int GetScrollLimit();
};

#endif
