#include "stdafx.h"
#include "WellGraphic.h"
// #include "TGraphics.h"

IMPLEMENT_DYNAMIC(CWellGraphic, CButton)

CWellGraphic::CWellGraphic(void)
{
	NumCells = 2;
	DesiredCells = 3;
}

CWellGraphic::~CWellGraphic(void)
{
}

BEGIN_MESSAGE_MAP(CWellGraphic, CButton)
END_MESSAGE_MAP()

void CWellGraphic::OverrideAndDrawInHere(CDC *pDC)
{
	CRect clientrect;
	CBrush *oldbrush, br1(RGB(240,240,240)), br2(GetWellColor());
	CPen *oldPen;
	CPen ellipsepen(PS_SOLID, 1, RGB(0,0,0));
	CString dispstr;

	GetClientRect(clientrect);
	oldbrush = pDC->SelectObject(&br1);
	oldPen = pDC->SelectObject(&ellipsepen);
	pDC->FillRect(clientrect, &br1);

	pDC->SelectObject(&br2);
	pDC->Ellipse(clientrect.left, clientrect.top, clientrect.right, clientrect.bottom);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SelectObject(*p_WellNumFont);
	dispstr.Format(_T("%i"), NumCells);
	pDC->TextOut(clientrect.right/2, clientrect.bottom * 0.9, dispstr);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldbrush);
}

COLORREF CWellGraphic::GetWellColor()
{
	if (NumCells < DesiredCells)
		return(RGB(0, 255, 255));

	if (DesiredCells == NumCells)
		return(RGB(0, 255, 0));

	return(RGB(255, 0, 0));
}