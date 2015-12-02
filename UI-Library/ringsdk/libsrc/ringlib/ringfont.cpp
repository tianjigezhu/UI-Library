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
//							RingSDK类库 ringlib.lib
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
//原文件名：ringfont.cpp
//
//说明：字体类实现代码
//
**********************************************************************/
#define MAKE_SELF_LIB

//#ifndef __BORLANDC__
//#include "ringlib.h"
//#else
#include "ringlib.h"
//#endif

ringFont::ringFont()
{
	m_font = NULL;
	m_bExtern = FALSE;
	m_crColor = 0;
}

ringFont::ringFont(LPCTSTR szFontname,int size,int nStyle/*=0*/,int angle/*=0*/)
{
	m_font = NULL;
	SetFont(szFontname,size,nStyle,angle);
	m_bExtern = FALSE;
}

ringFont::~ringFont()
{
	if(m_font && !m_bExtern)
		DeleteObject(m_font);
}

BOOL ringFont::GetLogFont(LOGFONT* lpLogFont)
{
	if(lpLogFont && m_font && GetObject(m_font,sizeof(LOGFONT),lpLogFont))
		return TRUE;
	return FALSE;
}

BOOL ringFont::GetWindowLogFont(HWND hWnd,LOGFONT* lpLogFont)
{
	if(lpLogFont && hWnd)
	{
		HFONT hf = (HFONT)SendMessage(hWnd,WM_GETFONT,0,0);
		if(hf && GetObject(hf,sizeof(LOGFONT),lpLogFont))
			return TRUE;
	}
	return FALSE;
}

BOOL ringFont::SetFont(HFONT hfont)
{
	if(m_font && !m_bExtern)
		DeleteObject(m_font);
	m_font = hfont;
	m_bExtern=TRUE;
	return (BOOL)hfont;
}

BOOL ringFont::SetFont(LOGFONT* lpLogFont)
{
	if(lpLogFont)
	{
		if(m_font && !m_bExtern)
			DeleteObject(m_font);

		memcpy(&m_lpMainFontRec,lpLogFont,sizeof(LOGFONT));

		m_font = CreateFontIndirect(&m_lpMainFontRec);
		m_bExtern = FALSE;
		return (BOOL)m_font;
	}
	return FALSE;
}

BOOL ringFont::SetFont(HWND hWnd)
{
	HFONT hf = (HFONT)SendMessage(hWnd,WM_GETFONT,0,0);
	if(hf)
	{
		if(GetObject(hf,sizeof(LOGFONT),&m_lpMainFontRec))
			return SetFont(&m_lpMainFontRec);
	}
	return FALSE;
}

BOOL ringFont::SetFont(LPCTSTR szFontname,int size,int nStyle/*=0*/,int angle/*=0*/)
{
	if(m_font && !m_bExtern)
		DeleteObject(m_font);

	//LPLOGFONT m_lpMainFontRec = (LPLOGFONT)New(sizeof(LOGFONT));
	
   //if(m_lpMainFontRec)
   //{
		int style = nStyle / 100 * 100;
		int flag = nStyle % 100;

		if(size < 0 || (flag & RINGFONT_PIXEL) != 0)
			m_lpMainFontRec.lfHeight = size;
		else
		{
			HDC hDC = GetDC(HWND_DESKTOP);
			m_lpMainFontRec.lfHeight = -MulDiv(size,GetDeviceCaps(hDC,LOGPIXELSY),72);
			ReleaseDC(HWND_DESKTOP,hDC);
		}
   	m_lpMainFontRec.lfWidth = 0;
	   m_lpMainFontRec.lfEscapement = angle;
   	m_lpMainFontRec.lfOrientation = angle;
		if(style == 0 && (flag & RINGFONT_BOLD))
			m_lpMainFontRec.lfWeight = FW_BOLD;
		else
			m_lpMainFontRec.lfWeight = style;
	   m_lpMainFontRec.lfItalic = (BYTE)(flag & RINGFONT_ITALIC);
		m_lpMainFontRec.lfUnderline = (BYTE)(flag & RINGFONT_UNDERLINE);
	   m_lpMainFontRec.lfStrikeOut = (BYTE)(flag & RINGFONT_STRIKEOUT);
	   m_lpMainFontRec.lfCharSet = DEFAULT_CHARSET;
   	m_lpMainFontRec.lfOutPrecision = OUT_DEFAULT_PRECIS;
	   m_lpMainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   	m_lpMainFontRec.lfQuality = PROOF_QUALITY;
	   m_lpMainFontRec.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
		strcpy(m_lpMainFontRec.lfFaceName,szFontname);
	   
		m_font = CreateFontIndirect(&m_lpMainFontRec);

		m_bExtern = FALSE;
   //}
	//Del(m_lpMainFontRec);
	return (BOOL)m_font;
}

void ringFont::SetWindowFont(HWND hWnd,LPCTSTR szFontname/*=NULL*/,int size/*=NULL*/,int nStyle/*=0*/)
{
	if(szFontname != NULL && size != 0)
		SetFont(szFontname,size,nStyle);

	if(m_font)
		SendMessage(hWnd,WM_SETFONT,(WPARAM)m_font,MAKELPARAM(TRUE,0));
}

HFONT ringFont::SetFontAngle(int nAngle)
{
	if(m_font)
	{
		if(!m_bExtern)
			DeleteObject(m_font);

		m_lpMainFontRec.lfEscapement = nAngle;
		m_lpMainFontRec.lfOrientation = nAngle;

		m_font = CreateFontIndirect(&m_lpMainFontRec);
		m_bExtern = FALSE;
	}
	return m_font;
}

HFONT ringFont::Select(HWND hWnd/*=NULL*/)
{
	CHOOSEFONT CF;
	memset(&CF,0,sizeof(CHOOSEFONT));
	//LOGFONT FontRec = m_lpMainFontRec;
	
	CF.lStructSize    = sizeof(CF);
	CF.hwndOwner      = hWnd;
	CF.Flags          = CF_ANSIONLY | CF_TTONLY | CF_SCREENFONTS |
      CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;// | CF_ENABLETEMPLATE;
	CF.nFontType      = SCREEN_FONTTYPE;
	CF.lpLogFont      = &m_lpMainFontRec;
	CF.nSizeMin       = 20;
	CF.nSizeMax       = 20;
	CF.rgbColors      = m_crColor;
	//CF.lpTemplateName = "FontDlg";
	CF.hInstance      = GetInstance();
	
	if (ChooseFont(&CF))
	{
		m_crColor = CF.rgbColors;
		if(m_font && !m_bExtern)
			DeleteObject(m_font);
		m_font = CreateFontIndirect(&m_lpMainFontRec);
		m_bExtern = FALSE;
	}
	return m_font;
}

HFONT ringFont::AutoAdjust(LPCTSTR szText,int w,int h,HDC hDC/* = NULL*/)
{
	if(m_font && szText)
	{
		SIZE size;
		int len = strlen(szText);
		BOOL bDelDC = FALSE;

		if(hDC == NULL)
		{
			hDC = GetDC(GetMainWnd());
			bDelDC = TRUE;
		}
		
		SelectObject(hDC,m_font);
		
		GetTextExtentPoint32(hDC,szText,len,&size);

		for(;(size.cx < w && m_lpMainFontRec.lfWidth);m_lpMainFontRec.lfWidth ++)
		{			
			m_font = CreateFontIndirect(&m_lpMainFontRec);
			DeleteObject((SelectObject(hDC,m_font)));
			GetTextExtentPoint32(hDC,szText,len,&size);
		}
		for(;(size.cx > w && m_lpMainFontRec.lfWidth);m_lpMainFontRec.lfWidth --)
		{			
			m_font = CreateFontIndirect(&m_lpMainFontRec);
			DeleteObject((SelectObject(hDC,m_font)));
			GetTextExtentPoint32(hDC,szText,len,&size);
		}
		for(;(size.cy < h && m_lpMainFontRec.lfHeight);m_lpMainFontRec.lfHeight ++)
		{			
			m_font = CreateFontIndirect(&m_lpMainFontRec);
			DeleteObject((SelectObject(hDC,m_font)));
			GetTextExtentPoint32(hDC,szText,len,&size);
		}
		for(;(size.cy > h && m_lpMainFontRec.lfHeight);m_lpMainFontRec.lfHeight --)
		{			
			m_font = CreateFontIndirect(&m_lpMainFontRec);
			DeleteObject((SelectObject(hDC,m_font)));
			GetTextExtentPoint32(hDC,szText,len,&size);
		}
		if(bDelDC)
			ReleaseDC(GetMainWnd(),hDC);
	}
	return m_font;
}
