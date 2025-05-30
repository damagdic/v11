#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "v11.h"
#endif

#include "v11Doc.h"
#include "v11View.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Cv11View

IMPLEMENT_DYNCREATE(Cv11View, CView)

BEGIN_MESSAGE_MAP(Cv11View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &Cv11View::OnFilePrintPreview)
	ON_COMMAND(ID_SHAPE, &Cv11View::OnShapeChanged)
	ON_COMMAND(ID_COLOR, &Cv11View::OnColorChanged)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_HIGHLIGHT_RIBBON_LIST_ITEM, &Cv11View::OnHighlightRibbonListItem)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// Cv11View construction/destruction

Cv11View::Cv11View() {
	m_shape = 0;
	m_color = RGB(0, 0, 0);
	m_bPreview = FALSE;
}

Cv11View::~Cv11View()
{
}

BOOL Cv11View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Cv11View drawing

void Cv11View::OnDraw(CDC* pDC)
{

    if (m_rc.IsRectEmpty())
    {
        m_rc = CRect(50, 50, 200, 150); // neki fiksni pravokutnik
    }
	CPen pen(PS_SOLID, 2, m_color);
	CPen* pOldPen = pDC->SelectObject(&pen);

	switch (m_shape)
	{
	case 0:
		pDC->Rectangle(m_rc);
		break;
	case 1:
		pDC->Ellipse(m_rc);
		break;
	case 2:
		pDC->RoundRect(m_rc, CPoint(20, 20));
		break;
	}

	pDC->SelectObject(pOldPen);
}


// Cv11View printing


void Cv11View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL Cv11View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void Cv11View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void Cv11View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void Cv11View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Cv11View::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRectTracker tracker;
	if (tracker.TrackRubberBand(this, point))
	{
		m_rc = tracker.m_rect;
		Invalidate();
	}

	CView::OnLButtonDown(nFlags, point);
}

void Cv11View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void Cv11View::OnShapeChanged()
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
    ((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_SHAPE, arr);
    if (!arr.IsEmpty())
    {
        CMFCRibbonGallery* pGallery = (CMFCRibbonGallery*)arr.GetAt(0);
        m_shape = pGallery->GetSelectedItem();
        Invalidate();
    }
}

void Cv11View::OnColorChanged()
{
    CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> arr;
    ((CMainFrame*)AfxGetMainWnd())->m_wndRibbonBar.GetElementsByID(ID_COLOR, arr);
    if (!arr.IsEmpty())
    {
        CMFCRibbonColorButton* pBtn = (CMFCRibbonColorButton*)arr.GetAt(0);
        m_color = pBtn->GetColor();
        Invalidate();
    }
}

LRESULT Cv11View::OnHighlightRibbonListItem(WPARAM wp, LPARAM lp)
{
    int index = (int)wp;
    CMFCRibbonBaseElement* pElem = (CMFCRibbonBaseElement*)lp;
    UINT id = pElem->GetID(); // ID_SHAPE ili ID_COLOR

    static int savedShape = -1;
    static COLORREF savedColor = RGB(0, 0, 0);

    if (id == ID_SHAPE)
    {
        if (index == -1)
        {
            m_shape = savedShape;
        }
        else
        {
            if (savedShape == -1)
                savedShape = m_shape;
            m_shape = index;
        }
    }
    else if (id == ID_COLOR)
    {
        CMFCRibbonColorButton* pColorBtn = (CMFCRibbonColorButton*)pElem;
        if (index == -1)
        {
            m_color = savedColor;
        }
        else
        {
            if (savedColor == RGB(0, 0, 0)) // inicijalna vrijednost, možeš bolje
                savedColor = m_color;
            m_color = pColorBtn->GetHighlightedColor();
        }
    }

    Invalidate();
    return 0;
}



// Cv11View diagnostics

#ifdef _DEBUG
void Cv11View::AssertValid() const
{
	CView::AssertValid();
}

void Cv11View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Cv11Doc* Cv11View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Cv11Doc)));
	return (Cv11Doc*)m_pDocument;
}
#endif //_DEBUG


// Cv11View message handlers

