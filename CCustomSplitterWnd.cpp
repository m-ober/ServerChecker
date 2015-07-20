#include "stdafx.h"
#include "CCustomSplitterWnd.h"

CCustomSplitterWnd::CCustomSplitterWnd()
{
	m_cxSplitter = 0;
	m_cySplitter = 0;
	m_cxBorderShare = 0;
	m_cyBorderShare = 0;
	m_cxSplitterGap = 0;
	m_cySplitterGap = 0;
}

int CCustomSplitterWnd::HitTest(CPoint /*pt*/) const
{
	ASSERT_VALID(this);
	return 0;
}