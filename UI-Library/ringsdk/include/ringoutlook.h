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
//原文件名：ringoutlook.h
//
//本文件说明：RingOutLookBar类声明
//
//类库功能：OutlookBar控件封装
//
**********************************************************************/
#ifndef _INC_RINGOUTLOOK
#define _INC_RINGOUTLOOK

class RingOutLookWnd;

class RingOutLookBar:public RingCtrlWnd
{
public:
	RingOutLookBar();
	~RingOutLookBar();

	BOOL Create();
	BOOL AddBarItem(LPCTSTR lpszTitle,UINT uId,UINT style=0);

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);

protected:
	int m_nButtonHeight;
	RingButton* m_currBar;

};

#endif
