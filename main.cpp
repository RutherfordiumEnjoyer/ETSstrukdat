#include <iostream>
#include <conio.h>
#include <string>
#include <stack>
using namespace std;

enum FormatType {
    NORMAL = 0,
    BOLD = 1,
    ITALIC = 2,
    UNDERLINE = 4
};

struct TextUnit {
    char character;
    int format;
};

class TextEditor {
private:
    string text;
    int cursorPos;
    stack<string> undoStack;
    stack<string> redoStack;
    
    void saveState() {
        undoStack.push(text);
        while (!redoStack.empty()) redoStack.pop();
    }
    
public:
    TextEditor() : cursorPos(0) {}
    
    void insertChar(char c) {
        saveState();
        text.insert(cursorPos++, 1, c);
        display();
    }
    
    void deleteChar() {
        if (cursorPos > 0) {
            saveState();
            text.erase(--cursorPos, 1);
            display();
        }
    }
    
    void undo() {
        if (!undoStack.empty()) {
            redoStack.push(text);
            text = undoStack.top();
            undoStack.pop();
            if (cursorPos > text.length()) cursorPos = text.length();
            display();
        }
    }
    
    void redo() {
        if (!redoStack.empty()) {
            undoStack.push(text);
            text = redoStack.top();
            redoStack.pop();
            if (cursorPos > text.length()) cursorPos = text.length();
            display();
        }
    }
    
    void display() {
        system("cls");
        cout << "===== Simple Text Editor =====" << endl;
        cout << "Text: " << text << endl;
        
        cout << "      ";
        for (int i = 0; i < cursorPos; i++) cout << " ";
        cout << "^" << endl << endl;
        
        cout << "Commands:" << endl;
        cout << "  Backspace: Hapus | Ctrl+Z: Undo | Ctrl+Y: Redo | Esc: Keluar" << endl;
    }
    
    void run() {
        display();
        bool running = true;
        while (running) {
            int input = _getch();
            
            if (input == 27) running = false;
            else if (input == 8) deleteChar();
            else if (input == 26) undo();
            else if (input == 25) redo();
            else if (input >= 32 && input <= 126) insertChar((char)input);
        }
    }
};

int main() {
    TextEditor editor;
    editor.run();
    return 0;
}
