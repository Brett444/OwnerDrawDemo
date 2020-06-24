#pragma once
#include "Animation.h"

class CWellGraphic : public CAnimation
{
	DECLARE_DYNAMIC(CWellGraphic)


public:
	int NumCells, DesiredCells;
	CFont *p_WellNumFont;

	CWellGraphic(void);
	virtual ~CWellGraphic(void);

protected:
	virtual void OverrideAndDrawInHere(CDC *pDC);

protected:
	DECLARE_MESSAGE_MAP()

	COLORREF GetWellColor();
};

