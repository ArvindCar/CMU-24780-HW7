#include <iostream>
#include <fstream>
#include <cstring>
#include "fssimplewindow.h"

class CharBitmap {
public:
    int wid, hei;
    char *pix;

    CharBitmap() 
    {
        wid = 0;
        hei = 0;
        pix = nullptr;
    }

    void CleanUp() 
    {
        if (pix != nullptr) {
            delete[] pix;
            pix = nullptr;
        }
        wid = 0;
        hei = 0;
    }

    ~CharBitmap() 
    {
        CleanUp();
    }

    void Create(int w, int h) 
    {
        CleanUp();  
        wid = w;
        hei = h;
        pix = new char[w * h];
        std::memset(pix, 0, w * h);  
    }

    void SetPixel(int x, int y, char p) 
    {
        if (x >= 0 && x < wid && y >= 0 && y < hei) {
            pix[y * wid + x] = p;
        }
    }

    char GetPixel(int x, int y) const 
    {
        if (x >= 0 && x < wid && y >= 0 && y < hei) {
            return pix[y * wid + x];
        }
        return 0;  
    }

    void Draw() const 
    {
        for (int y = 0; y < hei; ++y) {
            for (int x = 0; x < wid; ++x) {
                int pixelValue = GetPixel(x, y);
                switch (pixelValue) {
                    case 0: glColor3ub(0, 0, 0); break;
                    case 1: glColor3ub(0, 0, 255); break;
                    case 2: glColor3ub(255, 0, 0); break;
                    case 3: glColor3ub(255, 0, 255); break;
                    case 4: glColor3ub(0, 255, 0); break;
                    case 5: glColor3ub(0, 255, 255); break;
                    case 6: glColor3ub(255, 255, 0); break;
                    case 7: glColor3ub(255, 255, 255); break;
                }

                int px = x * 20;
                int py = y * 20;

                glBegin(GL_QUADS);
                glVertex2i(px, py);
                glVertex2i(px + 20, py);
                glVertex2i(px + 20, py + 20);
                glVertex2i(px, py + 20);
                glEnd();
            }
        }
    }

    void DrawGrid() const 
    {
        glColor3ub(255, 255, 255);  
        for (int x = 0; x <= wid; ++x) {
            glBegin(GL_LINES);
            glVertex2i(x * 20, 0);
            glVertex2i(x * 20, hei * 20);
            glEnd();
        }
        for (int y = 0; y <= hei; ++y) {
            glBegin(GL_LINES);
            glVertex2i(0, y * 20);
            glVertex2i(wid * 20, y * 20);
            glEnd();
        }
    }

    void SaveToFile(const char *filename) const 
    {
        std::ofstream ofs(filename);
        ofs << wid << " " << hei << "\n";
        for (int y = 0; y < hei; ++y) {
            for (int x = 0; x < wid; ++x) {
                ofs << (int)GetPixel(x, y) << " ";
            }
            ofs << "\n";
        }
        ofs.close();
    }

    void LoadFromFile(const char *filename) 
    {
        std::ifstream ifs(filename);
        if (ifs.is_open()) {
            int w, h;
            ifs >> w >> h;
            Create(w, h);  
            for (int y = 0; y < hei; ++y) {
                for (int x = 0; x < wid; ++x) {
                    int value;
                    ifs >> value;
                    SetPixel(x, y, (char)value);
                }
            }
            ifs.close();
        }
    }
};

int main() {
    int w = 0, h = 0;
    char input[100];
    while (true) {
        std::cout << "Enter Dimension> ";
        std::fgets(input, sizeof(input), stdin); 

        if (std::sscanf(input, "%d %d", &w, &h) == 2 && (w > 0 && h > 0 && w <= 64 && h <= 64)) {
            break;
        }
        std::cout << "Invalid input. Please enter two integers (1 <= w, h <= 64).\n";
    }

    CharBitmap bitmap;
    bitmap.Create(w, h);

    FsOpenWindow(16, 16, w * 20, h * 20, 1);

    while (FSKEY_ESC != FsInkey()) {
        FsPollDevice();

        int key = FsInkey();
        int lb, mb, rb, mx, my;
        FsGetMouseState(lb, mb, rb, mx, my);

        int x = mx / 20;
        int y = my / 20;

        if (key >= FSKEY_0 && key <= FSKEY_7) 
        {
            int colorCode = key - FSKEY_0;
            bitmap.SetPixel(x, y, (char)colorCode);
        }

        if (FSKEY_S == key) 
        {
            bitmap.SaveToFile("pattern.txt");
        }

        if (FSKEY_L == key) 
        {
            bitmap.LoadFromFile("pattern.txt");
        }

        if (key == FSKEY_ESC)
        {
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bitmap.Draw();
        bitmap.DrawGrid();
        FsSwapBuffers();
        FsSleep(10);
    }

    return 0;
}
