#include "pch.h"
#include "BlendTestDlg.h"
#include "blend2d.h"

xBlendTestDlg::xBlendTestDlg(QWidget* parent)
	: QDialog(parent) {
	ui.setupUi(this);

	connect(ui.btnTest1, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest1);
	connect(ui.btnTest2, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest2);
	connect(ui.btnTest3, &QPushButton::clicked, this, &xBlendTestDlg::OnBtnTest3);
}

xBlendTestDlg::~xBlendTestDlg() {
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
	img.writeToFile("C:\\Work\\Temp\\bl_sample_1.png");
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

	img.writeToFile("C:\\Work\\Temp\\bl_sample_2.png");
}

void xBlendTestDlg::OnBtnTest3() {
	BLImage img(1600, 800, BL_FORMAT_PRGB32);
	BLContext ctx(img);

	const char fontName[] = "C:\\Windows\\Fonts\\impact.ttf";//"ABeeZee-Regular.ttf";
	const char* str =
		"Hello Blend2D!\n"
		"I'm a simple multiline text example\n"
		"that uses GlyphBuffer and GlyphRun!";
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
	font.createFromFace(face, 60.0f);

	BLGlyphBuffer gb;
	BLTextMetrics tm;
	BLFontMetrics fm = font.metrics();
	double y = 190 + fm.ascent;

	ctx.clearAll();
	auto t0 = std::chrono::high_resolution_clock::now();
	auto str0 = str;
	for (int i = 0; i < 10000; i ++) {
		str = str0;
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

	img.writeToFile("C:\\Work\\Temp\\bl_sample_8.png");
}
