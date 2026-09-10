#include "randomPick.h"
#include "readInt.h"
#include "customWidgets.h"
#include <windows.h>
#include <iostream>

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
void cb_seed_changed(Fl_Widget* w, void*);

void startroll(Fl_Widget*, void* a);
void RSAPinit();
void RSAPsave();

using namespace std;

personVec class1;
poolVec pool;
uint32_t seed;
double ast;
bool isdef=false;
RFO* disp;

int main(int argc, char **argv) {
	RSAPinit();
	putenv("FLTK_GDIPLUS=0");
	
	Fl::set_font(FL_HELVETICA, "Consolas");
	Fl::set_font(FL_HELVETICA_BOLD, "BConsolas");
	cout<<seed<<endl;

	for(int i=0;i<41;i++){
		class1.push_back((Person){
			"",1
		});
	}
	
	pool=buildPool(class1);
	
	Fl_Double_Window* window = new Fl_Double_Window(300, 320, "RSAP");

	{
		Fl_Menu_Bar* bar = new Fl_Menu_Bar(0, 0, 300, 22);
		bar->box(FL_THIN_UP_BOX);
		bar->add("&File/Open Settings", 0, cb_setting);
		bar->add("&File/Exit", FL_CTRL + 'q', [](Fl_Widget * a, void* b) {
			Fl_Double_Window *c = (Fl_Double_Window*)b;
			c->hide();
		}, window);
		bar->add("&Help/About", FL_F + 1, cb_about);
		bar->color(FL_WHITE);
		bar->selection_color(FL_BLUE);
		
		bar->callback([](Fl_Widget* w, void* ud){
			Fl_Window* win= (Fl_Window*)ud;
			win->redraw();
		}, window);

		disp = new RFO(0, 22, 300, 198);
		disp->box(FL_PLASTIC_DOWN_BOX);
		disp->fontsize(200);
		disp->textcolor((Fl_Color)228);
		disp->value("??");
		window->add(disp);

		Fl_Button* start = new Fl_Button(190, 269, 86, 30, "Start");
		start->callback(startroll,disp);
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
	            "Randomly Select A Person: v0.0indev\n"
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
	VI vi(90, 33, 64, 22, "No.:");
	Fl_Button search(90, 93, 64, 24, "Search");
	Fl_Button apply(90, 125, 64, 24, "Apply");
	CAGrp.labelfont(1);
	CAGrp.end();
	VI autostopt(301, 10, 64, 22, "Auto Stop Time:");
	Fl_Button ok(287, 133, 64, 20, "OK");
	Fl_Button cancel(202, 133, 64, 20, "Cancel");
	Fl_Button CheckUPT(223, 95, 115, 20, "Check for Update");
	Fl_Box rseedb(202, 45, 148, 20, "Random Seed");
	rseedb.labelfont(1);
	VI rseed(202, 65, 148, 22, "");
	rseed.value(seed);
	rseed.step();

	rseed.callback(cb_seed_changed);

	cancel.callback([](Fl_Widget * w, void* ud) {
		((Fl_Window*)ud)->hide();
	}, &setting_wnd);

	ok.callback(cb_setting_ok, &setting_wnd);

	setting_wnd.end();

	setting_wnd.show();
	while (setting_wnd.shown()) Fl::wait();
}

void cb_setting_ok(Fl_Widget*, void* ud) {
	RSAPsave();

	((Fl_Window*)ud)->hide();
}

void cb_seed_changed(Fl_Widget* w, void*) {
	cout<<"cb_seed_changed"<<endl;
	VI* vi = (VI*)w;
	seed = (uint32_t)vi->value();
	isdef=false;
	if(seed==0)isdef=true;
	cout<<"Seed changed to "<<seed<<endl;
}



void startroll(Fl_Widget*, void* a) {
	int ii=randomPick(pool);
	disp->value(to_string(ii+1).c_str());
}

void RSAPinit() {
	seed = GetPrivateProfileIntA("RNG", "Seed", 0, "./settings.ini");
	if(seed==0){
		static std::random_device rd;
		seed = rd();
		isdef = true;
	}
	gen.seed(seed);
}

void RSAPsave(){
	cout<<seed<<endl;
	if(isdef){
		WritePrivateProfileStringA("RNG", "Seed", to_string(0).c_str(), "./settings.ini");
		cout<<"used def ";
	}else{
		WritePrivateProfileStringA("RNG", "Seed", to_string(seed).c_str(), "./settings.ini");
	}
	if(seed==0){
		static std::random_device rd;
		seed = rd();
		cout<<"rd seed:"<<seed<<endl;
	}
	gen.seed(seed);
}

