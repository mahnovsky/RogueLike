#include "window.hpp"
#include "basic/util_functions.hpp"

#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

// Get a matching FB config
static int visual_attribs[] = {GLX_X_RENDERABLE,
                               True,
                               GLX_DRAWABLE_TYPE,
                               GLX_WINDOW_BIT,
                               GLX_RENDER_TYPE,
                               GLX_RGBA_BIT,
                               GLX_X_VISUAL_TYPE,
                               GLX_TRUE_COLOR,
                               GLX_RED_SIZE,
                               8,
                               GLX_GREEN_SIZE,
                               8,
                               GLX_BLUE_SIZE,
                               8,
                               GLX_ALPHA_SIZE,
                               8,
                               GLX_DEPTH_SIZE,
                               24,
                               GLX_STENCIL_SIZE,
                               8,
                               GLX_DOUBLEBUFFER,
                               True,
                               // GLX_SAMPLE_BUFFERS  , 1,
                               // GLX_SAMPLES         , 4,
                               None};

typedef GLXContext ( *glXCreateContextAttribsARBProc )(
        Display*, GLXFBConfig, GLXContext, Bool, const int* );

static bool
isExtensionSupported( const char* extList, const char* extension )
{
    const char* start;
    const char *where, *terminator;

    /* Extension names should not have spaces. */
    where = strchr( extension, ' ' );
    if ( where || *extension == '\0' )
        return false;

    /* It takes a bit of care to be fool-proof about parsing the
       OpenGL extensions string. Don't be fooled by sub-strings,
       etc. */
    for ( start = extList;; )
    {
        where = strstr( start, extension );

        if ( !where )
            break;

        terminator = where + strlen( extension );

        if ( where == start || *( where - 1 ) == ' ' )
            if ( *terminator == ' ' || *terminator == '\0' )
                return true;

        start = terminator;
    }

    return false;
}

static bool ctxErrorOccurred = false;
static int
ctxErrorHandler( Display* dpy, XErrorEvent* ev )
{
    ctxErrorOccurred = true;
    return 0;
}

class X11Window : public IWindow
{
public:
    X11Window( )
        : m_running( true )
        , m_width( 0 )
        , m_height( 0 )
        , m_mouse_x( 0 )
        , m_mouse_y( 0 )
        , m_display( nullptr )
        , m_win( 0 )
        , m_cmap( 0 )
        , m_ctx( 0 )
    {
    }

    virtual ~X11Window( )
    {
        if ( m_display )
        {
            glXMakeContextCurrent( m_display, 0, 0, 0 );
            glXDestroyContext( m_display, m_ctx );
            XDestroyWindow( m_display, m_win );
            XFreeColormap( m_display, m_cmap );
            XCloseDisplay( m_display );
        }
    }

    bool init( int width, int height, const char* const title );

    void process_events( );

    void swap_buffers( );

    bool is_quit( ) const;

    bool get_mouse_pos( int& out_x, int& out_y );

private:
    bool m_running;

    int m_width;
    int m_height;

    int m_mouse_x;
    int m_mouse_y;

    Display* m_display;
    Window m_win;
    Colormap m_cmap;
    GLXContext m_ctx;
    Atom m_wm_delete_message;
};

IWindow*
IWindow::create( )
{
    return new X11Window( );
}

typedef GLXFBConfig* func_t(Display *, int, const int *, int *);
GLXFBConfig *glXChooseFBConfig1(Display *dpy, int screen, const int *attribList, int *nitems)
{
    static func_t *func;
    static const int attribs[] = {None};
    if (!func) {
         void *library = dlopen("libGL.so", RTLD_GLOBAL);
         func = (func_t *) dlsym(library, "glXChooseFBConfig");
    }
    if (!attribList)
        attribList = attribs;

    return ((*func)(dpy, screen, attribList, nitems));
}

bool
X11Window::init( int width, int height, const char* const title )
{
    m_width = width;
    m_height = height;

    m_display = XOpenDisplay( NULL );

    if ( m_display == NULL )
    {
        LOG( "\tcannot connect to X server" );
        return false;
    }

    m_wm_delete_message = XInternAtom( m_display, "WM_DELETE_WINDOW", False );

    int glx_major, glx_minor;

    // FBConfigs were added in GLX version 1.3.
    if ( !glXQueryVersion( m_display, &glx_major, &glx_minor )
         || ( ( glx_major == 1 ) && ( glx_minor < 3 ) )
         || ( glx_major < 1 ) )
    {
        LOG( "Invalid GLX version" );
        return false;
    }
    int visual_attribs[] = {0};
    int fbcount = 1;
    GLXFBConfig* fbc
            = glXChooseFBConfig( m_display, DefaultScreen( m_display ), visual_attribs, &fbcount );
    if ( !fbc )
    {
        LOG( "Failed to retrieve a framebuffer config" );
        return false;
    }
    LOG( "Found %d matching FB configs.", fbcount );

    // Pick the FB config/visual with the most samples per pixel
    // printf( "Getting XVisualInfos" );
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for ( i = 0; i < fbcount; ++i )
    {
        XVisualInfo* vi = glXGetVisualFromFBConfig( m_display, fbc[ i ] );
        if ( vi )
        {
            int samp_buf, samples;
            glXGetFBConfigAttrib( m_display, fbc[ i ], GLX_SAMPLE_BUFFERS, &samp_buf );
            glXGetFBConfigAttrib( m_display, fbc[ i ], GLX_SAMPLES, &samples );

            LOG( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                 " SAMPLES = %d",
                 i,
                 vi->visualid,
                 samp_buf,
                 samples );

            if ( ( best_fbc < 0 || samp_buf ) && ( samples > best_num_samp ) )
            {
                best_fbc = i;
                best_num_samp = samples;
            }
            if ( ( worst_fbc < 0 || !samp_buf ) || ( samples < worst_num_samp ) )
            {
                worst_fbc = i;
                worst_num_samp = samples;
            }
        }
        XFree( vi );
    }

    GLXFBConfig bestFbc = fbc[ best_fbc ];

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree( fbc );

    // Get a visual
    XVisualInfo* vi = glXGetVisualFromFBConfig( m_display, bestFbc );
    LOG( "Chosen visual ID = 0x%x", vi->visualid );

    LOG( "Creating colormap" );
    XSetWindowAttributes swa;

    swa.colormap = m_cmap = XCreateColormap(
            m_display, RootWindow( m_display, vi->screen ), vi->visual, AllocNone );
    swa.background_pixmap = None;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | StructureNotifyMask | SubstructureNotifyMask | ButtonPressMask
                     | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;

    LOG( "Creating window" );
    m_win = XCreateWindow( m_display,
                           RootWindow( m_display, vi->screen ),
                           0,
                           0,
                           width,
                           height,
                           0,
                           vi->depth,
                           InputOutput,
                           vi->visual,
                           CWBorderPixel | CWColormap | CWEventMask,
                           &swa );
    if ( !m_win )
    {
        LOG( "Failed to create window." );

        return false;
    }

    // Done with the visual info data
    XFree( vi );

    XStoreName( m_display, m_win, title );

    LOG( "Mapping window" );
    XMapWindow( m_display, m_win );

    XSetWMProtocols( m_display, m_win, &m_wm_delete_message, 1 );

    // Get the default screen's GLX extension list
    const char* glxExts = glXQueryExtensionsString( m_display, DefaultScreen( m_display ) );

    // NOTE: It is not necessary to create or make current to a context before
    // calling glXGetProcAddressARB
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
            (const GLubyte*)"glXCreateContextAttribsARB" );

    m_ctx = 0;

    // Install an X error handler so the application won't exit if GL 3.0
    // context allocation fails.
    //
    // Note this error handler is global.  All display connections in all threads
    // of a process use the same error handler, so be sure to guard against other
    // threads issuing X commands while this code is running.
    ctxErrorOccurred = false;
    int ( *oldHandler )( Display*, XErrorEvent* ) = XSetErrorHandler( &ctxErrorHandler );

    // Check for the GLX_ARB_create_context extension string and the function.
    // If either is not present, use GLX 1.3 context creation method.
    if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB )
    {
        LOG( "glXCreateContextAttribsARB() not found" );

        return false;
    }
    else
    {
        int context_attribs[]
                = {GLX_CONTEXT_MAJOR_VERSION_ARB,
                   3,
                   GLX_CONTEXT_MINOR_VERSION_ARB,
                   3,
                   // GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                   None};

        LOG( "Creating context" );
        m_ctx = glXCreateContextAttribsARB( m_display, bestFbc, 0, True, context_attribs );

        // Sync to ensure any errors generated are processed.
        XSync( m_display, False );
        if ( !ctxErrorOccurred && m_ctx )
        {
            LOG( "Created GL 3.3 context" );
        }
        else
        {
            LOG( "Failed to create GL 3.0 context" );

            return false;
        }
    }

    // Sync to ensure any errors generated are processed.
    XSync( m_display, False );

    // Restore the original error handler
    XSetErrorHandler( oldHandler );

    if ( ctxErrorOccurred || !m_ctx )
    {
        LOG( "Failed to create an OpenGL context" );
        
        return false;
    }

    // Verifying that context is a direct context
    if ( !glXIsDirect( m_display, m_ctx ) )
    {
        LOG( "Indirect GLX rendring context obtained" );
    }
    else
    {
        LOG( "Direct GLX rendering context obtained" );
    }

    LOG( "Making context current" );

    glXMakeCurrent( m_display, m_win, m_ctx );

    return true;
}

void
X11Window::process_events( )
{
    XEvent event;
    XNextEvent( m_display, &event );

    if ( !m_running )
    {
        return;
    }

    switch ( event.type )
    {
    case ClientMessage:
    {
        if ( event.xclient.data.l[ 0 ] == m_wm_delete_message )
        {
            m_running = false;
        }
    }
    break;
    case KeyPress:
    {
    }
    break;
    case MotionNotify:
    {
        m_mouse_x = event.xmotion.x;
        m_mouse_y = event.xmotion.y;
    }
    break;
    case ButtonPress:
    {
    }
    break;
    case ButtonRelease:
    {
    }
    break;
    default:
        break;
    }
}

void
X11Window::swap_buffers( )
{
    glXSwapBuffers( m_display, m_win );
}

bool
X11Window::is_quit( ) const
{
    return !m_running;
}

bool
X11Window::get_mouse_pos( int& out_x, int& out_y )
{
    out_x = m_mouse_x;
    out_y = m_mouse_y;

    return true;
}
