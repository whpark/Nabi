#include "pch.h"
#include "App.h"
#include "BlendTestDlg.h"
#include "blend2d.h"
#include "palette.h"

void putTextOutline(cv::InputOutputArray img, std::string const& str, gtl::xPoint2i org,
					int fontFace, double fontScale, cv::Scalar color, cv::Scalar crBkgnd,
					int thickness, int lineType,
					bool bottomLeftOrigin) {
		  //gtl::xSize2i size{ cv::getTextSize(str, fontFace, fontScale, thickness, nullptr) };

	if (true) {

		//crBkgnd = cv::Scalar(255, 255, 255) - color;

		int iShift = std::max(1, thickness / 2);
		cv::putText(img, str, org + gtl::xPoint2i(0, iShift), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
		cv::putText(img, str, org + gtl::xPoint2i(iShift, 0), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
		cv::putText(img, str, org + gtl::xPoint2i(-iShift, 0), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
		cv::putText(img, str, org + gtl::xPoint2i(0, -iShift), fontFace, fontScale, crBkgnd, thickness, lineType, bottomLeftOrigin);
	}

	cv::putText(img, str, org, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
}

xBlendTestDlg::xBlendTestDlg(QWidget* parent)
	: QDialog(parent) {
	ui.setupUi(this);

	// view
	{
		auto option = ui.view->GetOption();
		option.crBackground = cv::Vec3b(20, 10, 60);
		ui.view->SetOption(option, false);
	}

	// parameters
	{
		std::string buffer = ToString(theApp->GetReg().value("TestImage/option", "").toString());
		if (auto option = glz::read_json<sOption>(buffer)) {
			UpdateData<false>(*option);
		}
		UpdateImageValue();
	}

	connect(ui.btnMakeImage, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnMakeImage);
	connect(ui.btnSaveImageAs, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnSaveImageAs);
	connect(ui.cmbBPP, &QComboBox::currentTextChanged, this, &xBlendTestDlg::UpdateImageValue);
	connect(ui.chkColorImage, &QCheckBox::clicked, this, [this](auto) { UpdateImageValue(); });
	connect(ui.spinPaletteIndex, &QSpinBox::valueChanged, this, [this](auto) { UpdateImageValue(); });

	connect(ui.btnTest1, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest1);
	connect(ui.btnTest2, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest2);
	connect(ui.btnTest3, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest3);
	connect(ui.btnTest4, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest4);
}

xBlendTestDlg::~xBlendTestDlg() {
}

void xBlendTestDlg::OnBtnMakeImage() {
	gtl::qt::xWaitCursor wait;

	// Save parameters
	auto const o = UpdateData<true>();

	int nBPP = sBitmapSaveOption::GetBPP(o.bpp);
	if (o.index >= (1 << nBPP)) {
		QMessageBox::critical(this, "Error", "Palette Index is out of range !");
		return;
	}
	auto palette = GetPalette(nBPP, o.bColorImage);
	if (palette.empty()) {
		QMessageBox::critical(this, "Error", "Select Palette !");
		return;
	}
	auto value = GetImageValue(o.index, nBPP, o.bColorImage);
	int const nPart = 7;

	cv::Mat img = cv::Mat::zeros(cv::Size(o.width, o.height), CV_8UC1);
	auto strPattern = ToWString(ui.cmbPattern->currentText());
	if (strPattern.starts_with(L"입력")) {
		if (!value) {
			QMessageBox::critical(this, "Error", "Select Palette Index !");
			return;
		}
		img = o.index;
	}
	else if (strPattern.starts_with(L"Black")) {
		auto iter = std::find_if(palette.begin(), palette.end(), [](auto const& cr) { return cr.cr == 0x0; });
		auto index = std::distance(palette.begin(), iter);
		img = index;
	}
	else if (strPattern.starts_with(L"White")) {
		auto iter = std::find_if(palette.begin(), palette.end(), [](auto const& cr) { return cr.cr == 0xFFFFFF; });
		auto index = std::distance(palette.begin(), iter);
		img = index;
	}
	else if (strPattern.starts_with(L"가로")) {
		for (int i{}; i < nPart; i++) {
			cv::Rect rc;
			rc.x = 0;
			rc.y = img.rows * i / nPart;
			rc.width = img.cols;
			rc.height = img.rows / nPart;
			cv::Mat imgPart = img(rc);
			imgPart = (i % 2) ? 0 : o.index;
		}
	}
	else if (strPattern.starts_with(L"세로")) {
		for (int i{}; i < nPart; i++) {
			cv::Rect rc;
			rc.x = img.cols * i / nPart;
			rc.y = 0;
			rc.width = img.cols / nPart;
			rc.height = img.rows;
			cv::Mat imgPart = img(rc);
			imgPart = (i % 2) ? 0 : o.index;
		}
	}

	if (ui.grpMarks->isChecked()) {
		std::string str = "TopLeft";
		auto eFontFace = cv::FONT_HERSHEY_COMPLEX;
		auto dFontScale = o.dFontScale;
		int iFontThickness = o.iFontThickness;

		auto size = cv::getTextSize("BottomRight", eFontFace, dFontScale, iFontThickness, nullptr);
		size.width += iFontThickness*2;
		size.height += iFontThickness*2;
		auto crPen = 0;
		auto crBkgnd = o.index;
		if (crPen == crBkgnd)
			crBkgnd = std::clamp((1 << nBPP) - crPen, 0, (1<<nBPP)-1);
		putTextOutline(img, "TopLeft", { 0, size.height }, eFontFace, dFontScale, crPen, crBkgnd, iFontThickness, cv::LineTypes::FILLED, false);
		putTextOutline(img, "TopRight", { img.cols-size.width, size.height }, eFontFace, dFontScale, crPen, crBkgnd, iFontThickness, cv::LineTypes::FILLED, false);
		putTextOutline(img, "BottomLeft", { 0, img.rows - iFontThickness*2 - int(dFontScale*8) }, eFontFace, dFontScale, crPen, crBkgnd, iFontThickness, cv::LineTypes::FILLED, false);
		putTextOutline(img, "BottomRight", { img.cols - size.width, img.rows - iFontThickness*2 - int(dFontScale*8) }, eFontFace, dFontScale, crPen, crBkgnd, iFontThickness, cv::LineTypes::FILLED, false);
	}

	m_img = img;

	//cv::Mat pal = cv::Mat::zeros(1, 256, CV_8UC3);
	//for (int i{}; i < palette.size(); i++) {
	//	auto cr = palette[i];
	//	pal.at<cv::Vec3b>(i) = cv::Vec3b(cr.b, cr.g, cr.r);
	//}
	//cv::LUT(img, pal, m_imgLast);
	if (ui.chkIgnorePalette->isChecked()) {
		m_imgLast = m_img;
	}
	else {
		m_imgLast = cv::Mat::zeros(img.size(), CV_8UC3);
	#pragma omp parallel for
		for (int y = 0; y < img.rows; y++) {
			auto* src = img.ptr(y);
			auto* dst = m_imgLast.ptr<cv::Vec3b>(y);
			for (int x = 0; x < img.cols; x++) {
				auto cr = palette[src[x]];
				dst[x] = cv::Vec3b(cr.r, cr.g, cr.b);
			}
		}
	}
	m_optionLast = o;
	std::string buffer = glz::write_json(m_optionLast);
	theApp->GetReg().setValue("TestImage/option", ToQString(buffer));
	ui.view->SetImage(m_imgLast);
}

void xBlendTestDlg::OnBtnSaveImageAs() {
	auto img = m_img;
	if (img.empty())
		return;
	auto sOption = m_optionLast;
	auto path = GetImagePath(sOption);
	if (path.empty())
		return;

	if (!SaveImage(img, path, sOption)) {
		QMessageBox::critical(this, "Error", "Failed to save image !");
	}
}

bool xBlendTestDlg::SaveImage(cv::Mat img, std::filesystem::path const& path, sOption const& o) {
	int nBPP = sBitmapSaveOption::GetBPP(o.bpp);
	auto palette = GetPalette(nBPP, o.bColorImage);
	gtl::xSize2i pelsPerMeter;
	return gtl::SaveBitmapMat(path, img, nBPP, pelsPerMeter, palette, true, false);
}

std::filesystem::path xBlendTestDlg::GetImagePath(sOption const& o) {
	auto t = gtl::ToLocalTime(std::chrono::system_clock::now());
	auto nBPP = sBitmapSaveOption::GetBPP(o.bpp);
	auto title = fmt::format("w{}_h{}_{}bpp{}_pattern{}_v{}{}_{:%Y-%m-%d_%H-%M-%S}.bmp",
							 o.width, o.height, nBPP, o.bColorImage ? "_color" : "_gray",
							 o.ePattern, o.index, o.bMark ? "_Mark" : "", t);

	std::filesystem::path folder = theApp->m_folderCurrent;
	QString strFilter = "Image Files (*.bmp *.jpg *.jpeg *.png *.tif *.tiff);;All Files(*.*)";
	QString strPath = QFileDialog::getSaveFileName(this, "Save Image", ToQString(folder / title), strFilter);
	if (strPath.isEmpty())
		return {};

	return strPath.toStdWString();
}

std::optional<gtl::color_bgra_t> xBlendTestDlg::GetImageValue(std::optional<int> _index, std::optional<int> _nBPP, std::optional<bool> _bColorImage) {
	auto palette = GetPalette(_nBPP, _bColorImage);
	int index = _index.value_or(ui.spinPaletteIndex->value());	// palette index
	if (palette.size() <= index)
		return {};

	return palette[index];
}

std::span<gtl::color_bgra_t const> xBlendTestDlg::GetPalette(std::optional<int> _nBPP, std::optional<bool> _bColorImage) {
	int nBPP = _nBPP.value_or(ui.cmbBPP->currentText().toInt());
	bool bColorImage = _bColorImage.value_or(ui.chkColorImage->isChecked());
	if (bColorImage) {
		switch (nBPP) {
		case 1: return s_palette_1bit_bw; break;
		case 4: return s_palette_4bit_color; break;
		case 8: return s_palette_8bit_color; break;
		}
	}
	else {
		switch (nBPP) {
		case 1: return s_palette_1bit_bw; break;
		case 4: return s_palette_4bit_grayscale; break;
		case 8: return s_palette_8bit_grayscale; break;
		}
	}
	return {};
}

void xBlendTestDlg::UpdateImageValue() {
	if (auto value = GetImageValue()) {
		auto str = fmt::format(L"{}, {}, {}", value->r, value->g, value->b);
		ui.txtImageValue->setText(ToQString(str));
	}
	else {
		ui.txtImageValue->setText("N/A");
	}
}

void xBlendTestDlg::OnBtnTest1() {
	// Use constructor or `create()` function to
	// allocate a new image data of the required
	// format.
	BLImage img(480, 480, BL_FORMAT_PRGB32);

	// Attach a rendering context into `img`.
	BLContext ctx(img);

	// Clearing the image would make it transparent.
	ctx.clearAll();

	// Create a path having cubic curves.
	BLPath path;
	path.moveTo(26, 31);
	path.cubicTo(642, 132, 587, -136, 25, 464);
	path.cubicTo(882, 404, 144, 267, 27, 31);

	// Fill a path with opaque white - 0xAARRGGBB
	ctx.setFillStyle(BLRgba32(0xFFFFFFFF));
	ctx.fillPath(path/*, BLRgba32(0xFFFFFFFF)*/);

	// Detach the rendering context from `img`.
	ctx.end();

	// Let's use some built-in codecs provided by Blend2D.
	img.writeToFile(gtl::WtoU8A(theApp->m_folderCurrent / "bl_sample_1.png").c_str());
}

void xBlendTestDlg::OnBtnTest2() {
	BLImage img(480, 480, BL_FORMAT_PRGB32);
	BLContext ctx(img);

	ctx.clearAll();

	// Coordinates can be specified now or changed
	// later via BLGradient accessors.
	BLGradient linear(
		BLLinearGradientValues(0, 0, 0, 480));

	// Color stops can be added in any order.
	linear.addStop(0.0, BLRgba32(0xFFFFFFFF));
	linear.addStop(0.5, BLRgba32(0xFF5FAFDF));
	linear.addStop(1.0, BLRgba32(0xFF2F5FDF));

	// `setFillStyle()` can be used for both colors
	// and styles. Alternatively, a color or style
	// can be passed explicitly to a render function.
	ctx.setFillStyle(linear);

	// Rounded rect will be filled with the linear
	// gradient.
	ctx.fillRoundRect(40.0, 40.0, 400.0, 400.0, 45.5);
	ctx.end();

	img.writeToFile(gtl::WtoU8A(theApp->m_folderCurrent / "bl_sample_2.png").c_str());
}

void xBlendTestDlg::OnBtnTest3() {
	BLImage img(1600, 800, BL_FORMAT_PRGB32);
	BLContext ctx(img);

	const char fontName[] = "C:\\Windows\\Fonts\\impact.ttf";//"ABeeZee-Regular.ttf";
	const char* str =
		//"Hello Blend2D!\n"
		//"I'm a simple multiline text example\n"
		//"that uses GlyphBuffer and GlyphRun!";
		"123\n456\n789\n";
		//"12";
	BLRgba32 color(0xFFFFFFFFu);

	BLFontFace face;
	BLResult result = face.createFromFile(fontName);
	if (result != BL_SUCCESS) {
		QMessageBox::critical(this, "Error", "Failed to load a face");
		return;
		//printf("Failed to load a face (err=%u)\n", result);
		//return 1;
	}

	BLFont font;
	font.createFromFace(face, 20.0f);

	BLGlyphBuffer gb;
	BLTextMetrics tm;
	BLFontMetrics fm = font.metrics();

	auto t0 = std::chrono::high_resolution_clock::now();
	auto str0 = str;
	for (int i = 0, n = 4000; i < n; i ++) {
		if (i == n-1)
			ctx.clearAll();
		str = str0;
		double y = fm.ascent;
		do {
			const char* nl = strchr(str, '\n');
			gb.setUtf8Text(str,
				nl ? (size_t)(nl - str) : SIZE_MAX);
			font.shape(gb);
			font.getTextMetrics(gb, tm);

			double x = (tm.boundingBox.x1 - tm.boundingBox.x0);
			ctx.setFillStyle(color);
			ctx.fillGlyphRun(BLPoint((1600. - x) / 2, y),
				font, gb.glyphRun()/*, color*/);

			y += fm.ascent + fm.descent + fm.lineGap;
			str = nl ? nl + 1 : nullptr;
		} while (str);
	}
	auto t1 = std::chrono::high_resolution_clock::now();
	auto d = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	OutputDebugStringA(fmt::format("fillGlyphRun: {}\n", d).c_str());
	ctx.end();
	BLArray<uint8_t> data;
	auto lst = BLImageCodec::builtInCodecs();
	//img.writeToData(data, BLImageCodec::)

	img.writeToFile(gtl::WtoU8A(theApp->m_folderCurrent / "bl_sample_8.png").c_str());
}

void xBlendTestDlg::OnBtnTest4() {
	{
		cv::Mat img(1037, 1037, CV_8UC1);
		auto* ptr = img.ptr<uchar>(0);
		for (int x{}; x < img.cols; x++)
			ptr[x] = x / 100;
		for (int y{ 1 }, c{ 1 }; y < img.cols; y++, c++) {
			img.row(y) = c;
			if (c >= 100)
				c = 0;
		}
		for (int y{}; y < img.rows - 1; y++)
			img.at<uchar>(y + 1, 0) = (y / 100);
		cv::imwrite("C:\\Work\\Temp\\split_1037_y.png", img);
		cv::imwrite("C:\\Work\\Temp\\split_1037_x.png", img.t());
	}
	{
		cv::Mat img(1080, 1080, CV_8UC1);
		auto* ptr = img.ptr<uchar>(0);
		for (int x{}; x < img.cols; x++)
			ptr[x] = x / 100;
		for (int y{ 1 }, c{ 1 }; y < img.cols; y++, c++) {
			img.row(y) = c;
			if (c >= 100)
				c = 0;
		}
		for (int y{}; y < img.rows - 1; y++)
			img.at<uchar>(y + 1, 0) = (y / 100);

		cv::imwrite(gtl::WtoU8A(theApp->m_folderCurrent / "split_1080_y.png"), img);
		cv::imwrite(gtl::WtoU8A(theApp->m_folderCurrent / "split_1080_x.png"), img.t());
	}
}
