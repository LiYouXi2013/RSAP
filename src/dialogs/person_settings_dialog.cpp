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
#include "custom_widgets.h"
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/fl_ask.H>
#include <FL/fl_message.H>
#include "editable_table.h"

#include "utils.h"

namespace PersonSettingsDialog
{
    struct Ctx {
        AppState &realState;
        RandomSelector &sel;
        Fl_Window *wnd = nullptr;
        EditableTable *etable = nullptr;
    };

    static void cbOk(Fl_Widget*, void* ud)
    {
        Ctx* ctx = reinterpret_cast<Ctx *>(ud);
        std::vector<std::vector<std::string>> tabledat = ctx->etable->get_data();
        ctx->realState.persons.clear();
        for(const auto& i:tabledat){
            ctx->realState.persons.push_back({i[0],std::max(std::stoi(i[1]), 0)});
        }
        ctx->sel.setPersonList(ctx->realState.persons);
        ctx->wnd->hide();
    }
    static void cbCancel(Fl_Widget*, void* ud)
    {
        Fl_Window* wnd = reinterpret_cast<Fl_Window *>(ud);
        wnd->hide();
    }

    void showModal(Fl_Window* parent, AppState& state, RandomSelector& sel)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window wnd(px, py, 381, 185, "Settings");
        wnd.set_modal();

        Ctx ctx{state, sel};
        ctx.wnd = &wnd;

        std::vector<std::vector<std::string>> etVec;
        for (const Person& i : state.persons) {
            etVec.push_back({i.name, std::to_string(i.weight)});
        }

        EditableTable et = EditableTable(0, 0, 260, 185, 4, 2);
        et.set_data(etVec);

        et.set_col_headers({
            "Name", "Weight"
        });

        ctx.etable = &et;

        Fl_Button* ok = new Fl_Button(290, 100, 64, 20, "OK");
        ok->callback(cbOk, &ctx);
        Fl_Button* cancel = new Fl_Button(290, 150, 64, 20, "Cancel");
        cancel->callback(cbCancel, &wnd);

        wnd.end();
        wnd.show();
        while (wnd.shown()) Fl::wait();
    }
}
