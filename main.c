#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_easy_font.h"
#include "stb-master/stb_image.h"

//using namespace std;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

BOOL visible = FALSE;
float vertex[] = {-0.65,-0.9,0, 0.65,-0.9,0, 0.65,0.9,0, -0.65,0.9,0};
float texCoord[] = {0,1, 1,1, 1,0, 0,0};

float theta = 0.0f;


// Здесь находятся кнопки

void print_string(float x, float y, char *text, float r, float g, float b)
{
    static char buffer[99999]; // ~500 chars
    int num_quads;

    num_quads = stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

int width, height;

typedef struct {
    char name[30];
    float vert[8];
    BOOL hover;
} TButton;

TButton btn[] = {
    {"Num_lab", {0,0, 125,0, 125,30+20, 0,30+20}, FALSE},
    {"Full_name", {0,40, 125,40, 125,70+20, 0,70+20}, FALSE},
    {"Animation", {0,80, 125,80, 125,110+10, 0,110+10}, FALSE},
    {"Exit", {0,120, 125,120, 125,150+5, 0,150+5}, FALSE}
};

int btnCnt = sizeof(btn) / sizeof(btn[0]);

void TButton_Show(TButton btn)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    if (btn.hover) {

        glColor3ub(220, 20, 60);
    } else {
        glColor3ub(0, 255, 0);
    }
    glVertexPointer(2, GL_FLOAT, 0, btn.vert);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    glTranslatef(btn.vert[0], btn.vert[1], 0);
    glScalef(2, 2, 2);
    print_string(3, 3, btn.name, 0, 0, 0);
    glPopMatrix();
}


BOOL PointInButton(int x, int y, TButton btn)
{
    return (x > btn.vert[0]) && (x < btn.vert[4]) &&
           (y > btn.vert[1]) && (y < btn.vert[5]);
}

void ShowMenu() {
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    for (int i = 0; i < btnCnt; i++)
        TButton_Show(btn[i]);
    glPopMatrix();
}

void ShowAnimation(){
    glPushMatrix();
    glRotatef(theta, 1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);   glVertex2f(0.0f,   1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);   glVertex2f(0.87f,  -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f);   glVertex2f(-0.87f, -0.5f);
    glEnd();
    theta += 1.0f;
    Sleep (1);
    glPopMatrix();
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    setlocale(0,"");
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          1080,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ShowMenu();
            if (visible) {
             ShowAnimation();
            }

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_LBUTTONDOWN:
            for (int i = 0; i < btnCnt; i++) {
                if (PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i])) {
                    if (strcmp(btn[i].name, "Num_lab") == 0) {
                        printf ("  Лабораторная работа 2  |  Компьютерная графика  |\n");

                        visible = FALSE;
                    }
                    if (strcmp(btn[i].name, "Full_name") == 0)
                        printf ("  Субарман Илья Владимирович  |  Эвмб-22-1 |\n");
                        visible = FALSE;

                     if (strcmp(btn[i].name, "Animation") == 0)
                         visible = TRUE;

                    if (strcmp(btn[i].name, "Exit") == 0)
                        PostQuitMessage(0);
                }
            }
            break;

        case WM_MOUSEMOVE:
            for (int i = 0; i < btnCnt; i++)
                btn[i].hover = PointInButton(LOWORD(lParam), HIWORD(lParam), btn[i]);

            break;

        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            glViewport(0, 0, width, height);
            glLoadIdentity();
            float k = width / (float)height;
            glOrtho(-k, k, -1, 1, -1, 1);
            break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                    break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

