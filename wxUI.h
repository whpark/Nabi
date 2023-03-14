///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
namespace gtl{ namespace wx{ class xMatView; } }

#include <wx/string.h>
#include <wx/filepicker.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/dirctrl.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

namespace ui
{
	///////////////////////////////////////////////////////////////////////////////
	/// Class IMainWnd
	///////////////////////////////////////////////////////////////////////////////
	class IMainWnd : public wxFrame
	{
		private:

		protected:
			wxSplitterWindow* m_splitter;
			wxPanel* m_panelFolder;
			wxDirPickerCtrl* m_dirPicker;
			wxButton* m_btnGo;
			wxGenericDirCtrl* m_dir;
			gtl::wx::xMatView* m_view;

			// Virtual event handlers, override them in your derived class
			virtual void OnMove( wxMoveEvent& event ) = 0;
			virtual void OnSize( wxSizeEvent& event ) = 0;
			virtual void OnButtonClick_Go( wxCommandEvent& event ) = 0;
			virtual void OnDirctrlSelectionChanged( wxCommandEvent& event ) = 0;


		public:

			IMainWnd( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("나비"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 834,638 ), long style = wxDEFAULT_FRAME_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxTAB_TRAVERSAL );

			~IMainWnd();

			void m_splitterOnIdle( wxIdleEvent& )
			{
				m_splitter->SetSashPosition( 0 );
				m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( IMainWnd::m_splitterOnIdle ), NULL, this );
			}

	};

} // namespace ui

