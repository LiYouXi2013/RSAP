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

#include "randomPick.h"
#include "readInt.h"
#include "customWidgets.h"
#include <windows.h>
#include <iostream>
#include <string.h>
#include <sys/timeb.h>
// #include <sys/yjy>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Value_Input.H>
#include <Fl/fl_ask.H>
#include <FL/platform.H>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

void cb_min(Fl_Widget*, void* ud);
void cb_showit(Fl_Widget*, void* ud);
void cb_more(Fl_Widget*, void*);
void cb_about(Fl_Widget*, void*);
void cb_setting(Fl_Widget*, void*);
void cb_pplsettings(Fl_Widget*, void*);
void cb_setting_ok(Fl_Widget*, void* ud);
void cb_seed_changed(Fl_Widget* w, void*);
void cb_ast_changed(Fl_Widget* w, void*);
void cb_w_search(Fl_Widget*, void*);
void cb_w_apply(Fl_Widget*, void*);
void cb_n_search(Fl_Widget*, void*);
void cb_n_apply(Fl_Widget*, void*);

void startroll(Fl_Widget*, void* a);
void RSAPinit();
void RSAPsave();
void SpdLogInit();

using namespace std;

personVec class1;
poolVec pool;
uint32_t seed;
double ast;
bool isdef = false;
RFO *disp;
Fl_Button *start;
Fl_Output *no;
uint32_t ats;
bool as = false;
struct timeb tick;
int cc;
vector<int> weightdq;
bool shown = true;

Fl_Double_Window *window;
Fl_Double_Window *showit;

void keep_on_top(void*)
{
    HWND hwnd = fl_xid(window);
    if (hwnd) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }

    HWND hwnd2 = fl_xid(showit);
    if (hwnd2) {
        SetWindowPos(hwnd2, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    // 每 500ms 重新置顶一次
    Fl::repeat_timeout(0.5, keep_on_top);
}

int main(int argc, char **argv)
{
    SpdLogInit();
    spdlog::info("SpdLog Inited");
    RSAPinit();
    spdlog::info("Configurations read");

    Fl::set_font(FL_HELVETICA, "Consolas");
    Fl::set_font(FL_HELVETICA_BOLD, "BConsolas");
    Fl::set_font(FL_HELVETICA_ITALIC, "Huiwen-Fangsong");

    pool = buildPool(class1);

    window = new Fl_Double_Window(300, 320, "RSAP");

    {
        disp = new RFO(0, 0, 300, 188);
        disp->box(FL_PLASTIC_DOWN_BOX);
        disp->fontsize(200);
        disp->textcolor((Fl_Color)228);
        disp->value("??");
        window->add(disp);

        start = new Fl_Button(190, 247, 86, 30, "Start");
        start->callback(startroll, disp);
        start->box(FL_PLASTIC_UP_BOX);
        start->down_box(FL_PLASTIC_DOWN_BOX);
        start->labelfont(1);


        Fl_Check_Button* autostop = new Fl_Check_Button(190, 218, 86, 28, "Auto Stop");
        autostop->down_box(FL_DOWN_BOX);
        autostop->labelfont(1);
        autostop->callback([](Fl_Widget * w, void* ud) {
            Fl_Check_Button* cb = (Fl_Check_Button*)w;
            as = cb->value();
        }, nullptr);

        no = new Fl_Output(0, 188, 180, 60);
        no->box(FL_THIN_DOWN_BOX);
        no->textfont(FL_HELVETICA_ITALIC);
        no->textsize(60);

        Fl_Check_Button* norepeat = new Fl_Check_Button(190, 197, 86, 28, "No Repeat");
        norepeat->labelfont(1);

        Fl_Button* minimize = new Fl_Button(220, 290, 80, 30, "Hide");
        minimize->labelcolor(FL_BLUE);
        minimize->labelsize(30);
        minimize->box(FL_FLAT_BOX);
        minimize->callback(cb_min, nullptr);

        Fl_Button* os = new Fl_Button(50, 263, 86, 30, "More...");
        os->box(FL_PLASTIC_UP_BOX);
        os->labelfont(1);
        os->callback(cb_more);

    }
    window->end();
    window->show();
    Fl::get_system_colors();

    showit = new Fl_Double_Window(Fl::w(), Fl::h(), 36, 36, "");
    showit->set_modal();
    showit->border(0);
    Fl_Button* showitbtn = new Fl_Button(0, 0, 36, 36, "Show");
    showitbtn->box(FL_PLASTIC_UP_BOX);
    showitbtn->callback(cb_showit, nullptr);
    showit->end();
    // showit->show();

    Fl::repeat_timeout(0.5, keep_on_top);

    return Fl::run();
}

void RSAPinit()
{
    seed = GetPrivateProfileIntA("General", "Seed", 0, "./settings.ini");
    ast = GetPrivateProfileIntA("General", "AST", 0, "./settings.ini");

    char t1[65536];
    GetPrivateProfileStringA("General", "Weight", "TESTNAME-1", t1, sizeof(t1), "./settings.ini");
    string t_str = t1;
    class1 = readInt(t_str);

    for (Person i : class1) {
        spdlog::debug("Name: {} Weight: {}", i.name, i.weight);
    }

    if (seed == 0) {
        static std::random_device rd;
        seed = rd();
        isdef = true;
        spdlog::debug("Used Def Seed");
    }
    spdlog::info("Seed: {}", seed);
    gen.seed(seed);

    spdlog::info("Inited settings");
}

void RSAPsave()
{
    if (isdef) {
        WritePrivateProfileStringA("General", "Seed", to_string(0).c_str(), "./settings.ini");
        spdlog::debug("Used Def Seed");
    } else {
        WritePrivateProfileStringA("General", "Seed", to_string(seed).c_str(), "./settings.ini");
    }
    if (seed == 0) {
        static std::random_device rd;
        seed = rd();
    }
    spdlog::info("Seed: {}", seed);
    gen.seed(seed);
    spdlog::info("Seed writen into settings.ini");

    WritePrivateProfileStringA("General", "AST", to_string(ast).c_str(), "./settings.ini");
    WritePrivateProfileStringA("General", "Weight", join(class1).c_str(), "./settings.ini");
    spdlog::info("All settings saved");
}

void SpdLogInit()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("rsap.log", true);

    console_sink->set_level(spdlog::level::debug);
    file_sink->set_level(spdlog::level::info); // 文件只保存info及以上

    // 2. 合并sink，生成logger
    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("RSAP", sinks.begin(), sinks.end());
    logger->set_pattern("%Y-%m-%d %H:%M:%S [%n/%^%l%$] %v");
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
}

void cb_min(Fl_Widget*, void* ud)
{
    spdlog::debug("Main->Float");
    window->hide();
    showit->show();
}

void cb_showit(Fl_Widget*, void* ud)
{
    spdlog::debug("Float->Main");
    showit->hide();
    window->show();
}

void cb_more(Fl_Widget*, void*)
{
    Fl::remove_timeout(keep_on_top);
    Fl_Double_Window* morewnd = new Fl_Double_Window(220, 330);
    morewnd->set_modal();

    Fl_Group* settiings = new Fl_Group(0, 15, 220, 92, "Settings");
    settiings->box(FL_SHADOW_FRAME);
    settiings->labelfont(1);
    Fl_Button* gs = new Fl_Button(10, 25, 58, 28, "General");
    gs->box(FL_PLASTIC_UP_BOX);
    Fl_Button* ps = new Fl_Button(10, 69, 58, 28, "Person");
    ps->box(FL_PLASTIC_UP_BOX);
    settiings->end();
    Fl_Button* abt = new Fl_Button(60, 293, 64, 22, "About");
    abt->box(FL_PLASTIC_UP_BOX);
    Fl_Button* okbt = new Fl_Button(141, 293, 64, 22, "OK");
    okbt->box(FL_PLASTIC_UP_BOX);

    gs->callback(cb_setting);
    ps->callback(cb_pplsettings);
    abt->callback(cb_about);

    okbt->callback([](Fl_Widget*, void* ud) {
        Fl_Window* uud = (Fl_Window*)ud;
        uud->hide();
    }, morewnd);

    morewnd->end();
    morewnd->show();

    while (morewnd->shown())Fl::wait();
    Fl::repeat_timeout(0.5, keep_on_top);
}

void cb_about(Fl_Widget*, void*)
{
    Fl_Window dlg(360, 240, "About");
    dlg.set_modal(); // 模态，阻塞父窗口

    // 标题文字
    Fl_Box title(0, 20, 360, 40, "RSAP");
    title.box(FL_NO_BOX);
    title.labelsize(22);
    title.labelfont(FL_BOLD);
    title.align(FL_ALIGN_CENTER);

    // 版本信息
    Fl_Box info(0, 70, 360, 100,
                "Randomly Select A Person: v0.1indev\n"
                "Developed Using FLTK\n"
                "\n(c)2026 Candyman-RDFZ, LiYouXi2013 \nAll Rights Reserved.\n"
                "\n\nFonts: Consolas, BConsolas, 汇文仿宋"
               );
    info.box(FL_NO_BOX);
    info.labelsize(14);
    info.align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);

    // 确定按钮，关闭对话框
    Fl_Button ok_btn(130, 185, 100, 30, "OK");
    ok_btn.box(FL_PLASTIC_UP_BOX);
    ok_btn.down_box(FL_PLASTIC_DOWN_BOX);
    ok_btn.callback([](Fl_Widget * w, void* ud) {
        ((Fl_Window*)ud)->hide();
    }, &dlg);

    dlg.end();
    dlg.show();
    while (dlg.shown()) Fl::wait(); //模态等待窗口关闭

}

void cb_setting(Fl_Widget*, void*)
{
    Fl_Window* setting_wnd = new Fl_Window(235, 167, "Settings");
    setting_wnd->set_modal();
    VI* autostopt = new VI(151, 10, 64, 22, "Auto Stop Time:");
    autostopt->value(ast);
    Fl_Button* ok = new Fl_Button(127, 133, 64, 20, "OK");
    Fl_Button* cancel = new Fl_Button(42, 133, 64, 20, "Cancel");
    Fl_Button* CheckUPT = new Fl_Button(63, 95, 115, 20, "Check for Update");
    Fl_Box* rseedb = new Fl_Box(42, 45, 148, 20, "Random Seed");
    rseedb->labelfont(1);
    VI* rseed = new VI(42, 65, 148, 22, "");
    rseed->value(seed);

    rseed->callback(cb_seed_changed);
    autostopt->callback(cb_ast_changed);

    cancel->callback([](Fl_Widget * w, void* ud) {
        ((Fl_Window*)ud)->hide();
    }, setting_wnd);

    ok->callback(cb_setting_ok, setting_wnd);

    setting_wnd->end();

    setting_wnd->show();
    while (setting_wnd->shown()) Fl::wait();
}

VI *spinner;
VI *novi;
Fl_Input *spinner2;
VI *novi2;

void cb_pplsettings(Fl_Widget*, void*)
{
    Fl_Window* setting_wnd = new Fl_Window(381, 185, "Settings");
    setting_wnd->set_modal();
    Fl_Group* CAGrp = new Fl_Group(0, 15, 190, 152, "Chance Adjuster");
    CAGrp->box(FL_SHADOW_FRAME);
    spinner = new VI(90, 63, 64, 22, "Weight:");
    novi = new VI(90, 33, 64, 22, "No.:");
    Fl_Button* search = new Fl_Button(90, 93, 64, 24, "Search");
    Fl_Button* apply = new Fl_Button(90, 125, 64, 24, "Apply");
    CAGrp->labelfont(1);
    CAGrp->end();

    Fl_Group* NAGrp = new Fl_Group(191, 15, 190, 152, "Name");
    NAGrp->box(FL_SHADOW_FRAME);
    spinner2 = new Fl_Input(281, 63, 64, 22, "Name:");
    spinner2->value("无名氏");
    novi2 = new VI(281, 33, 64, 22, "No.:");
    Fl_Button* search2 = new Fl_Button(281, 93, 64, 24, "Search");
    Fl_Button* apply2 = new Fl_Button(281, 125, 64, 24, "Apply");
    NAGrp->labelfont(1);
    NAGrp->end();

    search->callback(cb_w_search);
    apply->callback(cb_w_apply);

    search2->callback(cb_n_search);
    apply2->callback(cb_n_apply);

    Fl_Button* ok = new Fl_Button(270, 165, 64, 20, "OK");
    Fl_Button* cancel = new Fl_Button(42, 165, 64, 20, "Cancel");

    ok->callback(cb_setting_ok, setting_wnd);
    cancel->callback([](Fl_Widget * w, void* ud) {
        ((Fl_Window*)ud)->hide();
    }, setting_wnd);


    setting_wnd->end();
    setting_wnd->show();
    while (setting_wnd->shown())Fl::wait();
}

void cb_setting_ok(Fl_Widget*, void* ud)
{
    RSAPsave();
    ((Fl_Window*)ud)->hide();
}

void cb_seed_changed(Fl_Widget* w, void*)
{
    VI* vi = (VI*)w;
    seed = (uint32_t)vi->value();
    isdef = false;
    if (seed == 0)isdef = true;
}

void cb_ast_changed(Fl_Widget* w, void*)
{
    VI* vi = (VI*)w;
    ast = (uint32_t)vi->value();
}

void cb_w_search(Fl_Widget*, void*)
{
    int no = (int)novi->value();
    if (no < 1 || no > class1.size()) {
        spinner->value(-1);
        return;
    }

    spinner->value(class1[no - 1].weight);
}

void cb_w_apply(Fl_Widget*, void*)
{
    int no = (int)novi->value();
    if (no < 1 || no > class1.size() || spinner->value() < 0) {
        fl_alert("Invalid input!\nYou can add a new person in \"Name Setting\".");
        return;
    }

    class1[no - 1].weight = (int)spinner->value();
    pool = buildPool(class1);
    cout << "Weight:" << spinner->value() << endl;
    fl_message("Weight of No.%d has been changed to %d.", no, class1[no - 1].weight);
}

void cb_n_search(Fl_Widget*, void*)
{
    int no = (int)novi2->value();
    if (no < 1 || no > class1.size()) {
        spinner2->value("无名氏");
        return;
    }

    spinner2->value(class1[no - 1].name.c_str());
}

void cb_n_apply(Fl_Widget*, void*)
{
    int no = (int)novi2->value();
    if (no < 1) {
        fl_alert("Invalid input!");
        return;
    }
    if (no > class1.size()) {
        no = class1.size() + 1;
        novi2->value(no);
        class1.push_back({spinner2->value(), 1});
    } else {
        class1[no - 1].name = (string)spinner2->value();
    }
    pool = buildPool(class1);
    fl_message("Name of No.%d has been changed to %s.", no, class1[no - 1].name.c_str());
}

bool is_rolling = false;

void do_a_roll(void* data)
{
    is_rolling = true;
    int ii = randomPick(pool);
    disp->value(to_string(ii + 1).c_str());
    no->value(class1[ii].name.c_str());
    no->redraw();
    cc--;
    if (cc > 0) {
        Fl::repeat_timeout(0.1, do_a_roll);
        start->copy_label(("Stop\nLeft: " + to_string(cc)).c_str());
    } else {
        is_rolling = false;
        start->copy_label("Start");
    }
}

void startroll(Fl_Widget*, void* a)
{
    if (is_rolling) {
        is_rolling = false;
        Fl::remove_timeout(do_a_roll);
        start->copy_label("Start");
        return;
    }
    if (!as) {
        int ii = randomPick(pool);
        disp->value(to_string(ii + 1).c_str());
        no->value(class1[ii].name.c_str());
        no->redraw();
    } else {
        ftime(&tick);
        cc = ast / 100;
        Fl::add_timeout(0, do_a_roll);
    }
}