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
//本文件说明：RingCheckList类声明
//
//类库功能：CheckList控件封装。CheckList是界面库特制的
//				控件，ListBox控件的扩展，每一个列表选项前有
//				一个选择框可供选择。
//
**********************************************************************/
#ifndef _INC_RINGCHECKLIST
#define _INC_RINGCHECKLIST

typedef struct _tagCheckListData
{
	BOOL m_bChecked;
	LPARAM m_data;
}RINGCHECKLISTDATA,*LPRINGCHECKLISTDATA;

class RingCheckList:public RingListBox
{
public:
	RingCheckList();
	virtual ~RingCheckList();
	
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	BOOL Create(UINT uId,int x=0,int y=0,int width=-999,int height=-999);

	//因重载了Attach(HWND),必须重载该函数，否则导致隐藏，要加RingControls::才能调用
	BOOL Attach(UINT uId,RingBaseWnd* parent=NULL){return RingControls::Attach(uId,parent);}
	BOOL Attach(HWND);
	
	//设置自绘，参数为TRUE表示由父窗口处理自绘
	void SetOwnerDraw(BOOL bParentDraw=FALSE)
		{RingControls::SetOwnerDraw(bParentDraw,LBS_OWNERDRAWFIXED);}
	
	//检测选择状态
	BOOL IsItemChecked(int nIndex = -1);
	//切换选择/非选择状态，ToggleItemCheck返回现状态，SetItemCheck返回原状态
	BOOL ToggleItemCheck(int nIndex = -1);
	BOOL SetItemCheck(int nIndex,BOOL bCheck);

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	BOOL OnDrawItem(LPDRAWITEMSTRUCT lpds);
	void CreateCheckBmp(HDC hDC,int nSize);
	LRESULT OnDestroy(RINGPARAMS& param);

private:
	void OnDelString(int index);
	LPRINGCHECKLISTDATA GetPriData(int nIndex);
	BOOL SetPriData(int nIndex,LPRINGCHECKLISTDATA lpData);

protected:
	HBITMAP m_hbmp;
	int m_nCheckSize;
};

#endif
