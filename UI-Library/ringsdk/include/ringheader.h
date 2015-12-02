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
//原文件名：ringheader.h
//
//本文件说明：RingHeader类声明
//
//类库功能：Header封装
//
**********************************************************************/
#ifndef _INC_RINGHEADER
#define _INC_RINGHEADER

#define HDCOL_HIDEMASK						0x10000000
#define HDCOL_EXTRACTORDER(lParam)		(((lParam) >> 16) & 0x0FFF)
#define HDCOL_ISHIDDEN(lParam)			MASK_MATCH((lParam),HDCOL_HIDEMASK)
#define HDMENU_BASEID						0x9000

class RingHeader:public RingControls
{
public:
	RingHeader();
	virtual ~RingHeader();

	LPCTSTR WndClassName(){return WC_HEADER;}

	void InitImageList(int cx=16,int cy=16);

	//添加图标
	int AddIcon(UINT uidIcon,HINSTANCE hInst=GetInstance());
	int AddIcon(LPCTSTR lpszIcon,HINSTANCE hInst=GetInstance());
	int AddIcon(HICON hicon);
	
	LONG GetData(int index);
	BOOL SetData(int index,LONG lParam);
	
	//子项操作
	int AddItem(int index,LPCTSTR szText,LPARAM lParam=0,int idIcon=-1);
	int AddItem(int index,int nVal,LPARAM lParam=0,int idIcon=-1);
	//显示图标，-1为不显示
	void Select(int index,int idIcon);

	//允许隐藏列头
	void EnableHideColumn(BOOL bEnable=TRUE){SendMessage(m_hWnd,HDM_ENABLECOLUMNHIDE,bEnable,0);}
	BOOL IsColumnHide(int iCol);
	//允许右键菜单选择显示/隐藏列头
	RingCoolMenu* EnableColumnMenu(BOOL bEnable=TRUE);

	//显示/隐藏列头，iCol为全部列头显示状态下的序号,显示/隐藏并不立即生效，
	//必须随后设置宽度才可以，这样做是因为ListView设置列宽会同时设置本HEADER的列宽，
	//函数返回原显示状态下列宽
	int ShowColumn(int iCol,BOOL bShow);

	//获取列头宽度，nMode:如果列头未隐藏，返回实际宽度，
	//如果列头已隐藏且nMode=SW_HIDE，返回未隐藏前的宽度
	int GetItemWidth(int iCol,int nMode=SW_SHOW);

	//虚函数重载
	BOOL SetValue(int val,int index);
	BOOL SetValue(LPCTSTR lpszVal,int index);

	//消息/宏封装
	/*未完成
	Header_CreateDragImage
	*/
	int ClearAllFilters(){return Header_ClearAllFilters(m_hWnd);}
	int ClearFilter(int i){return Header_ClearFilter(m_hWnd,i);}
	BOOL Delete(int index){return Header_DeleteItem(m_hWnd,index);}
	int EditFilter(int i,BOOL fDiscardChanges){return Header_EditFilter(m_hWnd,i,fDiscardChanges);}
	int GetBitmapMargin(){return Header_GetBitmapMargin(m_hWnd);}
	HIMAGELIST GetImageList(){return Header_GetImageList(m_hWnd);}
	//为免隐藏RingBaseWnd::GetItem，改成GetItemInfo
	BOOL GetItemInfo(int index,LPHDITEM phdi){return Header_GetItem(m_hWnd,index,phdi);}
	int GetCount(){return Header_GetItemCount(m_hWnd);}
	BOOL GetItemRect(int iIndex,LPRECT lpItemRect)
	{
		return Header_GetItemRect(m_hWnd,iIndex,lpItemRect);
	}
	BOOL GetOrderArray(int iSize,int *lpiArray)
	{
		return Header_GetOrderArray(m_hWnd,iSize,lpiArray);
	}
	int GetOrder(int index);
	BOOL GetUnicodeFormat(){return Header_GetUnicodeFormat(m_hWnd);}
	int InsertItem(int index,LPHDITEM phdi){return Header_InsertItem(m_hWnd,index,phdi);}
	BOOL Layout(LPHDLAYOUT playout){return Header_Layout(m_hWnd,playout);}
	int OrderToIndex(int iOrder){return Header_OrderToIndex(m_hWnd,iOrder);}
	int SetBitmapMargin(int iWidth){return Header_SetBitmapMargin(m_hWnd,iWidth);}
	int SetFilterChangeTimeout(int i){return Header_SetFilterChangeTimeout(m_hWnd,i);}
	int SetHotDivider(BOOL flag,DWORD dwInputValue)
	{
		return Header_SetHotDivider(m_hWnd,flag,dwInputValue);
	}
	HIMAGELIST SetImageList(HIMAGELIST himl){return Header_SetImageList(m_hWnd,himl);}
	BOOL SetItem(int iIndex,LPHDITEM phdItem){return Header_SetItem(m_hWnd,iIndex,phdItem);}
	BOOL SetItemWidth(int iCol,int nWidth);
	BOOL SetOrderArray(int iSize,int *lpiArray)
	{
		return Header_SetOrderArray(m_hWnd,iSize,lpiArray);
	}
	BOOL SetUnicodeFormat(BOOL fUnicode){return Header_SetUnicodeFormat(m_hWnd,fUnicode);}

protected:
	int GetItemText(int index,LPTSTR lpBuf,int size);
	//直接获取/设置数据，
	LPLONG GetRealData(int index);
	BOOL SetRealData(int index,LONG lParam);
	LPLONG GetHDInfo(int index);
	LRESULT RingdowProc(HWND,RINGPARAMS);

	BOOL IsMouseOnHiddenCol(int x,int y);

protected:
	HIMAGELIST m_hImage;
	RingCoolMenu *m_CoolMenu;

private:
	BOOL m_bCanHideColumn;
	int m_nHideColCnt;
};

#endif
