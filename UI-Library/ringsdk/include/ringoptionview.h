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
//原文件名：ringoptionview.h
//
//本文件说明：RingOptionView类声明
//
//类库功能：OptionView控件封装。OptionView是界面库特制的
//				控件，类似于DELPHI的控件属性设置，是一个定制
//				的ListBox，可有两列，第一列是属性名称，第二
//				列为属性值，单击后可以编辑。
//
**********************************************************************/
#ifndef _INC_RINGOPTIONVIEW
#define _INC_RINGOPTIONVIEW

//ITEM类型
#define OPTYPE_EDIT			0x0
#define OPTYPE_LIST			0x1
#define OPTYPE_BROWSE		0x2
#define OPTYPE_PACK			0x4
#define OPTYPE_NONE			0x8
//扩展类型
#define OPTYPE_READONLY		0x10
//仅EDIT类型有效，是否限制只输入数字
#define OPPROP_NUMBERONLY	1
#define OPPROP_PASSWORD		4
//仅BROWSE类型有效，是否可选择文件
#define OPPROP_BROWSEFILE	2
//类型检测掩码
#define OPTYPE_MASK		0x0F
//ITEM是否是外部数据，由调用程序负责分配内存和释放
#define OPTYPE_EXTERN	0x1000
#define ROPV_LEFTMARGIN	16
//删除选项标志（数据包展开/收缩，删除所有选项）
#define DELITEM_HIDE			0xABCDEF00
#define DELITEM_CLEAR		0xABCDEF01

class RingOptionItemPack;

typedef BOOL (RINGVCLASS::*ROPV_CHECKVALFUNC)(RingOptionItem*);

typedef struct tagRPDataPackItem
{
	RingOptionItem* m_item;
	tagRPDataPackItem* m_next;
}RPDATAPACKITEM,*LPRPDATAPACKITEM;

//事件通知消息（WM_NOTIFY）
#define ROPVN_VALUECHANGE	0x0180
#define ROPVN_BUTTONCLICK	0x0181
#define ROPVN_SELCHANGE		0x0182

class RingOptionView:public RingListBox
{
public:
	RingOptionView();
	virtual ~RingOptionView();
	
	BOOL Create(DWORD dwStyle,DWORD dwExStyle,LPCTSTR szInitText,UINT uId,int x,int y,int width,int height);
	BOOL Create(UINT uId,int x=0,int y=0,int width=-999,int height=-999);

	//因重载了Attach(HWND),必须重载该函数，否则导致隐藏，要加RingControls::才能调用
	BOOL Attach(UINT uId,RingBaseWnd* parent=NULL){return RingControls::Attach(uId,parent);}
	BOOL Attach(HWND);
	
	//设置自绘，参数为TRUE表示由父窗口处理自绘
	void SetOwnerDraw(BOOL bParentDraw=FALSE)
		{RingControls::SetOwnerDraw(bParentDraw,LBS_OWNERDRAWFIXED);}
	void SetLeftMargin(int nSize){m_LeftPos=(nSize>=0?nSize:m_LeftPos);}
	int GetLeftMargin(){return m_LeftPos;}

	//加入属性设置
	int AddOption(LPCTSTR szOption,LPCTSTR szDefValue=NULL,LPARAM lParam=0,
						UINT uEditType=OPTYPE_EDIT,LONG lTypeCfg=0);
	int AddOptionPack(LPCTSTR szOption,LPARAM lParam=0);
	int AddOption(RingOptionItem* item);
	int InsertOption(int index,LPCTSTR szOption,LPCTSTR szDefValue=NULL,LPARAM lParam=0,
							UINT uEditType=OPTYPE_EDIT,LONG lTypeCfg=0);
	int InsertOption(RingOptionItem* item,int index);
	int InsertOption(RingOptionItem* item,LPCTSTR lpString);
	int AddSubOption(LPCTSTR szPackName,LPCTSTR szOption,LPCTSTR szDefValue=NULL,
							LPARAM lParam=0,UINT uEditType=OPTYPE_EDIT,LONG lTypeCfg=0);
	int AddSubOption(int index,LPCTSTR szOption,LPCTSTR szDefValue=NULL,
							LPARAM lParam=0,UINT uEditType=OPTYPE_EDIT,LONG lTypeCfg=0);
	//删除，也可调用RingListBox::DelString
	int DelOption(RingOptionItem* item);
	//清空包数据
	int ClearPack(int index);
	int ClearPack(LPCTSTR lpName);

	//获取数据项,数据包子项只有在展开状态下才能得到，如需保证成功得到，
	//必须先GetPackItem获取数据包，然后调用其GetSubItem得到。
	RingOptionItem* GetOptionItem(int index = INDEX_CURSEL);
	RingOptionItem* GetOptionItem(LPCTSTR lpName);
	RingOptionItemPack* GetPackItem(LPCTSTR lpName);
	RingOptionItemPack* GetPackItem(int index);
	
	//重载GetInt为获得属性值,GetText不可重载，因其调用GetItemText,
	//故改变其行为需重载GetItemText，如果对属性设置过隐含值，均返回隐含值，
	//即LIST子项返回列表选择的值而非显示字符串，Browser子项同理
	int GetInt(int index=INDEX_CURSEL);
	virtual int GetInt(LPCTSTR lpName);
	
	//获取属性名称（原GetText的行为）
	LPCTSTR GetName(int index);
	//获取子项类型
	int GetType(int nIndex);

	//获取选项序号（对List子项有效）
	int GetSelectIndex(int index);
	int GetSelectIndex(LPCTSTR lpName);
	//获取选项DATA
	int GetSelectData(int index);
	int GetSelectData(LPCTSTR lpName);

	//获取数据包子项数据，数据包有可能收起，调用GetText和GetInt会失败或得不到
	//正确值，GetPackItemText和GetPackItemInt不管数据包是否收起总能获得正确值
	LPCTSTR GetPackItemText(int index,LPCTSTR szSubItem);
	LPCTSTR GetPackItemText(LPCTSTR szPackName,LPCTSTR szSubItem);
	LPCTSTR GetPackItemText(RingOptionItemPack* pack,LPCTSTR szSubItem);
	int GetPackItemInt(int index,LPCTSTR szSubItem);
	int GetPackItemInt(LPCTSTR szPackName,LPCTSTR szSubItem);	
	int GetPackItemInt(RingOptionItemPack* pack,LPCTSTR szSubItem);
	
	//清空数据（保留各子项）
	void Reset();
	//设置数据
	BOOL SetValue(int val,int index=INDEX_CURSEL);
	BOOL SetValue(LPCTSTR lpszVal,int index=INDEX_CURSEL);
	virtual BOOL SetValue(int index,LPARAM lVal,LPCTSTR szShowVal=NULL);
	virtual BOOL SetValue(LPCTSTR szItemName,LPARAM lVal,LPCTSTR szShowVal=NULL);
	//设置数据包子项数据
	virtual BOOL SetPackItemValue(int index,LPCTSTR szSubItem,LPARAM lVal,LPCTSTR szShowVal=NULL);
	virtual BOOL SetPackItemValue(LPCTSTR szPackName,LPCTSTR szSubItem,LPARAM lVal,LPCTSTR szShowVal=NULL);
	BOOL SetPackItemValue(RingOptionItemPack* pack,LPCTSTR szSubItem,LPARAM lVal,LPCTSTR szShowVal=NULL);
	//设置选项序号（对List子项有效）
	BOOL SetSelectIndex(int index,int val);
	BOOL SetSelectIndex(LPCTSTR lpName,int val);
	//根据隐藏值设置选项序号
	BOOL SetSelectIndexByData(int index,LONG lData,BOOL IsString=FALSE);
	BOOL SetSelectIndexByData(LPCTSTR lpName,LONG lData,BOOL IsString=FALSE);
	
	RingEdit* GetEdit(){return m_edit;}
	RingCombo* GetCombo(BOOL bReadOnly){return bReadOnly?m_comboSel:m_combo;}
	RingButton* GetButton(){return m_button;}
	
	//是否是仅剩的一个数据包（可能需要调整左边的空位）
	BOOL IsLastPack(RingOptionItem* item);
	//设定数值改变，按钮点击等事件处理函数
	BOOL SetItemValChangeFunc(int index,RINGVCLASS* obj,ROPV_CHECKVALFUNC func);
	BOOL SetItemValChangeFunc(LPCTSTR lpName,RINGVCLASS* obj,ROPV_CHECKVALFUNC func);
	BOOL SetItemValSelectFunc(int index,RINGVCLASS* obj,ROPV_CHECKVALFUNC func);
	BOOL SetItemValSelectFunc(LPCTSTR lpName,RINGVCLASS* obj,ROPV_CHECKVALFUNC func);
	BOOL SetItemBrowserFunc(int index,RINGVCLASS* obj,ROPV_CHECKVALFUNC func);
	BOOL SetItemBrowserFunc(LPCTSTR lpName,RINGVCLASS* obj,ROPV_CHECKVALFUNC func);

protected:
	LRESULT RingdowProc(HWND,RINGPARAMS);
	BOOL OnDrawItem(LPDRAWITEMSTRUCT lpds);
	BOOL OnFitSize(HWND,WPARAM,LPARAM);
	void OnCreate(HWND);
	LRESULT OnDestroy(RINGPARAMS& param);
	//重载该函数使GetText返回属性值，真正的Item Text
	//用GetName得到.本函数返回属性值字符串长度，不管
	//实际COPY到lpBuf多少字节。
	virtual int GetItemText(int index,LPTSTR lpBuf,int size);

private:
	RingOptionItem* OnAddString(DWORD type,LPCTSTR lpString);
	void OnDelString(int index,DWORD dwFlag);
	void SendNotify(int type);	
	friend class RingOptionItem;

protected:
	RingEdit* m_edit;
	RingCombo* m_combo,*m_comboSel;	//列表可输入，仅列表
	RingButton* m_button;
	HPEN m_pen;
	HCURSOR m_cursor,m_cursorSize;
	int m_SplitPos;
	int m_LeftPos;
	//数据包子项显示加粗字体
	ringFont m_fontPack,m_fontSubItem;

private:
	BOOL m_bCapture,m_bUpdate;
};

class RingOptionItem
{
public:
	RingOptionItem():m_edit(NULL),m_parent(NULL),
		m_bAutoDelete(FALSE),m_data(0),m_obj(NULL),
		m_OnCheckValFunc(NULL),m_parentItem(NULL),m_bReadOnly(FALSE){}
	virtual ~RingOptionItem()=0{}

	void SetName(LPCTSTR lpszName){m_strName = lpszName;}
	LPCTSTR GetName(){return m_strName.string();}
	//得到在OptionView中的排列序号(数据包子项返回LB_ERR)
	int GetIndex();
	//得到在数据包中的排列序号
	int GetSubIndex(RingOptionItemPack** parentItem=NULL);

	virtual int FindSubItem(LPCTSTR lpszName){return -1;}

	virtual LPCTSTR GetText();
	virtual int GetInt();
	virtual void Reset(){SetValue("");}
	//对于当前处于编辑状态的项，返回原设置值，否则同GetText;
	virtual LPCTSTR GetPrevText(){return m_strValue.string();}

	virtual void UpdateValue();
	
	BOOL IsReadOnly(){return m_bReadOnly;}
	void SetReadOnly(BOOL bReadOnly){m_bReadOnly = bReadOnly;}
	void SetData(LPARAM data){m_data = data;}
	LONG GetData(){return m_data;}
	//获取本身设定的数据（List,Sel类型）
	virtual LONG GetItemData(int index=INDEX_CURSEL){return 0;}
	virtual void SetItemData(LPARAM lParam,int index=INDEX_CURSEL){}
	//获取选项序号（对List类型有效）
	virtual int GetSelectIndex(){return LB_ERR;}
	virtual BOOL SetSelectIndex(int index){return FALSE;}
	virtual BOOL SetSelectIndexByData(LONG lData,BOOL IsString){return FALSE;}
	//virtual LPARAM GetValue(){return 0}
	virtual BOOL SetValue(int nVal);
	virtual BOOL SetValue(LPCTSTR lpszVal);
	virtual BOOL SetValue(LPARAM lVal,LPCTSTR lpszShowVal);
	//设置行为表现或内部数据(LIST:选项对应显示值及内部值，EDIT：是否限制数字，
	//SelFile:选择文件的扩展名
	virtual BOOL SetOption(LONG lConfig){return TRUE;}

	virtual int GetType(){return OPTYPE_NONE;}
	BOOL IsSubItem(){return (BOOL)m_parentItem;}
	virtual BOOL IsPackItem(){return FALSE;}
	//展开
	//virtual void Expand();
	//收起
	//virtual void Collapse();

	//只有处于编辑状态才返回控件指针
	virtual RingControls* GetCtrl(){return m_edit;}
	//编辑数据控件是否可见（在OptionView客户区可见，非控件可见属性）
	BOOL IsEditVisible();

	RingOptionView* GetParent(){return m_parent;}
		
	void SetCheckValueFunc(RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
	{		
		m_obj = obj;
		m_OnCheckValFunc = func;		
	}
	virtual BOOL SetSelChangeFunc(RINGVCLASS* obj,ROPV_CHECKVALFUNC func){return FALSE;}
	virtual BOOL SetBrowserFunc(RINGVCLASS* obj,ROPV_CHECKVALFUNC func){return FALSE;}
	
protected:
	virtual void EnterEdit(int,int,int,int);
	virtual BOOL ExitEdit();
	virtual void DrawValue(HDC hDC,LPRECT lprc);
	//激活控件行为设置
	virtual void ActiveOption(RingControls& edit);
	//值改变等发送通知消息之前的预处理
	virtual BOOL OnBeforeNotify(int type){return FALSE;}
	//void SetSubItem(){m_bIsSubItem = TRUE;}

private:
	void SetAutoDelete(BOOL bAuto){m_bAutoDelete = bAuto;}
	void SetParent(RingOptionView* parent){m_parent = parent;}
	virtual BOOL InterSetValue(LPARAM lVal,LPCTSTR lpszShowVal){return SetValue(lVal,lpszShowVal);}
	virtual RingControls* GetEditCtrl(){return NULL;}

	friend class RingOptionView;
	friend class RingOptionItemPack;

protected:
	ringStr m_strName,m_strValue;
	LPARAM m_data;		//自定义数据
	RingControls* m_edit;
	RINGVCLASS* m_obj;
	RingOptionView* m_parent;
	
	ROPV_CHECKVALFUNC m_OnCheckValFunc;

private:
	BOOL m_bAutoDelete;
	BOOL m_bReadOnly;
	RingOptionItemPack* m_parentItem;	
};

class RingOptionItemEdit:public RingOptionItem
{
public:
	RingOptionItemEdit():m_uType(0),m_strPsw(NULL){}
	virtual ~RingOptionItemEdit(){delete m_strPsw;}

	int GetType(){return OPTYPE_EDIT;}
	BOOL SetOption(LONG lConfig){m_uType = lConfig;return TRUE;}
	//重载的虚函数
	BOOL SetValue(LPCTSTR lpszVal);
	BOOL SetValue(int nVal);
	
protected:
	//激活控件行为设置
	void ActiveOption(RingControls& edit);
	void DrawValue(HDC hDC,LPRECT lprc);
	BOOL ExitEdit();
	BOOL OnBeforeNotify(int type);

private:
	UINT m_uType;
	ringStr* m_strPsw;

private:
	RingControls* GetEditCtrl(){return GetParent()?GetParent()->GetEdit():NULL;}
};

//////////////////////////////////////////////////
//
//列表选项的数据保存格式：
//		m_strList保存列表选项，以“|”分隔
//		m_InterData数组保存相应选项的DATA，注意DATA不能是指针，
//		因为删除时不会为各选项发送WM_DELETEITEM消息
//
//////////////////////////////////////////////////
class RingOptionItemList:public RingOptionItem
{
public:
	RingOptionItemList():m_objSel(NULL),m_SelectFunc(NULL),m_selData(NULL),
								m_nItemCnt(0),m_InterData(0),m_nDataSize(0){}
	virtual ~RingOptionItemList(){}
	
	int GetType(){return IsReadOnly()?(OPTYPE_LIST|OPTYPE_READONLY):OPTYPE_LIST;}
	
	//获取当前选择序号
	int GetSelectIndex();
	BOOL SetSelectIndex(int index);
	//根据DATA选择相应选项，IsString指定lData是否为字符串
	BOOL SetSelectIndexByData(LONG lData);

	int GetCount(){return m_nItemCnt;}
	LPCTSTR GetText();
	int GetInt();
	LONG GetItemData(int index=INDEX_CURSEL);
	void SetItemData(LPARAM lParam,int index=INDEX_CURSEL);
	int FindString(LPCTSTR lpString);

	BOOL SetValue(int nVal);
	BOOL SetValue(LPCTSTR lpszVal);

	//设置选项，参数为字符串，格式为"选项1|选项2|...||DATA1|DATA2|..."
	BOOL SetOption(LONG lConfig);

	BOOL SetSelection(LPCTSTR lpString);
	BOOL AddSelection(LPCTSTR lpString,LONG lData=0);
	BOOL DelSelection(LPCTSTR lpString);
	void Clear();

	BOOL SetSelChangeFunc(RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
	{
		m_objSel = obj;
		m_SelectFunc = func;
		return TRUE;
	}

protected:
	void EnterEdit(int,int,int,int);
	BOOL OnBeforeNotify(int type);

private:
	RingControls* GetEditCtrl(){return GetParent()?GetParent()->GetCombo(IsReadOnly()):NULL;}
	BOOL InterSetValue(LPARAM lVal,LPCTSTR lpszShowVal);

protected:
	ringStr m_strList;
	LPLONG m_selData;
	LONG m_InterData;
	int m_nItemCnt,m_nDataSize;
	RINGVCLASS* m_objSel;
	ROPV_CHECKVALFUNC m_SelectFunc;
};

class RingOptionItemSel:public RingOptionItem
{
public:
	RingOptionItemSel():m_button(NULL),m_bIsSelectFile(FALSE),m_ItemData(0),
								m_objBrowser(NULL),m_BrowserFunc(NULL){}
	virtual ~RingOptionItemSel(){}
	
	int GetType(){return OPTYPE_BROWSE;}
	LONG GetItemData(int index=INDEX_CURSEL){return m_ItemData;}
	void SetItemData(LPARAM lParam,int index=INDEX_CURSEL){m_ItemData = lParam;}

	//BOOL SetValue(int nVal){return RingOptionItem::SetValue(nVal);}
	//BOOL SetValue(LPCTSTR lpszVal){return RingOptionItem::SetValue(lpszVal);}
	//覆盖基类函数，隐藏了其余两个，因此需定义上面两个函数
	//BOOL SetValue(LPARAM lVal,LPCTSTR lpszShowVal);
	BOOL SetOption(LONG lConfig);

	BOOL SetBrowserFunc(RINGVCLASS* obj,ROPV_CHECKVALFUNC func)
	{
		m_objBrowser = obj;
		m_BrowserFunc = func;
		return TRUE;
	}

protected:
	void EnterEdit(int x,int y,int width,int height);
	BOOL ExitEdit();
	BOOL OnBeforeNotify(int type);

protected:
	RingButton* m_button;
	LPARAM m_ItemData;
	BOOL m_bIsSelectFile;
	RINGVCLASS* m_objBrowser;
	ROPV_CHECKVALFUNC m_BrowserFunc;
	TCHAR m_szFileExt[80];

private:
	RingControls* GetEditCtrl(){return GetParent()?GetParent()->GetEdit():NULL;}
};

class RingOptionItemPack:public RingOptionItem
{
public:
	RingOptionItemPack();
	virtual ~RingOptionItemPack();
	
	void Clear();
	void Reset();

	int GetType(){return OPTYPE_PACK;}
	BOOL IsPackItem(){return TRUE;}
	int GetSubItemCount();

	int AddSubItem(RingOptionItem* item);
	BOOL DelSubItem(RingOptionItem* item);
	RingOptionItem* GetSubItem(LPCTSTR lpszName);
	RingOptionItem* GetSubItem(int nIndex);
	int GetSubItemIndex(LPCTSTR lpszName);

	//展开
	void Expand();
	//收起
	void Collapse();
	//自动展开或收起
	void AutoExpand();

protected:
	void EnterEdit(int x,int y,int width,int height){};
	BOOL ExitEdit(){return FALSE;}
	void DrawValue(HDC hDC,LPRECT lprc);
	//void InsertItems();
	//void DeleteItems();

protected:
	BOOL m_bExpand;
	LPRPDATAPACKITEM m_SubItem;
		
private:
	RingControls* GetEditCtrl(){return NULL;}
};

#endif
