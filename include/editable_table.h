// editable_table.h
#ifndef EDITABLE_TABLE_H
#define EDITABLE_TABLE_H

#include <FL/Fl_Table.H>
#include <FL/Fl_Input.H>
#include <vector>
#include <string>
#include <bits/std_function.h>

class EditInput : public Fl_Input
{
public:
    using Callback = std::function<void()>;
    EditInput(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L) {}

    void set_on_unfocus(Callback cb)
    {
        on_unfocus = std::move(cb);
    }
    void set_on_cancel(Callback cb)
    {
        on_cancel  = std::move(cb);
    }

    int handle(int event) override
    {
        switch (event) {
            case FL_UNFOCUS:
                if (on_unfocus) on_unfocus();
                break;

            case FL_KEYDOWN: {
                int key = Fl::event_key();
                if (key == FL_Escape) {
                    if (on_cancel) on_cancel();
                    return 1;
                }
                break;
            }
        }
        return Fl_Input::handle(event);
    }

private:
    Callback on_unfocus;
    Callback on_cancel;
};

class EditableTable : public Fl_Table
{
public:
    using CellCallback = std::function<void(int R, int C, TableContext ctx)>;

    EditableTable(int X, int Y, int W, int H, int rolsum, int colsum, const char* L = 0);
    std::vector<std::vector<std::string>> get_data()
    {
        return data;
    }
    int getSelRow() const
    {
        return selected_row;
    }

    void set_data(const std::vector<std::vector<std::string>> &d);
    void set_cb(CellCallback cb)
    {
        external_cb = cb;
    }

    void set_col_headers(const std::vector<std::string> &headers);
    void set_row_headers(const std::vector<std::string> &headers);
    void add_row(const std::vector<std::string> &row);
    void remove_row(int index);

    int handle(int event) override;

private:
    CellCallback external_cb;
    static void internal_cb(Fl_Widget* w, void* data);

    std::vector<std::string> col_headers;
    std::vector<std::string> row_headers;

    EditInput *editor;                              // 浮动输入框
    std::vector<std::vector<std::string>> data;    // 数据
    int edit_row;                                  // 正在编辑的行
    int edit_col;                                  // 正在编辑的列
    int selected_row;

    void start_edit(int R, int C);                 // 开始编辑
    void commit_edit();                            // 提交编辑
    bool is_editing() const
    {
        return edit_row >= 0 && edit_col >= 0;
    }
    static void editor_cb(Fl_Widget* w, void* v);  // 输入框回调

protected:
    // 重写绘制
    void draw_cell(TableContext context, int R, int C,
                   int X, int Y, int W, int H) override;
};

#endif // EDITABLE_TABLE_H