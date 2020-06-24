#pragma once
#include "WellGraphic.h"

class CPlateGraphic : public CButton
{
	DECLARE_DYNAMIC(CPlateGraphic)

	int NumRows, NumCols;
	int WellWidth, WellHeight, Border;
	bool WellsCreated;
	CFont WellNumFont, RowColFont;

public:
	CPlateGraphic(void);
	virtual ~CPlateGraphic(void);

	void SetNumRowsCols(int rows, int cols);
	void ResetTray();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpdis);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

public:
	CWellGraphic **PlateMatrix;
};

