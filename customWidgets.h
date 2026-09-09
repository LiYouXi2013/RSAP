#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <Fl/Fl_Value_Input.H>
#include <FL/Fl_Output.H>
#include <string>

class VI : public Fl_Value_Input {
public:
    VI(int x, int y, int w, int h, const char *l = 0)
        : Fl_Value_Input(x, y, w, h, l) {}

    // 2. 重写 format 方法
    int format(char *buffer) override {
        // 使用 snprintf 以 %lld 格式输出 long long
        // 这样即使数值很大也能完整显示
        long long val = (long long)this->value();
        return snprintf(buffer, 128, "%lld", val);
    }
};

class RFO : public Fl_Output
{
public:
    RFO(int X, int Y, int W, int H, const char* L=nullptr) : Fl_Output(X,Y,W,H,L)
    {}

    void fontsize(int s) {
        size = s;
    }

    void draw() override
    {
        draw_box(); // 只绘制底板边框，不绘制原生左对齐文字

        const char* txt = value();
        if(!txt || !*txt) return;

        fl_font(FL_HELVETICA_BOLD, size);
        int textW, textH;
        fl_measure(txt,textW,textH,0);


        int innerW = w() - 8;
        int drawX = x() + 4 + (innerW - textW)/2;
        int drawY = y() + h()/4 + textH/2;

        fl_color(textcolor());

        fl_draw(txt, drawX, drawY);
        puts(txt);
        puts("OK");
    }

    protected:
        int size=1;
};