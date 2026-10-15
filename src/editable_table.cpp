// editable_table.cpp
#include "editable_table.h"
#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <cstdio>

EditableTable::EditableTable(int X, int Y, int W, int H, int rowsum, int colsum, const char* L)
    : Fl_Table(X, Y, W, H, L), editor(nullptr), edit_row(-1), edit_col(-1)
{
    rows(rowsum);
    cols(colsum);
    row_header(1);
    col_header(1);
    row_height_all(25);
    col_width_all(100);
    end();

    editor = new Fl_Input(0, 0, 0, 0);
    editor->hide();
    editor->callback(editor_cb, this);
    editor->when(FL_WHEN_ENTER_KEY | FL_WHEN_RELEASE);
}

void EditableTable::set_data(const std::vector<std::vector<std::string>> &d)
{
    data = d;

    // 根据数据自动设置行列数
    rows(data.empty() ? 0 : (int)data.size());
    cols(data.empty() ? 0 : (int)data[0].size());

    // 如果正在编辑，取消
    if (editor->visible()) {
        editor->hide();
        edit_row = edit_col = -1;
    }

    redraw();  // 重绘
}

void EditableTable::set_col_headers(const std::vector<std::string> &headers)
{
    col_headers = headers;
    cols((int)headers.size());   // 列数跟着表头走
    redraw();
}

void EditableTable::set_row_headers(const std::vector<std::string> &headers)
{
    row_headers = headers;
    rows((int)headers.size());   // 行数跟着行头走
    redraw();
}

void EditableTable::editor_cb(Fl_Widget* w, void* v)
{
    EditableTable* t = static_cast<EditableTable *>(v);
    t->commit_edit();
}

void EditableTable::commit_edit()
{
    if (edit_row >= 0 && edit_col >= 0) {
        data[edit_row][edit_col] = editor->value();
        edit_row = edit_col = -1;
    }
    editor->hide();
    redraw();
}

void EditableTable::start_edit(int R, int C)
{
    if (edit_row >= 0) commit_edit();

    int X, Y, W, H;
    find_cell(CONTEXT_CELL, R, C, X, Y, W, H);
    editor->resize(X, Y, W, H);
    editor->value(data[R][C].c_str());
    editor->show();
    editor->take_focus();

    edit_row = R;
    edit_col = C;
    redraw();
}

int EditableTable::handle(int event)
{
    if (event == FL_PUSH) {
        int R, C;
        ResizeFlag rf;
        TableContext ctx = cursor2rowcol(R, C, rf);
        if (ctx == CONTEXT_ROW_HEADER) {
            commit_edit();
            selected_row = R;
        }else{
            selected_row=-1;
        }
        if (ctx == CONTEXT_CELL && R >= 0 && C >= 0) {
            start_edit(R, C);
            return 1;
        }
    }
    return Fl_Table::handle(event);
}

void EditableTable::draw_cell(TableContext context, int R, int C,
                              int X, int Y, int W, int H)
{
    switch (context) {
        case CONTEXT_STARTPAGE:
            fl_font(FL_HELVETICA, 12);
            return;

        case CONTEXT_COL_HEADER: {
            std::string text;
            if (C >= 0 && C < (int)col_headers.size())
                text = col_headers[C];
            else {
                char s[16]; sprintf(s, "%d", C + 1); text = s;
            }

            fl_push_clip(X, Y, W, H);
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND2_COLOR);
            fl_color(FL_BLACK);
            fl_draw(text.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            fl_pop_clip();
            return;
        }

        case CONTEXT_ROW_HEADER: {
            std::string text;
            if (R >= 0 && R < (int)row_headers.size())
                text = row_headers[R];
            else {
                char s[16]; sprintf(s, "%d", R + 1); text = s;
            }

            fl_push_clip(X, Y, W, H);
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND2_COLOR);
            fl_color(FL_BLACK);
            fl_draw(text.c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            fl_pop_clip();
            return;
        }

        case CONTEXT_CELL: {
            if (R == edit_row && C == edit_col) return; // 正在编辑，不画
            fl_push_clip(X, Y, W, H);
            if (R == selected_row){
                fl_color(fl_rgb_color(228, 224, 243));
            } else if (C % 2 == 1) {
                fl_color(fl_rgb_color(208, 230, 175));
            } else {
                fl_color(fl_rgb_color(188, 216, 230));
            }
            // fl_color(FL_WHITE);
            fl_rectf(X, Y, W, H);
            fl_color(FL_BLACK);
            fl_draw(data[R][C].c_str(), X, Y, W, H, FL_ALIGN_LEFT);
            fl_color(FL_GRAY);
            fl_rect(X, Y, W, H);
            fl_pop_clip();
            return;
        }

        default:
            return;
    }
}