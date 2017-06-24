#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;

internal void
Win32ResizeDIBSection(int Width, int Height)
{
  if(BitmapMemory)
    {
      VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }
  
  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = Width;
  BitmapInfo.bmiHeader.biHeight = Height;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  int BytesPerPixel = 4;
  int BitmapMemorySize = BytesPerPixel*(Width*Height);
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
  StretchDIBits(DeviceContext,
		X, Y, Width, Height,
		X, Y, Width, Height,
		&BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}
  
LRESULT CALLBACK
Win32MainWindowCallback(_In_ HWND   Window,
		   _In_ UINT   Message,
		   _In_ WPARAM WParam,
		   _In_ LPARAM LParam)
{
  LRESULT Result;
  
  switch(Message) {
  case WM_SIZE:
    {
      RECT ClientRect;
      GetClientRect(Window, &ClientRect);
      int Width = ClientRect.right - ClientRect.left;
      int Height = ClientRect.bottom - ClientRect.top;
      Win32ResizeDIBSection(Width, Height);
      OutputDebugStringA("WM_SIZE\n");
    }break;
    
  case WM_DESTROY:
    {
      Running = false;
    }break;
    
  case WM_CLOSE:
    {
      Running = false;
    }break;
    
  case WM_ACTIVATEAPP:
    {
      OutputDebugStringA("WM_ACTIVATEAPP\n");
    }break;
    
  case WM_PAINT:
    {
      PAINTSTRUCT Paint;
      HDC DeviceContext = BeginPaint(Window, &Paint);
      int X = Paint.rcPaint.left;
      int Y = Paint.rcPaint.top;
      int Width = Paint.rcPaint.right - Paint.rcPaint.left;
      int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
      Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
      EndPaint(Window, &Paint);
    }break;
    
  default:
    {
      Result = DefWindowProc(Window, Message, WParam, LParam);
      //      OutputDebugStringA("DEFAULT\n");
    }break;
  }

  return Result;
}

int CALLBACK
WinMain(_In_ HINSTANCE Instance,
	_In_ HINSTANCE PrevInstance,
	_In_ LPSTR     CommandLine,
	_In_ int       ShowCode)
{
  WNDCLASS WindowClass = {};
  WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  //  WindowClass.hIcon = ;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";

  if (RegisterClassA(&WindowClass))
    {
      HWND WindowHandle = CreateWindowExA(
				   0,
				   WindowClass.lpszClassName,
				   "Handmade Hero",
				   WS_OVERLAPPEDWINDOW|WS_VISIBLE,
				   CW_USEDEFAULT,
				   CW_USEDEFAULT,
				   CW_USEDEFAULT,
				   CW_USEDEFAULT,
				   0,
				   0,
				   Instance,
				   0
				 );
      if(WindowHandle)
	{
	  Running = true;
	  while(Running)
	    {
	      MSG Message;
	      BOOL MessageResult = GetMessageA(&Message, NULL, 0, 0);
	      if (MessageResult > 0)
		{
		  TranslateMessage(&Message);
		  DispatchMessageA(&Message);
		}
	      else
		{
		  break;
		}
	    }
	}
      else
	{
	  
	}
    }
  else
    {
      
    }
  
  
  return 0;
};
