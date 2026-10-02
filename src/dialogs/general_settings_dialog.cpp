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

namespace GeneralSettingsDialog
{
    struct DialogCtx {
        AppState &state;
        RandomSelector &sel;
        UpdateChecker &uc;
        VI *viSeed = nullptr;
        VI *viAst = nullptr;
    };

    static void cbCheckUpdate(Fl_Widget*, void* ud)
    {
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        ctx->uc.backgroundCheck();
    }

    static void cbOk(Fl_Widget*, void* ud)
    {
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        ctx->state.seed = static_cast<uint32_t>(ctx->viSeed->value());
        ctx->state.ast = ctx->viAst->value();
        ctx->state.isDefaultSeed = (ctx->state.seed == 0);
        ctx->sel.setSeed(ctx->state.seed);
        ctx->state.saveToIni();
        ((Fl_Window*)ud)->hide();
    }

    static void cbCancel(Fl_Widget*, void* ud)
    {
        ((Fl_Window*)ud)->hide();
    }

    void showModal(Fl_Window* parent, AppState& state, RandomSelector& sel, UpdateChecker& uc)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window wnd(px, py, 235, 167, "Settings");
        wnd.set_modal();

        DialogCtx ctx{state, sel, uc};

        ctx.viAst = new VI(151, 10, 64, 22, "Auto Stop Time:");
        ctx.viAst->value(state.ast);

        new Fl_Box(42, 45, 148, 20, "Random Seed");
        ctx.viSeed = new VI(42, 65, 148, 22, "");
        ctx.viSeed->value(state.seed);

        Fl_Button* btnCheck = new Fl_Button(43, 95, 155, 20, "Check for Update");
        btnCheck->callback(cbCheckUpdate, &ctx);

        Fl_Button* btnOk = new Fl_Button(127, 133, 64, 20, "OK");
        btnOk->callback(cbOk, &ctx);

        Fl_Button* btnCancel = new Fl_Button(42, 133, 64, 20, "Cancel");
        btnCancel->callback(cbCancel, &wnd);

        wnd.end();
        wnd.show();
        while (wnd.shown()) Fl::wait();
    }
}
