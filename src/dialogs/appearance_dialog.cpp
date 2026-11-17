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
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_ask.H>
#include "os-themes.h"
#include <filesystem>

#include "utils.h"

namespace fs = std::filesystem;
std::string tmpLang;
extern bool cngTh;

namespace AppearanceSettingsDialog
{
    struct DialogCtx {
        AppState &realState;
        AppState state;
        i18n &tr;
        Fl_Window *wnd = nullptr;
    };
    

    void cbOk(Fl_Widget*, void* ud){
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        if (ctx->realState.lang != tmpLang){
            fl_alert(ctx->tr.gt(25).c_str());
        }

        if (ctx->state.theme != ctx->realState.theme){
            cngTh = true;
        }
        ctx->state.lang = tmpLang;
        ctx->realState = ctx->state;
        ctx->wnd->hide();
    }

    void cbCancel(Fl_Widget*, void* ud){
        DialogCtx* ctx = reinterpret_cast<DialogCtx *>(ud);
        ctx->wnd->hide();
    }

    void showModal(Fl_Window* parent, AppState& state, i18n& tr)
    {
        int px = parent ? parent->x() - 100 : 100;
        int py = parent ? parent->y() - 100 : 100;
        Fl_Window wnd(px, py, 240, 220, "Settings");
        wnd.set_modal();

        tmpLang = state.lang;

        AppState stateToBeChanged = state;
        DialogCtx ctx{state, stateToBeChanged, tr};
        ctx.wnd = &wnd;

        Fl_Choice *themeChooser = new Fl_Choice(100, 140, 120, 22, "");
        themeChooser->copy_label(tr.gt(30).c_str());
        themeChooser->add("Native");
        themeChooser->add("Plastic");
        themeChooser->add("GTK+");
        themeChooser->add("Gleam");
        themeChooser->add("Oxy");
        themeChooser->add("Classic");
        themeChooser->add("Aero");
        themeChooser->add("Metro");
        themeChooser->add("Aqua");
        themeChooser->add("GreyBird");
        themeChooser->add("Metal");
        themeChooser->add("Blue");
        themeChooser->add("Dark");

        themeChooser->value(stateToBeChanged.theme);

        themeChooser->callback([](Fl_Widget* a, void*ud){
            Fl_Choice *ll = (Fl_Choice*)a;
            DialogCtx *ctx = (DialogCtx*)ud;
            ctx->state.theme = ll->value();
            setTheme(ll->value());
            ctx->wnd->redraw();
        }, &ctx);
        

        std::vector<std::string> nameMap;

        Fl_Choice *langChooser = new Fl_Choice(100, 165, 120, 22, "");
        langChooser->copy_label(tr.gt(24).c_str());
        langChooser->callback([](Fl_Widget * a, void* ud) {
            Fl_Choice *ll = (Fl_Choice*)a;
            std::vector<std::string> *nameMap = (std::vector<std::string> *)ud;
            tmpLang = nameMap->at(ll->value());
        }, &nameMap);

        int i = 0;
        try {
            fs::directory_iterator("./lang/");
        } catch(...) {
            fs::create_directory("lang");
        }
        for (const auto &entry : fs::directory_iterator("./lang/")) {
            if (entry.is_regular_file()) {
                langChooser->add(getLangName(entry.path().string().c_str()).c_str());

                if (stateToBeChanged.lang == entry.path().stem().string()) {
                    langChooser->value(i);
                }

                nameMap.push_back(entry.path().stem().string());
            }

            i++;
        }

        Fl_Button* btnOk = new Fl_Button(127, 193, 64, 20, "");
        btnOk->callback(cbOk, &ctx);
        btnOk->copy_label(tr.gt(18).c_str());

        Fl_Button* btnCancel = new Fl_Button(42, 193, 64, 20, "");
        btnCancel->callback(cbCancel, &ctx);
        btnCancel->copy_label(tr.gt(19).c_str());

        wnd.end();
        wnd.show();
        while (wnd.shown()) Fl::wait();
    }
}
