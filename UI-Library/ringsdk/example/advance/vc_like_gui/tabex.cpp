#include "tabex.h"

LRESULT RingTabEx::RingdowProc(HWND,RINGPARAMS param)
{
	LRESULT res;
	switch(param.uMsg)
   {
		case WM_LBUTTONDOWN:
			res = RingTab::RingdowProc(m_hWnd,param);
			InvalidateRect(m_hWnd,NULL,TRUE);
			return res;
	}	
	return RingTab::RingdowProc(m_hWnd,param);
}

LRESULT RingTabEx::OnPaint(RINGPARAMS& param)
{
	PAINTSTRUCT ps;
	HDC hDC;
	ringFont rf,rfb;
	LOGFONT lf;
	POINT pt[4];
	HRGN hrgn = NULL;

	rf.SetFont(m_hWnd);
	rf.GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	rfb.SetFont(&lf);

	hDC = BeginPaint(m_hWnd,&ps);
	
	if(m_dockbar)
		m_dockbar->DrawBkg(hDC);

	RECT rc;
	int nCnt = GetCount();
	LPSTR lps;
	int nSel = GetCurSel();
	
	SetBkMode(hDC,TRANSPARENT);
	SelectObject(hDC,rf.GetFont());
	for(int i=0;i<nCnt;i++)
	{
		GetItemRect(i,&rc);
		if(i == nSel)
		{
			pt[0].x = rc.left;
			pt[0].y = rc.top;
			pt[1].x = rc.left + 7;
			pt[1].y = rc.bottom;
			pt[2].x = rc.right;
			pt[2].y = rc.bottom;
			pt[3].x = rc.right + 7;
			pt[3].y = rc.top;
			hrgn = CreatePolygonRgn(pt,4,ALTERNATE);
			FillRgn(hDC,hrgn,(HBRUSH)GetStockObject(WHITE_BRUSH));

			SelectObject(hDC,rfb.GetFont());
			SelectObject(hDC,m_pen);
			MoveToEx(hDC,rc.left,rc.top,NULL);
			LineTo(hDC,rc.left + 7,rc.bottom);
			LineTo(hDC,rc.right,rc.bottom);
			LineTo(hDC,rc.right + 7,rc.top);

			lps = (LPSTR)GetText(i);
			OffsetRect(&rc,3,0);
			DrawText(hDC,lps,strlen(lps),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

			SelectObject(hDC,rf.GetFont());
			DeleteObject(hrgn);
		}
		else
		{
			SelectObject(hDC,m_pen);
			if(i == nSel + 1 && i != 0)
				MoveToEx(hDC,rc.left+3,rc.bottom-8,NULL);
			else
				MoveToEx(hDC,rc.left,rc.top,NULL);
			LineTo(hDC,rc.left + 7,rc.bottom);
			LineTo(hDC,rc.right,rc.bottom);
			if(i != nCnt-1)
				LineTo(hDC,rc.right + 4,rc.bottom - 8);
			else
				LineTo(hDC,rc.right + 7,rc.top);
			if((i == nSel + 1 && i != 0) || i == nCnt - 1)
			{
				MoveToEx(hDC,rc.left+7,rc.top,NULL);
				LineTo(hDC,rc.right + 7,rc.top);
			}
			else
			{
				MoveToEx(hDC,rc.left,rc.top,NULL);
				LineTo(hDC,rc.right,rc.top);
			}
			MoveToEx(hDC,rc.right-1,rc.bottom,NULL);
			SelectObject(hDC,m_penGray);
			if(i == nCnt - 1)
				LineTo(hDC,rc.right + 7,rc.top);
			else
				LineTo(hDC,rc.right + 3,rc.bottom - 8);

			lps = (LPSTR)GetText(i);
			OffsetRect(&rc,3,0);
			DrawText(hDC,lps,strlen(lps),&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}		
	}
	EndPaint(m_hWnd,&ps);
	return TRUE;
}

