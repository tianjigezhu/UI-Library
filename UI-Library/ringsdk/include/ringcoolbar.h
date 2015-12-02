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
//原文件名：ringcoolbar.h
//
//本文件说明：ReBar类定义
//
**********************************************************************/
#ifndef _INC_RINGCOOLBAR
#define _INC_RINGCOOLBAR

class RingCoolBar:public RingControls
{
public:
	RingCoolBar();
   virtual ~RingCoolBar();
	
	LPCTSTR WndClassName(){return REBARCLASSNAME;}

	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	
	BOOL AddBand(RingCtrlWnd* child);
   
protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	
	LRESULT OnLButtonDown(RINGPARAMS& param);
	LRESULT OnMouseMove(RINGPARAMS& param);
	LRESULT OnLButtonUp(RINGPARAMS& param);
	
	BOOL OnParentNotify(HWND,UINT,UINT,LONG);
	
private:
	void init();
	
protected:   
   int m_State;
};

#endif
