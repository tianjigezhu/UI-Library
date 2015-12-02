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
//							RingSDK多媒体类库 ringdib.lib
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
//原文件名：dib_global.cpp
//
//说明：全局帮助函数实现
//
**********************************************************************/

#define MAKE_SELF_LIB
#include "ringdib.h"

/////////////////////////////////////////////////
//
//	以下函数应用于DIB图象缩放绘制比率为整数倍（但
//	包括缩小1/3倍）的情况，如果图象缩放比率带小数，
//	请勿调用（结果数据不准的）。
//
//	参数rate说明：
//		rate为图象缩放倍率，应用于图象缩放绘制的情况
//		rate的取值必须遵守以下规则：
//
//			rate >= 0,放大倍数为radio+1,
//						即原图象一个像素绘制到屏幕为radio+1个像素
//			rate == -1,缩小倍数为1/3,
//						即原图象4个像素绘制到屏幕为3个像素,绘制后
//						为原图象的66.7%
//			rate < -1,缩小倍数为(-radio)，
//						即原图象(-radio)个像素绘制到屏幕为一个像素
//
//		调用时应该设置rate = GetDIBShowRate(dib);
//		当然一般应用中要用到此功能都会为图象设置一个rate,
//		不用计算rate，而是根据rate计算图象的绘制目的矩形。
//
/////////////////////////////////////////////////

//根据在图象上的绝对位置求绘制到屏幕的位置
BOOL Get_OnDCRect(RingDIB* dib,int rate,CONST RECT* lpsrc,LPRECT lpdst)
{
	if(dib == NULL || lpsrc == NULL || lpdst == NULL)
		return FALSE;

	RECT src,dst;
	
	dib->GetSrcRect(&src);
	dib->GetDestRect(&dst);

	//rate >= 0,放大倍数为radio+1
	//rate == -1,缩小倍数为3/4
	//rate < -1,缩小倍数为(-radio)

	if(rate >= 0)
	{
		rate ++;
		lpdst->left = (lpsrc->left - src.left) * rate + dst.left;
		lpdst->top = (lpsrc->top - src.top) * rate + dst.top;
		lpdst->right = (lpsrc->right - src.left) * rate + dst.left;
		lpdst->bottom = (lpsrc->bottom - src.top) * rate + dst.top;
	}
	else if(rate == -1)
	{
		lpdst->left = (lpsrc->left - src.left) * 3/4 + dst.left;
		lpdst->top = (lpsrc->top - src.top) * 3/4 + dst.top;
		lpdst->right = (lpsrc->right - src.left) * 3/4 + dst.left;
		lpdst->bottom = (lpsrc->bottom - src.top) * 3/4 + dst.top;
	}
	else
	{
		rate = -rate;
		lpdst->left = (lpsrc->left - src.left) / rate + dst.left;
		lpdst->top = (lpsrc->top - src.top) / rate + dst.top;
		lpdst->right = (lpsrc->right - src.left) / rate + dst.left;
		lpdst->bottom = (lpsrc->bottom - src.top) / rate + dst.top;
	}
	return TRUE;
}

//根据绘制到屏幕的位置求在图象上的绝对位置
BOOL Get_OnDIBRect(RingDIB* dib,int rate,CONST RECT* lpsrc,LPRECT lpdst)
{
	if(dib == NULL || lpsrc == NULL || lpdst == NULL)
		return FALSE;

	RECT src,dst;
	
	dib->GetSrcRect(&src);
	dib->GetDestRect(&dst);

	//rate >= 0,放大倍数为radio+1
	//rate == -1,缩小倍数为3/4
	//rate < -1,缩小倍数为(-radio)

	if(rate >= 0)
	{
		rate ++;
		lpdst->left = (lpsrc->left - dst.left) / rate + src.left;
		lpdst->top = (lpsrc->top - dst.top) / rate + src.top;
		lpdst->right = (lpsrc->right - dst.left) / rate + src.left;
		lpdst->bottom = (lpsrc->bottom - dst.top) / rate + src.top;
	}
	else if(rate == -1)
	{
		lpdst->left = (lpsrc->left - dst.left) * 4/3 + src.left;
		lpdst->top = (lpsrc->top - dst.top) * 4/3 + src.top;
		lpdst->right = (lpsrc->right - dst.left) * 4/3 + src.left;
		lpdst->bottom = (lpsrc->bottom - dst.top) * 4/3 + src.top;
	}
	else
	{
		rate = -rate;
		lpdst->left = (lpsrc->left - dst.left) * rate + src.left;
		lpdst->top = (lpsrc->top - dst.top) * rate + src.top;
		lpdst->right = (lpsrc->right - dst.left) * rate + src.left;
		lpdst->bottom = (lpsrc->bottom - dst.top) * rate + src.top;
	}
	return TRUE;
}

int GetDIBShowRate(RingDIB* dib)
{
	if(dib == NULL)
		return 0;

	RECT rcd,rcs;
	int dw,sw;

	dib->GetDestRect(&rcd);
	dib->GetSrcRect(&rcs);
	dw = rcd.right - rcd.left;
	sw = rcs.right - rcs.left;

	if(dw >= sw)
		return (dw / sw) - 1;
	else
		if(dw * 4/3 == sw)
			return -1;
		else
			return -(sw / dw);
}

//根据在图象上的绝对位置求绘制到屏幕的位置
BOOL Get_OnDCPt(RingDIB* dib,int rate,CONST POINT* lpsrc,LPPOINT lpdst)
{
	if(dib == NULL || lpsrc == NULL || lpdst == NULL)
		return FALSE;

	RECT src,dst;
	
	dib->GetSrcRect(&src);
	dib->GetDestRect(&dst);

	//rate >= 0,放大倍数为radio+1
	//rate == -1,缩小倍数为3/4
	//rate < -1,缩小倍数为(-radio)

	if(rate >= 0)
	{
		rate ++;
		lpdst->x = (lpsrc->x - src.left) * rate + dst.left;
		lpdst->y = (lpsrc->y - src.top) * rate + dst.top;		
	}
	else if(rate == -1)
	{
		lpdst->x = (lpsrc->x - src.left) * 3/4 + dst.left;
		lpdst->y = (lpsrc->y - src.top) * 3/4 + dst.top;		
	}
	else
	{
		rate = -rate;
		lpdst->x = (lpsrc->x - src.left) / rate + dst.left;
		lpdst->y = (lpsrc->y - src.top) / rate + dst.top;		
	}
	return TRUE;
}

//根据绘制到屏幕的位置求在图象上的绝对位置
BOOL Get_OnDIBPt(RingDIB* dib,int rate,CONST POINT* lpsrc,LPPOINT lpdst)
{
	if(dib == NULL || lpsrc == NULL || lpdst == NULL)
		return FALSE;

	RECT src,dst;
	
	dib->GetSrcRect(&src);
	dib->GetDestRect(&dst);

	//rate >= 0,放大倍数为radio+1
	//rate == -1,缩小倍数为3/4
	//rate < -1,缩小倍数为(-radio)

	if(rate >= 0)
	{
		rate ++;
		lpdst->x = (lpsrc->x - dst.left) / rate + src.left;
		lpdst->y = (lpsrc->y - dst.top) / rate + src.top;		
	}
	else if(rate == -1)
	{
		lpdst->x = (lpsrc->x - dst.left) * 4/3 + src.left;
		lpdst->y = (lpsrc->y - dst.top) * 4/3 + src.top;		
	}
	else
	{
		rate = -rate;
		lpdst->x = (lpsrc->x - dst.left) * rate + src.left;
		lpdst->y = (lpsrc->y - dst.top) * rate + src.top;		
	}
	return TRUE;
}
