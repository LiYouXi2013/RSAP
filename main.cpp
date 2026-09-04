#include "randomPick.h"
#include "readInt.h"
#include <windows.h>

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
#include "icon.h"

void cb_about(Fl_Widget*, void*);
void cb_setting(Fl_Widget*, void*);
void cb_setting_ok(Fl_Widget*, void* ud);
void RSAPinit();
void RSAPsave();

personVec class1;
poolVec pool;
int seed;
double ast;

int main(int argc, char **argv) {
	RSAPinit();
	for(int i=0;i<41;i++){
		class1.push_back((Person){
			"",1
		});
	}
	
	pool=buildPool(class1);
	
	Fl_Double_Window* window = new Fl_Double_Window(301, 320, "RSAP");

	{
		Fl_Menu_Bar* bar = new Fl_Menu_Bar(0, 0, 300, 22);
		bar->box(FL_PLASTIC_UP_BOX);
		bar->add("&File/Open Settings", 0, cb_setting);
		bar->add("&File/Exit", FL_CTRL + 'q', [](Fl_Widget * a, void* b) {
			Fl_Double_Window *c = (Fl_Double_Window*)b;
			c->hide();
		}, window);
		bar->add("&Help/About", FL_F + 1, cb_about);
		bar->menu_box(FL_PLASTIC_UP_FRAME);

		Fl_Output* disp = new Fl_Output(0, 22, 300, 198);
		disp->box(FL_PLASTIC_DOWN_BOX);
		disp->textfont(1);
		disp->textsize(200);
		disp->textcolor((Fl_Color)228);

		Fl_Button* start = new Fl_Button(190, 269, 86, 30, "Start");
		start->callback([](Fl_Widget*, void* a) {
			Fl_Output* disp=(Fl_Output*)a;
			int ii=randomPick(pool);
			disp->value(to_string(ii+1).c_str());
		},disp);
		start->box(FL_PLASTIC_UP_BOX);
		start->down_box(FL_PLASTIC_DOWN_BOX);
		start->labelfont(1);
		

		Fl_Check_Button* autostop = new Fl_Check_Button(190, 234, 86, 28, "Auto Stop");
		autostop->down_box(FL_DOWN_BOX);
		autostop->labelfont(1);

		Fl_Box* icon = new Fl_Box(0, 220, 180, 100);
		icon->image( image_icon() );
		icon->align(Fl_Align(512));
	}
	window->end();
	window->show(argc, argv);

	return Fl::run();
}

void cb_about(Fl_Widget*, void*) {
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
	            "Randomly Select A People: v0.0indev\n"
	            "Developed Using FLTK\n"
	            "\n(c)2026 Candyman-RDFZ, LiYouXi2013 \nAll Rights Reserved.\n");
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

void cb_setting(Fl_Widget*, void*) {
	Fl_Window setting_wnd(365, 167, "Settings");
	setting_wnd.set_modal();
	Fl_Group CAGrp(0, 15, 190, 152, "Chance Adjuster");
	CAGrp.box(FL_SHADOW_FRAME);
	Fl_Spinner spinner(90, 63, 64, 22, "Weight:");
	Fl_Value_Input vi(90, 33, 64, 22, "No.:");
	Fl_Button search(90, 93, 64, 24, "Search");
	Fl_Button apply(90, 125, 64, 24, "Apply");
	CAGrp.end();
	Fl_Value_Input autostopt(301, 10, 64, 22, "Auto Stop Time:");
	Fl_Button ok(287, 133, 64, 20, "OK");
	Fl_Button cancel(202, 133, 64, 20, "Cancel");
	Fl_Button CheckUPT(223, 95, 115, 20, "Check for Update");
	Fl_Value_Input rseed(301, 40, 64, 22, "Random Seed:");
	cancel.callback([](Fl_Widget * w, void* ud) {
		((Fl_Window*)ud)->hide();
	}, &setting_wnd);
	setting_wnd.end();

	setting_wnd.show();
	while (setting_wnd.shown()) Fl::wait();
}

void cb_setting_ok(Fl_Widget*, void* ud) {
	RSAPsave();

	((Fl_Window*)ud)->hide();
}

void RSAPinit() {
	seed = GetPrivateProfileIntA("RNG", "Seed", 0, "./settings.ini");
	if(seed=0){
		static std::random_device rd;
		gen.seed(rd());
	}else{
		gen.seed(seed);
	}
}

void RSAPsave(){
	WritePrivateProfileStringA("RNG", "Seed", to_string(seed).c_str(), "./settings.ini");
	if(seed=0){
		static std::random_device rd;
		gen.seed(rd());
	}else{
		gen.seed(seed);
	}
}
