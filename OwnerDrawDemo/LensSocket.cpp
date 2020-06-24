#include "stdafx.h"
#include "OwnerDrawDemo.h"
#include "LensSocket.h"

// CLensSocket

IMPLEMENT_DYNAMIC(CLensSocket, CButton)

CLensSocket::CLensSocket()
{
}

CLensSocket::~CLensSocket()
{
}

BEGIN_MESSAGE_MAP(CLensSocket, CButton)
END_MESSAGE_MAP()

void CLensSocket::OverrideAndDrawInHere(CDC *pDC)
{
	CRect clientrect;

	GetClientRect(clientrect);

	pDC->SelectStockObject(BLACK_PEN);

	pDC->MoveTo(clientrect.left, clientrect.top);
	pDC->LineTo(clientrect.right, clientrect.bottom);

	pDC->MoveTo(clientrect.right, clientrect.top);
	pDC->LineTo(clientrect.left, clientrect.bottom);

	pDC->Ellipse(clientrect);
}

