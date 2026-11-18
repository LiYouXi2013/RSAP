/*
RSAP
Copyright (C) 2026 LiYouXi2013, Candyman_RDFZ

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "utils.h"
#include "os-themes.h"
#include <FL/Fl.H>
#include <FL/platform.H>
#include <cctype>
#include "spdlog_helper.h"

extern std::vector<spdlog::sink_ptr> sinks;

personVec readInt(const std::string& t)
{
    auto l = t.begin();
    while (l != t.end() && std::isspace(static_cast<unsigned char>(*l))) ++l;
    auto r = t.end();
    do {
        --r;
    } while (std::distance(l, r) > 0 && std::isspace(static_cast<unsigned char>(*r)));

    std::string s(l, r + 1);
    personVec res;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, ',')) {
        std::stringstream t3(item);
        std::string t1, t2;
        getline(t3, t1, '-');
        getline(t3, t2, '-');
        int val;
        try {
            val = std::stoi(t2);
        } catch (...) {
            continue;
        }
        res.push_back({t1, val});
    }
    return res;
}

std::string join(const personVec &v)
{
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) s += ',';
        s += v[i].name;
        s += '-';
        s += std::to_string(v[i].weight);
    }
    return s;
}

std::shared_ptr<spdlog::logger> get_logger(const std::string& name)
{
    auto l = spdlog::get(name);
    if (!l) {
        l = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
        l->set_pattern("%Y-%m-%d %H:%M:%S [%n/%^%l%$] %v");
        spdlog::register_logger(l);
    }
    return l;
}

/* 
* Themes
* OS-themes lib
   0native 1classic 2aero 3metro 4aqua 5greybird 6metal 7blue 8dark
*/
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <dwmapi.h>

void set_fltk_dark_title(Fl_Window* win, bool dark = true) {
	if (!win || !win->shown()) return;
	
	HWND hwnd = fl_xid(win);
	if (!hwnd) return;
	
	BOOL bDark = dark ? TRUE : FALSE;
	
	HRESULT hr = DwmSetWindowAttribute(hwnd, 20, &bDark, sizeof(bDark));
	
	if (FAILED(hr)) {
		DwmSetWindowAttribute(hwnd, 19, &bDark, sizeof(bDark));
	}
	SendMessage(hwnd, WM_NCACTIVATE, FALSE, 0);
	SendMessage(hwnd, WM_NCACTIVATE, TRUE, 0);
}
#endif

void setTheme(int id, Fl_Window *a){
#ifdef _WIN32
    set_fltk_dark_title(a, id == 8);
#endif

    switch (id) {
    case 1:
        OS::use_classic_theme();
        break;

    case 2:
        OS::use_aero_theme();
        break;

    case 3:
        OS::use_metro_theme();
        break;

    case 4:
        OS::use_aqua_theme();
        break;

    case 5:
        OS::use_greybird_theme();
        break;

    case 6:
        OS::use_metal_theme();
        break;

    case 7:
        OS::use_blue_theme();
        break;

    case 8:
        OS::use_dark_theme();
        break;

    default:
        OS::use_native_theme();
        break;
    }
}