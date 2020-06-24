
// OwnerDrawDemoDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "PlateGraphic.h"

// COwnerDrawDemoDlg dialog
class COwnerDrawDemoDlg : public CDialogEx
{
// Construction
public:
	COwnerDrawDemoDlg(CWnd* pParent = NULL);	// standard constructor
	~COwnerDrawDemoDlg();

// Dialog Data
	enum { IDD = IDD_OWNERDRAWDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CPlateGraphic WellPlateGraphic;
};
