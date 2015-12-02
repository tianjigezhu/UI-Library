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
//版本：0.1
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringpropsheet.h
//
//本文件说明：属性页类定义
//
//类库功能：属性页类定义。
//
**********************************************************************/

#ifndef _INC_RINGPROPSHEET
#define _INC_RINGPROPSHEET

//#include "prsht.h"
class RingPropPage;

class RingPropSheet:public RingDialog
{
public:
	RingPropSheet();
	~RingPropSheet();

	enum Page_AddStep {PAGE_ADDSTEP = 8};

	BOOL AddPage(RingPropPage* page,LPARAM dwInitParam=0,HINSTANCE hInst=GetInstance());
	BOOL DelPage(RingPropPage* page);
	BOOL SetActivePage(int index);
	BOOL SetActivePage(RingPropPage* page);
	
	void SetStyle();
	void SetWizardButtons(DWORD dwFlags);
	BOOL SetWizardMode();

	HWND Create(LPCTSTR lpszTitle=NULL,LPARAM dwInitParam = 0);
	int DoModal(LPCTSTR lpszTitle=NULL,LPARAM dwInitParam = 0);

protected:
	virtual LRESULT RingdowProc(HWND hWnd,RINGPARAMS param);
	   
protected:
	PROPSHEETHEADER m_psh;
	HPROPSHEETPAGE* m_lppsp;
	int m_PageSize;				//已创建的PROPPAGE数组大小（非加入的PROPPAGE数量）
};

class RingPropPage:public RingDialog
{
public:
	RingPropPage();
	~RingPropPage();

	//virtual LRESULT RingdowProc(HWND hWnd,RINGPARAMS);
	HPROPSHEETPAGE CreatePage(UINT uIconId=0,LPCTSTR lpTemplate=NULL,
									LPARAM dwInitParam=0,HINSTANCE hInst=GetInstance());

protected:
	static LRESULT CALLBACK WINAPI InitPageProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);

public:
	RINGWNDPARAM m_InitParam;
};

#endif
