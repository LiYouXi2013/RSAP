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
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Output.H>
#include "custom_widgets.h"
#include "app_state.h"
#include "random_selector.h"
#include "update_checker.h"

class MainWindow : public Fl_Double_Window
{
public:
    MainWindow(int X, int Y, int W, int H, const char* title,
               AppState& state, RandomSelector& sel, UpdateChecker& uc);
    ~MainWindow() override = default;

    static void staticKeepOnTop(void* ud);
    static void staticDoARoll(void* ud);

    void keepOnTop();
    void doARoll();
    void startRoll();
    void minimizeToFloat();
    void restoreFromFloat();
    void openMoreDialog();

private:
    // widgets
    RFO *m_disp = nullptr;
    Fl_Button *m_btnStart = nullptr;
    Fl_Check_Button *m_cbAutoStop = nullptr;
    Fl_Output *m_outputName = nullptr;
    Fl_Check_Button *m_cbNoRepeat = nullptr;
    Fl_Button *m_btnHide = nullptr;
    Fl_Button *m_btnMore = nullptr;

    Fl_Double_Window *m_floatWnd = nullptr;
    int m_winX = 0;
    int m_winY = 0;

    AppState &m_appState;
    RandomSelector &m_selector;
    UpdateChecker &m_updateChecker;

    bool m_isRolling = false;
    bool m_autoStop = false;
    int m_cc = 0;

    // static callback thunks
    static void cbStart(Fl_Widget*, void* ud);
    static void cbHide(Fl_Widget*, void* ud);
    static void cbMore(Fl_Widget*, void* ud);
    static void cbAutoStopToggle(Fl_Widget*, void* ud);
    static void cbNoRepeatToggle(Fl_Widget*, void* ud);
    static void cbFloatShow(Fl_Widget*, void* ud);
};
