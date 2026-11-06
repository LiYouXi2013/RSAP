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

#include "main_window.h"
#include "dialogs/about_dialog.h"
#include "dialogs/general_settings_dialog.h"
#include "dialogs/person_settings_dialog.h"
#include <FL/platform.H>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
using NativeHandle = HWND;

inline bool SetWindowTopmost(NativeHandle hWnd, int flag)
{
    if (!hWnd) return false;
    HWND insert = (flag == 1) ? HWND_TOPMOST : HWND_NOTOPMOST;
    return ::SetWindowPos(hWnd, insert, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) != 0;
}

#else
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cstring>
using NativeHandle = Window;

inline bool SetWindowTopmost(NativeHandle xid, int flag)
{
    if (xid == 0 || !fl_display) return false;

    Atom wm_state = XInternAtom(fl_display, "_NET_WM_STATE", False);
    Atom atom_above = XInternAtom(fl_display, "_NET_WM_STATE_ABOVE", False);

    XEvent ev{};
    memset(&ev, 0, sizeof ev);
    ev.xclient.type = ClientMessage;
    ev.xclient.window = xid;
    ev.xclient.message_type = wm_state;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = (flag == 1) ? 1 : 0;
    ev.xclient.data.l[1] = atom_above;
    ev.xclient.data.l[2] = 0;

    XSendEvent(fl_display, DefaultRootWindow(fl_display), True, SubstructureNotifyMask | SubstructureRedirectMask, &ev);
    XFlush(fl_display);
    return true;
}
#endif

#include "utils.h"

extern std::string settingsName;
std::string lastSN;

MainWindow::MainWindow(int X, int Y, int W, int H, const char* title,
                       AppState& state, RandomSelector& sel, UpdateChecker& uc, i18n& locallizer)
    : Fl_Double_Window(X, Y, W, H, title),
      m_appState(state),
      m_selector(sel),
      m_updateChecker(uc),
      m_localizer(locallizer)
{
    m_disp = new RFO(0, 0, 300, 188);
    m_disp->box(FL_PLASTIC_DOWN_BOX);
    m_disp->fontsize(200);
    m_disp->textcolor(static_cast<Fl_Color>(228));
    m_disp->value("??");
    add(m_disp);

    m_btnStart = new Fl_Button(190, 247, 86, 30, "");
    m_btnStart->callback(cbStart, this);
    m_btnStart->box(FL_PLASTIC_UP_BOX);
    m_btnStart->down_box(FL_PLASTIC_DOWN_BOX);
    m_btnStart->labelfont(1);
    m_btnStart->copy_label(locallizer.gt(0).c_str());

    m_cbAutoStop = new Fl_Check_Button(190, 218, 86, 28, "");
    m_cbAutoStop->down_box(FL_DOWN_BOX);
    m_cbAutoStop->labelfont(1);
    m_cbAutoStop->callback(cbAutoStopToggle, this);
    m_cbAutoStop->copy_label(locallizer.gt(1).c_str());

    m_outputName = new Fl_Output(0, 188, 180, 60);
    m_outputName->box(FL_THIN_DOWN_BOX);
    m_outputName->textfont(FL_HELVETICA_ITALIC);
    m_outputName->textsize(60);

    m_cbNoRepeat = new Fl_Check_Button(190, 197, 86, 28, "");
    m_cbNoRepeat->labelfont(1);
    m_cbNoRepeat->callback(cbNoRepeatToggle, this);
    m_cbNoRepeat->copy_label(locallizer.gt(2).c_str());

    m_btnHide = new Fl_Button(220, 290, 80, 30, "");
    m_btnHide->labelcolor(FL_BLUE);
    m_btnHide->labelsize(30);
    m_btnHide->box(FL_FLAT_BOX);
    m_btnHide->callback(cbHide, this);
    m_btnHide->copy_label(locallizer.gt(3).c_str());

    m_btnMore = new Fl_Button(50, 263, 86, 30, "");
    m_btnMore->box(FL_PLASTIC_UP_BOX);
    m_btnMore->labelfont(1);
    m_btnMore->callback(cbMore, this);
    m_btnMore->copy_label(locallizer.gt(4).c_str());

    end();

    // float tiny window
    m_floatWnd = new Fl_Double_Window(Fl::w(), Fl::h(), 36, 36, "");
    m_floatWnd->set_modal();
    m_floatWnd->border(0);
    Fl_Button* showBtn = new Fl_Button(0, 0, 36, 36, "");
    showBtn->box(FL_PLASTIC_UP_BOX);
    showBtn->callback(cbFloatShow, this);
    showBtn->copy_label(locallizer.gt(5).c_str());
    m_floatWnd->end();

    Fl::repeat_timeout(0.5, staticKeepOnTop, this);
}

void MainWindow::staticKeepOnTop(void* ud)
{
    auto* self = reinterpret_cast<MainWindow *>(ud);
    self->keepOnTop();
}

void MainWindow::keepOnTop()
{
    NativeHandle hwnd = fl_xid(this);
    if (hwnd) {
        SetWindowTopmost(hwnd, 1);
    }
    NativeHandle hwnd2 = fl_xid(m_floatWnd);
    if (hwnd2) {
        SetWindowTopmost(hwnd2, 1);
    }
    Fl::repeat_timeout(0.5, staticKeepOnTop, this);
}

void MainWindow::staticDoARoll(void* ud)
{
    auto* self = reinterpret_cast<MainWindow *>(ud);
    self->doARoll();
}

void MainWindow::doARoll()
{
    m_isRolling = true;
    int ii = m_selector.randomPick();
    m_disp->value(std::to_string(ii + 1).c_str());
    m_outputName->value(m_selector.getPersonList()[ii].name.c_str());
    m_outputName->redraw();
    m_cc--;
    if (m_cc > 0) {
        Fl::repeat_timeout(0.1, staticDoARoll, this);
        std::string lab = m_localizer.gt(5) + std::to_string(m_cc);
        m_btnStart->copy_label(lab.c_str());
    } else {
        m_isRolling = false;
        m_btnStart->copy_label(m_localizer.gt(0).c_str());
    }
}

void MainWindow::startRoll()
{
    if (m_isRolling) {
        m_isRolling = false;
        Fl::remove_timeout(staticDoARoll, this);
        m_btnStart->copy_label(m_localizer.gt(0).c_str());
        return;
    }
    if (!m_autoStop) {
        int ii = m_selector.randomPick();
        m_disp->value(std::to_string(ii + 1).c_str());
        try {
            m_outputName->value(m_selector.getPersonList().at(ii).name.c_str());
        } catch (...) {
            m_outputName->value(m_localizer.gt(6).c_str());
        }
        m_outputName->redraw();
    } else {
        m_cc = static_cast<int>(m_appState.ast / 100);
        Fl::add_timeout(0, staticDoARoll, this);
    }
}

void MainWindow::minimizeToFloat()
{
    get_logger("ui.mainWnd")->debug("Main->Float");
    m_winX = x();
    m_winY = y();
    hide();
    m_floatWnd->show();
}

void MainWindow::restoreFromFloat()
{
    get_logger("ui.mainWnd")->debug("Float->Main");
    position(m_winX, m_winY);
    m_floatWnd->hide();
    show();
}

void MainWindow::openMoreDialog()
{
    lastSN = settingsName;
    Fl::remove_timeout(staticKeepOnTop, this);
    Fl_Double_Window moreWnd(x() - 100, y() - 100, 220, 330);
    moreWnd.set_modal();

    Fl_Group* grpSet = new Fl_Group(0, 15, 220, 92, "");
    grpSet->box(FL_SHADOW_FRAME);
    grpSet->labelfont(1);
    grpSet->copy_label(m_localizer.gt(7).c_str());
    Fl_Button* btnGen = new Fl_Button(10, 25, 58, 28, "");
    btnGen->copy_label(m_localizer.gt(8).c_str());
    Fl_Button* btnPer = new Fl_Button(10, 69, 58, 28, "");
    btnPer->copy_label(m_localizer.gt(9).c_str());
    btnGen->box(FL_PLASTIC_UP_BOX);
    btnPer->box(FL_PLASTIC_UP_BOX);
    grpSet->end();

    Fl_Input *inputSettingsFileNameInput = new Fl_Input(100, 263, 120, 22, "");
    inputSettingsFileNameInput->value(settingsName.c_str());
    inputSettingsFileNameInput->copy_label(m_localizer.gt(10).c_str());

    Fl_Button* btnAbout = new Fl_Button(60, 293, 64, 22, "");
    btnAbout->box(FL_PLASTIC_UP_BOX);
    btnAbout->copy_label(m_localizer.gt(11).c_str());
    Fl_Button* btnOk = new Fl_Button(141, 293, 64, 22, "");
    btnOk->box(FL_PLASTIC_UP_BOX);
    btnOk->copy_label(m_localizer.gt(12).c_str());

    btnGen->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<MainWindow *>(ud);
        GeneralSettingsDialog::showModal(p, p->m_appState, p->m_selector, p->m_updateChecker);
    }, this);

    btnPer->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<MainWindow *>(ud);
        PersonSettingsDialog::showModal(p, p->m_appState, p->m_selector);
    }, this);

    inputSettingsFileNameInput->callback([](Fl_Widget * wg, void* ud) {
        MainWindow* p = reinterpret_cast<MainWindow *>(ud);
        Fl_Input* ipt = (Fl_Input*)wg;
        settingsName = ipt->value();
    }, this);

    btnAbout->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<MainWindow *>(ud);
        AboutDialog::showModal(p);
    }, this);

    struct uploadOK {
        MainWindow *mainwnd;
        Fl_Double_Window &more;
    };
    uploadOK upo{this, moreWnd};

    btnOk->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<uploadOK *>(ud);
        p->mainwnd->m_appState.saveToIni(lastSN);
        p->mainwnd->m_appState.loadFromIni(settingsName);

        std::ofstream createFile("snd");
        if (createFile.is_open()) {
            createFile << settingsName;
            createFile.close();
        }

        p->more.hide();
    }, &upo);

    moreWnd.end();
    moreWnd.show();
    while (moreWnd.shown()) Fl::wait();
    Fl::repeat_timeout(0.5, staticKeepOnTop, this);
}

void MainWindow::cbStart(Fl_Widget*, void* ud)
{
    reinterpret_cast<MainWindow *>(ud)->startRoll();
}
void MainWindow::cbHide(Fl_Widget*, void* ud)
{
    reinterpret_cast<MainWindow *>(ud)->minimizeToFloat();
}
void MainWindow::cbMore(Fl_Widget*, void* ud)
{
    reinterpret_cast<MainWindow *>(ud)->openMoreDialog();
}
void MainWindow::cbAutoStopToggle(Fl_Widget* w, void* ud)
{
    auto* self = reinterpret_cast<MainWindow *>(ud);
    Fl_Check_Button* cb = static_cast<Fl_Check_Button *>(w);
    self->m_autoStop = cb->value();
}
void MainWindow::cbNoRepeatToggle(Fl_Widget* w, void* ud)
{
    auto* self = reinterpret_cast<MainWindow *>(ud);
    Fl_Check_Button* cb = static_cast<Fl_Check_Button *>(w);
    self->m_selector.setNoRepeat(cb->value());
}
void MainWindow::cbFloatShow(Fl_Widget*, void* ud)
{
    reinterpret_cast<MainWindow *>(ud)->restoreFromFloat();
}
