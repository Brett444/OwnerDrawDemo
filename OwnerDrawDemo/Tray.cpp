#include "stdafx.h"
#include "OwnerDrawDemo.h"
#include "Tray.h"

IMPLEMENT_DYNAMIC(CTray, CButton)

CTray::CTray()
{
	for(int i=0; i<10; i++)
		Lens[i] = new CLensSocket();
}

CTray::~CTray()
{
	for(int i=0; i<10; i++)
		delete Lens[i];
}

BEGIN_MESSAGE_MAP(CTray, CButton)
END_MESSAGE_MAP()

void CTray::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	static bool firsttime=true;

	if (firsttime)
	{
		firsttime = false;
		for(int i=0; i<10; i++)
			Lens[i]->Create(_T(""), BS_OWNERDRAW | WS_CHILD | WS_VISIBLE, CRect(30*i, 30*i, 30*(i+1), 30*(i+1)), this, 100+i);
	}
}
