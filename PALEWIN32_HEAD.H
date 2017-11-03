
#ifndef __PALEWIN32_HEAD_H__
#define __PALEWIN32_HEAD_H__

#include <windows.h>



//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by resource.rc
//
#define IDR_MENU                        101
#define IDI_ICON                        102
#define ID_QUIT                         40001

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        105
#define _APS_NEXT_COMMAND_VALUE         40002
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif



static HWND g_hwnd;

LRESULT CALLBACK graywinWindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam) {

   SDL_Event event;

   switch(uMsg) {
      case WM_COMMAND:
         if (LOWORD(wParam) == ID_QUIT) {
            event.type = SDL_QUIT;
            SDL_PushEvent(&event);
         }
         break;

      default:
         return DefWindowProc(hwnd, uMsg, wParam, lParam);
         break;
   }

   return 1;
}


int init_win32_window(HINSTANCE hInst, int width, int height) {

   char windowid[100];
   WNDCLASSEX wcex;
   RECT rect;

   ZeroMemory(&wcex, sizeof(wcex));
   wcex.cbSize = sizeof(wcex);
   wcex.style = CS_BYTEALIGNCLIENT;
   wcex.lpfnWndProc = graywinWindowProc;
   wcex.hInstance = hInst;
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON));
   wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
   //wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
   wcex.lpszClassName = "Mainframe";

   if (!RegisterClassEx(&wcex)) {
      MessageBox(NULL, "could not register class", "Error", MB_ICONERROR);
      return 0;
   }

   ZeroMemory(&rect, sizeof(RECT));
   rect.right = width;
   rect.bottom = height;
   AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, TRUE, 0);

   g_hwnd = CreateWindowEx(
      0,
      "Mainframe",
      "SDL+Win32",
      WS_OVERLAPPEDWINDOW,
 		 20,0,
	    abs(rect.left) + rect.right,
      abs(rect.top) + rect.bottom,
      NULL,
      LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU)),
      hInst,
      NULL);

   if (!g_hwnd) {
      MessageBox(NULL, "could not create window", "Error", MB_ICONERROR);
      return 0;
   }

   //ShowWindow(g_hwnd, SW_NORMAL);
   //UpdateWindow(g_hwnd);

   strcpy(windowid, "SDL_WINDOWID=");
   _ltoa((long)g_hwnd, windowid+13, 10);
   _putenv(windowid);

   return 1;
}

#endif
