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
class wxGLCanvasAdapter;

#include <wx/button.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/timer.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/radiobox.h>
#include <wx/clrpicker.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

namespace ui
{
	#define wxID_SCROLL 1000

	///////////////////////////////////////////////////////////////////////////////
	/// Class IMatView
	///////////////////////////////////////////////////////////////////////////////
	class IMatView : public wxPanel
	{
		private:

		protected:
			wxBoxSizer* m_sizerTop;
			wxToolBar* m_toolBar;
			wxButton* m_btnHide;
			wxComboBox* m_cmbZoomMode;
			wxSpinCtrlDouble* m_spinCtrlZoom;
			wxToolBarToolBase* m_toolZoomIn;
			wxToolBarToolBase* m_toolZoomOut;
			wxToolBarToolBase* m_toolZoomReset;
			wxToolBarToolBase* m_toolSettings;
			wxTextCtrl* m_textStatus;
			wxGLCanvasAdapter* m_view;
			wxTimer m_timerScroll;

			// Virtual event handlers, override them in your derived class
			virtual void OnCharHook( wxKeyEvent& event ) = 0;
			virtual void OnButtonClick_Hide( wxCommandEvent& event ) = 0;
			virtual void OnCombobox_ZoomMode( wxCommandEvent& event ) = 0;
			virtual void OnSpinCtrlDouble_ZoomValue( wxSpinDoubleEvent& event ) = 0;
			virtual void OnTextEnter_ZoomValue( wxCommandEvent& event ) = 0;
			virtual void OnZoomIn( wxCommandEvent& event ) = 0;
			virtual void OnZoomOut( wxCommandEvent& event ) = 0;
			virtual void OnZoomFit( wxCommandEvent& event ) = 0;
			virtual void OnSettings( wxCommandEvent& event ) = 0;
			virtual void OnCharHook_View( wxKeyEvent& event ) = 0;
			virtual void OnLeftDown_View( wxMouseEvent& event ) = 0;
			virtual void OnLeftUp_View( wxMouseEvent& event ) = 0;
			virtual void OnMiddleDown_View( wxMouseEvent& event ) = 0;
			virtual void OnMiddleUp_View( wxMouseEvent& event ) = 0;
			virtual void OnMotion_View( wxMouseEvent& event ) = 0;
			virtual void OnMouseWheel_View( wxMouseEvent& event ) = 0;
			virtual void OnPaint_View( wxPaintEvent& event ) = 0;
			virtual void OnRightDown_View( wxMouseEvent& event ) = 0;
			virtual void OnRightUp_View( wxMouseEvent& event ) = 0;
			virtual void OnSize_View( wxSizeEvent& event ) = 0;
			virtual void OnTimerScroll( wxTimerEvent& event ) = 0;


		public:

			IMatView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 652,140 ), long style = wxALWAYS_SHOW_SB|wxTAB_TRAVERSAL|wxWANTS_CHARS, const wxString& name = wxEmptyString );

			~IMatView();

	};

	///////////////////////////////////////////////////////////////////////////////
	/// Class IDlgMatViewOption
	///////////////////////////////////////////////////////////////////////////////
	class IDlgMatViewOption : public wxDialog
	{
		private:

		protected:
			wxCheckBox* m_chkGLonly;
			wxCheckBox* m_chkSkia;
			wxCheckBox* m_chkZoomLock;
			wxCheckBox* m_chkPanningLock;
			wxCheckBox* m_chkExtendedPanning;
			wxCheckBox* m_chkKeyboardNavigation;
			wxCheckBox* m_chkDrawPixelValue;
			wxCheckBox* m_chkPyrImageDown;
			wxStaticText* m_staticText1;
			wxSpinCtrlDouble* m_spinMouseSpeed;
			wxStaticText* m_staticText2;
			wxSpinCtrl* m_spinScrollMargin;
			wxRadioBox* m_radioZoomInMethod;
			wxRadioBox* m_radioZoomOutMethod;
			wxStaticText* m_staticText3;
			wxColourPickerCtrl* m_crBackground;
			wxButton* m_btnRestoreColor;
			wxButton* m_btnOK;
			wxButton* m_btnCancel;

			// Virtual event handlers, override them in your derived class
			virtual void OnCheckBox_Skia( wxCommandEvent& event ) = 0;
			virtual void OnButtonClick_RestoreBackgroundColor( wxCommandEvent& event ) = 0;
			virtual void OnButtonClick_OK( wxCommandEvent& event ) = 0;
			virtual void OnButtonClick_Cancel( wxCommandEvent& event ) = 0;


		public:

			IDlgMatViewOption( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("options..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 312,490 ), long style = wxDEFAULT_DIALOG_STYLE );

			~IDlgMatViewOption();

	};

} // namespace ui

