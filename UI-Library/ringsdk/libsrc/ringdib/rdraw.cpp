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
//原文件名：rdraw.cpp
//
//说明：图象显示部分RingDraw类的定义
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdib.h"

RingDIB* _rBack = NULL;

HDRAWDIB RingDraw::m_hDrawDib = NULL;
int RingDraw::m_count = 0;
BOOL RingDraw::m_bWithBack = FALSE;
BITMAPINFO RingDraw::m_bmif = {{sizeof(BITMAPINFOHEADER),0,0,1,32,BI_RGB,0,0,0,0,0,}};

HDRAWDIB RingDraw::Open()
{
   if(!m_hDrawDib)
   	m_hDrawDib = DrawDibOpen();

   m_count ++;

   return m_hDrawDib;
}

void RingDraw::Close()
{
   m_count --;

   if(m_bWithBack && m_count == 1)
      FreeBack();

   if(m_count == 0)
   {
   	DrawDibClose(m_hDrawDib);
      m_hDrawDib = NULL;
   }
}

BOOL RingDraw::Draw(HDC hDC,RingDIB* rdib,UINT wFlags)
{
	if(rdib->Data())
   {
	   m_bmif.bmiHeader.biWidth=rdib->m_width;
	   m_bmif.bmiHeader.biHeight=rdib->m_height;

   	return DrawDibDraw( m_hDrawDib,hDC,rdib->dest.left,rdib->dest.top,rdib->dest.right-rdib->dest.left,rdib->dest.bottom-rdib->dest.top,
					&(m_bmif.bmiHeader),rdib->Data(),rdib->src.left,rdib->src.top,rdib->src.right-rdib->src.left,rdib->src.bottom-rdib->src.top,wFlags );
   }
   else
   	return FALSE;
}

BOOL RingDraw::Draw(RingDIB* rdib,UINT wFlags)
{
	HDC hDC=GetDC(rdib->r_hWnd);
	BOOL bOK = Draw(hDC,rdib,wFlags);	
	ReleaseDC(rdib->r_hWnd,hDC);
   return bOK;
}

BOOL RingDraw::Paint(RingDIB* rdib,PAINTSTRUCT* lps,UINT wFlags)
{
	if(rdib->Data())
   {
	   BOOL bEnd = FALSE;
		PAINTSTRUCT ps;

		if(lps == NULL)
		{
			lps = &ps;
   		BeginPaint(rdib->r_hWnd,lps);
			bEnd = TRUE;
		}

	   m_bmif.bmiHeader.biWidth=rdib->m_width;
   	m_bmif.bmiHeader.biHeight=rdib->m_height;

	   BOOL bOK = DrawDibDraw ( m_hDrawDib, lps->hdc, rdib->dest.left,rdib->dest.top,rdib->dest.right-rdib->dest.left,rdib->dest.bottom-rdib->dest.top,
			&(m_bmif.bmiHeader), rdib->Data(),rdib->src.left,rdib->src.top,rdib->src.right-rdib->src.left,rdib->src.bottom-rdib->src.top,wFlags );

		if(bEnd)
			EndPaint(rdib->r_hWnd,lps);

   	return bOK;
   }
   else
   	return FALSE;
}

BOOL RingDraw::DrawTo(RingDIB* src,RingDIB* dest,BOOL bTrans)
{
   int i,j,a,t,r,b,x,y;
   COLORREF *Src,*Dst,*srcbak,*dstbak;

	src->CorrectDest();

   //求剪裁矩形
   x = src->dest.left + src->src.right - src->src.left;
   y = src->dest.top + src->src.bottom - src->src.top;
   r = dest->m_width > x?x:dest->m_width;
   b = dest->m_height > y?y:dest->m_height;

   a = (src->m_height - ((b - src->dest.top) + src->src.top)) * src->m_width + src->src.left;
   t = (dest->m_height - b) * dest->m_width + src->dest.left;

   srcbak = Src = src->Data() + a;
   dstbak = Dst = dest->Data() + t;

   if(src->m_crKey == 0xffffffff || !bTrans)		//不透明
      for(j=src->dest.top;j<b;j++)
      {
   		for(i=src->dest.left;i<r;i++)
      	{
            *Dst = *Src;
           	Src++;Dst++;
         }
         srcbak += src->m_width;
         dstbak += dest->m_width;
         Src = srcbak;
         Dst = dstbak;
      }
   else       //透明
      for(j=src->dest.top;j<b;j++)
      {
   		for(i=src->dest.left;i<r;i++)
      	{
            if(*Src != src->m_crKey)
	   	      *Dst = *Src;
           	Src++;Dst++;
         }
         srcbak += src->m_width;
         dstbak += dest->m_width;
         Src = srcbak;
         Dst = dstbak;
      }

   return TRUE;
}

//拉伸绘制，忽略src->dest，目标为Dest的整个图象
BOOL RingDraw::StretchTo(RingDIB* src,RingDIB* dest,BOOL bAll)
{
	COLORREF *lpData,*lpBak,*lpBako;
   COLORREF* lpdData = dest->Data();
   int width = dest->m_width;
   int height = dest->m_height;
	float sw,sh;
	int sl,st;
		      
	src->CorrectDest();

	if(bAll)	//src的整个图象
	{
		sw = (float)src->Width();
		sh = (float)src->Height();
		sl = 0;
		st = 0;
	}
	else
	{
		sw = (float)(src->src.right - src->src.left);
		sh = (float)(src->src.bottom - src->src.top);
		sl	= src->src.left;
		st = src->m_height - src->src.bottom;
		st = max(st,0);
	}

	float x = sw/(float)width;
   float y = sh/(float)height;
	float x_cnt,y_cnt=0.00;

	lpBak = lpBako = lpData = src->Data() + src->m_width * st + sl;

   for(int j=0;j<height;j++)
   {
		x_cnt=0.00;
     	for(int i=0;i<width;i++)
      {
        	*lpdData = *lpData;
         lpdData ++;

         x_cnt += x;
         lpData = lpBak + (int)x_cnt;
      }
		y_cnt += y;
  	   lpBak = lpBako + src->m_width * (int)y_cnt;
		lpData = lpBak;
   }
   return TRUE;
}

BOOL RingDraw::CopyToBack(RingDIB* rdib)
{
	int i,j,a,b,c,d;

   a=c=rdib->src.top*rdib->m_width+rdib->src.left;
   b=d=0;

   for(j=0;j<_rBack->m_height;j++)
   {
   	for(i=0;i<_rBack->m_width;i++)
     	{
     		_rBack->Data()[b]=rdib->Data()[a];
        	a++;b++;
      }
     	c+=rdib->m_width;d+=_rBack->m_width;
      a=c;b=d;
   }
   return TRUE;
}

BOOL RingDraw::CreateBack(HWND hWnd,int x,int y)
{
   if(!_rBack)
   {
	   _rBack = new RingDIB;

   	if(!_rBack)
   		return FALSE;

	   _rBack->r_hWnd = hWnd;
   	m_bWithBack = TRUE;
	   return _rBack->Create(x,y);
   }
   else
   	return TRUE;
}

void RingDraw::FreeBack(void)
{
	if(_rBack)
	   delete _rBack;
   _rBack = NULL;
   m_bWithBack = FALSE;
}

