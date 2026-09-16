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
#include "icon.h"

void cb_about(Fl_Widget*, void*);
void cb_setting(Fl_Widget*, void*);
void cb_setting_ok(Fl_Widget*, void* ud);
void cb_seed_changed(Fl_Widget* w, void*);
void cb_ast_changed(Fl_Widget* w, void*);
void cb_search(Fl_Widget*, void*);
void cb_apply(Fl_Widget*, void*);

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
Fl_Button* start;
Fl_Output* no;
uint32_t ats;
bool as=false;
struct timeb tick;
int cc;
vector<int> weightdq;

int main(int argc, char **argv) {
	RSAPinit();
	putenv("FLTK_GDIPLUS=0");
	HWND hwnd=FindWindow(NULL,"RSAP");
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE);
	
	Fl::set_font(FL_HELVETICA, "Consolas");
	Fl::set_font(FL_HELVETICA_BOLD, "BConsolas");
	Fl::set_font(FL_HELVETICA_ITALIC, "BFZYTK");
	cout<<seed<<endl;
	
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

		start = new Fl_Button(190, 269, 86, 30, "Start");
		start->callback(startroll,disp);
		start->box(FL_PLASTIC_UP_BOX);
		start->down_box(FL_PLASTIC_DOWN_BOX);
		start->labelfont(1);
		

		Fl_Check_Button* autostop = new Fl_Check_Button(190, 241, 86, 28, "Auto Stop");
		autostop->down_box(FL_DOWN_BOX);
		autostop->labelfont(1);
		autostop->callback([](Fl_Widget* w, void* ud){
			Fl_Check_Button* cb = (Fl_Check_Button*)w;
			as = cb->value();
		}, nullptr);

		Fl_Box* icon = new Fl_Box(0, 220, 180, 100);
		icon->image( image_icon() );
		icon->align(Fl_Align(512));

		no = new Fl_Output(210, 220, 92, 20);
		no->box(FL_UP_BOX);
		no->textfont(FL_HELVETICA_ITALIC);
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
	            "Randomly Select A Person: v0.1indev\n"
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

Fl_Spinner* spinner;
VI* novi; 

void cb_setting(Fl_Widget*, void*) {
	Fl_Window* setting_wnd = new Fl_Window(365, 167, "Settings");
	setting_wnd->set_modal();
	Fl_Group* CAGrp = new Fl_Group(0, 15, 190, 152, "Chance Adjuster");
	CAGrp->box(FL_SHADOW_FRAME);
	spinner = new Fl_Spinner(90, 63, 64, 22, "Weight:");
	novi = new VI(90, 33, 64, 22, "No.:");
	Fl_Button* search = new Fl_Button(90, 93, 64, 24, "Search");
	Fl_Button* apply = new Fl_Button(90, 125, 64, 24, "Apply");
	CAGrp->labelfont(1);
	CAGrp->end();
	VI* autostopt = new VI(301, 10, 64, 22, "Auto Stop Time:");
	autostopt->value(ast);
	Fl_Button* ok = new Fl_Button(287, 133, 64, 20, "OK");
	Fl_Button* cancel = new Fl_Button(202, 133, 64, 20, "Cancel");
	Fl_Button* CheckUPT = new Fl_Button(223, 95, 115, 20, "Check for Update");
	Fl_Box* rseedb = new Fl_Box(202, 45, 148, 20, "Random Seed");
	rseedb->labelfont(1);
	VI* rseed = new VI(202, 65, 148, 22, "");
	rseed->value(seed);

	search->callback(cb_search);
	apply->callback(cb_apply);
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

void cb_setting_ok(Fl_Widget*, void* ud) {
	RSAPsave();
	cout<<"saved"<<endl;
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

void cb_ast_changed(Fl_Widget* w, void*) {
	cout<<"cb_ast_changed"<<endl;
	VI* vi = (VI*)w;
	ast = (uint32_t)vi->value();
}

void cb_search(Fl_Widget*, void*) {
	int no = (int)novi->value();
	if(no<1 || no>class1.size()){
		spinner->value(-1);
		return;
	}

	spinner->value(class1[no-1].weight);
}

void cb_apply(Fl_Widget*, void*) {
	int no = (int)novi->value();
	if(no<1 || no>class1.size() || spinner->value()<0){
		fl_alert("Invalid input!");
		return;
	}

	class1[no-1].weight = (int)spinner->value();
	pool = buildPool(class1);
	fl_message("Weight of No.%d has been changed to %d.", no, class1[no-1].weight);
}

bool di=false;

void do_a_roll(void* data) {
	di=true;
	int ii=randomPick(pool);
	disp->value(to_string(ii+1).c_str());
	no->value(class1[ii].name.c_str());
	cc--;
	if(cc>0){
    	Fl::repeat_timeout(0.1, do_a_roll);
		start->copy_label(("Stop\nLeft: "+to_string(cc)).c_str());
	}else{
		di=false;
		start->copy_label("Start");
	}
}

void startroll(Fl_Widget*, void* a) {
	if(di){
		di=false;
		Fl::remove_timeout(do_a_roll);
		start->copy_label("Start");
		return;
	}
	cout<<"AS:"<<as<<endl;
	if(!as){
		int ii=randomPick(pool);
		disp->value(to_string(ii+1).c_str());
		no->value(class1[ii].name.c_str());
	}else{
		ftime(&tick);
		cc=ast/100;
		Fl::add_timeout(0, do_a_roll);
	}
}

void RSAPinit() {
	seed = GetPrivateProfileIntA("General", "Seed", 0, "./settings.ini");
	ast = GetPrivateProfileIntA("General", "AST", 0, "./settings.ini");
	
	char t1[65536];
	GetPrivateProfileStringA("General", "Weight", "冯心仪-1,胡熙冉-1,李景仪-1,齐泽雨-1,田依暄-1,王锦璠-1,王逸舟-1,谢卓妍-1,邢与庭-1,鄢紫羲-1,袁家怡-1,战清欢-1,赵骐萱-1,周雨泉-1,邹佳晨-1,安昱-1,李尚-1,郭佳诺-1,韩津桐-1,胡煜橦-1,黄之尧-1,纪千豪-1,李宥羲-1,李宇新-1,李宗泽-1,马悠然-1,孙一硕-1,唐启轩-1,田雨瑞-1,胥林昊-1,张嘉宸-1,张嘉彧-1,赵康迪-1,赵天予-1,赵奕茗-1,赵逸和-1,郑博睿-1,郑峻宇-0,周泓予-1,周雨辰-1,孙奕萱-1", t1, sizeof(t1), "./settings.ini");
	string t_str = t1;
	class1 = readInt(t_str);

	for(int i : weightdq){
		class1.push_back((Person){
			"",i
		});
	}
	
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
		WritePrivateProfileStringA("General", "Seed", to_string(0).c_str(), "./settings.ini");
		cout<<"used def ";
	}else{
		WritePrivateProfileStringA("General", "Seed", to_string(seed).c_str(), "./settings.ini");
	}
	if(seed==0){
		static std::random_device rd;
		seed = rd();
		cout<<"rd seed:"<<seed<<endl;
	}
	gen.seed(seed);
	
	WritePrivateProfileStringA("General", "AST", to_string(ast).c_str(), "./settings.ini");
	WritePrivateProfileStringA("General", "Weight", join(class1).c_str(), "./settings.ini");
}

