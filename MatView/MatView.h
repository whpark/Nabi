﻿#pragma once

#ifndef __BuiscuitViewxMatView__
#define __BuiscuitViewxMatView__

/**
@file
Subclass of xMatView, which is generated by wxFormBuilder.
*/

#include "wxMatView.h"

//#define SK_GL

#ifdef SK_GL
#include "skia/core/SkCanvas.h"
#include "skia/core/SkSurface.h"
#include "skia/core/SkFont.h"
#include "skia/gpu/GrDirectContext.h"
#endif

//=================================================================================================================================
// xMatView
class xMatView : public ui::IMatView {
public:
	using this_t = xMatView;
	using base_t = ui::IMatView;

public:
	using string_t = std::wstring;
	using xPoint2i = gtl::xPoint2i;
	using xPoint2d = gtl::xPoint2d;
	using xRect2i = gtl::xRect2i;
	using xRect2d = gtl::xRect2d;
	using xSize2i = gtl::xSize2i;
	using xSize2d = gtl::xSize2d;
	using xCoordTrans = gtl::xCoordTrans2d;

public:
	// ZOOM
	enum class eZOOM : int8_t { none = -1, one2one, fit2window, fit2width, fit2height, mouse_wheel_locked, free };	// lock : 
	enum class eZOOM_IN : uint8_t { nearest, linear, bicubic, lanczos4/* EDSR, ESPCN, FSRCNN, LapSRN */};
	enum class eZOOM_OUT : uint8_t { nearest, area, };

	struct S_OPTION {
	#if defined(SK_GL)
		bool bGLonly{};						// using openGL, openCV only. no skia
		bool bSkia{true};					// using only skia
	#endif
		bool bZoomLock{false};				// if eZoom is NOT free, zoom is locked
		bool bPanningLock{true};			// image can be moved only eZoom == free
		bool bExtendedPanning{true};		// image can move out of screen
		bool bKeyboardNavigation{};			// page up/down, scroll down to next blank of image
		bool bDrawPixelValue{true};			// draw pixel value on image
		bool bPyrImageDown{true};			// build cache image for down sampling. (such as mipmap)
		double dMouseSpeed{2.0};			// Image Panning Speed. 1.0 is same with mouse move.
		int nScrollMargin{5};				// bExtendedPanning, px margin to scroll
		std::chrono::milliseconds tsScroll{250ms};	// Smooth Scroll. duration
		eZOOM_IN eZoomIn{eZOOM_IN::nearest};
		eZOOM_OUT eZoomOut{eZOOM_OUT::area};
		cv::Vec3b crBackground{161, 114, 230};	// rgb
	};

	struct S_SCROLL_GEOMETRY {
		xRect2i rectClient, rectImageScreen, rectScrollRange;
	};

protected:
	// gl
	std::unique_ptr<wxGLContext> m_context;
	//GLuint m_textureID{};

#ifdef SK_GL
	// skia
	sk_sp<const GrGLInterface> m_interface;
	sk_sp<GrDirectContext> m_skContext;
	sk_sp<SkTypeface> m_skTypeface;
#endif

	cv::Mat m_imgOriginal;	// original image
	mutable cv::Mat m_img;	// image for screen
	mutable struct {
		std::mutex mtx;
		std::deque<cv::Mat> imgs;
		std::jthread threadPyramidMaker;
	} m_pyramid;
	mutable struct {
		bool bInSelectionMode{};
		bool bRectSelected{};
		std::optional<xPoint2i> ptAnchor;			// Anchor Point in Screen Coordinate
		xPoint2i ptOffset0;			// offset backup
		xPoint2d ptSel0, ptSel1;	// Selection Point in Image Coordinate
	} m_mouse;
	mutable struct {
		xPoint2d pt0, pt1;
		std::chrono::steady_clock::time_point t0, t1;
	} m_smooth_scroll;

	S_OPTION m_option;
	eZOOM m_eZoom{eZOOM::one2one};
	xCoordTrans m_ctScreenFromImage;

public:
	using base_t::base_t;
	/** Constructor */
	xMatView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxALWAYS_SHOW_SB|wxTAB_TRAVERSAL, const wxString& name = wxEmptyString ) noexcept;
	~xMatView();

public:
	bool SetImage(cv::Mat const& img, bool bCenter = true, eZOOM eZoomMode = eZOOM::none, bool bCopy = false);
	bool SetZoomMode(eZOOM eZoomMode, bool bCenter = true);
	std::optional<xRect2i> GetSelectionRect() const {
		if (!m_mouse.bRectSelected)
			return {};
		xRect2i rect(xPoint2i(gtl::Floor(m_mouse.ptSel0)), xPoint2i(gtl::Floor(m_mouse.ptSel1)));
		rect.NormalizeRect();
		return rect;
	}
	std::optional<std::pair<xPoint2d, xPoint2d>> GetSelectionPoints() const {
		if (!m_mouse.bRectSelected)
			return {};
		return std::pair<xPoint2d, xPoint2d>{m_mouse.ptSel0, m_mouse.ptSel1};
	}
	void SetSelectionRect(xRect2i const& rect);
	void ClearSelectionRect();

	S_OPTION const& GetOption() const { return m_option; }
	void SetOption(S_OPTION const& option);

	//virtual void OnClose(wxCloseEvent& event) override;

	// ClientRect, ImageRect, ScrollRange
	S_SCROLL_GEOMETRY GetScrollGeometry();
	bool UpdateCT(bool bCenter = false, eZOOM eZoom = eZOOM::none);
	bool UpdateScrollBars();
	bool ZoomInOut(double step, xPoint2i ptAnchor, bool bCenter);
	bool SetZoom(double scale, xPoint2i ptAnchor, bool bCenter);
	bool ScrollTo(xPoint2d pt, stdc::milliseconds tsScroll = -1ms);
	bool Scroll(xPoint2d delta, stdc::milliseconds tsScroll = -1ms);
	bool KeyboardNavigate(int key, bool ctrl = false, bool alt = false, bool shift = false);

protected:
	bool MakeScreenCache(std::stop_token stop);
	bool OnKeyEvent(wxKeyEvent& event);

protected:
	virtual void OnCharHook( wxKeyEvent& event ) override;
	virtual void OnCombobox_ZoomMode( wxCommandEvent& event ) override;
	void OnSpinCtrl(double scale);
	virtual void OnSpinCtrlDouble_ZoomValue( wxSpinDoubleEvent& event ) override;
	virtual void OnTextEnter_ZoomValue( wxCommandEvent& event ) override;
	virtual void OnZoomIn( wxCommandEvent& event ) override;
	virtual void OnZoomOut( wxCommandEvent& event ) override;
	virtual void OnZoomFit( wxCommandEvent& event ) override;
	virtual void OnSetings( wxCommandEvent& event ) override;
	virtual void OnCharHook_View( wxKeyEvent& event ) override { event.Skip();/*OnKeyEvent(event);*/ }
	virtual void OnLeftDown_View( wxMouseEvent& event ) override;
	virtual void OnLeftUp_View( wxMouseEvent& event ) override;
	virtual void OnMiddleDown_View( wxMouseEvent& event ) override;
	virtual void OnMiddleUp_View( wxMouseEvent& event ) override;
	virtual void OnMotion_View( wxMouseEvent& event ) override;
	virtual void OnMouseWheel_View( wxMouseEvent& event ) override;
	virtual void OnPaint_View( wxPaintEvent& event ) override;
	virtual void OnRightDown_View( wxMouseEvent& event ) override;
	virtual void OnRightUp_View( wxMouseEvent& event ) override;
	virtual void OnSize_View( wxSizeEvent& event ) override;
	virtual void OnTimerScroll( wxTimerEvent& event ) override;
	void OnScrollWin_View( wxScrollWinEvent& event);
	void OnMouseCaptureLost_View(wxMouseCaptureLostEvent& event);
};

#endif // __xMatView__
