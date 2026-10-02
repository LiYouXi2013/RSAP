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

namespace PersonSettingsDialog
{
    struct Ctx {
        AppState &state;
        RandomSelector &sel;
        VI *viNoW = nullptr;
        VI *viWeight = nullptr;
        VI *viNoN = nullptr;
        Fl_Input *inpName = nullptr;
    };

    static void cbSearchWeight(Fl_Widget*, void* ud)
    {
        Ctx* ctx = reinterpret_cast<Ctx *>(ud);
        int no = static_cast<int>(ctx->viNoW->value());
        auto& pl = ctx->state.persons;
        if (no < 1 || no > (int)pl.size()) {
            ctx->viWeight->value(-1);
            return;
        }
        ctx->viWeight->value(pl[no - 1].weight);
    }

    static void cbApplyWeight(Fl_Widget*, void* ud)
    {
        Ctx* ctx = reinterpret_cast<Ctx *>(ud);
        int no = static_cast<int>(ctx->viNoW->value());
        auto& pl = ctx->state.persons;
        int w = static_cast<int>(ctx->viWeight->value());
        if (no < 1 || no > (int)pl.size() || w < 0) {
            fl_alert("Invalid input!\nYou can add a new person in \"Name Setting\".");
            return;
        }
        pl[no - 1].weight = w;
        ctx->sel.setPersonList(pl);
        fl_message("Weight of No.%d has been changed to %d.", no, w);
    }

    static void cbSearchName(Fl_Widget*, void* ud)
    {
        Ctx* ctx = reinterpret_cast<Ctx *>(ud);
        int no = static_cast<int>(ctx->viNoN->value());
        auto& pl = ctx->state.persons;
        if (no < 1 || no > (int)pl.size()) {
            ctx->inpName->value("无名氏");
            return;
        }
        ctx->inpName->value(pl[no - 1].name.c_str());
    }

    static void cbApplyName(Fl_Widget*, void* ud)
    {
        Ctx* ctx = reinterpret_cast<Ctx *>(ud);
        int no = static_cast<int>(ctx->viNoN->value());
        auto& pl = ctx->state.persons;
        const char *nm = ctx->inpName->value();
        if (no < 1) {
            fl_alert("Invalid input!");
            return;
        }
        if (no > (int)pl.size()) {
            no = static_cast<int>(pl.size() + 1);
            ctx->viNoN->value(no);
            pl.push_back({std::string(nm), 1});
        } else {
            pl[no - 1].name = std::string(nm);
        }
        ctx->sel.setPersonList(pl);
        fl_message("Name of No.%d has been changed to %s.", no, pl[no - 1].name.c_str());
    }

    static void cbOk(Fl_Widget*, void* ud)
    {
        Ctx* ctx = reinterpret_cast<Ctx *>(ud);
        ctx->state.saveToIni();
        ((Fl_Window*)ud)->hide();
    }
    static void cbCancel(Fl_Widget*, void* ud)
    {
        ((Fl_Window*)ud)->hide();
    }

    void showModal(Fl_Window* parent, AppState& state, RandomSelector& sel)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window wnd(px, py, 381, 185, "Settings");
        wnd.set_modal();
        Ctx ctx{state, sel};

        Fl_Group* grpW = new Fl_Group(0, 15, 190, 152, "Chance Adjuster");
        grpW->box(FL_SHADOW_FRAME);
        ctx.viNoW = new VI(90, 33, 64, 22, "No.:");
        ctx.viWeight = new VI(90, 63, 64, 22, "Weight:");
        Fl_Button* btnSearchW = new Fl_Button(90, 93, 64, 24, "Search");
        Fl_Button* btnApplyW = new Fl_Button(90, 125, 64, 24, "Apply");
        btnSearchW->callback(cbSearchWeight, &ctx);
        btnApplyW->callback(cbApplyWeight, &ctx);
        grpW->labelfont(1);
        grpW->end();

        Fl_Group* grpN = new Fl_Group(191, 15, 190, 152, "Name");
        grpN->box(FL_SHADOW_FRAME);
        ctx.viNoN = new VI(281, 33, 64, 22, "No.:");
        ctx.inpName = new Fl_Input(281, 63, 64, 22, "Name:");
        ctx.inpName->value("无名氏");
        Fl_Button* btnSearchN = new Fl_Button(281, 93, 64, 24, "Search");
        Fl_Button* btnApplyN = new Fl_Button(281, 125, 64, 24, "Apply");
        btnSearchN->callback(cbSearchName, &ctx);
        btnApplyN->callback(cbApplyName, &ctx);
        grpN->labelfont(1);
        grpN->end();

        Fl_Button* ok = new Fl_Button(270, 165, 64, 20, "OK");
        ok->callback(cbOk, &ctx);
        Fl_Button* cancel = new Fl_Button(42, 165, 64, 20, "Cancel");
        cancel->callback(cbCancel, &wnd);

        wnd.end();
        wnd.show();
        while (wnd.shown()) Fl::wait();
    }
}
