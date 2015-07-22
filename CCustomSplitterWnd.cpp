#include "stdafx.h"
#include "CCustomSplitterWnd.h"

CCustomSplitterWnd::CCustomSplitterWnd()
{
	m_cxSplitter = m_cySplitter = 5;
	m_cxBorderShare = m_cyBorderShare = 0;
	m_cxSplitterGap = m_cySplitterGap = 5;
	m_cxBorder = m_cyBorder = 1;
}

int CCustomSplitterWnd::HitTest(CPoint /*pt*/) const
{
	ASSERT_VALID(this);
	return 0;
}

void CCustomSplitterWnd::OnDrawSplitter(CDC* pDC, ESplitType nType,
	const CRect& rectArg)
{
	// Let CSplitterWnd handle everything but the border-drawing
	if ((nType != splitBorder) || (pDC == NULL))
	{
		CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
		return;
	}

	ASSERT_VALID(pDC);

	pDC->Draw3dRect(rectArg, GetSysColor(COLOR_BTNSHADOW),
		GetSysColor(COLOR_BTNHIGHLIGHT));
}