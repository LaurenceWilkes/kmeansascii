#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "display.hpp"

namespace image_processor {
namespace {

constexpr wchar_t kWindowClassName[] = L"fontPicImageDisplay";

struct DisplayData {
  int width;
  int height;
  std::vector<unsigned char> pixels;
};

unsigned char toByte(double value) {
  return static_cast<unsigned char>(
      std::clamp(std::lround(value), 0L, 255L));
}

LRESULT CALLBACK windowProc(HWND window, UINT message,
                            WPARAM wparam, LPARAM lparam) {
  if (message == WM_NCCREATE) {
    const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
    SetWindowLongPtrW(window, GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(create->lpCreateParams));
  }

  auto* data = reinterpret_cast<DisplayData*>(
      GetWindowLongPtrW(window, GWLP_USERDATA));

  switch (message) {
    case WM_PAINT: {
      PAINTSTRUCT paint{};
      HDC device = BeginPaint(window, &paint);

      if (data != nullptr) {
        BITMAPINFO bitmap{};
        bitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap.bmiHeader.biWidth = data->width;
        bitmap.bmiHeader.biHeight = -data->height; // top-down rows
        bitmap.bmiHeader.biPlanes = 1;
        bitmap.bmiHeader.biBitCount = 24;
        bitmap.bmiHeader.biCompression = BI_RGB;

        SetDIBitsToDevice(device, 0, 0, data->width, data->height,
                           0, 0, 0, data->height, data->pixels.data(),
                           &bitmap, DIB_RGB_COLORS);
      }

      EndPaint(window, &paint);
      return 0;
    }

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    default:
      return DefWindowProcW(window, message, wparam, lparam);
  }
}

void registerWindowClass() {
  WNDCLASSW windowClass{};
  windowClass.lpfnWndProc = windowProc;
  windowClass.hInstance = GetModuleHandleW(nullptr);
  windowClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
  windowClass.lpszClassName = kWindowClassName;

  if (RegisterClassW(&windowClass) == 0 &&
      GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
    throw std::runtime_error("Could not register display window");
  }
}

} // namespace

void display(const Image& image) {
  if (image.width() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
      image.height() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
    throw std::out_of_range("Image is too large to display");
  }

  const int width = static_cast<int>(image.width());
  const int height = static_cast<int>(image.height());
  const std::size_t rowBytes =
      (static_cast<std::size_t>(width) * 3 + 3) & ~std::size_t{3};

  DisplayData data{width, height, std::vector<unsigned char>(rowBytes * height)};

  for (std::size_t i = 0; i < image.width() * image.height(); ++i) {
    const Colour colour = image.colourAt(i);
    const double cb = static_cast<double>(colour.cb) - 128.0;
    const double cr = static_cast<double>(colour.cr) - 128.0;
    const std::size_t row = i / image.width();
    const std::size_t column = i % image.width();
    unsigned char* pixel = data.pixels.data() + row * rowBytes + column * 3;

    pixel[0] = toByte(colour.y + 1.772 * cb);                 // B
    pixel[1] = toByte(colour.y - 0.344136 * cb - 0.714136 * cr); // G
    pixel[2] = toByte(colour.y + 1.402 * cr);                 // R
  }

  registerWindowClass();

  RECT client{0, 0, width, height};
  AdjustWindowRect(&client, WS_OVERLAPPEDWINDOW, FALSE);

  HWND window = CreateWindowExW(
      0, kWindowClassName, L"fontPic", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, client.right - client.left,
      client.bottom - client.top, nullptr, nullptr, GetModuleHandleW(nullptr),
      &data);

  if (window == nullptr) {
    throw std::runtime_error("Could not create display window");
  }

  MSG message{};
  while (GetMessageW(&message, nullptr, 0, 0) > 0) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
}

} // namespace image_processor
