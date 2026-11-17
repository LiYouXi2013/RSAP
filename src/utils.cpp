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
* 
* FLTK--Built In
*  1Plastic 2GTK+ 3Gleam 4Oxy
* OS-themes lib
   0native 5classic 6aero 7metro 8aqua 9greybird 10metal 11blue 12dark
*/
void setTheme(int id){
    switch (id) {    
    case 1:
        Fl::scheme("plastic");
        break;

    case 2:
        Fl::scheme("gtk+");
        break;

    case 3:
        Fl::scheme("gleam");
        break;

    case 4:
        Fl::scheme("oxy");
        break;

    case 5:
        OS::use_classic_theme();
        break;

    case 6:
        OS::use_aero_theme();
        break;

    case 7:
        OS::use_metro_theme();
        break;

    case 8:
        OS::use_aqua_theme();
        break;

    case 9:
        OS::use_greybird_theme();
        break;

    case 10:
        OS::use_metal_theme();
        break;

    case 11:
        OS::use_blue_theme();
        break;

    case 12:
        OS::use_dark_theme();
        break;

    default:
        OS::use_native_theme();
        break;
    }
}