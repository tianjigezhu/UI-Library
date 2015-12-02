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
//原文件名：ringimagelist.h
//
//本文件说明：RingImageList类声明
//
//类库功能：ImageList封装
//
**********************************************************************/
#ifndef _INC_RINGIMAGELIST
#define _INC_RINGIMAGELIST

class RingImageList
{
public:
	RingImageList():m_handle(NULL){}
	virtual ~RingImageList(){Destroy();}
		
	HIMAGELIST Handle(){return m_handle;}
	operator	HIMAGELIST() const{return m_handle;}
	int Count(){return ImageList_GetImageCount(m_handle);}
	
	BOOL Clear(){return ImageList_RemoveAll(m_handle);}
	HIMAGELIST Create(int cx,int cy,UINT flags,int cInitial,int cGrow);
	BOOL Destroy();
	int Add(HBITMAP hbm,HBITMAP hbmMask=NULL){return ImageList_Add(m_handle,hbm,hbmMask);}
	int AddMasked(HBITMAP hbm,COLORREF crMask){return ImageList_AddMasked(m_handle,hbm,crMask);}
	BOOL BeginDrag(int idx,int x,int y){return ImageList_BeginDrag(m_handle,idx,x,y);}

	HICON GetIcon(int idx,UINT flags){return ImageList_GetIcon(m_handle,idx,flags);}
	BOOL GetIconSize(int *cx,int *cy){return ImageList_GetIconSize(m_handle,cx,cy);}

protected:

private:
	HIMAGELIST m_handle;	
};

#endif