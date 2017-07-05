#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;


internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
  int Width = BitmapWidth;
  int Height = BitmapHeight;
  
  int Pitch = Width*BytesPerPixel;
  uint8_t *Row = (uint8_t *) BitmapMemory;
  for (int Y = 0; Y < BitmapHeight; ++Y)
    {
      uint32_t *Pixel = (uint32_t *)Row;
      for (int X = 0; X < BitmapWidth; ++X)
	{
	  uint8_t Blue = X + XOffset;
	  uint8_t Green = Y + YOffset;

	  // because the architecture is little endian
	  // the Blue channel is the least significant byte
	  // then the green
	  // then the red
	  // then padding
	  *Pixel++ = (uint32_t)(Blue | Green << 8);
	}

      Row += Pitch;
    }
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
  if(BitmapMemory)
    {
      VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

  BitmapWidth = Width;
  BitmapHeight = Height;
  
  BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
  BitmapInfo.bmiHeader.biWidth = BitmapWidth;
  BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
  BitmapInfo.bmiHeader.biPlanes = 1;
  BitmapInfo.bmiHeader.biBitCount = 32;
  BitmapInfo.bmiHeader.biCompression = BI_RGB;

  int BitmapMemorySize = BytesPerPixel*(BitmapWidth*BitmapHeight);
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height)
{
  int WindowWidth = ClientRect->right - ClientRect->left;
  int WindowHeight = ClientRect->bottom - ClientRect->top;
  StretchDIBits(DeviceContext,
		0, 0, BitmapWidth, BitmapHeight,
		0, 0, WindowWidth, WindowHeight,
		BitmapMemory,
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

      RECT ClientRect;
      GetClientRect(Window, &ClientRect);
      
      Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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
      HWND Window = CreateWindowExA(
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
      if(Window)
	{
	  int XOffset = 0;
	  int YOffset = 0;

	  Running = true;
	  while(Running)
	    {
	      
	      MSG Message;
	      while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
		{
		  if (Message.message == WM_QUIT)
		    {
		      Running = false;
		    }
		  
		  TranslateMessage(&Message);
		  DispatchMessageA(&Message);
		}

	      RenderWeirdGradient(XOffset, YOffset);

	      HDC DeviceContext = GetDC(Window);
	      RECT ClientRect;
	      GetClientRect(Window, &ClientRect);
	      int WindowWidth = ClientRect.right - ClientRect.left;
	      int WindowHeight = ClientRect.bottom - ClientRect.top;
	      Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
	      ReleaseDC(Window, DeviceContext);

	      ++ XOffset;
	      
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
