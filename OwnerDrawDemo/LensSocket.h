#pragma once

#include "Animation.h"

class CLensSocket : public CAnimation
{
	DECLARE_DYNAMIC(CLensSocket)

public:
	CLensSocket();
	virtual ~CLensSocket();

protected:
	virtual void OverrideAndDrawInHere(CDC *pDC);

protected:
	DECLARE_MESSAGE_MAP()
};
