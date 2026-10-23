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

#include "dialogs/general_settings_dialog.h"
#include "custom_widgets.h"
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Check_Button.H>

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

namespace GeneralSettingsDialog
{
    struct DialogCtx {
        AppState &realState;
        AppState state;
        RandomSelector &sel;
        UpdateChecker &uc;
        VI *viSeed = nullptr;
        VI *viAst = nullptr;
        Fl_Window *wnd = nullptr;
        Fl_Check_Button *btnHideCons = nullptr;
    };

    static void cbCheckUpdate(Fl_Widget*, void* ud)
    {
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        ctx->uc.backgroundCheck();
    }

    static void cbOk(Fl_Widget*, void* ud)
    {
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        ctx->state.ast = ctx->viAst->value();
        ctx->state.showCons = ctx->btnHideCons->value();
        ctx->sel.setSeed(ctx->state.seed);
        ctx->realState = ctx->state;
        ctx->wnd->hide();
    }

    static void cbCancel(Fl_Widget*, void* ud)
    {
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        ctx->wnd->hide();
    }

    void showModal(Fl_Window* parent, AppState& state, RandomSelector& sel, UpdateChecker& uc)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window wnd(px, py, 235, 207, "Settings");
        wnd.set_modal();

        AppState stateToBeChanged = state;
        DialogCtx ctx{state, stateToBeChanged, sel, uc};
        ctx.wnd = &wnd;

        ctx.viAst = new VI(151, 10, 64, 22, "Auto Stop Time:");
        ctx.viAst->value(state.ast);

        new Fl_Box(42, 45, 148, 20, "Random Seed");
        ctx.viSeed = new VI(42, 65, 148, 22, "");
        ctx.viSeed->value(state.seed);
        ctx.viSeed->callback([](Fl_Widget * a, void* ud) {
            VI* vi = (VI*)a;
            DialogCtx* ctx = (DialogCtx*)ud;
            ctx->state.seed = (uint32_t)vi->value();
            ctx->state.isDefaultSeed = false;
            if (ctx->state.seed == 0) {
                ctx->state.isDefaultSeed = true;
            }
        }, &ctx);

        Fl_Button* btnCheck = new Fl_Button(43, 95, 155, 20, "Check for Update");
        btnCheck->callback(cbCheckUpdate, &ctx);

        Fl_Check_Button* btnHideConsole = new Fl_Check_Button(42, 125, 148, 22, "Show Console");
        btnHideConsole->value(state.showCons);
        btnHideConsole->callback([](Fl_Widget * a, void*) {
            ShowHideConsole(GetConsoleWindowHandle(), ((Fl_Check_Button*)a)->value());
        });

        ctx.btnHideCons = btnHideConsole;

        Fl_Button* btnOk = new Fl_Button(127, 153, 64, 20, "OK");
        btnOk->callback(cbOk, &ctx);

        Fl_Button* btnCancel = new Fl_Button(42, 153, 64, 20, "Cancel");
        btnCancel->callback(cbCancel, &wnd);

        wnd.end();
        wnd.show();
        while (wnd.shown()) Fl::wait();
    }
}
