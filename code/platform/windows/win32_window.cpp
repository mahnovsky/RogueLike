#include "window.hpp"
#include "render_common.hpp"
#include "string.hpp"

#include <windowsx.h>


extern HINSTANCE g_instance;

class Win32_Window : public IWindow
{
public:
	Win32_Window();
	~Win32_Window() override = default;

	void get_size(int& out_width, int& out_height) const override;

	bool init(int width, int height, const char* const title) override;

	void process_events(input::InputListener*) override;

	void swap_buffers() override;

	bool is_quit() const override;

	static LRESULT CALLBACK WindowProc(
		HWND   hwnd,
		UINT   uMsg,
		WPARAM wParam,
		LPARAM lParam
	);

private:
	bool init_opengl();
	void fill_key_map();

private:
	HWND m_handle;
	HINSTANCE m_instance;
	WNDCLASSA m_wnd_class;
	HDC m_device_context;
	HGLRC m_render_context;
	basic::String m_class_name;
	bool m_is_running;
	int m_width;
	int m_height;

	input::KeyCode key_map[0xFF] = { input::KeyCode::Invalid };
};

IWindow* IWindow::create()
{
	return NEW_OBJ(Win32_Window);
}

LRESULT CALLBACK Win32_Window::WindowProc(
	HWND   handle,
	UINT   message,
	WPARAM wparam,
	LPARAM lparam
)
{
	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
	}

	return DefWindowProc(handle, message, wparam, lparam);
}

Win32_Window::Win32_Window()
	:m_handle(nullptr)
	, m_instance(nullptr)
	, m_wnd_class()
	, m_device_context(nullptr)
	, m_render_context(nullptr)
	, m_is_running(true)
	, m_width(800)
	, m_height(600)
{
	fill_key_map();
}

void Win32_Window::get_size(int& out_width, int& out_height) const
{
	out_width = m_width;
	out_height = m_height;
}

bool Win32_Window::init(int width, int height, const char* const title)
{
	m_class_name = title;
	m_class_name += "_ClassName";
	m_instance = g_instance;

	m_wnd_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	m_wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	m_wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	m_wnd_class.lpfnWndProc = &Win32_Window::WindowProc;
	m_wnd_class.hInstance = m_instance;
	m_wnd_class.lpszClassName = m_class_name.get_cstr();

	if (!RegisterClassA(&m_wnd_class))
	{
		LOG("Failed register wnd class ");
		return false;
	}

	DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	DWORD exStyle = WS_EX_APPWINDOW;

	int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	RECT rect;
	rect.left = x;
	rect.right = x + width;
	rect.top = y;
	rect.bottom = y + height;

	AdjustWindowRectEx(&rect, style, FALSE, exStyle);

	m_width = width;
	m_height = height;

	m_handle = CreateWindowEx(exStyle, m_class_name.get_cstr(), title, style, rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, m_instance, NULL);


	if (!m_handle)
	{
		LOG("Failed create window");
		return false;
	}

	bool result = init_opengl();

	if (result)
	{
		//ShowCursor(FALSE);
		ShowWindow(m_handle, SW_NORMAL);
		SetForegroundWindow(m_handle);
		SetFocus(m_handle);
		UpdateWindow(m_handle);
	}

	return result;
}

bool Win32_Window::init_opengl()
{
	m_device_context = GetDC(m_handle);

	PIXELFORMATDESCRIPTOR pfd = {};
	int format;

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	format = ChoosePixelFormat(m_device_context, &pfd);

	if (!format || !SetPixelFormat(m_device_context, format, &pfd))
	{
		LOG("Setting pixel format fail (%d)\n", GetLastError());
		return false;
	}

	HGLRC rc_temp = wglCreateContext(m_device_context);

	if (!rc_temp || !wglMakeCurrent(m_device_context, rc_temp))
	{
		LOG("Сreating temp render context fail (%d)\n", GetLastError());
		return false;
	}

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(rc_temp);

	if (!wglCreateContextAttribsARB)
	{
		LOG("wglCreateContextAttribsARB fail (%d)\n", GetLastError());
		return false;
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	m_render_context = wglCreateContextAttribsARB(m_device_context, 0, attribs);
	if (!m_render_context || !wglMakeCurrent(m_device_context, m_render_context))
	{
		LOG("Creating render context fail (%d)\n", GetLastError());
		return false;
	}

	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	LOG("OpenGL render context information:\n"
		"  Renderer       : %s\n"
		"  Vendor         : %s\n"
		"  Version        : %s\n"
		"  GLSL version   : %s\n"
		"  OpenGL version : %d.%d\n",
		(const char*)glGetString(GL_RENDERER),
		(const char*)glGetString(GL_VENDOR),
		(const char*)glGetString(GL_VERSION),
		(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
		major, minor
	);

	return true;
}

void Win32_Window::fill_key_map()
{
	key_map[VK_SPACE] = input::KeyCode::Space;
	key_map[VK_RETURN] = input::KeyCode::Enter;
	key_map[VK_LCONTROL] = input::KeyCode::LeftCtrl;
	key_map[VK_RCONTROL] = input::KeyCode::RightCtrl;
	key_map[VK_LSHIFT] = input::KeyCode::LeftShift;
	key_map[VK_RSHIFT] = input::KeyCode::RightShift;
	key_map[VK_TAB] = input::KeyCode::Tab;
	key_map[VK_ESCAPE] = input::KeyCode::Esc;
	key_map[VK_BACK] = input::KeyCode::Backspace;
	key_map[VK_CAPITAL] = input::KeyCode::CapsLock;

	const char beg = static_cast<char>(input::KeyCode::A);
	for (char c = 'A'; c < 'Z'; ++c)
	{
		key_map[c] = static_cast<input::KeyCode>(beg + (c - 'A'));
	}

	for (char c = 'a'; c < 'z'; ++c)
	{
		key_map[c] = static_cast<input::KeyCode>(beg + (c - 'A'));
	}
}

void Win32_Window::process_events(input::InputListener* listener)
{
	static MSG msg;
	static input::MouseEvent mouse_event;
	while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			m_is_running = false;
		}

		POINT pos;
		pos.x = GET_X_LPARAM(msg.lParam);
		pos.y = GET_Y_LPARAM(msg.lParam);
		mouse_event.pos_x = pos.x;
		mouse_event.pos_y = pos.y;

		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
			mouse_event.type = input::MouseEventType::Pressed;
			mouse_event.button = input::MouseButton::Left;

			listener->on_mouse_event(mouse_event);
			break;
		case WM_RBUTTONDOWN:
			mouse_event.type = input::MouseEventType::Pressed;
			mouse_event.button = input::MouseButton::Right;

			listener->on_mouse_event(mouse_event);
			break;
		case WM_MBUTTONUP:
			mouse_event.type = input::MouseEventType::Pressed;
			mouse_event.button = input::MouseButton::Middle;

			listener->on_mouse_event(mouse_event);
			break;
		case WM_LBUTTONUP:
			mouse_event.type = input::MouseEventType::Released;
			mouse_event.button = input::MouseButton::Left;

			listener->on_mouse_event(mouse_event);
			break;
		case WM_RBUTTONUP:
			mouse_event.type = input::MouseEventType::Released;
			mouse_event.button = input::MouseButton::Right;

			listener->on_mouse_event(mouse_event);
			break;

		case WM_MOUSEMOVE:
			mouse_event.type = input::MouseEventType::Moved;

			listener->on_mouse_event(mouse_event);
			break;
		case WM_KEYDOWN:
			listener->key_pressed(key_map[msg.wParam], static_cast<wchar_t>(msg.wParam));
			break;
		case WM_KEYUP:
			listener->key_released(key_map[msg.wParam], static_cast<wchar_t>(msg.wParam));
			break;
		default:
			break;
		}

	}
}

void Win32_Window::swap_buffers()
{
	SwapBuffers(m_device_context);
}

bool Win32_Window::is_quit() const
{
	return !m_is_running;
}

