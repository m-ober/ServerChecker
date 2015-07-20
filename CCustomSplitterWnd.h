#pragma once

#include "afxext.h"

class CCustomSplitterWnd : public CSplitterWnd
{
public:
	CCustomSplitterWnd();
	int HitTest(CPoint /*pt*/) const;
};

