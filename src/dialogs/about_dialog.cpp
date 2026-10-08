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

#include "dialogs/about_dialog.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

namespace AboutDialog
{
    void showModal(Fl_Window* parent)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window dlg(px, py, 360, 240, "About");
        dlg.set_modal();

        Fl_Box title(0, 20, 360, 40, "RSAP");
        title.box(FL_NO_BOX);
        title.labelsize(22);
        title.labelfont(FL_BOLD);
        title.align(FL_ALIGN_CENTER);

        Fl_Box info(0, 70, 360, 100,
                    "Randomly Select A Person: build 5\n"
                    "Developed Using FLTK\n\n"
                    "(c)2026 Candyman-RDFZ, LiYouXi2013 \nAll Rights Reserved.\n\n"
                    "Fonts: Consolas, BConsolas");
        info.box(FL_NO_BOX);
        info.labelsize(14);
        info.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

        Fl_Button ok_btn(130, 185, 100, 30, "OK");
        ok_btn.box(FL_PLASTIC_UP_BOX);
        ok_btn.down_box(FL_PLASTIC_DOWN_BOX);
        ok_btn.callback([](Fl_Widget*, void* ud) {
            ((Fl_Window*)ud)->hide();
        }, &dlg);

        dlg.end();
        dlg.show();
        while (dlg.shown()) Fl::wait();
    }
}
