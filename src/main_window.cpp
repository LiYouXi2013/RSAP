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
#include <windows.h>
#include <spdlog/spdlog.h>
#include <fl/platform.H>

MainWindow::MainWindow(int X, int Y, int W, int H, const char* title,
                       AppState& state, RandomSelector& sel, UpdateChecker& uc)
    : Fl_Double_Window(X, Y, W, H, title),
      m_appState(state),
      m_selector(sel),
      m_updateChecker(uc)
{
    m_disp = new RFO(0, 0, 300, 188);
    m_disp->box(FL_PLASTIC_DOWN_BOX);
    m_disp->fontsize(200);
    m_disp->textcolor(static_cast<Fl_Color>(228));
    m_disp->value("??");
    add(m_disp);

    m_btnStart = new Fl_Button(190, 247, 86, 30, "Start");
    m_btnStart->callback(cbStart, this);
    m_btnStart->box(FL_PLASTIC_UP_BOX);
    m_btnStart->down_box(FL_PLASTIC_DOWN_BOX);
    m_btnStart->labelfont(1);

    m_cbAutoStop = new Fl_Check_Button(190, 218, 86, 28, "Auto Stop");
    m_cbAutoStop->down_box(FL_DOWN_BOX);
    m_cbAutoStop->labelfont(1);
    m_cbAutoStop->callback(cbAutoStopToggle, this);

    m_outputName = new Fl_Output(0, 188, 180, 60);
    m_outputName->box(FL_THIN_DOWN_BOX);
    m_outputName->textfont(FL_HELVETICA_ITALIC);
    m_outputName->textsize(60);

    m_cbNoRepeat = new Fl_Check_Button(190, 197, 86, 28, "No Repeat");
    m_cbNoRepeat->labelfont(1);
    m_cbNoRepeat->callback(cbNoRepeatToggle, this);

    m_btnHide = new Fl_Button(220, 290, 80, 30, "Hide");
    m_btnHide->labelcolor(FL_BLUE);
    m_btnHide->labelsize(30);
    m_btnHide->box(FL_FLAT_BOX);
    m_btnHide->callback(cbHide, this);

    m_btnMore = new Fl_Button(50, 263, 86, 30, "More...");
    m_btnMore->box(FL_PLASTIC_UP_BOX);
    m_btnMore->labelfont(1);
    m_btnMore->callback(cbMore, this);

    end();

    // float tiny window
    m_floatWnd = new Fl_Double_Window(Fl::w(), Fl::h(), 36, 36, "");
    m_floatWnd->set_modal();
    m_floatWnd->border(0);
    Fl_Button* showBtn = new Fl_Button(0, 0, 36, 36, "Show");
    showBtn->box(FL_PLASTIC_UP_BOX);
    showBtn->callback(cbFloatShow, this);
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
    HWND hwnd = fl_xid(this);
    if (hwnd) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    HWND hwnd2 = fl_xid(m_floatWnd);
    if (hwnd2) {
        SetWindowPos(hwnd2, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
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
        std::string lab = "Stop\nLeft: " + std::to_string(m_cc);
        m_btnStart->copy_label(lab.c_str());
    } else {
        m_isRolling = false;
        m_btnStart->copy_label("Start");
    }
}

void MainWindow::startRoll()
{
    if (m_isRolling) {
        m_isRolling = false;
        Fl::remove_timeout(staticDoARoll, this);
        m_btnStart->copy_label("Start");
        return;
    }
    if (!m_autoStop) {
        int ii = m_selector.randomPick();
        m_disp->value(std::to_string(ii + 1).c_str());
        m_outputName->value(m_selector.getPersonList()[ii].name.c_str());
        m_outputName->redraw();
    } else {
        m_cc = static_cast<int>(m_appState.ast / 100);
        Fl::add_timeout(0, staticDoARoll, this);
    }
}

void MainWindow::minimizeToFloat()
{
    spdlog::debug("Main->Float");
    m_winX = x();
    m_winY = y();
    hide();
    m_floatWnd->show();
}

void MainWindow::restoreFromFloat()
{
    spdlog::debug("Float->Main");
    position(m_winX, m_winY);
    m_floatWnd->hide();
    show();
}

void MainWindow::openMoreDialog()
{
    Fl::remove_timeout(staticKeepOnTop, this);
    Fl_Double_Window moreWnd(x() - 100, y() - 100, 220, 330);
    moreWnd.set_modal();

    Fl_Group* grpSet = new Fl_Group(0, 15, 220, 92, "Settings");
    grpSet->box(FL_SHADOW_FRAME);
    grpSet->labelfont(1);
    Fl_Button* btnGen = new Fl_Button(10, 25, 58, 28, "General");
    Fl_Button* btnPer = new Fl_Button(10, 69, 58, 28, "Person");
    btnGen->box(FL_PLASTIC_UP_BOX);
    btnPer->box(FL_PLASTIC_UP_BOX);
    grpSet->end();

    Fl_Button* btnAbout = new Fl_Button(60, 293, 64, 22, "About");
    btnAbout->box(FL_PLASTIC_UP_BOX);
    Fl_Button* btnOk = new Fl_Button(141, 293, 64, 22, "OK");
    btnOk->box(FL_PLASTIC_UP_BOX);

    btnGen->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<MainWindow *>(ud);
        GeneralSettingsDialog::showModal(p, p->m_appState, p->m_selector, p->m_updateChecker);
    }, this);

    btnPer->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<MainWindow *>(ud);
        PersonSettingsDialog::showModal(p, p->m_appState, p->m_selector);
    }, this);

    btnAbout->callback([](Fl_Widget*, void* ud) {
        auto* p = reinterpret_cast<MainWindow *>(ud);
        AboutDialog::showModal(p);
    }, this);

    btnOk->callback([](Fl_Widget*, void* ud) {
        ((Fl_Window*)ud)->hide();
    }, &moreWnd);

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
