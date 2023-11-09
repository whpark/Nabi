#pragma once

#define OPENCV_IO_MAX_IMAGE_PIXELS	0x1000000000ull

#include "gsl/gsl"

#include "gtl/gtl.h"
#include "gtl/qt/qt.h"
#include "gtl/qt/util.h"
#include "gtl/qt/MatView/MatView.h"
#include "gtl/qt/MatBitmapArchive.h"
#include "gtl/qt/ProgressDlg.h"

#include "opencv2/core/utility.hpp"

#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QTimer>

using namespace std::literals;
using namespace gtl::literals;

using gtl::qt::ToString;
using gtl::qt::ToWString;
using gtl::qt::ToQString;
