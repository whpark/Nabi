// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include <GL/glew.h>

#include "wx/wx.h"
#include "wx/dataview.h"
#include "wx/webview.h"
#include "wx/glcanvas.h"
#include "wx/stdpaths.h"
#include "wx/msw/registry.h"

#include "targetver.h"
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//// Windows Header Files
//#include <windows.h>
//// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>

#include "gtl/gtl.h"
#include "gtl/json_proxy.h"

#include "spdlog/spdlog.h"
#include "spdlog/stopwatch.h"

using namespace std::literals;
using namespace gtl::literals;
using namespace nlohmann::literals;
using json = nlohmann::ordered_json;

namespace stdfs = std::filesystem;
namespace stdc = std::chrono;
namespace stdr = std::ranges;
namespace stdv = std::views;
