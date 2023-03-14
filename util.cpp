#include "pch.h"
#include "util.h"


bool SaveWindowPosition(wxRegKey& regParent, std::string const& nameWindow, wxWindow* pWindow) {
	if (!pWindow)
		return false;

	wxRegKey reg(regParent, "Position");
	if (!reg.Create())
		return false;

	gtl::xRect2i rc {pWindow->GetRect()};
	reg.SetValue(nameWindow, gtl::ToString<wchar_t>(rc));
	return true;
}
bool LoadWindowPosition(wxRegKey& regParent, std::string const& nameWindow, wxWindow* pWindow) {
	if (!pWindow)
		return false;

	wxRegKey reg(regParent, "Position");
	if (!reg.Create())
		return false;

	wxString value;
	if (reg.HasValue(nameWindow))
		reg.QueryValue(nameWindow, value);
	auto rc = gtl::FromString<gtl::xRect2i, wchar_t>({value.c_str(), value.size()});
	gtl::xRect2i bound {wxGetClientDisplayRect()};
	rc &= bound;
	if (rc.IsRectEmpty())
		return false;
	pWindow->SetSize(wxSize(rc.pt1()-rc.pt0()));
	pWindow->Move(rc.pt0());
	return true;
}
