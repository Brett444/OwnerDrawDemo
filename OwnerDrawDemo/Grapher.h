#pragma once

#include "Animation.h"

#define MAXNUMOFDATASETS 30

#define CUSM_DATACHANGE		(WM_USER+1)

#define GT_LINE				1
#define GT_DOT				2
#define GT_LINEDOT			3
#define GT_DASHEDLINE		4
#define GT_DASHEDLINEDOT	5

#define XPERCENTBORDER 0.1
#define YPERCENTBORDER 0.1

#define POINTRADIUS 6

class CDataSet
{
public:
	double *Xptr;  
	double *Yptr;  
	int BufferSize;
	int Head;
	int NumPoints;
	int SetId;
	int LineStyle;
	COLORREF Color;
};

class CAxisParams
{
public:
	int MinPix;
	int MaxPix;
	double MinVal;
	double MaxVal;
	double Div;
	double DivStart;
};

class CDataSetPoint
{
public:
	int setnum;
	int id;
	int index;
};

/////////////////////////////////////////////////////////////////////////////
// CGrapher window

class CGrapher : public CAnimation
{
// Operations
public:
	int HoldFlag;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrapher)
	//}}AFX_VIRTUAL

// Implementation
protected:
	int NumOfDataSets, FirstTime, DrawZoomBox, LButtonDown, DraggingAPoint;
	int OkToChangePoints, OkToDragInXDir, MouseOnPoint, MouseMoved;
	int Interpolate;
	CDataSet DataSet[MAXNUMOFDATASETS];
	CAxisParams XAxis, YAxis, StoredXAxis, StoredYAxis;
	CPoint DragBoxStart, DragBoxPoint;
	CDataSetPoint PointUnderMouse;
	CWnd *pParent;
	double AddPoint[2];

	void ZeroTheDataSets();
	void DrawBox(CDC *pDC, int left, int top, int right, int bottom);
	void DrawDataSets(CDC *pDC);
	void DrawAxis(CDC *pDC);
	void DrawCoords(CDC *pDC);
	void DrawDivisions(CDC *pDC);
	void LabelDivisions(CDC *pDC);
	int XValToPixel(double xval);
	int YValToPixel(double yval);
	double XPixelToVal(int xpix);
	double YPixelToVal(int ypix);
	int NumOfDecPlaces(double val);
	double XToTheY(double x, int y);
	void DrawDataAsLines(CDC *pDC, int set);
	void DrawDataAsDots(CDC *pDC, int set);
    int CircInc(int ind, int set);
	double round125(double val);
	double RoundToNPlaces(double val, int n);
    double ClosestTo125(double val);
	void SetDivXValues();
	void SetDivYValues();
    void SetMinMaxValues(int x1, int y1, int x2, int y2);
	void ModifyPointBeingDragged(CPoint point);
	int PointIsInGraphingZone(CPoint point);
	int MouseIsOnAPoint(CPoint point);

protected:
	virtual void OverrideAndDrawInHere(CDC *pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CGrapher)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	CGrapher();
	virtual ~CGrapher();

	void AddDataSet(double *xdata, double *ydata, int buffersize, int head, int numpoints, int id, COLORREF color = RGB(0,0,150), int linestyle = GT_LINE);
	void DeleteDataSet(int id);
	void DeleteAllDataSets();
	void SetGraphColor(int id, COLORREF color);
	int GetNumOfDataSets();
	void SetHeadAndNumPoints(int id, int head, int numpoints);
	void ChangeId(int id, int newid);
	void AutoScaleXAxis();
	void AutoScaleYAxis();
	void SetYAxisParams(CAxisParams axisparams, int autodiv=1);
	void SetXAxisParams(CAxisParams axisparams, int autodiv=1);
	void SetOkToChangePoints(int oktochange);
	int GetOkToChangePoints();
	void GetXAxisParams(CAxisParams *axisparams);
	void GetYAxisParams(CAxisParams *axisparams);
	void SetXDirDragging(int xdirok);
	int GetXDirDragging();
	void ZoomOut();
	void SetParent(CWnd *pparent);
	void SetInterpolate(int interp);
	int IdExists(int id);
	void StoreAxisParams();
	void RestoreAxisParams();
	int CalcSpeeds(int *pids, double *pspeeds); // returns number of speeds calc'd
	void SetLineStyle(int id, int linestyle);
	int GetLineStyle(int id);
	int GetId(int setnum);
	int NumOfSetsWithPnts();

private:
	int YAxisHasBeenSet;
	int XAxisHasBeenSet;
};

