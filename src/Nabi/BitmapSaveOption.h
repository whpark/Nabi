#pragma once

//---------------------------------------------------------------------------------------------------------------------------------
struct sBitmapSaveOption {
	enum class eBPP : int { _1, /*_2, */_4, _8, _24/*, _32*/, none = -1 };

	eBPP bpp{};
	gtl::xSize2i dpi;
	bool bTopToBottom{false};

	void Reset() {
		*this = {};
	}

	static constexpr int GetBPP(eBPP e) {
		switch (e) {
		case eBPP::_1: return 1;
		//case eBPP::_2: return 2;
		case eBPP::_4: return 4;
		case eBPP::_8: return 8;
		case eBPP::_24: return 24;
		//case eBPP::_32: return 32;
		default : return 0;
		}
	}
	static constexpr eBPP GetBPP(int nBPP) {
		switch (nBPP) {
		case 1: return eBPP::_1;
		//case 2: return eBPP::_2;
		case 4: return eBPP::_4;
		case 8: return eBPP::_8;
		case 24: return eBPP::_24;
		//case 32: return eBPP::_32;
		default : return eBPP::none;
		}
	}
	static constexpr gtl::xSize2i GetPelsPerMeter(gtl::xSize2d dpi) {
		return { gtl::Round(dpi.cx * 1000 / 25.4), gtl::Round(dpi.cy * 1000/25.4)};
	}
	static constexpr gtl::xSize2d GetDPI(gtl::xSize2i pelsPerMeter) {
		return { pelsPerMeter.cx * 25.4 * 1.e-3, pelsPerMeter.cy * 25.4 * 1.e-3};
	}
};
