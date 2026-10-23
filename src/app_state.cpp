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
#pragma once

#include "app_state.h"

#include <random>
#include <fstream>
#include <nlohmann/json.hpp>
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
using ConsoleHandle = HWND;

inline ConsoleHandle GetConsoleWindowHandle()
{
    return ::GetConsoleWindow();
}

// flag: 0=隐藏，1=显示
inline bool ShowHideConsole(ConsoleHandle hCon, int flag)
{
    if (!hCon) return false;
    if (flag == 0)
        return ::ShowWindow(hCon, SW_HIDE) != 0;
    else if (flag == 1)
        return ::ShowWindow(hCon, SW_SHOW) != 0;
    return false;
}

#else
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <sys/types.h>
using ConsoleHandle = Window;

static Window find_xwindow_by_pid(Display* dpy, pid_t target_pid)
{
    Window root, parent;
    Window* children = nullptr;
    unsigned int nchild = 0;
    if (!XQueryTree(dpy, DefaultRootWindow(dpy), &root, &parent, &children, &nchild))
        return 0;

    Window result = 0;
    Atom wm_pid_atom = XInternAtom(dpy, "_NET_WM_PID", False);

    for (unsigned int i = 0; i < nchild; ++i) {
        unsigned long *val = nullptr;
        Atom type;
        int fmt;
        unsigned long nItems, bytesAfter;

        if (XGetWindowProperty(dpy, children[i], wm_pid_atom, 0, 1, False, XA_CARDINAL,
                               &type, &fmt, &nItems, &bytesAfter,
                               reinterpret_cast<unsigned char * *>(&val)) == Success
                && type == XA_CARDINAL && nItems >= 1) {
            if (static_cast<pid_t>(*val) == target_pid) {
                result = children[i];
                XFree(val);
                break;
            }
            XFree(val);
        }
    }
    XFree(children);
    return result;
}

inline ConsoleHandle GetConsoleWindowHandle()
{
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return 0;

    pid_t ppid = getppid();
    Window xid = find_xwindow_by_pid(dpy, ppid);
    XCloseDisplay(dpy);
    return xid;
}

// flag: 0=隐藏，1=显示
inline bool ShowHideConsole(ConsoleHandle hCon, int flag)
{
    if (hCon == 0) return false;
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return false;

    bool ok = false;
    if (flag == 0) {
        XUnmapWindow(dpy, hCon);
        ok = true;
    } else if (flag == 1) {
        XMapWindow(dpy, hCon);
        ok = true;
    }
    XFlush(dpy);
    XCloseDisplay(dpy);
    return ok;
}
#endif

using json = nlohmann::json;

bool AppState::loadFromIni(const std::string& iniPathin)
{
    auto l = get_logger("worker.appstate");

    json j;
    {
        std::ifstream f(iniPathin);
        if (!f.is_open()) {
            l->warn("Config file not found: {}", iniPathin);
            freopen("settings.json", "w", stdout);
            std::cout<<'\n';
            fclose(stdout);
        } 

        try {
            f >> j;
        } catch (const std::exception& e) {
            l->error("Failed to parse config {}: {}", iniPathin, e.what());
            j = json::object();
        }
    }

    seed     = j.value("Seed", 0u);
    ast      = j.value("AST", 0);
    showCons = j.value("ShowCons", 0) != 0;

    persons.clear();
    if (j.is_object() && j.contains("People") && j["People"].is_array()) {
        const auto& pplArray = j["People"];
        persons.reserve(pplArray.size());

        for (const auto& ppl : pplArray) {
            if (ppl.is_object()) {
                std::string name = ppl.value("Name", "Unknown");
                int weight = ppl.value("Weight", 0);
                persons.push_back({name, weight});
            }
        }
    }

    for (const auto& p : persons) {
        l->info("Name: {} Weight: {}", p.name, p.weight);
    }

    if (seed == 0) {
        static std::random_device rd;
        seed = rd();
        isDefaultSeed = true;
        l->info("Used Def Seed");
    }
    l->info("Seed: {}", seed);

    auto hCon = GetConsoleWindowHandle();
    ShowHideConsole(hCon, showCons);

    return true;
}

bool AppState::saveToIni(const std::string& iniPathin)
{
    auto l = get_logger("worker.appstate");

    json j;
    j["Seed"]     = isDefaultSeed ? 0u : seed;
    j["AST"]      = static_cast<uint32_t>(ast);
    j["ShowCons"] = static_cast<int>(showCons);

    j["People"] = json::array();
    for (const auto& p : persons) {
        json pplObj;
        pplObj["Name"] = p.name;
        pplObj["Weight"] = p.weight;

        // 塞入数组
        j["People"].push_back(pplObj);
    }


    if (isDefaultSeed) {
        l->info("Used Def Seed");
    }

    if (seed == 0) {
        static std::random_device rd;
        seed = rd();
    }

    std::ofstream f(iniPathin);
    if (!f.is_open()) {
        l->error("Failed to open config for writing: {}", iniPathin);
        return false;
    }
    f << j.dump(4) << '\n';

    l->info("All settings saved");
    return true;
}
