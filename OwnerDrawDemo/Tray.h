#pragma once

#include "LensSocket.h"

class CTray : public CButton
{
	DECLARE_DYNAMIC(CTray)

public:
	CTray();
	virtual ~CTray();

	CLensSocket *Lens[10];

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


