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

#include "spdlog_helper.h"
#include "app_state.h"
#include "random_selector.h"
#include "main_window.h"
#include "update_checker.h"
#include <FL/Fl.H>

int main(int argc, char **argv)
{
    spdlogInit();
    auto l = get_logger("rsap.main");
    l->info("SpdLog Inited");

    AppState appState;
    appState.loadFromIni();
    l->info("Configurations read");

    RandomSelector selector;
    selector.setSeed(appState.seed);
    selector.setPersonList(appState.persons);

    UpdateChecker updateChecker;

    Fl::set_font(FL_HELVETICA, "Consolas");
    Fl::set_font(FL_HELVETICA_BOLD, "BConsolas");
    Fl::set_font(FL_HELVETICA_ITALIC, "Huiwen‑Fangsong");

    MainWindow wnd(Fl::w() - 400, Fl::h() - 420, 300, 320, "RSAP", appState, selector, updateChecker);
    wnd.show();
    Fl::get_system_colors();
    Fl::lock();

    return Fl::run();
}
