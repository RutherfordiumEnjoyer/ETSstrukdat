#include <iostream>
#include <conio.h>

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
public:
    TextEditor() {
    }
    
    void run() {
        cout << "Text Editor sedang berjalan.." << endl;
        cout << "Tekan tombol apapun untuk keluar" << endl;
        _getch();
    }
};

int main() {
    TextEditor editor;
    editor.run();
    return 0;
}
