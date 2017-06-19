// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw.h"

#define MAX_LOADSTRING 100
#define TMR_1 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name


HWND hWnd;
HWND hwndButton;

bool isUpClicked = false;
bool isDownClicked = false;
bool isRightClicked = false;
bool isLeftClicked = false;


const int boxAmount = 10; 
const int CraneArmHeight = 50;
const int CraneArmWidth = 30;
const int CraneJibWidth = 1000;
const int CraneJibHeight = 20;

const int FloorLevel = 470;

const Point CraneJibPosition(10, 10);

Point CraneArmPosition(600, 400);

int windowSizeX;
int windowSizeY;


struct box { 
  int X; 
  int Y; 
  int width; 
  int height; 
  int mass; 
}; 
 

box boxes[boxAmount]; 


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void BoxInit() { 
  for (size_t i = 0; i < boxAmount; i++) 
  { 
    boxes[i].X = 200 + 60 * i; 
    boxes[i].Y = FloorLevel - 50; 
    boxes[i].width = 50; 
    boxes[i].height = 50; 
    boxes[i].mass = 100; 
  } 
} 
 
 
void DrawBoxes(Graphics* graph) { 
  Pen blackPen(Color(255, 0, 0, 0)); 
 
  for (size_t i = 0; i < boxAmount; i++) 
  { 
    graph->DrawRectangle(&blackPen, boxes[i].X, boxes[i].Y, boxes[i].width, boxes[i].height); 
  } 
} 
 
void DrawCrane(Graphics* graph) {
	Pen blackPen(Color(255, 0, 0, 0));
	SolidBrush yellowBrush(Color(255, 255, 255, 0));
	Rect CraneJib(CraneJibPosition.X, CraneJibPosition.Y, CraneJibWidth, CraneJibHeight);
	graph->FillRectangle(&yellowBrush, CraneJib);
	graph->DrawRectangle(&blackPen, CraneJib);

	graph->DrawLine(&blackPen, Point(CraneArmPosition.X, CraneJibPosition.Y + CraneJibHeight), CraneArmPosition);
	graph->DrawRectangle(&blackPen, CraneArmPosition.X - CraneArmWidth / 2, CraneArmPosition.Y, CraneArmWidth, CraneArmHeight);
}


void DrawFloor(Graphics* graph) {
	Pen blackPen(Color(255, 0, 0, 0));
	graph->DrawLine(&blackPen, 0, FloorLevel, windowSizeX, FloorLevel);
}


void UpdateArmPosition() {
	int a;
	if (isRightClicked) {
		if (CraneArmPosition.X < CraneJibPosition.X + CraneJibWidth)
			CraneArmPosition.X++;
		
	}
	if (isLeftClicked) {
		if (CraneArmPosition.X > CraneJibPosition.X)
			CraneArmPosition.X--;
	}
}


void MyOnPaint(HDC hdc)
{
	Bitmap* bmp = new Bitmap(windowSizeX, windowSizeY);
	Graphics* graph = Graphics::FromImage(bmp);

	graph->Clear(Color(255, 255, 255));

	DrawBoxes(graph); 
	DrawFloor(graph);
	DrawCrane(graph);


	Graphics graphics(hdc);
	graphics.DrawImage(bmp, 0, 0, windowSizeX, windowSizeY);

	delete bmp;
	delete graph;
}


int OnCreate()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	windowSizeX = rect.right - rect.left;
	windowSizeY = rect.bottom - rect.top;
	BoxInit(); 
	SetTimer(hWnd, TMR_1, 50, 0);
	return 0;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAW));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	GdiplusShutdown(gdiplusToken);

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DRAW);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(	szWindowClass,
						szTitle, 
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,
						0, 
						CW_USEDEFAULT, 
						0, 
						NULL, 
						NULL, 
						hInstance, 
						NULL);

   hwndButton = CreateWindow(TEXT("button"),        // The class name required is button
	   TEXT("Up"),									// the caption of the button
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // the styles
	   550, 480,                                    // the left and top co-ordinates
	   60, 20,                                      // width and height
	   hWnd,                                        // parent window handle
	   (HMENU)ID_BUTTON_UP,							// the ID of your button
	   hInstance,                                   // the instance of your application
	   NULL);										// extra bits you dont really need

   hwndButton = CreateWindow(TEXT("button"),        // The class name required is button
	   TEXT("Catch"),								// the caption of the button
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // the styles
	   550, 505,                                    // the left and top co-ordinates
	   60, 20,										// width and height
	   hWnd,                                        // parent window handle
	   (HMENU)ID_BUTTON_CATCH,                      // the ID of your button
	   hInstance,                                   // the instance of your application
	   NULL);                                       // extra bits you dont really need

   hwndButton = CreateWindow(TEXT("button"),        // The class name required is button
	   TEXT("Down"),                                // the caption of the button
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // the styles
	   550, 530,                                    // the left and top co-ordinates
	   60, 20,                                      // width and height
	   hWnd,                                        // parent window handle
	   (HMENU)ID_BUTTON_DOWN,                       // the ID of your button
	   hInstance,                                   // the instance of your application
	   NULL);                                       // extra bits you dont really need

   hwndButton = CreateWindow(TEXT("button"),        // The class name required is button
	   TEXT("Left"),                                // the caption of the button
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // the styles
	   485, 505,                                    // the left and top co-ordinates
	   60, 20,                                      // width and height
	   hWnd,                                        // parent window handle
	   (HMENU)ID_BUTTON_LEFT,                       // the ID of your button
	   hInstance,                                   // the instance of your application
	   NULL);                                       // extra bits you dont really need

   hwndButton = CreateWindow(TEXT("button"),        // The class name required is button
	   TEXT("Right"),                               // the caption of the button
	   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,       // the styles
	   615, 505,                                    // the left and top co-ordinates
	   60, 20,                                      // width and height
	   hWnd,                                        // parent window handle
	   (HMENU)ID_BUTTON_RIGHT,                      // the ID of your button
	   hInstance,                                   // the instance of your application
	   NULL);                                       // extra bits you dont really need



   
   OnCreate();

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON_RIGHT:
			if (isRightClicked) {
				isRightClicked = false;
			}
			else {
				isRightClicked = true;
			}
			if (isLeftClicked) {
				isLeftClicked = false;
				isRightClicked = true;
			}
			break;
		case ID_BUTTON_LEFT:
			if (isLeftClicked) {
				isLeftClicked = false;
			}
			else {
				isLeftClicked = true;
			}
			if (isRightClicked) {
				isRightClicked = false;
				isLeftClicked = true;
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_ERASEBKGND:
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		
		MyOnPaint(hdc);
		EndPaint(hWnd, &ps);
		break;

	
	case WM_DESTROY:
		KillTimer(hWnd, TMR_1);
		PostQuitMessage(0);
		break;

	case WM_TIMER:
		switch (wParam)
		{
			case TMR_1:
				RECT draw_area = {
					0,
					0,
					windowSizeX,
					FloorLevel
				};
				InvalidateRect(hWnd, &draw_area, TRUE);
				UpdateArmPosition();
				hdc = BeginPaint(hWnd, &ps);				
				MyOnPaint(hdc);
				UpdateArmPosition();
				EndPaint(hWnd, &ps);
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
