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
//原文件名：ringstatic.h
//
//本文件说明：RingStatic类声明
//
//类库功能：静态控件封装
//
**********************************************************************/
#ifndef _INC_RINGSTATIC
#define _INC_RINGSTATIC

class RingStatic:public RingControls
{
public:
	RingStatic();
	~RingStatic();
	
   LPCTSTR WndClassName(){return "Static";}
	void SetHyperlink(COLORREF crNormal = 0x00FF0000,COLORREF crHover = 0x00FF0000);
	void SetHyperStyle(UINT styNormal=0,UINT styHover=RINGFONT_UNDERLINE);
	void SetTransparent(){m_brush = (HBRUSH)GetStockObject(NULL_BRUSH);}
	BOOL SizeFitText();

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	//void OnParentNotify(HWND,UINT,UINT,LONG);
	LRESULT OnBkColor(UINT,HDC);

protected:
	COLORREF m_crNormal,m_crHover;
	ringFont m_fontHover;
	HCURSOR m_cursor;
};

#endif
