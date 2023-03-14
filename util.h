#pragma once

#include <version>

inline std::string WtoU8A(std::wstring const& str) {
	std::string r = (std::string&)gtl::ToStringU8(str);
	return r;
}
inline std::u8string WtoU8(std::wstring const& str) {
	return gtl::ToStringU8(str);
}
inline std::wstring U8toW(std::string const& str) {
	return gtl::ToStringW((std::u8string const&)str);
}
inline std::wstring U8toW(std::u8string const& str) {
	return gtl::ToStringW(str);
}

inline bool IsImageExtension(std::filesystem::path const& path) {
	auto ext = gtl::ToLower<char>(path.extension().string());
	return gtl::IsValueOneOf(ext, ".bmp", ".jpg", ".jpeg", ".tiff", ".png", ".gif", ".jfif");
}

template <typename tvalue_>
std::remove_cvref_t<tvalue_> GetRegValue(wxRegKey& reg, std::string const& key, tvalue_&& valueDefault) {
	if (!reg.HasValue(key)) {
		return std::forward<tvalue_>(valueDefault);
	}

	//wxString str;
	//if (!reg.QueryValue(key, str))
	//	return std::forward<tvalue_>(valueDefault);

	using tvalue = std::remove_cvref_t<tvalue_>;
	if constexpr (std::is_integral_v<tvalue>) {
		long v{};
		if (!reg.QueryValue(key, &v))
			return valueDefault;
		return (tvalue)v;
	}
	else if constexpr (std::is_floating_point_v<tvalue>) {
		wxString str;
		if (!reg.QueryValue(key, str))
			return valueDefault;
		double v{};
		if (!str.ToDouble(&v))
			return valueDefault;
		return (tvalue)v;
	}
	else if constexpr (std::is_same_v<std::string, tvalue>) {
		wxString str;
		if (!reg.QueryValue(key, str))
			return valueDefault;
		return str.ToStdString();
	}
	else if constexpr (std::is_same_v<std::wstring, tvalue>) {
		wxString str;
		if (!reg.QueryValue(key, str))
			return valueDefault;
		return str.ToStdWstring();
	}
	else if constexpr (std::is_same_v<wxString, tvalue>) {
		wxString str;
		if (!reg.QueryValue(key, str))
			return valueDefault;
		return str;
	}
	else {
		static_assert(gtlc::dependent_false_v);
	}

	return valueDefault;
};

bool SaveWindowPosition(wxRegKey& regParent, std::string const& nameWindow, wxWindow* pWindow);
bool LoadWindowPosition(wxRegKey& regParent, std::string const& nameWindow, wxWindow* pWindow);
