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
//原文件名：png.cpp
//
//说明：载入PNG图象。
// 		PNG的解码/编码用到了两个现成的类库png.lib和zlib.lib，文件位置：
//			free_lib\png和free_lib\zlib.原始版权信息请见这两个类库目录中
//			的代码和版权信息。
//
**********************************************************************/
#define MAKE_SELF_LIB
#include "ringdib.h"
#include "png.h"

void ImportRGB(LPBYTE lpd,LPBYTE* lpb,int width,int height);
void ImportRGBA(LPBYTE lpd,LPBYTE* lpb,int width,int height);
void ImportGray(LPBYTE lpd,LPBYTE* lpb,int width,int height);
void ImportGrayA(LPBYTE lpd,LPBYTE* lpb,int width,int height);

BOOL LoadPNG(LPCTSTR szFilename,RingDIB* rdib,int iFlag,LPSTR lpszFormat)
{
	if(iFlag == DIB_GETFORMAT)
	{
		if(lpszFormat)
		{
			wsprintf(lpszFormat,"png");
			return TRUE;
		}
		else
			return FALSE;
	}
	
	ringFile rf(szFilename);
   rf.Open();

	int nread = 8;
	BYTE buffer[8];
	
	if(!rf.Read(buffer,nread,TRUE))
		return FALSE;	
	
	//检测文件头
	if(png_sig_cmp(buffer,0,nread) != 0)
		return FALSE;
	else if(iFlag == DIB_QUERYSUPPORT)
		return TRUE;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if(png_ptr == NULL)
		return FALSE;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	
	if(info_ptr == NULL)
	{
		png_destroy_read_struct(&png_ptr,0,0);
		return FALSE;
	}
	
	//保存堆栈现场
	if(setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr,&info_ptr,0);
		return FALSE;
	}
	
	png_ptr->io_ptr = (png_voidp)rf.Handle();
	//恢复文件指针
	rf.Seek(0);
	//图象解码
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	
	//导入解码后的图象数据
	if(rdib->Create(png_get_image_width(png_ptr,info_ptr),png_get_image_height(png_ptr,info_ptr)))
	{
		LPBYTE* lpb = png_get_rows(png_ptr,info_ptr);
		LPBYTE lpd = (LPBYTE)rdib->Data();

		//获取颜色信息
		switch(png_get_color_type(png_ptr,info_ptr))
		{
		case PNG_COLOR_TYPE_RGB:
			ImportRGB(lpd,lpb,rdib->Width(),rdib->Height());
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			ImportRGBA(lpd,lpb,rdib->Width(),rdib->Height());
			break;
		case PNG_COLOR_TYPE_GRAY:
			ImportGray(lpd,lpb,rdib->Width(),rdib->Height());
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			ImportGrayA(lpd,lpb,rdib->Width(),rdib->Height());
			break;
		}
	}
	//释放内存
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	
	rf.Close();
	return TRUE;
}

BOOL SavePNG(LPCTSTR szFilename,RingDIB* rdib)
{
	if(rdib)
	{
		return TRUE;
	}
	else
		return FALSE;
}

void ImportRGB(LPBYTE lpd,LPBYTE* lpb,int width,int height)
{
	int nwCnt = width * sizeof(COLORREF);
	int nw = width * 3;
	LPBYTE lpLine = lpd + (height - 1) * nwCnt;

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < nw; j += 3)
		{
			*lpLine ++ = lpb[i][j + 2];	// blue
			*lpLine ++ = lpb[i][j + 1];	// green
			*lpLine ++ = lpb[i][j];		// red
			*lpLine ++;
		}
		
		lpLine -= (nwCnt << 1);
	}
}

void ImportRGBA(LPBYTE lpd,LPBYTE* lpb,int width,int height)
{
	int nwCnt = width * sizeof(COLORREF);
	LPBYTE lpLine = lpd + (height - 1) * nwCnt;

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < nwCnt; j += 4)
		{
			*lpLine ++ = lpb[i][j + 2];	// blue
			*lpLine ++ = lpb[i][j + 1];	// green
			*lpLine ++ = lpb[i][j];		// red
			*lpLine ++ = lpb[i][j + 3];	// alpha
		}
		
		lpLine -= (nwCnt << 1);
	}
}

void ImportGray(LPBYTE lpd,LPBYTE* lpb,int width,int height)
{
	BYTE cr;
	COLORREF* lpLine = (COLORREF*)lpd;
	lpLine += (height - 1) * width;
	
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j ++)
		{
			cr = lpb[i][j];
			*lpLine ++ = RGB(cr,cr,cr);			
		}		
		lpLine -= (width << 1);
	}
}

void ImportGrayA(LPBYTE lpd,LPBYTE* lpb,int width,int height)
{
	BYTE cr;
	int nwCnt = width << 1;
	COLORREF* lpLine = (COLORREF*)lpd;
	lpLine += (height - 1) * width;
	
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < nwCnt; j += 2)
		{
			cr = lpb[i][j];
			*lpLine ++ = (RGB(cr,cr,cr) | (((COLORREF)lpb[i][j+1]) << 24));			
		}		
		lpLine -= (width << 1);
	}
}
