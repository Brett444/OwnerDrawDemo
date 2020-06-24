// Animation.cpp : implementation file
//

#include "stdafx.h"
#include "Animation.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static TCHAR THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimation

CAnimation::CAnimation()
{
	firstdraw = 1;
	BackGndColor = RGB(255,255,255);
}

CAnimation::~CAnimation()
{
	if (!firstdraw)
	{
	   MemDC.SelectObject(OldBitmap);
	   delete NewBitmap;
	   MemDC.DeleteDC();
	}
}


BEGIN_MESSAGE_MAP(CAnimation, CButton)
	//{{AFX_MSG_MAP(CAnimation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimation message handlers

void CAnimation::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	DrawScene();
}

void CAnimation::DrawScene()
{
	CClientDC DrawDC(this);
	CRect WinRect;
    GetClientRect(&WinRect);
	CBrush BackBrush(BackGndColor), *OrigBrush;
	CPen BackPen(PS_SOLID, 1, BackGndColor), *OrigPen;

	if (firstdraw)
	{
       MemDC.CreateCompatibleDC(&DrawDC);
       NewBitmap = new CBitmap;
       NewBitmap->CreateCompatibleBitmap(&DrawDC, WinRect.right, WinRect.bottom);
       OldBitmap = MemDC.SelectObject(NewBitmap);
	   firstdraw = 0;
	}

//  MemDC.PatBlt(0, 0, WinRect.right, WinRect.bottom, WHITENESS);
	OrigBrush = MemDC.SelectObject(&BackBrush);
	OrigPen = MemDC.SelectObject(&BackPen);
	MemDC.Rectangle(&WinRect);
	MemDC.SelectObject(OrigBrush);
	MemDC.SelectObject(OrigPen);
	BackBrush.DeleteObject();
	BackPen.DeleteObject();
	OverrideAndDrawInHere(&MemDC);
	DrawDC.BitBlt(0,0,WinRect.right,WinRect.bottom,&MemDC,0,0,SRCCOPY);
}

void CAnimation::OverrideAndDrawInHere(CDC * pDC)
{

}

void CAnimation::SetBackGndColor(COLORREF backgndcolor)
{
   BackGndColor = backgndcolor;
}

