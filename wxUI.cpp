///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "MatView/MatView.h"

#include "wxUI.h"

///////////////////////////////////////////////////////////////////////////
using namespace ui;

IMainWnd::IMainWnd( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizerTop;
	bSizerTop = new wxBoxSizer( wxVERTICAL );

	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( IMainWnd::m_splitterOnIdle ), NULL, this );

	m_panelFolder = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panelFolder, wxID_ANY, _("Folder") ), wxVERTICAL );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_dirPicker = new wxDirPickerCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST|wxDIRP_SMALL|wxDIRP_USE_TEXTCTRL );
	bSizer4->Add( m_dirPicker, 1, wxBOTTOM, 5 );

	m_btnGo = new wxButton( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 30,-1 ), 0 );

	m_btnGo->SetBitmap( wxArtProvider::GetBitmap( wxART_GO_FORWARD, wxART_BUTTON ) );
	bSizer4->Add( m_btnGo, 0, wxLEFT, 5 );


	sbSizer2->Add( bSizer4, 0, wxEXPAND, 5 );

	m_dir = new wxGenericDirCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxDIRCTRL_SHOW_FILTERS|wxSUNKEN_BORDER, _("ImageFiles(*.png;*.bmp;*.jpg;*.gif;*.tif;*.jpeg;*.tiff)|*.png;*.bmp;*.jpg;*.gif;*.tif;*.jpeg;*.tiff|All Files|*.*"), 0 );

	m_dir->ShowHidden( false );
	sbSizer2->Add( m_dir, 1, wxEXPAND, 5 );


	m_panelFolder->SetSizer( sbSizer2 );
	m_panelFolder->Layout();
	sbSizer2->Fit( m_panelFolder );
	m_view = new xMatView( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_splitter->SplitVertically( m_panelFolder, m_view, 0 );
	bSizerTop->Add( m_splitter, 1, wxEXPAND, 5 );


	this->SetSizer( bSizerTop );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_btnGo->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMainWnd::OnButtonClick_Go ), NULL, this );
	m_dir->Connect( wxEVT_DIRCTRL_SELECTIONCHANGED, wxCommandEventHandler( IMainWnd::OnDirctrlSelectionChanged ), NULL, this );
}

IMainWnd::~IMainWnd()
{
	// Disconnect Events
	m_btnGo->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMainWnd::OnButtonClick_Go ), NULL, this );
	m_dir->Disconnect( wxEVT_DIRCTRL_SELECTIONCHANGED, wxCommandEventHandler( IMainWnd::OnDirctrlSelectionChanged ), NULL, this );

}
