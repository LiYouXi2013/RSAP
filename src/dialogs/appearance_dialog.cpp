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

#include "dialogs/person_settings_dialog.h"
#include "os-themes.h"

#include "utils.h"

namespace AppearanceSettingsDialog
{
    void showModal(Fl_Window* parent, AppState& state, i18n& tr)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window wnd(px, py, 381, 185, "Settings");
        wnd.set_modal();

        wnd.end();
        wnd.show();
        while (wnd.shown()) Fl::wait();
    }
}
