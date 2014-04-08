// 此类为一个窗口分切类，可以设置第一个视图的最大宽度
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLUMNSPLITTER_H__D19A8782_5D70_41E2_9753_66A181D526ED__INCLUDED_)
#define AFX_COLUMNSPLITTER_H__D19A8782_5D70_41E2_9753_66A181D526ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CColumnSplitter : public CSplitterWnd  
{
	DECLARE_DYNCREATE(CColumnSplitter)

private:
	int m_iMinWdith;

public:
	void SetMinWidth( int width);
	CColumnSplitter();
	virtual ~CColumnSplitter();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_COLUMNSPLITTER_H__D19A8782_5D70_41E2_9753_66A181D526ED__INCLUDED_)
