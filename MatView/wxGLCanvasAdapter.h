#pragma once

#include "wx/glcanvas.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGLCanvasAdapter
///////////////////////////////////////////////////////////////////////////////
class wxGLCanvasAdapter : public wxGLCanvas {
private:

public:
	using this_t = wxGLCanvasAdapter;
	using base_t = wxGLCanvas;

public:
	wxGLCanvasAdapter( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxALWAYS_SHOW_SB|wxTAB_TRAVERSAL, const wxString& name = wxEmptyString )
		: wxGLCanvas( parent, id, nullptr, pos, size, style)
	{
	}
	~wxGLCanvasAdapter() {}

};
