// editable_table.h
#ifndef EDITABLE_TABLE_H
#define EDITABLE_TABLE_H

#include <FL/Fl_Table.H>
#include <FL/Fl_Input.H>
#include <vector>
#include <string>

class EditableTable : public Fl_Table
{
public:
    EditableTable(int X, int Y, int W, int H, int rolsum, int colsum, const char* L = 0);
    void set_data(const std::vector<std::vector<std::string>> &d);

    void set_col_headers(const std::vector<std::string> &headers);
    void set_row_headers(const std::vector<std::string> &headers);

    int handle(int event) override;

private:
    std::vector<std::string> col_headers;
    std::vector<std::string> row_headers;

    Fl_Input *editor;                              // 浮动输入框
    std::vector<std::vector<std::string>> data;    // 数据
    int edit_row;                                  // 正在编辑的行
    int edit_col;                                  // 正在编辑的列

    void start_edit(int R, int C);                 // 开始编辑
    void commit_edit();                            // 提交编辑
    static void editor_cb(Fl_Widget* w, void* v);  // 输入框回调

protected:
    // 重写绘制
    void draw_cell(TableContext context, int R, int C,
                   int X, int Y, int W, int H) override;
};

#endif // EDITABLE_TABLE_H