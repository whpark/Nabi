///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "wxGLCanvasAdapter.h"

#include "wxMatView.h"

///////////////////////////////////////////////////////////////////////////
using namespace ui;

IMatView::IMatView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	m_sizerTop = new wxBoxSizer( wxVERTICAL );

	m_toolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
	m_btnHide = new wxButton( m_toolBar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 20,-1 ), 0 );

	m_btnHide->SetBitmap( wxArtProvider::GetBitmap( wxART_CLOSE, wxART_TOOLBAR ) );
	m_toolBar->AddControl( m_btnHide );
	m_toolBar->AddSeparator();

	m_cmbZoomMode = new wxComboBox( m_toolBar, wxID_ANY, _("fit2window"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	m_cmbZoomMode->Append( _("1:1") );
	m_cmbZoomMode->Append( _("fit2window") );
	m_cmbZoomMode->Append( _("fit2width") );
	m_cmbZoomMode->Append( _("fit2height") );
	m_cmbZoomMode->Append( _("lock (mouse wheel)") );
	m_cmbZoomMode->Append( _("free") );
	m_cmbZoomMode->SetSelection( 1 );
	m_toolBar->AddControl( m_cmbZoomMode );
	m_spinCtrlZoom = new wxSpinCtrlDouble( m_toolBar, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxSP_ARROW_KEYS, 0.001, 1000, 1, 0.25 );
	m_spinCtrlZoom->SetDigits( 4 );
	m_spinCtrlZoom->SetToolTip( _("zoom") );

	m_toolBar->AddControl( m_spinCtrlZoom );
	m_toolZoomIn = m_toolBar->AddTool( wxID_ANY, wxEmptyString, wxArtProvider::GetBitmap( wxART_PLUS, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolZoomOut = m_toolBar->AddTool( wxID_ANY, _("tool"), wxArtProvider::GetBitmap( wxART_MINUS, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolZoomReset = m_toolBar->AddTool( wxID_ANY, _("tool"), wxArtProvider::GetBitmap( wxART_FULL_SCREEN, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolSettings = m_toolBar->AddTool( wxID_ANY, _("tool"), wxArtProvider::GetBitmap( wxART_LIST_VIEW, wxART_TOOLBAR ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_textStatus = new wxTextCtrl( m_toolBar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), wxTE_READONLY );
	m_toolBar->AddControl( m_textStatus );
	m_toolBar->Realize();

	m_sizerTop->Add( m_toolBar, 0, wxEXPAND|wxRIGHT|wxLEFT, 0 );

	m_view = new wxGLCanvasAdapter( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB|wxHSCROLL|wxVSCROLL );
	m_sizerTop->Add( m_view, 1, wxEXPAND, 5 );


	this->SetSizer( m_sizerTop );
	this->Layout();
	m_timerScroll.SetOwner( this, wxID_SCROLL );

	// Connect Events
	this->Connect( wxEVT_CHAR_HOOK, wxKeyEventHandler( IMatView::OnCharHook ) );
	m_btnHide->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMatView::OnButtonClick_Hide ), NULL, this );
	m_cmbZoomMode->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( IMatView::OnCombobox_ZoomMode ), NULL, this );
	m_spinCtrlZoom->Connect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( IMatView::OnSpinCtrlDouble_ZoomValue ), NULL, this );
	m_spinCtrlZoom->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( IMatView::OnTextEnter_ZoomValue ), NULL, this );
	this->Connect( m_toolZoomIn->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnZoomIn ) );
	this->Connect( m_toolZoomOut->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnZoomOut ) );
	this->Connect( m_toolZoomReset->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnZoomFit ) );
	this->Connect( m_toolSettings->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnSettings ) );
	m_view->Connect( wxEVT_CHAR_HOOK, wxKeyEventHandler( IMatView::OnCharHook_View ), NULL, this );
	m_view->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( IMatView::OnLeftDown_View ), NULL, this );
	m_view->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( IMatView::OnLeftUp_View ), NULL, this );
	m_view->Connect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( IMatView::OnMiddleDown_View ), NULL, this );
	m_view->Connect( wxEVT_MIDDLE_UP, wxMouseEventHandler( IMatView::OnMiddleUp_View ), NULL, this );
	m_view->Connect( wxEVT_MOTION, wxMouseEventHandler( IMatView::OnMotion_View ), NULL, this );
	m_view->Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( IMatView::OnMouseWheel_View ), NULL, this );
	m_view->Connect( wxEVT_PAINT, wxPaintEventHandler( IMatView::OnPaint_View ), NULL, this );
	m_view->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( IMatView::OnRightDown_View ), NULL, this );
	m_view->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( IMatView::OnRightUp_View ), NULL, this );
	m_view->Connect( wxEVT_SIZE, wxSizeEventHandler( IMatView::OnSize_View ), NULL, this );
	this->Connect( wxID_SCROLL, wxEVT_TIMER, wxTimerEventHandler( IMatView::OnTimerScroll ) );
}

IMatView::~IMatView()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CHAR_HOOK, wxKeyEventHandler( IMatView::OnCharHook ) );
	m_btnHide->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMatView::OnButtonClick_Hide ), NULL, this );
	m_cmbZoomMode->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( IMatView::OnCombobox_ZoomMode ), NULL, this );
	m_spinCtrlZoom->Disconnect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( IMatView::OnSpinCtrlDouble_ZoomValue ), NULL, this );
	m_spinCtrlZoom->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( IMatView::OnTextEnter_ZoomValue ), NULL, this );
	this->Disconnect( m_toolZoomIn->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnZoomIn ) );
	this->Disconnect( m_toolZoomOut->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnZoomOut ) );
	this->Disconnect( m_toolZoomReset->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnZoomFit ) );
	this->Disconnect( m_toolSettings->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( IMatView::OnSettings ) );
	m_view->Disconnect( wxEVT_CHAR_HOOK, wxKeyEventHandler( IMatView::OnCharHook_View ), NULL, this );
	m_view->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( IMatView::OnLeftDown_View ), NULL, this );
	m_view->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( IMatView::OnLeftUp_View ), NULL, this );
	m_view->Disconnect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( IMatView::OnMiddleDown_View ), NULL, this );
	m_view->Disconnect( wxEVT_MIDDLE_UP, wxMouseEventHandler( IMatView::OnMiddleUp_View ), NULL, this );
	m_view->Disconnect( wxEVT_MOTION, wxMouseEventHandler( IMatView::OnMotion_View ), NULL, this );
	m_view->Disconnect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( IMatView::OnMouseWheel_View ), NULL, this );
	m_view->Disconnect( wxEVT_PAINT, wxPaintEventHandler( IMatView::OnPaint_View ), NULL, this );
	m_view->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( IMatView::OnRightDown_View ), NULL, this );
	m_view->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( IMatView::OnRightUp_View ), NULL, this );
	m_view->Disconnect( wxEVT_SIZE, wxSizeEventHandler( IMatView::OnSize_View ), NULL, this );
	this->Disconnect( wxID_SCROLL, wxEVT_TIMER, wxTimerEventHandler( IMatView::OnTimerScroll ) );

}

IDlgMatViewOption::IDlgMatViewOption( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerTOP;
	bSizerTOP = new wxBoxSizer( wxVERTICAL );

	m_chkGLonly = new wxCheckBox( this, wxID_ANY, _("openGL only"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkGLonly, 0, wxALL, 5 );

	m_chkSkia = new wxCheckBox( this, wxID_ANY, _("Skia"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkSkia, 0, wxALL, 5 );

	m_chkZoomLock = new wxCheckBox( this, wxID_ANY, _("Zoom Lock"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkZoomLock, 0, wxALL, 5 );

	m_chkPanningLock = new wxCheckBox( this, wxID_ANY, _("Panning Lock"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkPanningLock, 0, wxALL, 5 );

	m_chkExtendedPanning = new wxCheckBox( this, wxID_ANY, _("Extended Panning"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkExtendedPanning, 0, wxALL, 5 );

	m_chkKeyboardNavigation = new wxCheckBox( this, wxID_ANY, _("Keyboard navigation"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkKeyboardNavigation, 0, wxALL, 5 );

	m_chkDrawPixelValue = new wxCheckBox( this, wxID_ANY, _("Draw Pixel Value"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkDrawPixelValue, 0, wxALL, 5 );

	m_chkPyrImageDown = new wxCheckBox( this, wxID_ANY, _("Build Image Pyramid (for down sampling)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTOP->Add( m_chkPyrImageDown, 0, wxALL, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Mouse Speed :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_spinMouseSpeed = new wxSpinCtrlDouble( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT, 0.01, 10, 1.000000, 0.25 );
	m_spinMouseSpeed->SetDigits( 3 );
	m_spinMouseSpeed->SetMinSize( wxSize( 80,-1 ) );

	bSizer3->Add( m_spinMouseSpeed, 0, wxALL, 5 );


	bSizerTOP->Add( bSizer3, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Scroll Margin :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_spinScrollMargin = new wxSpinCtrl( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT|wxSP_ARROW_KEYS, 0, 1000, 0 );
	m_spinScrollMargin->SetMinSize( wxSize( 80,-1 ) );

	bSizer4->Add( m_spinScrollMargin, 0, wxALL, 5 );


	bSizerTOP->Add( bSizer4, 0, wxEXPAND, 5 );

	wxString m_radioZoomInMethodChoices[] = { _("Neareset"), _("Linear"), _("Bicubic"), _("Lanczos4") };
	int m_radioZoomInMethodNChoices = sizeof( m_radioZoomInMethodChoices ) / sizeof( wxString );
	m_radioZoomInMethod = new wxRadioBox( this, wxID_ANY, _("Zoom In Method"), wxDefaultPosition, wxDefaultSize, m_radioZoomInMethodNChoices, m_radioZoomInMethodChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioZoomInMethod->SetSelection( 0 );
	bSizerTOP->Add( m_radioZoomInMethod, 0, wxALL, 5 );

	wxString m_radioZoomOutMethodChoices[] = { _("Neareset"), _("Area") };
	int m_radioZoomOutMethodNChoices = sizeof( m_radioZoomOutMethodChoices ) / sizeof( wxString );
	m_radioZoomOutMethod = new wxRadioBox( this, wxID_ANY, _("Zoom Out Method"), wxDefaultPosition, wxDefaultSize, m_radioZoomOutMethodNChoices, m_radioZoomOutMethodChoices, 1, wxRA_SPECIFY_ROWS );
	m_radioZoomOutMethod->SetSelection( 0 );
	bSizerTOP->Add( m_radioZoomOutMethod, 0, wxALL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Background Color :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer5->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_crBackground = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	bSizer5->Add( m_crBackground, 0, wxALL, 5 );

	m_btnRestoreColor = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 30,-1 ), 0 );

	m_btnRestoreColor->SetBitmap( wxArtProvider::GetBitmap( wxART_UNDO, wxART_BUTTON ) );
	bSizer5->Add( m_btnRestoreColor, 0, wxALL, 5 );


	bSizerTOP->Add( bSizer5, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_btnOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnOK->SetDefault();
	bSizer6->Add( m_btnOK, 0, wxALL|wxALIGN_BOTTOM, 5 );

	m_btnCancel = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_btnCancel, 0, wxALL|wxALIGN_BOTTOM, 5 );


	bSizerTOP->Add( bSizer6, 1, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizerTOP );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_chkSkia->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnCheckBox_Skia ), NULL, this );
	m_btnRestoreColor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnButtonClick_RestoreBackgroundColor ), NULL, this );
	m_btnOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnButtonClick_OK ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnButtonClick_Cancel ), NULL, this );
}

IDlgMatViewOption::~IDlgMatViewOption()
{
	// Disconnect Events
	m_chkSkia->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnCheckBox_Skia ), NULL, this );
	m_btnRestoreColor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnButtonClick_RestoreBackgroundColor ), NULL, this );
	m_btnOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnButtonClick_OK ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDlgMatViewOption::OnButtonClick_Cancel ), NULL, this );

}
