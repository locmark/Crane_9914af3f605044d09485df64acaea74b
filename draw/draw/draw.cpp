// draw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "draw.h"
#include <ctime>

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

int acceleration;
const int accelerationStep = 5;

const int step = 5;
const int timerInterval = 100;

const int massMax = 100;
const int massLiftable = 50;

const int CraneHookHeight = 50;
const int CraneHookWidth = 30;
const int CraneMountingHeight = 25;
const int CraneMountingWidth = 45;
const int CraneJibWidth = 885;
const int FloorLevel = 470;

const int boxAmount = 10;
const int BoxSize = 50;

int CatchedBox = 0;
bool isCatched = false;

const Point CraneJibPosition(320, 88);
Point BoxPositionRelativeToHook;
Point CraneHookPosition(320, 400);


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
  srand((unsigned)time(NULL));
  for (ptrdiff_t i = 0; i < boxAmount; i++) 
  { 
    boxes[i].X = 400 + 60 * i; 
    boxes[i].Y = FloorLevel - (BoxSize + 1); 
    boxes[i].width = BoxSize;
    boxes[i].height = BoxSize;
    boxes[i].mass = rand() % massMax + 1; 
  } 
} 
 
 
void DrawBoxes(Graphics* graph) { 
  Pen blackPen(Color(255, 0, 0, 0));
  SolidBrush* boxBrush; 
  for (size_t i = 0; i < boxAmount; i++) 
  {
	  if (boxes[i].mass <= massLiftable) {
		  boxBrush = new SolidBrush(Color::Green);
	  }
	  else {
		  boxBrush = new SolidBrush(Color::Red);
	  }
	  graph->DrawRectangle(&blackPen, boxes[i].X, boxes[i].Y, boxes[i].width, boxes[i].height);
	  graph->FillRectangle(boxBrush, boxes[i].X + 1, boxes[i].Y + 1, boxes[i].width - 1, boxes[i].height - 1);
	  delete boxBrush;
  } 
} 
 

void DrawCrane(Graphics* graph) {
	Pen blackPen(Color(255, 0, 0, 0));
	SolidBrush yellowBrush(Color(255, 255, 255, 0));
	Image craneImage(L"images\\crane.png", FALSE);
	Image mountingImage(L"images\\mounting.png", FALSE);
	Image hookImage(L"images\\hook.png", FALSE);
	// crane body
	graph->DrawImage(&craneImage, 0, 0, CraneJibWidth + CraneJibPosition.X, FloorLevel);
	//crane mounting
	graph->DrawImage(&mountingImage, CraneHookPosition.X - CraneMountingWidth / 2, CraneJibPosition.Y, CraneMountingWidth, CraneMountingHeight);
	// crane line
	graph->DrawLine(&blackPen, CraneHookPosition.X, CraneJibPosition.Y + CraneMountingHeight, CraneHookPosition.X, CraneHookPosition.Y - CraneHookHeight);
	// crane hook
	graph->DrawImage(&hookImage, CraneHookPosition.X - CraneHookWidth/2, CraneHookPosition.Y - CraneHookHeight, CraneHookWidth, CraneHookHeight);
}


void DrawFloor(Graphics* graph) {
	Pen blackPen(Color(255, 0, 0, 0));
	graph->DrawLine(&blackPen, 0, FloorLevel, windowSizeX, FloorLevel);
}


bool IsColliding (int& i) {
	for (i = 0; i < boxAmount; i++)
	{
		if (boxes[i].X <= CraneHookPosition.X && CraneHookPosition.X <= boxes[i].X + BoxSize) {
			if (boxes[i].Y <= CraneHookPosition.Y) {
				return true;
			}
		}
	}
	return false;
}


bool IsCollidingFromRight() {
	for (size_t i = 0; i < boxAmount; i++)
	{
		if (boxes[i].Y <= CraneHookPosition.Y &&
			boxes[i].X + BoxSize <= (CraneHookPosition.X - CraneHookWidth / 2) + 2 &&
			boxes[i].X + BoxSize >= (CraneHookPosition.X - CraneHookWidth / 2) - 2
		) {
			return true;
		}
	}
	return false;
}


bool IsCollidingFromLeft() {
	for (size_t i = 0; i < boxAmount; i++)
	{
		if (boxes[i].Y <= CraneHookPosition.Y &&
			boxes[i].X >= (CraneHookPosition.X + CraneHookWidth / 2) - 2 &&
			boxes[i].X <= (CraneHookPosition.X + CraneHookWidth / 2) + 2
			) {
			return true;
		}
	}
	return false;
}


bool IsBoxCollidingWithAnotherBoxFromTop(int id) {
	for (size_t i = 0; i < boxAmount; i++)
	{
		if (i != id && 
			boxes[id].X > boxes[i].X - BoxSize && 
			boxes[id].X < boxes[i].X + BoxSize &&
			boxes[id].Y + BoxSize >= boxes[i].Y
		) {
			return true;
		}
	}
	return false;
}


bool IsBoxCollidingWithAnotherBoxFromRight(int id) {
	for (size_t i = 0; i < boxAmount; i++)
	{
		if (i != id &&
			boxes[id].Y >= boxes[i].Y - BoxSize &&
			boxes[id].Y <= boxes[i].Y + BoxSize &&
			boxes[id].X - BoxSize - 2 < boxes[i].X && 
			boxes[id].X - BoxSize + 2 > boxes[i].X
			) {
			return true;
		}
	}
	return false;
}


bool IsBoxCollidingWithAnotherBoxFromLeft(int id) {
	for (size_t i = 0; i < boxAmount; i++)
	{
		if (i != id &&
			boxes[id].Y >= boxes[i].Y - BoxSize &&
			boxes[id].Y <= boxes[i].Y + BoxSize &&
			boxes[id].X + BoxSize - 2 < boxes[i].X &&
			boxes[id].X + BoxSize + 2 > boxes[i].X
			) {
			return true;
		}
	}
	return false;
}


void UpdateHookPosition() {
	int i;
	if (isRightClicked && !IsBoxCollidingWithAnotherBoxFromLeft(CatchedBox)) {
		if (CraneHookPosition.X < CraneJibPosition.X + CraneJibWidth - CraneMountingWidth && !IsCollidingFromLeft())
		CraneHookPosition.X += step;
		
	}
	if (isLeftClicked && !IsBoxCollidingWithAnotherBoxFromRight(CatchedBox)) {
		if (CraneHookPosition.X > CraneJibPosition.X && !IsCollidingFromRight())
			CraneHookPosition.X -= step;
	}
	if (isUpClicked) {
		if (CraneHookPosition.Y > CraneHookHeight + CraneJibPosition.Y + CraneMountingHeight)
			CraneHookPosition.Y -= step;

	}

	if (isDownClicked && (!IsColliding(i) || isCatched)) {
		if (CraneHookPosition.Y < FloorLevel - (isCatched ? BoxSize : 0) && (isCatched ? !IsBoxCollidingWithAnotherBoxFromTop(CatchedBox) : 1)) {
			CraneHookPosition.Y += step;
		}
	}
}


void CatchBox() {
	int i;
	if (IsColliding(i)) {
		if (isCatched) {
			isCatched = false;
		} else {
			if (boxes[i].mass <= massLiftable) {
				isCatched = true;
				acceleration = 0;
				CatchedBox = i;
				BoxPositionRelativeToHook.X = boxes[i].X - CraneHookPosition.X;
				BoxPositionRelativeToHook.Y = boxes[i].Y - CraneHookPosition.Y;
			}
		}
	}
}


void MoveBox()
{
	if (isCatched) {
		boxes[CatchedBox].X = CraneHookPosition.X + BoxPositionRelativeToHook.X;
		boxes[CatchedBox].Y = CraneHookPosition.Y + BoxPositionRelativeToHook.Y;
	}
	else {
		if (CatchedBox >= 0 && CatchedBox < boxAmount) {
			if (!IsBoxCollidingWithAnotherBoxFromTop(CatchedBox)) {
				boxes[CatchedBox].Y += (step + acceleration);
				if (IsBoxCollidingWithAnotherBoxFromTop(CatchedBox)) {
					boxes[CatchedBox].Y = FloorLevel - (2*BoxSize + 1);
					return;
				}					
				if (boxes[CatchedBox].Y >= FloorLevel - (BoxSize + 1)) {
					boxes[CatchedBox].Y = FloorLevel - (BoxSize + 1);
					return;
				}
				acceleration += accelerationStep;
			}
		}
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
	SetTimer(hWnd, TMR_1, timerInterval, 0);
	return 0;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// *****show console***** //
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	FILE *stream;
	freopen_s(&stream, "CON", "w", stdout);
	// ********************** //
	
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
		case ID_BUTTON_UP:
			if (isUpClicked) {
				isUpClicked = false;
			}
			else {
				isUpClicked = true;
			}
			if (isDownClicked) {
				isDownClicked = false;
				isUpClicked = true;
			}
			break;
		case ID_BUTTON_DOWN:
			if (isDownClicked) {
				isDownClicked = false;
			}
			else {
				isDownClicked = true;
			}
			if (isUpClicked) {
				isUpClicked = false;
				isDownClicked = true;
			}
			break;
		case ID_BUTTON_CATCH:
			CatchBox();
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
				UpdateHookPosition();
				MoveBox();
				hdc = BeginPaint(hWnd, &ps);				
				MyOnPaint(hdc);
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
