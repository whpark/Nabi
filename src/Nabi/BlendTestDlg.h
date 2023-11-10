#pragma once

#include <QDialog>
#include "ui_BlendTestDlg.h"
#include "BitmapSaveOption.h"

class xBlendTestDlg : public QDialog {
	Q_OBJECT
public:

	struct sOption {
		using eBPP = sBitmapSaveOption::eBPP;
		int width{};
		int height{};
		eBPP bpp{eBPP::_1};
		bool bColorImage{false};	// word align
		int ePattern{};
		int index{};
		bool bMark{true};			// word align
		double dFontScale{1.0};
		int iFontThickness{1};

		int pelsPerMeterX{ gtl::Round(360 * 1'000 / 25.4) }, pelsPerMeterY{ gtl::Round(360 * 1'000 / 25.4) };

		GLZ_LOCAL_META(sOption, width, height, bpp, bColorImage, ePattern, index, bMark, dFontScale, iFontThickness, pelsPerMeterX, pelsPerMeterY);
	};

public:
	xBlendTestDlg(QWidget* parent = nullptr);
	~xBlendTestDlg();

	template < bool bSave >
	std::conditional_t<bSave, sOption, sOption&> UpdateData(std::conditional_t<bSave, sOption&&, sOption&> option = {}) {
		using gtl::qt::UpdateWidgetValue;

		UpdateWidgetValue(bSave, ui.spinImageWidth, option.width);
		UpdateWidgetValue(bSave, ui.spinImageHeight, option.height);
		UpdateWidgetValue(bSave, ui.cmbBPP, option.bpp);
		UpdateWidgetValue(bSave, ui.chkColorImage, option.bColorImage);
		UpdateWidgetValue(bSave, ui.cmbPattern, option.ePattern);
		UpdateWidgetValue(bSave, ui.spinPaletteIndex, option.index);
		UpdateWidgetValue(bSave, ui.grpMarks, option.bMark);
		UpdateWidgetValue(bSave, ui.spinFontScale, option.dFontScale);
		UpdateWidgetValue(bSave, ui.spinFontThickness, option.iFontThickness);
		UpdateWidgetValue(bSave, ui.spinPelsPerMeterX, option.pelsPerMeterX);
		UpdateWidgetValue(bSave, ui.spinPelsPerMeterY, option.pelsPerMeterY);

		if constexpr (bSave) {

		}
		else {
			UpdateImageValue();
			UpdateDPI();
		}

		return option;
	}

protected:
	cv::Mat m_img;
	mutable cv::Mat m_imgLast;
	mutable sOption m_optionLast;

	void OnBtnMakeImage();
	bool SaveImage(cv::Mat img, std::filesystem::path const& path, sOption const& option);
	std::filesystem::path GetImagePath(sOption const& option);
	void OnBtnSaveImageAs();
	std::optional<gtl::color_bgra_t> GetImageValue(std::optional<int> index = {}, std::optional<int> bpp ={}, std::optional<bool> bColorImage ={});
	std::span<gtl::color_bgra_t const> GetPalette(std::optional<int> bpp = {}, std::optional<bool> bGrayscale = {});
	void UpdateImageValue();
	void UpdateDPI();

	void OnBtnTest1();
	void OnBtnTest2();
	void OnBtnTest3();
	void OnBtnTest4();

private:
	Ui::BlendTestDlgClass ui;
};
