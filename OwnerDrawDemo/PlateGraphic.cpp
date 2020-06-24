#include "stdafx.h"
#include "PlateGraphic.h"

IMPLEMENT_DYNAMIC(CPlateGraphic, CButton)

CPlateGraphic::CPlateGraphic()
{
	PlateMatrix = nullptr;
	NumRows = NumCols = 0;
	WellWidth = WellHeight = 0;
	WellsCreated = false;
	RowColFont.CreateFont(18, 9, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("Segoe UI Light"));
	Border = 20;
}

CPlateGraphic::~CPlateGraphic()
{
	if (WellsCreated)
		DeleteObject(WellNumFont);

	DeleteObject(RowColFont);
	SetNumRowsCols(0, 0);
}

BEGIN_MESSAGE_MAP(CPlateGraphic, CButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CPlateGraphic::SetNumRowsCols(int rows, int cols)
{
	int c1, c2, r1, r2;
	CRect clientrect;

	if (rows == NumRows && cols == NumCols)
		return;

	if (PlateMatrix != nullptr)
	{
		for (int r=0; r<NumRows; r++)
			for (int c=0; c<NumCols; c++)
				PlateMatrix[r][c].DestroyWindow();

		delete[] PlateMatrix[0];
		delete[] PlateMatrix;
		PlateMatrix = nullptr;
		NumRows = 0;
		NumCols = 0;
		if (WellsCreated) DeleteObject(WellNumFont);
		WellsCreated = false;
		// return;
	}

	if (rows <= 0 || cols <= 0 || rows >= 100 || cols >= 100) // rows and/or cols is out of bounds
		return; 

	NumRows = rows;
	NumCols = cols;

	PlateMatrix = new CWellGraphic*[NumRows];
	PlateMatrix[0] = new CWellGraphic[NumRows*NumCols];

	for (int r=1; r<rows; r++)
		PlateMatrix[r] = &(PlateMatrix[0][r*cols]);

	GetClientRect(&clientrect);
	WellWidth = (clientrect.Width()-Border) / NumCols;
	WellHeight = (clientrect.Height()-Border) / NumRows;
	if (WellWidth < WellHeight)	WellHeight = WellWidth; else WellWidth = WellHeight;

	for (int r=0; r<NumRows; r++)
		for (int c=0; c<NumCols; c++)
		{
			c1 = WellWidth*c+Border;
			r1 = WellHeight*r+Border;
			c2 = WellWidth*(c+1)+Border;
			r2 = WellHeight*(r+1)+Border;
			PlateMatrix[r][c].Create(_T(""), BS_OWNERDRAW | WS_CHILD | WS_VISIBLE, CRect(c1, r1, c2, r2), this, r*NumCols+c);
		}

	WellNumFont.CreateFont(WellHeight*0.9, WellHeight*0.5, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("Segoe UI Light"));

	for (int r=0; r<NumRows; r++)
		for (int c=0; c<NumCols; c++)
			PlateMatrix[r][c].p_WellNumFont = &WellNumFont;

	WellsCreated = true;
}

void CPlateGraphic::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	if (!WellsCreated) return;

	CDC dc;
	dc.Attach(lpdis->hDC);
	CRect rect = lpdis->rcItem;
	CString dispstr;

	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(RowColFont);

	dc.SetTextAlign(TA_CENTER | TA_TOP);
	for (int c=0; c<NumCols; c++)
	{
		dispstr.Format(_T("%i"), c+1);
		dc.TextOut(Border + WellWidth*c + WellWidth/2, 0, dispstr);
	}

	dc.SetTextAlign(TA_LEFT | TA_BASELINE);
	for (int r=0; r<NumRows; r++)
	{
		dispstr.Format(_T("%c"), 'A' + r);
		dc.TextOut(3, Border + WellHeight*r + WellHeight/2, dispstr);
	}
		
	dc.Detach();
}

void CPlateGraphic::ResetTray()
{
	for (int r=0; r<NumRows; r++)
		for (int c=0; c<NumCols; c++)
			PlateMatrix[r][c].Invalidate();
}

void CPlateGraphic::OnTimer(UINT_PTR nIDEvent)
{
	CButton::OnTimer(nIDEvent);
}

BOOL CPlateGraphic::PreTranslateMessage(MSG* pMsg)
{	
	return CButton::PreTranslateMessage(pMsg);
}
