// ColumnSplitter.cpp: implementation of the CColumnSplitter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "diskscan.h"
#include "ColumnSplitter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CColumnSplitter, CSplitterWnd)

BEGIN_MESSAGE_MAP(CColumnSplitter, CSplitterWnd)
// ON_WM_LBUTTONDOWN()
// ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CColumnSplitter::CColumnSplitter()
: m_iMinWdith(0)
{

}

CColumnSplitter::~CColumnSplitter()
{

}
void CColumnSplitter::OnMouseMove(UINT nFlags, CPoint point)
{
	//阻止将分切窗口拉的太大
	if(GetKeyState(VK_LBUTTON) & 0x8000)
		if(point.x >= m_iMinWdith) return;// 最大值


	CSplitterWnd::OnMouseMove(nFlags, point);
}

void CColumnSplitter::SetMinWidth(int width)
{
	this->m_iMinWdith = width;
}
