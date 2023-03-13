#include "pch.h"
#include "MatView.h"
#include "wxGLCanvasAdapter.h"
#include "DlgMatViewOption.h"

#ifdef SK_GL
#include "skia/core/SkTypes.h"
#include "skia/core/SkCanvas.h"
#include "skia/core/SkData.h"
#include "skia/core/SkImage.h"
#include "skia/core/SkStream.h"
#include "skia/gpu/GrDirectContext.h"
#include "skia/gpu/GrBackendSurface.h"
#include "skia/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/core/SkColorSpace.h"
#endif

inline cv::Scalar GetMatValue(uchar const* ptr, int depth, int channel, int row, int col) {
	//if ( mat.empty() or (row < 0) or (row >= mat.rows) or (col < 0) or (col >= mat.cols) )
	//	return;

	cv::Scalar v;
	auto GetValue = [&]<typename T>(T&&){
		for (int i = 0; i < channel; ++i)
			v[i] = ptr[col * channel + i];
	};
	switch (depth) {
	case CV_8U:		GetValue(uint8_t{}); break;
	case CV_8S:		GetValue(int8_t{}); break;
	case CV_16U:	GetValue(uint16_t{}); break;
	case CV_16S:	GetValue(int16_t{}); break;
	case CV_32S:	GetValue(int32_t{}); break;
	case CV_32F:	GetValue(float{}); break;
	case CV_64F:	GetValue(double{}); break;
	//case CV_16F:	GetValue(uint16_t{}); break;
	}

	return v;
}

// returns eInternalColorFormat, ColorFormat, PixelType
std::tuple<GLint, GLenum, GLenum> GetGLImageFormatType(int type) {
	static std::unordered_map<int, std::tuple<GLint, GLenum, GLenum>> const m {
		{ CV_8UC1,	{GL_LUMINANCE,	GL_LUMINANCE,		GL_UNSIGNED_BYTE	} },
		{ CV_8UC3,	{GL_RGB,		GL_RGB,				GL_UNSIGNED_BYTE	} },
		{ CV_8UC4,	{GL_RGBA,		GL_RGBA,			GL_UNSIGNED_BYTE	} },
		{ CV_16UC1,	{GL_LUMINANCE,	GL_LUMINANCE,		GL_UNSIGNED_SHORT	} },
		{ CV_16UC3,	{GL_RGB,		GL_RGB,				GL_UNSIGNED_SHORT	} },
		{ CV_16UC4,	{GL_RGBA,		GL_RGBA,			GL_UNSIGNED_SHORT	} },
		{ CV_16SC1,	{GL_LUMINANCE,	GL_LUMINANCE,		GL_SHORT			} },
		{ CV_16SC3,	{GL_RGB,		GL_RGB,				GL_SHORT			} },
		{ CV_16SC4,	{GL_RGBA,		GL_RGBA,			GL_SHORT			} },
		{ CV_32SC1,	{GL_LUMINANCE,	GL_LUMINANCE,		GL_INT				} },
		{ CV_32SC3,	{GL_RGB,		GL_RGB,				GL_INT				} },
		{ CV_32SC4,	{GL_RGBA,		GL_RGBA,			GL_INT				} },
		{ CV_32FC1,	{GL_LUMINANCE,	GL_LUMINANCE,		GL_FLOAT			} },
		{ CV_32FC3,	{GL_RGB,		GL_RGB,				GL_FLOAT			} },
		{ CV_32FC4,	{GL_RGBA,		GL_RGBA,			GL_FLOAT			} },
	};
	if (auto iter = m.find(type); iter != m.end())
		return iter->second;
	return {};
}

bool PutMatAsTexture(GLuint textureID, cv::Mat const& img, int width, gtl::xRect2i const& rect) {
	if (!textureID or img.empty() or !img.isContinuous() /*or (img.step %4)*/)
		return false;

	glBindTexture(GL_TEXTURE_2D, textureID);
	if (img.step%4)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Create the texture
	auto [eColorType, eFormat, ePixelType] = GetGLImageFormatType(img.type());
	glTexImage2D(GL_TEXTURE_2D, 0, eColorType, img.cols, img.rows, 0, eFormat, ePixelType, img.ptr());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// patch
	glBegin(GL_QUADS);
	double r = (double)width/img.cols;
	glTexCoord2f(0.f, 0.f);	glVertex2i(rect.left,   rect.top);
	glTexCoord2f(0.f, 1.f);	glVertex2i(rect.left,   rect.bottom);
	glTexCoord2f(r, 1.f);	glVertex2i(rect.right,  rect.bottom);
	glTexCoord2f(r, 0.f);	glVertex2i(rect.right,  rect.top);
	glEnd();
}

bool DrawPixelValue(cv::Mat& canvas, cv::Mat const& imgOriginal, cv::Rect roi, gtl::xCoordTrans2d const& ctCanvasFromImage) {
	using xPoint2d = gtl::xPoint2d;

	// Draw Grid / pixel value
	if (ctCanvasFromImage.m_scale < 4)
		return false;
	cv::Scalar cr{127, 127, 127, 255};
	// grid - horizontal
	for (int y{roi.y}, y1{roi.y+roi.height}; y < y1; y++) {
		auto pt0 = ctCanvasFromImage(xPoint2d{roi.x, y});
		auto pt1 = ctCanvasFromImage(xPoint2d{roi.x+roi.width, y});
		cv::line(canvas, pt0, pt1, cr);
	}
	// grid - vertical
	for (int x{roi.x}, x1{roi.x+roi.width}; x < x1; x++) {
		auto pt0 = ctCanvasFromImage(xPoint2d{x, roi.y});
		auto pt1 = ctCanvasFromImage(xPoint2d{x, roi.y+roi.height});
		cv::line(canvas, pt0, pt1, cr);
	}

	// Pixel Value
	auto nChannel = imgOriginal.channels();
	auto depth = imgOriginal.depth();
	double const minHeight = 8;

	if ( ctCanvasFromImage.m_scale < ((nChannel+0.5)*minHeight) )
		return false;
	double heightFont = std::min(20., ctCanvasFromImage.m_scale/(nChannel+0.5)) / 40.;
	//auto t0 = stdc::steady_clock::now();
	for (int y{roi.y}, y1{roi.y+roi.height}; y < y1; y++) {
		auto* ptr = imgOriginal.ptr(y);
		int x1{roi.x+roi.width};
		//#pragma omp parallel for --------- little improvement
		for (int x{roi.x}; x < x1; x++) {
			auto pt = ctCanvasFromImage(xPoint2d{x, y});
			//auto p = SkPoint::Make(pt.x, pt.y);
			auto& org = imgOriginal;
			auto v = GetMatValue(ptr, depth, nChannel, y, x);
			auto avg = (v[0] + v[1] + v[2]) / nChannel;
			auto cr = (avg > 128) ? cv::Scalar{0, 0, 0, 255} : cv::Scalar{255, 255, 255, 255};
			for (int ch{}; ch < nChannel; ch++) {
				auto str = fmt::format("{:3}", v[ch]);
				cv::putText(canvas, str, cv::Point(pt.x, pt.y+(ch+1)*minHeight*heightFont*5), 0, heightFont, cr, 1, true);
			}
		}
	}
	//auto t1 = stdc::steady_clock::now();
	//auto dur = stdc::duration_cast<stdc::milliseconds>(t1-t0).count();
	//OutputDebugString(std::format(L"{} ms\n", dur).c_str());

	return true;
}

#ifdef SK_GL
bool DrawPixelValue(SkCanvas& canvas, sk_sp<SkTypeface>& skTypeface, cv::Mat const& imgOriginal, cv::Rect roi, gtl::xCoordTrans2d const& ctCanvasFromImage) {
	using xPoint2d = gtl::xPoint2d;

	// Draw Grid / pixel value
	if (ctCanvasFromImage.m_scale < 4)
		return false;

	SkPaint paint;
	paint.setColor(SkColorSetARGB(255, 255, 255, 255));
	paint.setBlendMode(SkBlendMode::kXor);
	// grid - horizontal
	for (int y{roi.y}, y1{roi.y+roi.height}; y < y1; y++) {
		auto pt0 = ctCanvasFromImage(xPoint2d{roi.x, y});
		auto pt1 = ctCanvasFromImage(xPoint2d{roi.x+roi.width, y});
		auto p0 = SkPoint::Make(pt0.x, pt0.y);
		auto p1 = SkPoint::Make(pt1.x, pt1.y);
		canvas.drawLine(p0, p1, paint);
	}
	// grid - vertical
	for (int x{roi.x}, x1{roi.x+roi.width}; x < x1; x++) {
		auto pt0 = ctCanvasFromImage(xPoint2d{x, roi.y});
		auto pt1 = ctCanvasFromImage(xPoint2d{x, roi.y+roi.height});
		auto p0 = SkPoint::Make(pt0.x, pt0.y);
		auto p1 = SkPoint::Make(pt1.x, pt1.y);
		canvas.drawLine(p0, p1, paint);
	}

	// Pixel Value
	auto nChannel = imgOriginal.channels();
	auto depth = imgOriginal.depth();
	double const minHeightFont = 8;

	if ( skTypeface and (ctCanvasFromImage.m_scale >= ((nChannel+0.5)*minHeightFont)) ) {
		SkPaint paintW, paintB;
		paintW.setColor(SkColorSetARGB(255, 255, 255, 255));
		paintB.setColor(SkColorSetARGB(255, 0, 0, 0));

		//SkFont font;
		SkFont font(skTypeface);
		font.setSubpixel(false);
		double heightFont = std::min(20., ctCanvasFromImage.m_scale/(nChannel+0.5));
		font.setSize(heightFont);
		//auto t0 = stdc::steady_clock::now();
		for (int y{roi.y}, y1{roi.y+roi.height}; y < y1; y++) {
			auto* ptr = imgOriginal.ptr(y);
			int x1{roi.x+roi.width};
		//#pragma omp parallel for ...... NOT for multi-thread.
			for (int x{roi.x}; x < x1; x++) {
				auto pt = ctCanvasFromImage(xPoint2d{x, y});
				auto p = SkPoint::Make(pt.x, pt.y);
				auto& org = imgOriginal;
				auto v = GetMatValue(ptr, depth, nChannel, y, x);
				auto avg = (v[0] + v[1] + v[2]) / nChannel;
				auto& paint = avg > 127 ? paintB : paintW;
				auto& paintO = avg > 127 ? paintW : paintB;
				for (int ch{}; ch < nChannel; ch++) {
					auto str = fmt::format(u8"{:3}", v[ch]);
				#if 0
					// 외곽선
					canvas.drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, p.x()+1, p.y()+1+(ch+1)*heightFont, font, paintO);
					canvas.drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, p.x()+1, p.y()-1+(ch+1)*heightFont, font, paintO);
					canvas.drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, p.x()-1, p.y()+1+(ch+1)*heightFont, font, paintO);
					canvas.drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, p.x()-1, p.y()-1+(ch+1)*heightFont, font, paintO);
				#endif
					canvas.drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, p.x(), p.y()+(ch+1)*heightFont, font, paint);
				}
			}
		}
		//auto t1 = stdc::steady_clock::now();
		//auto dur = stdc::duration_cast<stdc::milliseconds>(t1-t0).count();
		//OutputDebugString(std::format(L"{} ms\n", dur).c_str());
	}

	return true;
}
#endif

//xMatView::xMatView( wxWindow* parent ) : base_t( parent ) {
//
//}
static double const dZoomLevels[] = {
	1./8192, 1./4096, 1./2048, 1./1024,
	1./512, 1./256, 1./128, 1./64, 1./32, 1./16, 1./8, 1./4., 1./2.,
	3./4, 1, 1.25, 1.5, 1.75, 2, 2.5, 3, 4, 5, 6, 7, 8, 9, 10,
	12.5, 15, 17.5, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100,
	125, 150, 175, 200, 250, 300, 350, 400, 450, 500,
	600, 700, 800, 900, 1000,
	//1250, 1500, 1750, 2000, 2500, 3000, 3500, 4000, 4500, 5000,
	//6000, 7000, 8000, 9000, 10000,
	//12500, 15000, 17500, 20000, 25000, 30000, 35000, 40000, 45000, 50000,
	//60000, 70000, 80000, 90000, 100000,
	//125000, 150000, 175000, 200000, 250000, 300000, 350000, 400000, 450000, 500000,
	//600000, 700000, 800000, 900000, 1000000,
	//1250000, 1500000, 1750000, 2000000, 2500000, 3000000, 3500000, 4000000, 4500000, 5000000,
	//6000000, 7000000, 8000000, 9000000, 10000000,
	//12500000, 15000000, 17500000, 20000000, 25000000, 30000000, 35000000, 40000000, 45000000, 50000000,
	//60000000, 70000000, 80000000, 90000000
};

xMatView::xMatView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) noexcept
 : ui::IMatView(parent, id, pos, size, style, name)
{
	m_context = std::make_unique<wxGLContext>(m_view);
	m_view->SetCurrent(*m_context);
	//glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		const GLubyte* msg = glewGetErrorString(err);
		wxString str(msg);
		wxMessageBox(str);
		//throw std::exception(reinterpret_cast<const char*>(msg));
	}

	m_view->Connect(wxEVT_SCROLLWIN_TOP,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_BOTTOM,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_LINEUP,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_LINEDOWN,		wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_PAGEUP,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_PAGEDOWN,		wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_THUMBTRACK,		wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_SCROLLWIN_THUMBRELEASE,	wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Connect(wxEVT_MOUSE_CAPTURE_LOST,		wxMouseCaptureLostEventHandler(xMatView::OnMouseCaptureLost_View), nullptr, this);

#ifdef SK_GL
	//m_option.bGLonly = false;
	//m_option.bSkia = true;
	m_skContext = GrDirectContext::MakeGL();

	// todo: get font folder from shell
	m_skTypeface = SkTypeface::MakeFromFile("C:\\Windows\\Fonts\\batang.ttc", 3);
#endif

#if defined(_DEBUG)
	// Color Bar
	//cv::Mat img = cv::Mat::zeros(1080*2, 1920*2, CV_8UC3);
	cv::Mat img = cv::Mat::zeros(800, 600, CV_8UC3);
	{
		auto cy = img.rows/3;
		auto cy2 = cy*2;
		auto v = (img.depth() == CV_8U) ? 255. : 1.;
		auto a = v / cy;
		for (int y{}; y < cy; y++) {
			double v = a*(y+1.);
			//if (y&1)
			//	continue;
			img.row(y) = cv::Scalar(v, 0, 0, 0);
			img.row(y+cy) = cv::Scalar(0, v, 0, 255);
			img.row(y+cy2) = cv::Scalar(0, 0, v, 255);
		}
		for (int x{}; x < img.cols; x++) {
			double v = (x+1.)/img.cols;
			img.col(x) *= (x+1.)/img.cols;
		}
		img.col(0) = cv::Scalar(v, v, v);
		img.col(img.cols-1) = cv::Scalar(v, v, v);
		img.row(0) = cv::Scalar(v, v, v);
		img.row(img.rows-1) = cv::Scalar(v, v, v);
	}
	// Color Bar
	//img = cv::Mat::zeros(1080*2, 1920*2, CV_32SC3);
	//{
	//	auto cy = img.rows/3;
	//	auto cy2 = cy*2;
	//	for (int y{}; y < cy; y++) {
	//		double v = y*(std::numeric_limits<int>::max()/cy);//(y+1.)/cy;
	//		img.row(y) = cv::Scalar(v, 0, 0);
	//		img.row(y+cy) = cv::Scalar(0, v, 0);
	//		img.row(y+cy2) = cv::Scalar(0, 0, v);
	//	}
	//}
	//m_option.bGLonly = false;
	//m_option.bSkia = true;
	SetImage(img, true, eZOOM::fit2window, false);
#endif
}

xMatView::~xMatView() {

	m_view->SetCurrent(*m_context);
#ifdef SK_GL
	m_skTypeface.reset();
	m_skContext.reset();
#endif

	m_cmbZoomMode->SetSelection(std::to_underlying(m_eZoom));

	m_view->Disconnect(wxEVT_SCROLLWIN_TOP,				wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_BOTTOM,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_LINEUP,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_LINEDOWN,		wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_PAGEUP,			wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_PAGEDOWN,		wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_THUMBTRACK,		wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );
	m_view->Disconnect(wxEVT_SCROLLWIN_THUMBRELEASE,	wxScrollWinEventHandler(xMatView::OnScrollWin_View), nullptr, this );

}

bool xMatView::SetImage(cv::Mat const& img, bool bCenter, eZOOM eZoomMode, bool bCopy) {
	wxBusyCursor wc;

	// original image
	if (bCopy)
		img.copyTo(m_imgOriginal);
	else
		m_imgOriginal = img;

#ifdef SK_GL
	// for display (skia uses 4channel image)
	if (img.channels() != 4) {
		bCopy = false;
		int eCode{};
		if (img.channels() == 1)
			eCode = cv::COLOR_GRAY2RGBA;
		else if (img.channels() == 3)
			eCode = cv::COLOR_BGR2RGBA;
		if (eCode)
			cv::cvtColor(img, m_img, eCode);
		else
			return false;
	} else {
		if (&img != &m_img) {
			if (bCopy)
				img.copyTo(m_img);
			else
				m_img = img;
		}
	}
#else
	m_img = m_imgOriginal;
#endif

	// Build Pyramid Image for down sampling { cv::InterpolationFlags::INTER_AREA }
	if (m_pyramid.threadPyramidMaker.joinable())
		m_pyramid.threadPyramidMaker.join();
	m_pyramid.imgs.clear();
	m_pyramid.imgs.push_front(m_img);
	if (m_option.bPyrImageDown) {
		m_pyramid.threadPyramidMaker = std::jthread([this](std::stop_token stop) {
			cv::Mat imgPyr = m_pyramid.imgs[0];
			while (!stop.stop_requested() and (imgPyr.size().area() > 1'000*1'000) ) {
				cv::pyrDown(imgPyr, imgPyr);
				{
					std::unique_lock lock{m_pyramid.mtx};
					m_pyramid.imgs.emplace_front(imgPyr);
				}
			}
		});
	}

	// check (opengl) texture format
	auto [eColorType, eFormat, ePixelType] = GetGLImageFormatType(m_img.type());
	if (eColorType == 0)
		return false;

	if (!m_img.isContinuous()) {
		assert(false);
		return false;
	}

	if (eZoomMode != eZOOM::none) {
		base_t::m_cmbZoomMode->SetSelection((int)eZoomMode);
		wxCommandEvent evt;
		OnCombobox_ZoomMode(evt);
	} else {
		UpdateCT(bCenter);
		UpdateScrollBars();
		m_view->Refresh();
		m_view->Update();
	}
	return true;
}

bool xMatView::SetZoomMode(eZOOM eZoomMode, bool bCenter) {
	m_cmbZoomMode->SetSelection((int)eZoomMode);
	UpdateCT(bCenter, eZoomMode);
	UpdateScrollBars();
	m_view->Refresh();
	return false;
}

void xMatView::SetSelectionRect(xRect2i const& rect) {
	m_mouse.bRectSelected = true;
	m_mouse.ptSel0 = rect.pt0();
	m_mouse.ptSel1 = rect.pt1();
	m_view->Refresh(false);
	m_view->Update();
}
void xMatView::ClearSelectionRect() {
	m_mouse.bRectSelected = false;
}

void xMatView::SetOption(S_OPTION const& option) {
	if (&m_option != &option)
		m_option = option;

	UpdateCT(false, eZOOM::none);
	UpdateScrollBars();
	m_view->Refresh(false);
	m_view->Update();
}

bool xMatView::UpdateCT(bool bCenter, eZOOM eZoom) {
	if (m_img.empty())
		return false;
	if (eZoom == eZOOM::none)
		eZoom = m_eZoom;

	xRect2i rectClient;
	rectClient = m_view->GetClientRect();
	xSize2i sizeClient;
	sizeClient = m_view->GetClientSize();

	// scale
	double dScale{-1.0};
	switch (eZoom) {
		using enum eZOOM;
	case one2one:		dScale = 1.0; break;
	case fit2window:	dScale = std::min((double)sizeClient.cx / m_img.cols, (double)sizeClient.cy / m_img.rows); break;
	case fit2width:		dScale = (double)sizeClient.cx / m_img.cols; break;
	case fit2height:	dScale = (double)sizeClient.cy / m_img.rows; break;
	//case free:			dScale = m_ctScreenFromImage.m_scale; break;
	}
	if (dScale > 0)
		m_ctScreenFromImage.m_scale = dScale;

	// constraints. make image put on the center of the screen
	if ( bCenter or gtl::IsValueOneOf(eZoom, eZOOM::fit2window, eZOOM::fit2width, eZOOM::fit2height) ) {
		xCoordTrans ct2 = m_ctScreenFromImage;
		ct2.m_origin = xPoint2d(m_img.size())/2.;
		ct2.m_offset = xRect2d(rectClient).CenterPoint();

		xPoint2d ptOrigin = {};
		xPoint2i ptLT = ct2(ptOrigin);

		if (bCenter or eZoom == eZOOM::fit2window) {
			m_ctScreenFromImage.m_origin = {};
			m_ctScreenFromImage.m_offset = ptLT;
		}
		else if (eZoom == eZOOM::fit2width) {
			m_ctScreenFromImage.m_origin.x = 0;
			m_ctScreenFromImage.m_offset.x = ptLT.x;
		}
		else if (eZoom == eZOOM::fit2height) {
			m_ctScreenFromImage.m_origin.y = 0;
			m_ctScreenFromImage.m_offset.y = ptLT.y;
		}
	}

	// panning constraints.
	xPoint2d pt0 = m_ctScreenFromImage(xPoint2d(0,0));
	xPoint2d pt1 = m_ctScreenFromImage(xPoint2d(m_img.cols, m_img.rows));
	xRect2i rectImageScreen(pt0, pt1);
	rectImageScreen.NormalizeRect();
	if (m_option.bExtendedPanning) {
		int marginX {std::min(m_option.nScrollMargin, rectImageScreen.Width())};
		int marginY {std::min(m_option.nScrollMargin, rectImageScreen.Height())};
		// make any part of image stays inside the screen
		if (rectImageScreen.right < rectClient.left) {
			m_ctScreenFromImage.m_offset.x += rectClient.left - rectImageScreen.right + marginX;
		}
		else if (rectImageScreen.left > rectClient.right) {
			m_ctScreenFromImage.m_offset.x += rectClient.right - rectImageScreen.left - marginX;
		}
		if (rectImageScreen.bottom < rectClient.top) {
			m_ctScreenFromImage.m_offset.y += rectClient.top - rectImageScreen.bottom + marginY;
		}
		else if (rectImageScreen.top > rectClient.bottom) {
			m_ctScreenFromImage.m_offset.y += rectClient.bottom - rectImageScreen.top - marginY;
		}
	} else {
		// default panning. make image stays inside the screen
		if (rectImageScreen.Width() <= rectClient.Width()) {
			auto pt = m_ctScreenFromImage(xPoint2d(m_img.cols/2, 0));
			m_ctScreenFromImage.m_offset.x += rectClient.CenterPoint().x - pt.x;
		}
		if (rectImageScreen.Width() > rectClient.Width()) {
			if (rectImageScreen.left > rectClient.left)
				m_ctScreenFromImage.m_offset.x += rectClient.left - rectImageScreen.left;
			else if (rectImageScreen.right < rectClient.right)
				m_ctScreenFromImage.m_offset.x += rectClient.right - rectImageScreen.right;
		}
		if (rectImageScreen.Height() <= rectClient.Height()) {
			auto pt = m_ctScreenFromImage(xPoint2d(0, m_img.rows/2));
			m_ctScreenFromImage.m_offset.y += rectClient.Height() / 2 - pt.y;
		}
		if (rectImageScreen.Height() > rectClient.Height()) {
			if (rectImageScreen.top > rectClient.top)
				m_ctScreenFromImage.m_offset.y += rectClient.top - rectImageScreen.top;
			else if (rectImageScreen.bottom < rectClient.bottom)
				m_ctScreenFromImage.m_offset.y += rectClient.bottom - rectImageScreen.bottom;
		}
	}

	return true;
}

xMatView::S_SCROLL_GEOMETRY xMatView::GetScrollGeometry() {
	xRect2i rectClient{m_view->GetClientRect()};
	auto pt0 = m_ctScreenFromImage(xPoint2d(0,0));
	auto pt1 = m_ctScreenFromImage(xPoint2d(m_img.cols, m_img.rows));
	xRect2i rectImageScreen(pt0, pt1);
	rectImageScreen.NormalizeRect();
	xRect2i rectScrollRange(rectClient);
	if (m_option.bExtendedPanning) {
		rectScrollRange.pt0() -= rectImageScreen.GetSize();
		rectScrollRange.pt1() += rectClient.GetSize();
		int nScrollMarginX = std::min(m_option.nScrollMargin, rectImageScreen.Width());
		int nScrollMarginY = std::min(m_option.nScrollMargin, rectImageScreen.Height());
		rectScrollRange.DeflateRect(nScrollMarginX, nScrollMarginY);
	} else {
		rectScrollRange |= rectImageScreen;
	}
	return {rectClient, rectImageScreen, rectScrollRange};
}

bool xMatView::UpdateScrollBars() {
	if (m_img.empty())
		return false;

	using namespace gtl;

	auto [rectClient, rectImageScreen, rectScrollRange] = GetScrollGeometry();

	if (m_view->GetWindowStyleFlag() & wxHSCROLL) {
		int p = m_option.bExtendedPanning
			? rectScrollRange.Width() - (m_ctScreenFromImage.m_offset.x + std::max(0, rectImageScreen.Width() - m_option.nScrollMargin) + rectClient.Width())
			: -m_ctScreenFromImage.m_offset.x;
		p = std::clamp(p, 0, rectScrollRange.Width());
		m_view->SetScrollbar(wxHORIZONTAL, p, rectClient.Width(), rectScrollRange.Width(), true);
	}

	if (m_view->GetWindowStyleFlag() & wxVSCROLL) {
		int p = m_option.bExtendedPanning
			? rectScrollRange.Height() - (m_ctScreenFromImage.m_offset.y + std::max(0, rectImageScreen.Height() - m_option.nScrollMargin) + rectClient.Height())
			: -m_ctScreenFromImage.m_offset.y;
		p = std::clamp(p, 0, rectScrollRange.Height());
		m_view->SetScrollbar(wxVERTICAL, p, rectClient.Height(), rectScrollRange.Height(), true);
	}

	return true;
}

bool xMatView::ZoomInOut(double step, xPoint2i ptAnchor, bool bCenter) {
	auto scale = m_ctScreenFromImage.m_scale;
	if (step > 0) {
		for (auto dZoom : dZoomLevels) {
			if (dZoom > scale) {
				scale = dZoom;
				break;
			}
		}
	}
	else {
		for (auto pos = std::rbegin(dZoomLevels); pos != std::rend(dZoomLevels); pos++) {
			auto dZoom = *pos;
			if (dZoom < scale) {
				scale = dZoom;
				break;
			}
		}
	}
	return SetZoom(scale, ptAnchor, bCenter);
}

bool xMatView::SetZoom(double scale, xPoint2i ptAnchor, bool bCenter) {
	if (m_img.empty())
		return false;
	// Backup Image Position
	xPoint2d ptImage = m_ctScreenFromImage.TransI(xPoint2d(ptAnchor));
	// Get Scroll Amount
	if (m_ctScreenFromImage.m_scale == scale)
		return true;
	else
		m_ctScreenFromImage.m_scale = scale;

	auto rectClient = GetClientRect();
	auto dMinZoom = std::max(4., rectClient.width/16.) / std::min(m_img.cols, m_img.rows);
	m_ctScreenFromImage.m_scale = std::clamp(m_ctScreenFromImage.m_scale, dMinZoom, 1.e3);
	m_ctScreenFromImage.m_offset += ptAnchor - m_ctScreenFromImage(ptImage);
	// Anchor point
	if (m_eZoom != eZOOM::mouse_wheel_locked)
		m_eZoom = eZOOM::free;
	base_t::m_cmbZoomMode->SetSelection((int)m_eZoom);
	UpdateCT(bCenter);
	UpdateScrollBars();
	m_view->Refresh(false);
	m_view->Update();
	return true;
}

bool xMatView::ScrollTo(xPoint2d pt, stdc::milliseconds tsScroll) {
	if (tsScroll == 0ms) {
		m_timerScroll.Stop();	// 이미 동작하고 있는 Scroll 중지
		m_ctScreenFromImage.m_offset = pt;
		UpdateCT(false);
		UpdateScrollBars();
		m_view->Refresh();
		m_view->Update();
	}
	else {
		m_smooth_scroll.pt0 = m_ctScreenFromImage.m_offset;
		m_smooth_scroll.pt1 = pt;
		m_smooth_scroll.t0 = std::chrono::steady_clock::now();
		if (tsScroll < 0ms)
			tsScroll = m_option.tsScroll;
		m_smooth_scroll.t1 = m_smooth_scroll.t0 + tsScroll;
		m_timerScroll.Start(10);
	}
	return true;
}

bool xMatView::Scroll(xPoint2d delta, stdc::milliseconds tsScroll) {
	return ScrollTo(m_ctScreenFromImage.m_offset + delta, tsScroll);
}

bool xMatView::KeyboardNavigate(int key, bool ctrl, bool alt, bool shift) {
	if (ctrl or alt or m_img.empty())
		return false;

	xCoordTrans ctS2I;
	m_ctScreenFromImage.GetInv(ctS2I);
	xRect2i rectClient{m_view->GetClientRect()};

	switch (key) {
	case WXK_LEFT :
	case WXK_RIGHT :
	case WXK_UP :
	case WXK_DOWN :
	case WXK_PAGEUP:
	case WXK_PAGEDOWN :
		{
			auto ptShift = ctS2I(xPoint2d{100., 100.}) - ctS2I(xPoint2d{0., 0.});
			auto ptShiftPage = ctS2I(rectClient.pt1()) - ctS2I(rectClient.pt0());
			ptShiftPage *= 0.9;

			xPoint2d delta;
			if (key == WXK_LEFT)			delta.x += ptShift.x;
			else if (key == WXK_RIGHT)		delta.x -= ptShift.x;
			else if (key == WXK_UP)			delta.y += ptShift.y;
			else if (key == WXK_DOWN)		delta.y -= ptShift.y;
			else if (key == WXK_PAGEUP)		delta.y += ptShiftPage.y;
			else if (key == WXK_PAGEDOWN)	delta.y -= ptShiftPage.y;

			Scroll(delta);
		}
		break;

	case WXK_SPACE :
		{
			int heightMin { (int) (ctS2I.Trans((double)rectClient.Height()) * 5.0 / 10) };
			int deltaDefaultScreen{ rectClient.Height() * 95 / 100 };
			double deltaImage { ctS2I.Trans((double)deltaDefaultScreen) };
			int heightMax { (int) (ctS2I.Trans((double)rectClient.Height()) * 9.5 / 10) };
			xPoint2i ptImgBottom = ctS2I(xPoint2i{rectClient.left, rectClient.bottom});
			xPoint2i ptImgTop = ctS2I(xPoint2i{rectClient.left, rectClient.top});
			auto y0 = std::min(ptImgBottom.y + heightMax, m_img.rows - 1);

			auto CheckIfBlank = [&](int y) -> bool {
				cv::Rect rc(0, y, m_img.cols, 1);
				if (!gtl::IsROI_Valid(rc, m_img.size()))
					return false;
				auto img = m_img(rc);
				if (img.empty())
					return true;
				//auto const vMax = 250;
				auto const vMin = 5;
				if (img.channels() == 4) {
					auto* ptr = img.ptr<cv::Vec4b>(0);
					auto min = ptr[0], max = ptr[0];
					int nDiff{};
					for (int x = 1; x < img.cols; x++) {
						auto const& v = ptr[x];
						for (int i = 0; i < 3; i++) {
							min[i] = std::min(v[i], min[i]);
							max[i] = std::max(v[i], max[i]);
							if (max[i] - min[i] > vMin) {
								if (nDiff++ > 2)
									return false;
							}
						}
					}
				}
				return true;
			};

			{
				if (CheckIfBlank(ptImgBottom.y)) {
					auto yE = m_img.rows;
					for (auto y = ptImgBottom.y; y < yE; y++) {
						if (!CheckIfBlank(y)) {
							deltaImage = y-ptImgTop.y;
							break;
						}
					}
				}
				else {
					deltaImage = heightMax;
					auto y2 = ptImgTop.y + heightMin*1/2;
					auto y = ptImgBottom.y;
					for (; y >= y2; y--) {
						if (CheckIfBlank(y)) {
							deltaImage = y - ptImgTop.y;
							break;
						}
					}
					if (y < y2) {	// not found
						auto yE = ptImgBottom.y + heightMax;
						for (; y < yE; y++) {
							if (CheckIfBlank(y)) {
								deltaImage = y - ptImgBottom.y;
								break;
							}
						}
					}
				}
			}

			Scroll({0., -deltaImage});
		}
		break;

	case WXK_HOME :
		ScrollTo({});
		break;

	case WXK_END :
		{
			auto pt0 = m_ctScreenFromImage(xPoint2d{0.0, (double)0.0});
			auto pt1 = m_ctScreenFromImage(xPoint2d{0.0, (double)m_img.rows});
			auto p = (pt0.y - pt1.y) - rectClient.Height();
			ScrollTo({0., p});
		}
		break;

	default:
		return false;
	}
	return true;
}

void xMatView::OnScrollWin_View( wxScrollWinEvent& event) {
	auto const orientation = event.GetOrientation();
	auto const bHorz = orientation == wxHORIZONTAL;
	auto const thumb = event.GetPosition();
	auto const pos0 = m_view->GetScrollPos(orientation);
	auto pos = pos0;

	auto [rectClient, rectImageScreen, rectScrollRange] = GetScrollGeometry();

	auto evt = event.GetEventType();
	bool bUpdateScroll{true};
	int range = bHorz ? rectScrollRange.Width() : rectScrollRange.Height();
	int page =  bHorz ? rectClient.Width() : rectClient.Height();

	if (evt == wxEVT_SCROLLWIN_TOP) { pos = 0; }
	else if (evt == wxEVT_SCROLLWIN_BOTTOM) { pos = range-page; }
	else if (evt == wxEVT_SCROLLWIN_LINEUP) { pos--; }
	else if (evt == wxEVT_SCROLLWIN_LINEDOWN) { pos++; }
	else if (evt == wxEVT_SCROLLWIN_PAGEUP) { pos -= page; }
	else if (evt == wxEVT_SCROLLWIN_PAGEDOWN) { pos += page; }
	else if ( (evt == wxEVT_SCROLLWIN_THUMBTRACK) or (evt == wxEVT_SCROLLWIN_THUMBRELEASE) ) {
		bUpdateScroll = (evt == wxEVT_SCROLLWIN_THUMBRELEASE);
		pos = thumb;
	}

	pos = std::clamp<std::remove_cvref_t<decltype(pos)>>(pos, 0, range);
	if (bHorz) {
		m_ctScreenFromImage.m_offset.x = - pos;
		if (m_option.bExtendedPanning)
			m_ctScreenFromImage.m_offset.x += rectScrollRange.Width() - std::max(0, rectImageScreen.Width() - m_option.nScrollMargin) - rectClient.Width();
	} else {
		m_ctScreenFromImage.m_offset.y = - pos;
		if (m_option.bExtendedPanning)
			m_ctScreenFromImage.m_offset.y += rectScrollRange.Height() - std::max(0, rectImageScreen.Height() - m_option.nScrollMargin) - rectClient.Height();
	}

	if (bUpdateScroll)
		UpdateScrollBars();
	m_view->Refresh(false);
}

void xMatView::OnMouseCaptureLost_View(wxMouseCaptureLostEvent& event) {
	m_mouse.ptAnchor.reset();
}

bool xMatView::MakeScreenCache(std::stop_token stop) {
	return false;
}

void xMatView::OnTimerScroll(wxTimerEvent& event) {
	auto t = std::chrono::steady_clock::now();
	if (m_smooth_scroll.t0 < t and t < m_smooth_scroll.t1) {
		auto& ss = m_smooth_scroll;

		double len = (ss.t1-ss.t0).count();
		double pos = (t-ss.t0).count();
		double s = sin(pos / len * std::numbers::pi/2.);
		m_ctScreenFromImage.m_offset = ss.pt0 + (ss.pt1-ss.pt0) * s;
	}
	if (t >= m_smooth_scroll.t1) {
		m_timerScroll.Stop();
		m_ctScreenFromImage.m_offset = m_smooth_scroll.pt1;
	}
	UpdateCT(false);
	UpdateScrollBars();
	m_view->Refresh(false);
}

//void xMatView::OnClose(wxCloseEvent& event) {
//	Show(false);
//}

bool xMatView::OnKeyEvent(wxKeyEvent& event) {
	//// Print Char Pressed
	//auto l = std::source_location::current();
	//OutputDebugString(std::format(L"{}: {}\n", gtl::ToStringW(l.function_name()), event.GetKeyCode()).c_str());

	if ( (event.GetKeyCode() == WXK_ESCAPE)
		and (m_mouse.bRectSelected or m_mouse.bInSelectionMode)
		)
	{
		m_mouse.bRectSelected = m_mouse.bInSelectionMode = false;
		m_view->Refresh(false);
		m_view->Update();
		return true;
	}

	if (m_option.bKeyboardNavigation
		and KeyboardNavigate(event.GetKeyCode(), wxGetKeyState(WXK_CONTROL), wxGetKeyState(WXK_ALT), wxGetKeyState(WXK_SHIFT)))
	{
		return true;
	}

	event.Skip();
	return false;
}

void xMatView::OnCharHook(wxKeyEvent& event) {
	OnKeyEvent(event);
}

void xMatView::OnCombobox_ZoomMode(wxCommandEvent& ) {
	{
		eZOOM cur = (eZOOM)base_t::m_cmbZoomMode->GetCurrentSelection();
		using enum eZOOM;
		if (m_eZoom == cur and gtl::IsValueOneOf(m_eZoom, fit2window, one2one, mouse_wheel_locked, free))
			return;
		m_eZoom = cur;
	}

	// Scroll Bar Visibility
	bool bHorz{true}, bVert{true};
	m_view->AlwaysShowScrollbars(bHorz, bVert);

	UpdateCT(true);
	UpdateScrollBars();
	Refresh(false);
}

void xMatView::OnSpinCtrl(double scale) {
	if ( m_option.bZoomLock and gtl::IsValueNoneOf(m_eZoom, eZOOM::free, eZOOM::mouse_wheel_locked) )
		return;
	if (m_ctScreenFromImage.m_scale == scale)
		return;
	xRect2d rect{m_view->GetClientRect()};
	auto ptImage = m_ctScreenFromImage.TransI(rect.CenterPoint());
	m_ctScreenFromImage.m_scale = scale;
	auto pt2 = m_ctScreenFromImage(ptImage);
	m_ctScreenFromImage.m_offset += rect.CenterPoint() - m_ctScreenFromImage(ptImage);
	UpdateScrollBars();
	Refresh(false);
}

void xMatView::OnSpinCtrlDouble_ZoomValue(wxSpinDoubleEvent& event) {
	double const scale = event.GetValue();
	OnSpinCtrl(scale);
}

void xMatView::OnTextEnter_ZoomValue(wxCommandEvent& event) {
	// 발생 안함....
	double const scale = base_t::m_spinCtrlZoom->GetValue();
	OnSpinCtrl(scale);
}

void xMatView::OnZoomIn(wxCommandEvent& event) {
	if ( m_option.bZoomLock and gtl::IsValueNoneOf(m_eZoom, eZOOM::free, eZOOM::mouse_wheel_locked) )
		return;
	ZoomInOut(100, xRect2i{m_view->GetClientRect()}.CenterPoint(), false);
}

void xMatView::OnZoomOut(wxCommandEvent& event) {
	if ( m_option.bZoomLock and gtl::IsValueNoneOf(m_eZoom, eZOOM::free, eZOOM::mouse_wheel_locked) )
		return;
	ZoomInOut(-100, xRect2i{m_view->GetClientRect()}.CenterPoint(), false);
}

void xMatView::OnZoomFit(wxCommandEvent& event) {
	if ( m_option.bZoomLock and gtl::IsValueNoneOf(m_eZoom, eZOOM::free, eZOOM::mouse_wheel_locked) )
		return;
	UpdateCT(true, eZOOM::fit2window);
	UpdateScrollBars();
	m_view->Refresh(false);
	m_view->Update();
}

void xMatView::OnSetings(wxCommandEvent& event) {
	xDlgMatViewOption dlg(m_option, this);
	if (dlg.ShowModal() != wxStandardID::wxID_OK)
		return;
	SetOption(dlg.m_option);
}

void xMatView::OnPaint_View( wxPaintEvent& event ) {
	using namespace gtl;

	//event.Skip();
	if (!m_view or !m_context)
		return;

	// Show zoom Scale
	base_t::m_spinCtrlZoom->SetValue(m_ctScreenFromImage.m_scale);

	// Client Rect
	xRect2i rectClient;
	rectClient = m_view->GetClientRect();
	if (rectClient.IsRectEmpty())
		return;
	xSize2i const size = rectClient.GetSize();

	//==================
	// Get Image - ROI
	auto const& ct = m_ctScreenFromImage;

	// Position
	xRect2i rectImage;
	rectImage.pt0() = ct.TransInverse(xPoint2d(rectClient.pt0())).value_or(xPoint2d{});
	rectImage.pt1() = ct.TransInverse(xPoint2d(rectClient.pt1())).value_or(xPoint2d{});
	rectImage.NormalizeRect();
	rectImage.InflateRect(1, 1);
	rectImage &= xRect2i{ 0, 0, m_img.cols, m_img.rows };
	if (rectImage.IsRectEmpty())
		return;
	cv::Rect roi(rectImage);
	xRect2i rectTarget;
	rectTarget.pt0() = ct(rectImage.pt0());
	rectTarget.pt1() = ct.Trans(xPoint2d(rectImage.pt1()));
	rectTarget.NormalizeRect();
	if (!gtl::IsROI_Valid(roi, m_img.size()))
		return;

	// img (roi)
	cv::Rect rcTarget(cv::Point2i{}, cv::Size2i(rectTarget.Width(), rectTarget.Height()));
	cv::Rect rcTargetC(rcTarget);	// 4 byte align
	if (rcTarget.width*m_img.elemSize() % 4)
		rcTargetC.width = gtl::AdjustAlign32(rcTargetC.width);
	cv::Mat img(rcTargetC.size(), m_img.type());
	//img = m_option.crBackground;
	int eInterpolation = cv::INTER_LINEAR;
	try {
		if (ct.m_scale < 1.) {
			static std::unordered_map<eZOOM_OUT, cv::InterpolationFlags> const mapInterpolation = {
				{eZOOM_OUT::nearest, cv::InterpolationFlags::INTER_NEAREST}, {eZOOM_OUT::area, cv::InterpolationFlags::INTER_AREA},
			};
			if (auto pos = mapInterpolation.find(m_option.eZoomOut); pos != mapInterpolation.end())
				eInterpolation = pos->second;

			// resize from pyramid image
			cv::Size2d size {ct.m_scale*m_img.cols, ct.m_scale*m_img.rows};
			cv::Mat imgPyr;
			{
				std::unique_lock lock{m_pyramid.mtx};
				for (auto const& img : m_pyramid.imgs) {
					if (img.cols < size.width)
						continue;
					imgPyr = img;
					break;
				}
			}
			if (!imgPyr.empty()) {
				double scaleP = (double)imgPyr.cols / m_img.cols;
				double scale = (double)size.width / imgPyr.cols;
				cv::Rect roiP(roi);
				roiP.x *= scaleP;
				roiP.y *= scaleP;
				roiP.width *= scaleP;
				roiP.height *= scaleP;
				if (gtl::IsROI_Valid(roiP, imgPyr.size()))
					cv::resize(imgPyr(roiP), img(rcTarget), rcTarget.size(), 0., 0., eInterpolation);
			}
		}
		else if (ct.m_scale > 1.) {
			static std::unordered_map<eZOOM_IN, cv::InterpolationFlags> const mapInterpolation = {
				{eZOOM_IN::nearest, cv::InterpolationFlags::INTER_NEAREST}, {eZOOM_IN::linear, cv::InterpolationFlags::INTER_LINEAR},
				{eZOOM_IN::bicubic, cv::InterpolationFlags::INTER_CUBIC}, {eZOOM_IN::lanczos4, cv::InterpolationFlags::INTER_LANCZOS4},
			};
			if (auto pos = mapInterpolation.find(m_option.eZoomIn); pos != mapInterpolation.end())
				eInterpolation = pos->second;
			cv::resize(m_img(roi), img(rcTarget), rcTarget.size(), 0., 0., eInterpolation);
		}
		else {
			m_img(roi).copyTo(img(rcTarget));
		}
	} catch (std::exception& e) {
		OutputDebugStringA(std::format("cv::{}.......\n", e.what()).c_str());
	} catch (...) {
		OutputDebugStringA("cv::.......\n");
	}

	//================
	// openGL
	m_view->SetCurrent(*m_context);

	glViewport(0, 0, size.cx, size.cy);

	glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
	glLoadIdentity();
	glOrtho(0.0, size.cx, size.cy, 0.0, 0.0, 100.0);

	glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling
	cv::Scalar cr = m_option.crBackground;
	glClearColor(cr[0]/255.f, cr[1]/255.f, cr[2]/255.f, 1.0f);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window

	gtl::xFinalAction faSwapBuffer{[&]{
		//OutputDebugStringA("Flush\n");
		glFlush();
		m_view->SwapBuffers();
	}};

	if (
		!img.empty()
	#if defined(SK_GL)
		and m_option.bGLonly
	#endif
		)
	{
		if (m_option.bDrawPixelValue) {
			auto ctCanvas = m_ctScreenFromImage;
			ctCanvas.m_offset -= m_ctScreenFromImage(roi.tl());
			DrawPixelValue(img, m_imgOriginal, roi, ctCanvas);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		GLuint textures[2]{};
		glGenTextures(std::size(textures), textures);
		if (!textures[0]) {
			OutputDebugStringA("glGenTextures failed\n");
			return;
		}
		gtl::xFinalAction finalAction([&] {
			glDisable(GL_TEXTURE_2D);
			glDeleteTextures(std::size(textures), textures);
		});

		PutMatAsTexture(textures[0], img, rcTarget.width, rectTarget);

		// Draw Selection Rect
		if (m_mouse.bInSelectionMode or m_mouse.bRectSelected) {
			xRect2i rect;
			rect.pt0() = m_ctScreenFromImage(m_mouse.ptSel0);
			rect.pt1() = m_ctScreenFromImage(m_mouse.ptSel1);
			rect.NormalizeRect();
			rect &= rectClient;
			if (!rect.IsRectEmpty()) {
				cv::Mat rectangle(16, 16, CV_8UC4);
				rectangle = cv::Scalar(255, 255, 127, 128);
				PutMatAsTexture(textures[1], rectangle, rectangle.cols, rect);
			}
		}
	}

#ifdef SK_GL
	//================
	// skia
	//GLuint buf{};
	if (m_option.bSkia and m_skContext) try {
		SkColorType eColorType;
		sk_sp<SkSurface> skSurface;
		{
			GrGLFramebufferInfo framebufferInfo{.fFBOID = 0, .fFormat = GL_RGBA8};
			GrBackendRenderTarget backendRenderTarget(size.cx, size.cy, 0, 0, framebufferInfo);
			eColorType = [&]{
				switch (m_img.depth()) {
				case CV_32F :	return kRGBA_F32_SkColorType;
				case CV_8U :	return kRGBA_8888_SkColorType;
				case CV_16S :	return kR16G16B16A16_unorm_SkColorType;
				default :		return kRGBA_8888_SkColorType;
				}
			}();
			skSurface = SkSurface::MakeFromBackendRenderTarget(m_skContext.get(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin, eColorType, nullptr, nullptr);
		}

		if (skSurface) {
			auto* canvas = skSurface->getCanvas();

			// MAIN. put image
			if (!img.empty()) {
				auto info = SkImageInfo::Make(img.cols, img.rows, eColorType, kOpaque_SkAlphaType);
				canvas->writePixels(info, img.data, img.step, rectTarget.left, rectTarget.top);
			}

			// Draw Selection Rect
			if (m_mouse.bInSelectionMode or m_mouse.bRectSelected) {
				SkPaint paint;
				paint.setColor(SkColorSetARGB(255, 255, 127, 128));
				xRect2d rect;
				rect.pt0() = m_ctScreenFromImage(m_mouse.ptSel0);
				rect.pt1() = m_ctScreenFromImage(m_mouse.ptSel1);
				SkRect r = SkRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom);
				canvas->drawRect(r, paint);
			}

			if (m_option.bDrawPixelValue) {
				DrawPixelValue(*canvas, m_skTypeface, m_imgOriginal, roi, m_ctScreenFromImage);
			}

			m_skContext->flush();
		}
	} catch (...) {
		OutputDebugString(L"cv::.......\n");
	}
#endif

}

void xMatView::OnSize_View( wxSizeEvent& event ) {
	if (!m_view or !m_context)
		return;

	UpdateCT();
	UpdateScrollBars();
}

void xMatView::OnLeftDown_View( wxMouseEvent& event ) {
	event.Skip();
	if (m_option.bPanningLock and (m_eZoom == eZOOM::fit2window))
		return;
	if (GetCapture())
		return;
	m_view->CaptureMouse();
	m_mouse.ptAnchor = event.GetPosition();
	m_mouse.ptOffset0 = m_ctScreenFromImage.m_offset;
}

void xMatView::OnLeftUp_View( wxMouseEvent& event ) {
	event.Skip();
	if (!m_view->HasCapture())
		return;
	m_view->ReleaseMouse();
	m_mouse.ptAnchor.reset();
}

void xMatView::OnMiddleDown_View( wxMouseEvent& event ) {
}

void xMatView::OnMiddleUp_View( wxMouseEvent& event ) {
}

void xMatView::OnMotion_View( wxMouseEvent& event ) {
	event.Skip();
	xPoint2i pt;
	pt = event.GetPosition();
	if (m_mouse.ptAnchor) {
		if (!m_option.bPanningLock) {
			switch (m_eZoom) {
			case eZOOM::one2one: break;
			case eZOOM::mouse_wheel_locked: break;
			case eZOOM::free: break;
			default :
				m_eZoom = eZOOM::free;
				base_t::m_cmbZoomMode->SetSelection((int)m_eZoom);
				break;
			}
		}
		auto dMouseSpeed = m_mouse.bInSelectionMode ? 1.0 : m_option.dMouseSpeed;
		auto ptOffset = (pt - *m_mouse.ptAnchor) * dMouseSpeed;
		if (m_eZoom == eZOOM::fit2width)
			ptOffset.x = 0;
		if (m_eZoom == eZOOM::fit2height)
			ptOffset.y = 0;
		m_ctScreenFromImage.m_offset = m_mouse.ptOffset0 + ptOffset;
		UpdateCT();
		UpdateScrollBars();
		m_view->Refresh(false);
	}
	if (m_mouse.bInSelectionMode) {
		m_mouse.ptSel1 = m_ctScreenFromImage.TransI(event.GetPosition());
		m_view->Refresh(false);
		m_view->Update();
	}

	// Mouse Posittion
	{
		auto ptImage = m_ctScreenFromImage.TransI(pt);
		m_textStatus->SetValue(std::format(L"{},{}", ptImage.x, ptImage.y).c_str());
	}
}

void xMatView::OnMouseWheel_View( wxMouseEvent& event ) {
	if ((m_eZoom == eZOOM::mouse_wheel_locked) or (m_option.bZoomLock and m_eZoom != eZOOM::free)) {
		event.Skip();
		return;
	}
	ZoomInOut(event.GetWheelRotation(), xPoint2i(event.GetPosition()), false);
}

void xMatView::OnRightDown_View( wxMouseEvent& event ) {
	if (m_mouse.bInSelectionMode) {
		m_mouse.bInSelectionMode = false;
		m_mouse.bRectSelected = true;
		m_mouse.ptSel1 = m_ctScreenFromImage.TransI(event.GetPosition());
	} else {
		m_mouse.bRectSelected = false;
		m_mouse.bInSelectionMode = true;
		m_mouse.ptSel0 = m_mouse.ptSel1 = m_ctScreenFromImage.TransI(event.GetPosition());
	}
	m_view->Refresh(false);
	m_view->Update();
}

void xMatView::OnRightUp_View( wxMouseEvent& event ) {
}
