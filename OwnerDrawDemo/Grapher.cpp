// Grapher.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "Grapher.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static TCHAR THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CGrapher

CGrapher::CGrapher()
{
	FirstTime = 1;
	LButtonDown = 0;
	DraggingAPoint = 0;
	NumOfDataSets = 0;
	ZeroTheDataSets();
	XAxis.MinVal = 0;
	XAxis.MaxVal = 10;
	XAxis.DivStart = 0;
	XAxis.Div = 2;
	YAxis.MinVal = 0;
	YAxis.MaxVal = 10;
	YAxis.DivStart = 0;
	YAxis.Div = 2;
	StoredXAxis = XAxis;
	StoredYAxis = YAxis;
	DrawZoomBox = 0;
	OkToChangePoints = 0;
	OkToDragInXDir = 1;
	DraggingAPoint = 0;
	MouseOnPoint = 0;
	MouseMoved = 0;
	Interpolate = 1;

	XAxisHasBeenSet = 0;
	YAxisHasBeenSet = 0;

	HoldFlag = 0;
}

CGrapher::~CGrapher()
{
}


BEGIN_MESSAGE_MAP(CGrapher, CAnimation)
	//{{AFX_MSG_MAP(CGrapher)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrapher message handlers

void CGrapher::SetParent(CWnd *pparent)
{
	pParent = pparent;
}

void CGrapher::AddDataSet(double *xdata, double *ydata, int buffersize, int head, int numpoints, int id, COLORREF color, int linestyle)
{
	int i;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
			return;

	HoldFlag = 1;

	if (NumOfDataSets < MAXNUMOFDATASETS)
	{
		DataSet[NumOfDataSets].Xptr = xdata;
		DataSet[NumOfDataSets].Yptr = ydata;
		DataSet[NumOfDataSets].BufferSize = buffersize;
		DataSet[NumOfDataSets].Head = head;
		DataSet[NumOfDataSets].NumPoints = numpoints;
		DataSet[NumOfDataSets].SetId = id;
		DataSet[NumOfDataSets].Color = color;
		DataSet[NumOfDataSets].LineStyle = linestyle;
		NumOfDataSets++;
	}

	HoldFlag = 0;
}

void CGrapher::DeleteDataSet(int id)
{
	int i, j;

	HoldFlag = 1;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
		{
			for(j=i; j<NumOfDataSets-1; j++)
				DataSet[j] = DataSet[j+1];
			NumOfDataSets--;
		}

	HoldFlag = 0;
}

void CGrapher::DeleteAllDataSets()
{
	NumOfDataSets = 0;
	ZeroTheDataSets();
}

void CGrapher::SetInterpolate(int interp)
{
	Interpolate = interp;
}

void CGrapher::SetLineStyle(int id, int linestyle)
{
	int i;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
		{
			DataSet[i].LineStyle = linestyle;
		}
}

void CGrapher::SetHeadAndNumPoints(int id, int head, int numpoints)
{
	int i;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
		{
			DataSet[i].Head = head;
			DataSet[i].NumPoints = numpoints;
		}
}

void CGrapher::SetGraphColor(int id, COLORREF color)
{
	int i;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
		{
			DataSet[i].Color = color;
		}
}

int CGrapher::GetNumOfDataSets()
{
	return(NumOfDataSets);
}

int CGrapher::IdExists(int id)
{
	int i, retval=0;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
			retval = 1;

	return(retval);
}

void CGrapher::ChangeId(int id, int newid)
{
	int i;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
		{
			DataSet[i].SetId = newid;
		}
}

void CGrapher::ZeroTheDataSets()
{
	int i;

	HoldFlag = 1;

	for(i=0; i<MAXNUMOFDATASETS; i++)
	{
		DataSet[i].Xptr = NULL;
		DataSet[i].Yptr = NULL;
		DataSet[i].BufferSize = 0;
		DataSet[i].Head = 0;
		DataSet[i].NumPoints = 0;
		DataSet[i].SetId = 0;
		DataSet[i].Color = 0;
		DataSet[i].LineStyle = GT_LINE;
	}

	HoldFlag = 0;
}

void CGrapher::OverrideAndDrawInHere(CDC *pDC)
{
    RECT WinRect;

	HoldFlag = 1;

	if (FirstTime)
	{
	    GetClientRect(&WinRect);
		XAxis.MinPix = (int)(WinRect.right*XPERCENTBORDER);
		XAxis.MaxPix = WinRect.right;
		YAxis.MinPix = (int)(WinRect.bottom*(1-YPERCENTBORDER));
		YAxis.MaxPix = 0;
		if (!XAxisHasBeenSet)
			AutoScaleXAxis();
		if (!YAxisHasBeenSet)
			AutoScaleYAxis();
		FirstTime = 0;
	}
	DrawAxis(pDC);
	DrawDataSets(pDC);
	if (DrawZoomBox)
		DrawBox(pDC, DragBoxStart.x, DragBoxStart.y, DragBoxPoint.x, DragBoxPoint.y);

	if (DraggingAPoint || MouseOnPoint)
		DrawCoords(pDC);

	HoldFlag = 0;
}

void CGrapher::DrawAxis(CDC *pDC)
{
	DrawBox(pDC, XAxis.MinPix, YAxis.MaxPix-1, XAxis.MaxPix, YAxis.MinPix);
	DrawDivisions(pDC);
	LabelDivisions(pDC);
}

void CGrapher::DrawDataSets(CDC *pDC)
{
	 int i;
	 CPen *oldpen, DataPen;
	 CBrush *oldbrush, DataBrush;
	 CRgn ClipRegion;

	 ClipRegion.CreateRectRgn(XAxis.MinPix, YAxis.MaxPix, XAxis.MaxPix, YAxis.MinPix);
	 pDC->SelectClipRgn(&ClipRegion);

	 for(i=0; i<NumOfDataSets; i++)
	 {
		 if (DataSet[i].NumPoints > 0)
		 {
			 if ( (DataSet[i].LineStyle == GT_DASHEDLINE)    ||
				  (DataSet[i].LineStyle == GT_DASHEDLINEDOT) ) 
				 DataPen.CreatePen(PS_DASH, 1, DataSet[i].Color);
			 else
				 DataPen.CreatePen(PS_SOLID, 1, DataSet[i].Color);

			 oldpen = (CPen *)pDC->SelectObject(&DataPen);
			 DataBrush.CreateSolidBrush(DataSet[i].Color);
			 oldbrush = (CBrush *)pDC->SelectObject(&DataBrush);
			 switch(DataSet[i].LineStyle)
			 {
				 case GT_LINE:
					 DrawDataAsLines(pDC, i);
					 break;
				 case GT_DOT:
					 DrawDataAsDots(pDC, i);
					 break;
				 case GT_LINEDOT:
					 DrawDataAsLines(pDC, i);
					 DrawDataAsDots(pDC, i);
					 break;
				 case GT_DASHEDLINE:
					 DrawDataAsLines(pDC, i);
					 break;
				 case GT_DASHEDLINEDOT:
					 DrawDataAsLines(pDC, i);
					 DrawDataAsDots(pDC, i);
					 break;
			 }
			 pDC->SelectObject(oldbrush);
			 pDC->SelectObject(oldpen);
			 DataBrush.DeleteObject();
			 DataPen.DeleteObject();
		 }
	 }
	 pDC->SelectClipRgn(NULL);
	 ClipRegion.DeleteObject();
}

void CGrapher::DrawBox(CDC *pDC, int left, int top, int right, int bottom)
{
	pDC->MoveTo(left, top);
	pDC->LineTo(right, top);
	pDC->LineTo(right, bottom);
	pDC->LineTo(left, bottom);
	pDC->LineTo(left, top);
}

void CGrapher::SetDivXValues()
{
	 XAxis.Div = (XAxis.MaxVal - XAxis.MinVal) / 5.0;
	 XAxis.Div = round125(XAxis.Div);

	 if (XAxis.Div != 0)
		XAxis.DivStart = floor(XAxis.MinVal / XAxis.Div) * XAxis.Div;
	 else
	 {
		XAxis.Div = XAxis.MaxVal - XAxis.MinVal;
		XAxis.DivStart = XAxis.MinVal;
     } 			 

	 if (XAxis.DivStart < XAxis.MinVal)
		 XAxis.DivStart += XAxis.Div;
}

void CGrapher::SetDivYValues()
{
	 YAxis.Div = (YAxis.MaxVal - YAxis.MinVal) / 5.0;
	 YAxis.Div = round125(YAxis.Div);

	 if (YAxis.Div != 0)
		YAxis.DivStart = floor(YAxis.MinVal / YAxis.Div) * YAxis.Div;
	 else
	 {
		YAxis.Div = YAxis.MaxVal - YAxis.MinVal;
		YAxis.DivStart = YAxis.MinVal;
     } 			 

	 if (YAxis.DivStart < YAxis.MinVal)
		 YAxis.DivStart += YAxis.Div;
}

void CGrapher::AutoScaleXAxis()
{
	int i, j, c;
	bool firstset=true;

	if (NumOfSetsWithPnts() == 0)
	{
		XAxis.MinVal = 0;
		XAxis.MaxVal = 10;
		XAxis.DivStart = 0;
		XAxis.Div = 2;
	}
	else
	{
		for(i=0; i<NumOfDataSets; i++)
		{
			if (DataSet[i].NumPoints > 0 && firstset)
			{
				XAxis.MinVal = DataSet[i].Xptr[DataSet[i].Head];
				XAxis.MaxVal = DataSet[i].Xptr[DataSet[i].Head];
				firstset = false;
			}

			for (c=0, j=DataSet[i].Head; c < DataSet[i].NumPoints; c++, j=CircInc(j,i))
			{
				if (DataSet[i].Xptr[j] < XAxis.MinVal)
					  XAxis.MinVal = DataSet[i].Xptr[j];
				if (DataSet[i].Xptr[j] > XAxis.MaxVal)
					  XAxis.MaxVal = DataSet[i].Xptr[j];
			}
		}
		if (XAxis.MinVal == XAxis.MaxVal)
		{
			XAxis.MinVal--; XAxis.MaxVal++;
		}
		SetDivXValues();
	}
}

void CGrapher::AutoScaleYAxis()
{
	int i, j, c;
	double span;
	bool firstset=true;

	if (NumOfSetsWithPnts() == 0)
	{
		YAxis.MinVal = 0;
		YAxis.MaxVal = 10;
		YAxis.DivStart = 0;
		YAxis.Div = 2;
	}
	else
	{
		for(i=0; i<NumOfDataSets; i++)
		{
			if (DataSet[i].NumPoints > 0 && firstset)
			{
				YAxis.MinVal = DataSet[i].Yptr[DataSet[i].Head];
				YAxis.MaxVal = DataSet[i].Yptr[DataSet[i].Head];
				firstset = false;
			}

			for (c=0, j=DataSet[i].Head; c<DataSet[i].NumPoints; c++, j=CircInc(j,i))
			{
				if (DataSet[i].Yptr[j] < YAxis.MinVal)
					  YAxis.MinVal = DataSet[i].Yptr[j];
				if (DataSet[i].Yptr[j] > YAxis.MaxVal)
					  YAxis.MaxVal = DataSet[i].Yptr[j];
			}
		}

		if (YAxis.MinVal == YAxis.MaxVal)
		{
			YAxis.MinVal--; YAxis.MaxVal++;
		}
		else
		{
			span = (YAxis.MaxVal - YAxis.MinVal) * 0.1;
			YAxis.MaxVal += span;  YAxis.MinVal -= span;
		}

		SetDivYValues();
	}
}

// public function used to manually set border params
void CGrapher::SetYAxisParams(CAxisParams axisparams, int autodiv)
{
	YAxis.MinVal = axisparams.MinVal;
	YAxis.MaxVal = axisparams.MaxVal;
	if (autodiv)
	{
		SetDivYValues();
	}
	else
	{
		YAxis.Div = axisparams.Div;
		YAxis.DivStart = axisparams.DivStart;
	}
	YAxisHasBeenSet = 1;
}

// public function used to manually set border params
void CGrapher::SetXAxisParams(CAxisParams axisparams, int autodiv)
{
	XAxis.MinVal = axisparams.MinVal;
	XAxis.MaxVal = axisparams.MaxVal;
	if (autodiv)
	{
		SetDivXValues();
	}
	else
	{
		XAxis.Div = axisparams.Div;
		XAxis.DivStart = axisparams.DivStart;
	}
	XAxisHasBeenSet = 1;
}

void CGrapher::StoreAxisParams()
{
	StoredXAxis = XAxis;
	StoredYAxis = YAxis;
}

void CGrapher::RestoreAxisParams()
{
	XAxis = StoredXAxis;
	YAxis = StoredYAxis;
}

void CGrapher::DrawDivisions(CDC *pDC)
{
	 int i;

	 i=0;
	 while(YAxis.Div*i+YAxis.DivStart <= YAxis.MaxVal)
	 {
			pDC->MoveTo(XAxis.MinPix-10, YValToPixel(YAxis.Div*i + YAxis.DivStart));
			pDC->LineTo(XAxis.MinPix, YValToPixel(YAxis.Div*i + YAxis.DivStart));
			i++;
	 }

	 i=0;
	 while(XAxis.Div*i+XAxis.DivStart <= XAxis.MaxVal)
	 {
			pDC->MoveTo(XValToPixel(XAxis.Div*i + XAxis.DivStart), YAxis.MinPix);
			pDC->LineTo(XValToPixel(XAxis.Div*i + XAxis.DivStart), YAxis.MinPix+10);
			i++;
	 }
}

void CGrapher::LabelDivisions(CDC *pDC)
{
	 int i, xloc;
	 CString axisstr;
	 CString formatstr;
	 double axisval;

	 pDC->SelectClipRgn(NULL);
	 i=0;
	 pDC->SetBkMode(TRANSPARENT);
	 pDC->SetTextAlign(TA_RIGHT | TA_TOP);
	 while(YAxis.Div*i+YAxis.DivStart <= YAxis.MaxVal)
	 {
			axisval = YAxis.Div*i+YAxis.DivStart;
			formatstr.Format(_T("%c0.%dlf"), _T('%'), NumOfDecPlaces(YAxis.Div));
			axisstr.Format(formatstr, axisval);
			pDC->TextOut(XAxis.MinPix-13, YValToPixel(axisval)+1, axisstr);
			i++;
	 }

	 i=0;
	 pDC->SetTextAlign(TA_CENTER | TA_TOP);
	 while(XAxis.Div*i+XAxis.DivStart <= XAxis.MaxVal)
	 {
			axisval = XAxis.Div*i+XAxis.DivStart;
			formatstr.Format(_T("%c0.%dlf"), _T('%'), NumOfDecPlaces(XAxis.Div));
			axisstr.Format(formatstr, axisval);
			xloc = XValToPixel(axisval);
			if (XAxis.Div*(i+1)+XAxis.DivStart > XAxis.MaxVal)
			{
				 pDC->SetTextAlign(TA_RIGHT | TA_TOP);
				 xloc -= 2;
			}
			pDC->TextOut(xloc, YAxis.MinPix+13, axisstr);
			i++;
	 }
}

int CGrapher::NumOfDecPlaces(double val)
{
	 int i=0;
	 double tol;

	 tol = 0.000000000001;

	 while(fabs(val - floor(val+0.5)) > tol)
	 {
		val *= 10;
		i++;
		tol *= 10;
	 }
	 return(i);
}

int CGrapher::XValToPixel(double xval)
{
	 double slope, xpix, maxval, minval;

	 maxval = 32767;
	 minval = -maxval;

	 if (XAxis.MaxVal != XAxis.MinVal)
		slope = (XAxis.MaxPix - XAxis.MinPix) / (XAxis.MaxVal - XAxis.MinVal);
	 else
		slope = 1;
	 xpix = (xval - XAxis.MinVal) * slope + XAxis.MinPix;
	 xpix = floor(xpix+0.5);

	 if (xpix > maxval)
		 xpix = maxval;
	 if (xpix < minval)
		 xpix = minval;

	 return((int)xpix);
}

int CGrapher::YValToPixel(double yval)
{
	 double slope, ypix, maxval, minval;

	 maxval = 32767;
	 minval = -maxval;

	 if (YAxis.MinVal != YAxis.MaxVal)
		slope = (YAxis.MinPix - YAxis.MaxPix) / (YAxis.MinVal - YAxis.MaxVal);
	 else
	    slope = 1;
	 ypix = (yval - YAxis.MaxVal) * slope + 0;
	 ypix = floor(ypix+0.5);

	 if (ypix > maxval)
		 ypix = maxval;
	 if (ypix < minval)
		 ypix = minval;

	 return((int)ypix);
}

double CGrapher::XPixelToVal(int xpix)
{
	 double slope, xval;

	 if (XAxis.MaxPix != XAxis.MinPix)
		slope =  (XAxis.MaxVal - XAxis.MinVal) / (XAxis.MaxPix - XAxis.MinPix);
	 else
		slope = 1;
	 xval = (xpix - XAxis.MinPix) * slope + XAxis.MinVal;
	 return((xval));
}

double CGrapher::YPixelToVal(int ypix)
{
	 double slope, yval;

	 if (YAxis.MaxPix != YAxis.MinPix)
		slope =  (YAxis.MaxVal - YAxis.MinVal) / (YAxis.MaxPix - YAxis.MinPix);
	 else
		slope = 1;
	 yval = (ypix - 0) * slope + YAxis.MaxVal;
	 return((yval));
}

double CGrapher::XToTheY(double x, int y)
{
	 BOOL reciprical;
	 double retval;

	 if (y < 0)
	 {
		y *= -1;
		reciprical = TRUE;
	 }
	 else
		reciprical = FALSE;

	 retval = 1;
	 for (int i=1; i<=y; i++)
		retval *= x;

	 if (reciprical)
		retval = 1 / retval;

	 return(retval);
}

void CGrapher::DrawDataAsLines(CDC *pDC, int set)
{
	 int j, c;

	 j = DataSet[set].Head;
	 pDC->MoveTo(XValToPixel(DataSet[set].Xptr[j]), YValToPixel(DataSet[set].Yptr[j]));
	 j = CircInc(j, set);

	 if (Interpolate)
	 {
		for (c=1; c<DataSet[set].NumPoints; c++, j=CircInc(j, set))
			pDC->LineTo(XValToPixel(DataSet[set].Xptr[j]), YValToPixel(DataSet[set].Yptr[j]));
	 }
	 else
	 {
		for (c=1; c<DataSet[set].NumPoints; c++, j=CircInc(j, set))
		{
			pDC->LineTo(XValToPixel(DataSet[set].Xptr[j]), YValToPixel(DataSet[set].Yptr[j-1]));
			pDC->LineTo(XValToPixel(DataSet[set].Xptr[j]), YValToPixel(DataSet[set].Yptr[j]));
		}
	 }
}

void CGrapher::DrawDataAsDots(CDC *pDC, int set)
{
	 int j, x1, y1, x2, y2, c;

	 for (c=0, j=DataSet[set].Head; c<DataSet[set].NumPoints; c++, j=CircInc(j, set))
	 {
		 x1 = XValToPixel(DataSet[set].Xptr[j]) - POINTRADIUS;
		 y1 = YValToPixel(DataSet[set].Yptr[j]) - POINTRADIUS;
		 x2 = x1 + 2*POINTRADIUS;
		 y2 = y1 + 2*POINTRADIUS;

		 if ( (x2 >= XValToPixel(XAxis.MinVal)) && (x1 <= XValToPixel(XAxis.MaxVal)) &&
			  (y2 >= YValToPixel(YAxis.MaxVal)) && (y1 <= YValToPixel(YAxis.MinVal)) )
			 pDC->Ellipse(x1, y1, x2, y2);
	 }

}

int CGrapher::CircInc(int ind, int set)
{
	int retval;

	retval = ind + 1;
	if (retval >= DataSet[set].BufferSize)
		retval = 0;

	return(retval);
}

double CGrapher::RoundToNPlaces(double val, int n)
{
	if (n>=0)
	{
	   val *= pow(10.0, n);
	   val = (long int)(val + 0.5);
	   val /= pow(10.0, n);
	}

	return(val);
}

double CGrapher::round125(double val)
{
	 int i;
	 double newval;

	 if (val > 1)
	 {
		i = 0;
		while (val >= 7.5)
		{
			 val /= 10.0;
			 i++;
		}
		newval = ClosestTo125(val);
		if (i>0)
			 newval *= XToTheY(10, i);
	 }
	 else
	 {
		i = 0;
		while (val < .75)
		{
			 val *= 10.0;
			 i++;
		}
		newval = ClosestTo125(val);
		if (i>0)
			 newval /= XToTheY(10, i);
	 }

	 return(newval);
}

double CGrapher::ClosestTo125(double val)
{
	 if (val < 1.5)
			return(1);
	 if (val < 3.5)
			return(2);
	 return(5);
}

void CGrapher::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (!DraggingAPoint)
	{
		SetCapture();
		LButtonDown = TRUE;
		DragBoxStart.x = point.x;
		DragBoxStart.y = point.y;
		MouseMoved = 0;
	}
	CAnimation::OnLButtonDown(nFlags, point);
}

void CGrapher::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	 if (LButtonDown)
	 {
		 ReleaseCapture();
		 LButtonDown = FALSE;
		 
		 if (MouseMoved)  // Don't do anything on clicks or double clicks
		 {
			 // Set axis values here and redraw.
			 if (fabs(double(DragBoxStart.x - point.x)) > 10 && fabs(double(DragBoxStart.y - point.y)) > 10)
			 {
				 StoreAxisParams();
				 SetMinMaxValues(DragBoxStart.x, DragBoxStart.y, point.x, point.y);
				 SetDivXValues();
				 SetDivYValues();
			 }
			 DrawZoomBox = 0;
			 DrawScene();
		 }
	 }

	 CAnimation::OnLButtonUp(nFlags, point);
}

// One of three mechanisms may need servicing in this func.
// 1. Zoom in box dragging
// 2. Dragging a point.
// 3. Mouse just went onto or off of a point.  (need to know for displaying point value)
void CGrapher::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	 MouseMoved = 1;

	 if (LButtonDown)
	 {
		DragBoxPoint.x = point.x;
		DragBoxPoint.y = point.y;
		DrawZoomBox = 1;
		MouseOnPoint = 0;
		DrawScene();
	 }
	 else if (DraggingAPoint)
	 {
		ModifyPointBeingDragged(point);
		MouseOnPoint = 0;
		DrawScene();
	 }
	 else if (MouseIsOnAPoint(point) != MouseOnPoint)
	 {
		 MouseOnPoint = MouseIsOnAPoint(point);
		 DrawScene();
	 }

	 CAnimation::OnMouseMove(nFlags, point);
}

void CGrapher::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (!LButtonDown && OkToChangePoints && MouseIsOnAPoint(point))
	{
		SetCapture();
		DraggingAPoint = 1;
		MouseMoved = 0;  
	}

	CAnimation::OnRButtonDown(nFlags, point);
}

void CGrapher::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (DraggingAPoint)
	{
		ReleaseCapture();
		DraggingAPoint = 0;
		if (MouseMoved)  // Don't do anything on clicks or double clicks
		{
			DrawScene();
			pParent->PostMessage(CUSM_DATACHANGE, PointUnderMouse.id, PointUnderMouse.index);
		}
	}
	
	CAnimation::OnRButtonUp(nFlags, point);
}

void CGrapher::SetMinMaxValues(int x1, int y1, int x2, int y2)
{
	int minx, miny, maxx, maxy;
	double buff1, buff2, buff3, buff4;

	if (x1 < XAxis.MinPix) x1 = XAxis.MinPix;
	if (x1 > XAxis.MaxPix) x1 = XAxis.MaxPix;
	if (x2 < XAxis.MinPix) x2 = XAxis.MinPix;
	if (x2 > XAxis.MaxPix) x2 = XAxis.MaxPix;

	if (y1 > YAxis.MinPix) y1 = YAxis.MinPix;
	if (y1 < YAxis.MaxPix) y1 = YAxis.MaxPix;
	if (y2 > YAxis.MinPix) y2 = YAxis.MinPix;
	if (y2 < YAxis.MaxPix) y2 = YAxis.MaxPix;

	if ((x1 == x2) || (y1 == y2))
		return;

	if (x1 < x2)
	{
		minx = x1;
		maxx = x2;
	}
	else
	{
		minx = x2;
		maxx = x1;
	}

	if (y1 < y2)
	{
		miny = y1;
		maxy = y2;
	}
	else
	{
		miny = y2;
		maxy = y1;
	}

	// buffers are needed because otherwise second call to ?PixelToVal will
	// use different values than the first.
	buff1 = XPixelToVal(minx);
	buff2 = XPixelToVal(maxx);

	buff3 = YPixelToVal(miny);
	buff4 = YPixelToVal(maxy);

	if ( (buff1 != buff2) && (buff3 != buff4) )
	{
		XAxis.MinVal = buff1;
		XAxis.MaxVal = buff2;

		YAxis.MinVal = buff4;
		YAxis.MaxVal = buff3;
	}
}

void CGrapher::SetOkToChangePoints(int oktochange)
{
	OkToChangePoints = oktochange;
}

void CGrapher::SetXDirDragging(int xdirok)
{
	OkToDragInXDir = xdirok;
}

int CGrapher::GetOkToChangePoints()
{
	return(OkToChangePoints);
}

int CGrapher::GetXDirDragging()
{
	return(OkToDragInXDir);
}

void CGrapher::GetXAxisParams(CAxisParams *axisparams)
{
	axisparams->MinVal = XAxis.MinVal;
	axisparams->MaxVal = XAxis.MaxVal;
	axisparams->MinPix = XAxis.MinPix;
	axisparams->MaxPix = XAxis.MaxPix;
	axisparams->Div = XAxis.Div;
	axisparams->DivStart = XAxis.DivStart;
}

void CGrapher::GetYAxisParams(CAxisParams *axisparams)
{
	axisparams->MinVal = YAxis.MinVal;
	axisparams->MaxVal = YAxis.MaxVal;
	axisparams->MinPix = YAxis.MinPix;
	axisparams->MaxPix = YAxis.MaxPix;
	axisparams->Div = YAxis.Div;
	axisparams->DivStart = YAxis.DivStart;
}

void CGrapher::DrawCoords(CDC *pDC)
{
	CFont CoordFont, *OrigFont;
	CString numstr;
	CString formatstr;
	int xpl, ypl;

	CoordFont.CreateFont(22,11,0,0,600,0,0,0,0,0,0,0,0,_T("MS Sans Serif"));
	OrigFont = pDC->SelectObject(&CoordFont);

	pDC->SelectClipRgn(NULL);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_RIGHT | TA_TOP);

	xpl = NumOfDecPlaces(DataSet[PointUnderMouse.setnum].Xptr[PointUnderMouse.index]);
	if (xpl > 3) xpl = 3;
	ypl = NumOfDecPlaces(DataSet[PointUnderMouse.setnum].Yptr[PointUnderMouse.index]);
	if (ypl > 6) ypl = 6;

	formatstr.Format(_T("%c0.%dlf, %c0.%dlf"), _T('%'), xpl, _T('%'), ypl);

	numstr.Format(formatstr, DataSet[PointUnderMouse.setnum].Xptr[PointUnderMouse.index], 
			                 DataSet[PointUnderMouse.setnum].Yptr[PointUnderMouse.index]);

	pDC->TextOut(XValToPixel(DataSet[PointUnderMouse.setnum].Xptr[PointUnderMouse.index]) - 5,
		         YValToPixel(DataSet[PointUnderMouse.setnum].Yptr[PointUnderMouse.index]) + 5,
				 numstr);

	pDC->SelectObject(OrigFont);
	CoordFont.DeleteObject();
}

void CGrapher::ModifyPointBeingDragged(CPoint point)
{
	if (OkToDragInXDir)
	   DataSet[PointUnderMouse.setnum].Xptr[PointUnderMouse.index] = RoundToNPlaces(XPixelToVal(point.x), NumOfDecPlaces(XAxis.Div)+1);
	DataSet[PointUnderMouse.setnum].Yptr[PointUnderMouse.index] = RoundToNPlaces(YPixelToVal(point.y), NumOfDecPlaces(YAxis.Div)+1);
}

int CGrapher::PointIsInGraphingZone(CPoint point)
{
	if (point.x < XAxis.MinPix)
	   return(0);
	if (point.x > XAxis.MaxPix)
	   return(0);
	if (point.y > YAxis.MinPix)
	   return(0);
	if (point.y < YAxis.MaxPix)
	   return(0);
	
	return(1);
}

int CGrapher::MouseIsOnAPoint(CPoint point)
{
	int i, j, c;

	for(i=0; i<NumOfDataSets; i++)
	{
		for (c=0, j=DataSet[i].Head; c<DataSet[i].NumPoints; c++, j=CircInc(j, i))
		{
			if ( (point.x >= XValToPixel(DataSet[i].Xptr[j])-POINTRADIUS) &&
			     (point.x <= XValToPixel(DataSet[i].Xptr[j])+POINTRADIUS) &&
			     (point.y >= YValToPixel(DataSet[i].Yptr[j])-POINTRADIUS) &&
			     (point.y <= YValToPixel(DataSet[i].Yptr[j])+POINTRADIUS) )
			{
				PointUnderMouse.setnum = i;
				PointUnderMouse.id = DataSet[i].SetId;
				PointUnderMouse.index = j;
				return(1);
			}
		}
	}
	return(0);
}

void CGrapher::ZoomOut()
{
	double expand;

	expand = (XAxis.MaxVal - XAxis.MinVal) / 2.0;
	XAxis.MinVal -= expand;
	XAxis.MaxVal += expand;
	SetDivXValues();

	expand = (YAxis.MaxVal - YAxis.MinVal) / 2.0;
	YAxis.MinVal -= expand;
	YAxis.MaxVal += expand;
	SetDivYValues();
}

int CGrapher::CalcSpeeds(int *pids, double *pspeeds)
{
	int s, p, p1, p2, num=0;
	CPoint pntloc;

	for(s=0; s<NumOfDataSets; s++)
	{
		p1 = p2 = -1;
		for(p=0; p<DataSet[s].NumPoints; p++)
		{
			pntloc.x = XValToPixel(DataSet[s].Xptr[p]);
			pntloc.y = YValToPixel(DataSet[s].Yptr[p]);
			if (p1 == -1)
			{
				if (PointIsInGraphingZone(pntloc))
					p1 = p;
			}
			else if (!PointIsInGraphingZone(pntloc) || p == DataSet[s].NumPoints-1)
			{
				p2 = p-1;
				p = DataSet[s].NumPoints-1;
			}
		}
		if (p1 >= 0 && p2 > p1)
		{
			pids[num] = DataSet[s].SetId;

			if (DataSet[s].Yptr[p1] == DataSet[s].Yptr[p2])
				pspeeds[num] = 0;
			else if (DataSet[s].Xptr[p1] == DataSet[s].Xptr[p2])
				pspeeds[num] = 1e300;
			else
				pspeeds[num] = (DataSet[s].Yptr[p1] - DataSet[s].Yptr[p2]) / (DataSet[s].Xptr[p1] - DataSet[s].Xptr[p2]);

			num++;
		}
	}

	return(num);
}

int CGrapher::GetLineStyle(int id)
{
	int i;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].SetId == id)
			return(DataSet[i].LineStyle);

	return(-1);
}

int CGrapher::GetId(int setnum)
{
	if (setnum >= 0)
		if (setnum < NumOfDataSets)
			return(DataSet[setnum].SetId);

	return(-1);
}

int CGrapher::NumOfSetsWithPnts()
{
	int i, count=0;

	for(i=0; i<NumOfDataSets; i++)
		if (DataSet[i].NumPoints > 0)
			count++;

	return(count);
}
