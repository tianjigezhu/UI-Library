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
//原文件名：bmp.cpp
//
//说明：BMP图象载入功能实现。
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdib.h"

BOOL RingDIB::LoadBmp(LPCTSTR szFilename,RingDIB* lprdib,int iFlag,LPSTR lpszFormat)
{
	if(iFlag == DIB_GETFORMAT)
	{
		if(lpszFormat)
		{
			wsprintf(lpszFormat,"bmp");
			return TRUE;
		}
		else
			return FALSE;
	}

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
   HBITMAP hbm;
   BITMAP bm;
	ringFile rf;
	
	//BMP格式太多（颜色数，压缩不压缩，扫描行倒序不倒序），因此这里
	//直接调用API载入，懒得自己写解码了。
   if(szFilename && !IS_INTRESOURCE(szFilename))
	{
		if(rf.Open(szFilename) && rf.Read(&bmfh,sizeof(BITMAPFILEHEADER)) && 
			rf.Read(&bmih,sizeof(BITMAPINFOHEADER)))
		{
  	  		if(bmfh.bfType != 0x4D42 || iFlag == DIB_QUERYSUPPORT)
				return (BOOL)(bmfh.bfType == 0x4D42);
				
			lprdib->Create(bmih.biWidth,bmih.biHeight>0?bmih.biHeight:-bmih.biHeight);
			hbm = (HBITMAP)LoadImage(NULL,szFilename,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
			if(hbm==NULL)
	   		return FALSE;
		}
		else
     		return FALSE;
   }
   else
   {
      hbm = LoadBitmap(GetInstance(),szFilename);

   	if(hbm==NULL)
     		return FALSE;
      GetObject(hbm,sizeof(BITMAP),&bm);
	   lprdib->Create(bm.bmWidth,bm.bmHeight);
   }

   lprdib->m_bmif.bmiHeader.biWidth = lprdib->m_width;
   lprdib->m_bmif.bmiHeader.biHeight = lprdib->m_height;

   HDC hDC = GetDC(lprdib->m_hWnd);
   GetDIBits(hDC,hbm,0,lprdib->m_height,lprdib->m_Bits,&lprdib->m_bmif,DIB_RGB_COLORS);
	ReleaseDC(lprdib->m_hWnd,hDC);

	lprdib->SetSrc(0,0);
	lprdib->SetDest(0,0);

   DeleteObject(hbm);

   return TRUE;
}

HBITMAP RingDIB::GetBitmap()
{
	if(m_Bits == NULL)
		return NULL;

	m_bmif.bmiHeader.biWidth=m_width;
   m_bmif.bmiHeader.biHeight=m_height;

	HDC hDC = GetDC(m_hWnd);
	HBITMAP hbm = CreateCompatibleBitmap(hDC,m_width,m_height);
	if(hbm)
		SetDIBits(hDC,hbm,0,m_height,m_Bits,&m_bmif,DIB_RGB_COLORS);
	
	ReleaseDC(m_hWnd,hDC);

	return hbm;
}

BOOL RingDIB::Import(HBITMAP hbm)
{
	BITMAP bm;

	if(hbm==NULL)
     	return FALSE;

   GetObject(hbm,sizeof(BITMAP),&bm);
	Create(bm.bmWidth,bm.bmHeight);
   
   m_bmif.bmiHeader.biWidth=m_width;
   m_bmif.bmiHeader.biHeight=m_height;

   HDC hDC = GetDC(m_hWnd);
   GetDIBits (hDC,hbm,0,m_height,m_Bits,&m_bmif,DIB_RGB_COLORS);
   ReleaseDC(m_hWnd,hDC);

	SetSrc(0,0);
   return TRUE;
}

BOOL SaveBMP(LPCTSTR szFilename,RingDIB* rdib)
{
	BITMAPFILEHEADER bmfh;
   BITMAPINFOHEADER bmih;
   ringFile rf = szFilename;
   LPBYTE lpb,lpd,lps;
   
   if(rdib == NULL)
      return FALSE;

	int w = rdib->Width(),h = rdib->Height();
   ZeroMemory(&bmfh,sizeof(BITMAPFILEHEADER));
   bmfh.bfType = 0x4d42;
	//cw必须是4的倍数((x+3)&0b1100[=0xC]为实现该功能的一个小小技巧）
	int cw = (int)(((w * 3) + 3) & 0xFFFFFFFC) ;

	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + cw * h;
   bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

   if(!rf.Create(RF_NEW))
      return FALSE;

   if(!rf.Write(&bmfh,sizeof(BITMAPFILEHEADER),TRUE))
   {
      rf.Close();
      return FALSE;
   }

   ZeroMemory(&bmih,sizeof(BITMAPINFOHEADER));
   bmih.biSize = sizeof(BITMAPINFOHEADER);
   bmih.biWidth = w;
   bmih.biHeight = h;
   bmih.biPlanes = 1;
   bmih.biBitCount = 24;
   bmih.biCompression = BI_RGB;

   if(!rf.Write(&bmih,sizeof(BITMAPINFOHEADER),TRUE))
      return FALSE;

   lpb = (LPBYTE)New(cw * h);
   if(lpb == NULL)
   {
      rf.Close();
      return FALSE;
   }

   lps = (LPBYTE)rdib->Data();
	lpd = lpb;

	if(lpd)
	{
//		   lpd = lpb + w * 3 * (h - 1);
//         int height_plus= w * 6;
      int n=0;   
		for(int j=0;j<h;j++)
		{
  			for(int i=0;i<w;i++)
			{
				lpd[0] = lps[0];
				lpd[1] = lps[1];
				lpd[2] = lps[2];
				lpd += 3;
				n++;
				lps += 4;
				if(n == 4)
					n = 0;
			}
			lpd += n;//lpb + j * cw;
			n = 0;
		}      
	}	
	
   if(!rf.Write(lpb,cw * h,TRUE))
   {
      Del(lpb);
      rf.Close();
      return FALSE;
   }

   Del(lpb);

   rf.Close();
   return TRUE;
}
