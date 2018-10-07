#include "window.hpp"

extern HINSTANCE g_instance;

class Win32_Window : public IWindow
{
public:
    Win32_Window();
    ~Win32_Window();

    virtual bool init( int width, int height, const char* const title );

    virtual void process_events( );

    virtual void swap_buffers( );

    virtual bool is_quit( ) const;

    static LRESULT CALLBACK WindowProc(
        HWND   hwnd,
        UINT   uMsg,
        WPARAM wParam,
        LPARAM lParam
    );

private:
    HWND m_handle;
    HINSTANCE m_instance;
    WNDCLASSA m_wnd_class;
    basic::String m_class_name;
    bool m_is_running;
};

IWindow* IWindow::create()
{
    return new Win32_Window();
}

LRESULT CALLBACK Win32_Window::WindowProc(
        HWND   handle,
        UINT   message,
        WPARAM wparam,
        LPARAM lparam
    )
{
    if( message == WM_DESTROY )
    {
        PostQuitMessage( 0 );
    }

    return DefWindowProc( handle, message, wparam, lparam );
}

Win32_Window::Win32_Window()
    :m_handle(nullptr)
    ,m_instance(nullptr)
    ,m_wnd_class()
    ,m_is_running(true)
{
}

Win32_Window::~Win32_Window()
{
}

bool Win32_Window::init( int width, int height, const char* const title )
{
    m_class_name = title;
    m_class_name += "_ClassName";
    m_instance = g_instance;

    m_wnd_class.style = CS_HREDRAW | CS_VREDRAW;
    m_wnd_class.lpfnWndProc = &Win32_Window::WindowProc;
    m_wnd_class.hInstance = m_instance;
    m_wnd_class.lpszClassName = m_class_name.get_cstr();

    RegisterClassA( &m_wnd_class );

    m_handle = CreateWindowA(
        m_class_name.get_cstr(),
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        nullptr,
        nullptr,
        m_instance,
        nullptr
    );

    return true;
}

void Win32_Window::process_events()
{
    static MSG msg;
    if( PeekMessageA( &msg, 0, 0, 0, PM_REMOVE ) != 0 )
    {
        TraslateMessageA( &msg );
        DispatchMessage( &msg );

        if( msg.message == WM_QUIT )
        {
            m_is_running = false;
        }
    }
}

void Win32_Window::swap_buffers()
{

}

bool Win32_Window::is_quit() const
{
    return !m_is_running;
}

