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
//原文件名：ringdowlist.cpp
//
//本文件说明：RingdowList类实现代码
//
//类库功能：窗口对象指针列表封装，每个窗口类对象建立时会把
//				指针存放到该列表，程序退出时如果对象未释放，本
//				窗口指针列表会自动释放每个对象。
//
//算法实现：
//		本类利用HASH表储存窗口对象指针，因为窗口对象指针一般
//		为偶数，故HASH函数为将对象指针低位值除以2再模除16。
//		类成员数据m_pList为长度16的指针数组，指向实际数据存储区，
//		数据存储区可动态增加，其首地址留用为指向下一块数据区。
//
//		m_pList			数据区首地址留用以用于析构时释放
//		| 0 |--|  	 |留用|--------------------->|留用|----------->...
//		| 1 |  |---->|	数 |-------		 |-------->| 数 |
//		| 2 |----	 |	据 | HASH |		 |数据     | 据 |
//		| . |	  |	 | 区 | 冲突 |		 |区满     | 区 |
//		| . |	  |	 |	.	| 数据 |		 |开辟     | .	 |
//		| . |	  |--->|	.	| 存到 |		 |新区     | .	 |
//		| . |			 |	.	| 空区 |		 |		     | .	 |
//		| . |			 |	.	|<-----|		 |		     | .	 |
//		| 15|			 |	.	|-------------		     | .	 |
//
**********************************************************************/

#define MAKE_SELF_LIB
//#ifndef __BORLANDC__
#include "ringdows.h"
//#else
//#include "include\ringdows.h"
//#endif

#ifdef _DEBUG

LPRINGDOWPTRBUF RingdowList::first_data = NULL;
LPRINGDOWPTR* RingdowList::m_pList = NULL;
int RingdowList::m_objCnt = 0;
CRITICAL_SECTION RingdowList::m_RingdowList_critical;
//int RingdowList::m_wndCnt = 0;

RingdowList::RingdowList()
{
   if(m_pList == NULL)
	{
		InitializeCriticalSection(&m_RingdowList_critical);
		m_pList = (LPRINGDOWPTR*)New(sizeof(LPRINGDOWPTR) << 4);
		if(m_pList)
			memset(m_pList,0,sizeof(LPRINGDOWPTR) << 4);
	}
	m_objCnt ++;
}

RingdowList::~RingdowList()
{
	//本类对象记数，注意所有窗口类不可包含本类为成员数据，
	//(静态成员除外)否则如果未释放本类对象也不释放，记数
	//不可能为0，也就不会自动释放未释放对象
	EnterCriticalSection(&m_RingdowList_critical);
	m_objCnt --;
   if(m_objCnt == 0)
	{
		LPRINGDOWPTRBUF temp,save;
		LPRINGDOWPTR del;

		temp = first_data;
		int i;
		char s[400];
		while(temp)
		{
			save = temp;

			del = &temp->data[0];
			for(i=0;i<16;i++)
			{
				if(del->winptr != NULL)
				{
					wsprintf(s,"文件%s,第%d行分配的对象未释放，已自动释放。\n警告：发布版本不会自动释放，必须修改代码以确定释放对象。\0",del->file,del->line);
					OutputDebugString(s);
					::delete del->winptr;
				}
				del ++;
			}
			temp = temp->next;
			Del(save);
		}
		Del(m_pList);
		LeaveCriticalSection(&m_RingdowList_critical);
		DeleteCriticalSection(&m_RingdowList_critical);
	}
	else
		LeaveCriticalSection(&m_RingdowList_critical);
}

LPRINGDOWPTR RingdowList::operator[](RingBaseWnd* w)
{
	int index = GetHashKey((DWORD)w);

	if(first_data == NULL)
	{
		first_data = (LPRINGDOWPTRBUF)New(sizeof(RINGDOWPTRBUF));
		if(first_data)
			memset(first_data,0,sizeof(RINGDOWPTRBUF));
	}

	LPRINGDOWPTR temp = FindEmpty();
	
	if(m_pList[index] == NULL)		
		m_pList[index] = temp;
	else
	{
		LPRINGDOWPTR prev = m_pList[index];
		while(prev->next)
			prev = prev->next;

		prev->next = temp;
	}

	return temp;
}

LPRINGDOWPTR RingdowList::FindEmpty()
{
	LPRINGDOWPTR temp;
	LPRINGDOWPTRBUF save,pt = first_data;
	int i;

	while(pt)
	{
		temp = &pt->data[0];
		for(i=0;i<16;i++)
		{
			if(temp->winptr == NULL)
				return temp;
			temp ++;
		}
		save = pt;
		pt = pt->next;
	}
	pt = save;
	pt->next = (LPRINGDOWPTRBUF)New(sizeof(RINGDOWPTRBUF));

	if(pt->next)
	{
		memset(pt->next,0,sizeof(RINGDOWPTRBUF));
		return &pt->next->data[0];
	}

	return NULL;
}

//int RingdowList::GetwinptrCnt()
//{
//	return m_winptr->m_winptrCnt;
//}

LPRINGDOWPTR RingdowList::GetItem(RingBaseWnd* w)
{
	EnterCriticalSection(&m_RingdowList_critical);
	LPRINGDOWPTR temp = m_pList[GetHashKey((DWORD)w)];
	
	while(temp)
	{
		if(temp->winptr == w)
		{
			LeaveCriticalSection(&m_RingdowList_critical);
			return temp;
		}
		temp = temp->next;
	}
	LeaveCriticalSection(&m_RingdowList_critical);
   return NULL;
}

void RingdowList::AddItem(RingBaseWnd* winptr,LPCTSTR file,int line)
{
   if(winptr && GetItem(winptr) == NULL)
	{
		EnterCriticalSection(&m_RingdowList_critical);
   	LPRINGDOWPTR temp = (*this)[winptr];
		temp->winptr = winptr;
		temp->file = file;
		temp->line = line;		
		LeaveCriticalSection(&m_RingdowList_critical);
	}
}

void RingdowList::DelItem(RingBaseWnd* w)
{
	if(w)
	{
		EnterCriticalSection(&m_RingdowList_critical);
		LPRINGDOWPTR prev,temp;
		int index = GetHashKey((DWORD)w);
		temp = m_pList[index];
		prev = NULL;

		while(temp)
		{
			if(temp->winptr)
			{
				temp->winptr = NULL;
				if(prev == NULL)
					m_pList[index] = NULL;
				else
					prev->next = temp->next;
				return;
			}
			prev = temp;
			temp = temp->next;
		}
		LeaveCriticalSection(&m_RingdowList_critical);
	}
}

#endif

/*
void RingdowList::Record(RingCtrlWnd* wnd)
{
	ringFile rf="ringdowlist.txt";
	int i;

	rf.Create(RF_NEW);
	rf.Write("m_pList:\r\n",10);

	char s[200];
	wsprintf(s,"%x\r\n\0\0",wnd);
	rf.Write(s,strlen(s));

	for(i=0;i<16;i++)
	{
		wsprintf(s,"[%d]:\t%x\r\n\0\0",i,m_pList[i]);
		rf.Write(s,strlen(s));
	}

	rf.Write("\r\ndata:\r\n",9);

	LPRINGDOWPTRBUF pt = first_data;
	i = 0;
	int j;

	while(pt)
	{
		wsprintf(s,"addr[%d]:\t%x\r\n\0\0",i,pt->next);
		rf.Write(s,strlen(s));

		for(j=0;j<16;j++)
		{
			wsprintf(s,"addr[%d-%d]:\t%x\tdata:%x\r\n\0\0",i,j,&pt->data[j],pt->data[j].winptr);
			rf.Write(s,strlen(s));
		}
		i++;
		pt = pt->next;
	}

	rf.Close();
}
*/