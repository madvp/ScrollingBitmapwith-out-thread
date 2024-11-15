#include <windows.h>
//#include <cstdlib>
//#include <ctime>

const int WIDTH = 256;
const int HEIGHT = 300;
RECT dirtyRect = { 0, 0, WIDTH, HEIGHT };

HBITMAP hBitmap = NULL;
BYTE* pBits = NULL;  // Changed from void* to BYTE*
BYTE* pRow = NULL;   // Pointer for a single row

// Initialize DIB section
void InitBitmap(HWND hwnd) {
    BITMAPINFO bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo));

    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = WIDTH;
    bmpInfo.bmiHeader.biHeight = HEIGHT;  // Top-down DIB
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(hwnd);
    hBitmap = CreateDIBSection(hdc, &bmpInfo, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
    ReleaseDC(hwnd, hdc);

    if (!hBitmap) {
        MessageBox(hwnd, L"Failed to create DIB section!", L"Error", MB_OK);
    }

    // Set the pointer to the start of the first row
    pRow = pBits;
}

// Update the bitmap data by shifting the rows and adding a new row at the bottom
void UpdateBitmapData() {
    if (pBits == NULL || pRow == NULL) return;

    // Shift all rows up by one
    memmove(pBits, pBits + WIDTH * 3, (HEIGHT - 1) * WIDTH * 3);

    // Generate a new row of random color data
    BYTE* newRow = pRow + (HEIGHT - 1) * WIDTH * 3;
    for (int x = 0; x < WIDTH; ++x) {
        newRow[x * 3 + 0] = rand() % 256; // Blue
        newRow[x * 3 + 1] = rand() % 256; // Green
        newRow[x * 3 + 2] = rand() % 256; // Red
    }

    // Set the modified area (the entire bitmap)
    dirtyRect.left = 0;
    dirtyRect.right = WIDTH;
    dirtyRect.top = 0;
    dirtyRect.bottom = HEIGHT;
}

// Draw the bitmap using SetDIBitsToDevice
void PaintBitmap(HWND hwnd, HDC hdc) {
    BITMAPINFO bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo));

    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = WIDTH;
    bmpInfo.bmiHeader.biHeight = HEIGHT;  // Top-down DIB
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 24;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    // Create a memory DC (off-screen buffer)
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
    HBITMAP hOldBuffer = (HBITMAP)SelectObject(hdcMem, hbmBuffer);

    // Draw the bitmap in the memory DC (off-screen buffer)
    SetDIBitsToDevice(hdcMem, 0, 0, WIDTH, HEIGHT, 0, 0, 0, HEIGHT, pBits, &bmpInfo, DIB_RGB_COLORS);

    // Copy the memory DC to the screen
    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hdcMem, 0, 0, SRCCOPY);

    // Clean up
    SelectObject(hdcMem, hOldBuffer);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcMem);
}
