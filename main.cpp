#include <ncurses.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <stack>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

const int SIDEBAR_WIDTH = 25;

stack<vector<string>> undoStack;
stack<vector<string>> redoStack;
// BALQIS//
string bold(const string& text) {
    return "**" + text + "**";
}

string italic(const string& text) {
    return "*" + text + "*";
}

string underline(const string& text) {
    return "_" + text + "_";
}

string formatText(const string& text, const string& formatType) {
    if (formatType == "bold") {
        return bold(text);
    } else if (formatType == "italic") {
        return italic(text);
    } else if (formatType == "underline") {
        return underline(text);
    } else {
        return text;
    }
}

void list_directory_tree(const fs::path& path, vector<string>& files, int depth = 0) {
    if (!fs::exists(path) || !fs::is_directory(path)) return;

    for (const auto& entry : fs::directory_iterator(path)) {
        string indent(depth * 2, ' ');
        string name = entry.path().filename().string();

        if (entry.is_directory()) {
            files.push_back(indent + "(D) " + name);
            list_directory_tree(entry.path(), files, depth + 1);
        } else {
            files.push_back(indent + "(F) " + name);
        }
    }
}

void draw_separator() {
    for (int i = 1; i < LINES - 1; i++) {
        mvaddch(i, SIDEBAR_WIDTH - 3, ACS_VLINE);
    }
}

void draw_header() {
    attron(A_REVERSE);
    mvhline(0, 0, ' ', COLS + 3);
    mvprintw(0, (COLS - 25) / 2, " SimpleNano - Terminal Editor ");
    attroff(A_REVERSE);
}

void draw_footer() {
    int y = LINES - 1;
    attron(A_REVERSE);
    mvhline(y, 0, ' ', COLS + 3);
    mvprintw(y, 1, " CTRL+X: Exit  CTRL+Z: Undo  CTRL+Y: Redo  CTRL+B: Bold  CTRL+I: Italic  CTRL+U: Underline ");
    attroff(A_REVERSE);
}

void draw_sidebar(const vector<string>& files) {
    for (int i = 0; i < files.size() && i < LINES - 2; ++i) {
        mvprintw(i + 2, 1, "%-*s", SIDEBAR_WIDTH - 1, files[i].c_str());
    }
}

void draw_editor(const vector<string>& lines) {
    for (int i = 0; i < lines.size() && i < LINES - 2; ++i) {
        mvprintw(i + 2, SIDEBAR_WIDTH, "%s", lines[i].c_str());
    }
}

void save_undo_state(const vector<string>& lines) {
    undoStack.push(lines);
    while (!redoStack.empty()) redoStack.pop();
}

int main() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(1);

    vector<string> lines(1, "");
    vector<string> files;
    int cx = 0, cy = 0;
    bool running = true;
    string formatMode = "";

    while (running) {
        clear();
        files.clear();
        list_directory_tree(".", files);
        draw_header();
        draw_sidebar(files);
        draw_separator();
        draw_editor(lines);
        draw_footer();
        move(cy + 2, cx + SIDEBAR_WIDTH);
        refresh();

        int ch = getch();

        if (ch == 24) { // Ctrl+X
            running = false;
        } else if (ch == 26) { // Ctrl+Z (Undo)
            if (!undoStack.empty()) {
                redoStack.push(lines);
                lines = undoStack.top();
                undoStack.pop();
                cx = cy = 0;
            }
        } else if (ch == 25) { // Ctrl+Y (Redo)
            if (!redoStack.empty()) {
                undoStack.push(lines);
                lines = redoStack.top();
                redoStack.pop();
                cx = cy = 0;
            }
        } else if (ch == 10) { // Enter
            save_undo_state(lines);
            lines.insert(lines.begin() + cy + 1, "");
            cy++;
            cx = 0;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (cx > 0) {
                save_undo_state(lines);
                lines[cy].erase(cx - 1, 1);
                cx--;
            } else if (cy > 0) {
                save_undo_state(lines);
                cx = lines[cy - 1].size();
                lines[cy - 1] += lines[cy];
                lines.erase(lines.begin() + cy);
                cy--;
            }
        } else if (ch == KEY_LEFT && cx > 0) {
            cx--;
        } else if (ch == KEY_RIGHT && cx < lines[cy].size()) {
            cx++;
        } else if (ch == KEY_UP && cy > 0) {
            cy--;
            if (cx > lines[cy].size()) cx = lines[cy].size();
        } else if (ch == KEY_DOWN && cy + 1 < lines.size()) {
            cy++;
            if (cx > lines[cy].size()) cx = lines[cy].size();
        } else if (ch == 2) { // Ctrl+B (Bold)
            formatMode = "bold";
        } else if (ch == 9) { // Ctrl+I (Italic)
            formatMode = "italic";
        } else if (ch == 21) { // Ctrl+U (Underline)
            formatMode = "underline";
        } else if (isprint(ch)) {
            save_undo_state(lines);
            string charStr(1, (char)ch);
            if (!formatMode.empty()) {
                charStr = formatText(charStr, formatMode);
                formatMode = "";
            }
            lines[cy].insert(cx, charStr);
            cx += charStr.size();
        }
    }

    endwin();
    return 0;
}
