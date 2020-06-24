#if !defined(AFX_ANIMATION_H__8961FAF5_86D8_11D1_B7EE_000000000000__INCLUDED_)
#define AFX_ANIMATION_H__8961FAF5_86D8_11D1_B7EE_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Animation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnimation window

class CAnimation : public CButton
{
// Construction
public:
	CAnimation();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimation)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetBackGndColor(COLORREF backgndcolor);
	void DrawScene();
	virtual ~CAnimation();

protected:
	virtual void OverrideAndDrawInHere(CDC *pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CAnimation)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	COLORREF BackGndColor;
	CBitmap * OldBitmap;
	CBitmap * NewBitmap;
	int firstdraw;
	CDC MemDC;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATION_H__8961FAF5_86D8_11D1_B7EE_000000000000__INCLUDED_)
