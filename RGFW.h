/*
* Copyright (C) 2023 ColleagueRiley
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
*
*/

/*
	Credits :
		EimaMei/Sacode : Much of the code for creating windows using winapi, Wrote the Silicon library, helped with MacOS Support

		stb - This project is heavily inspired by the stb single header files

		GLFW:
			certain parts of winapi and X11 are very poorly documented,
			GLFW's source code was referenced and used throughout the project (used code is marked in some way),
			this mainly includes, code for drag and drops, code for setting the icon to a bitmap and the code for managing the clipboard for X11 (as these parts are not documented very well)

			GLFW Copyright, https::/github.com/GLFW/GLFW

			Copyright (c) 2002-2006 Marcus Geelnard
			Copyright (c) 2006-2019 Camilla Löwy
*/

/*
	#define RGFW_IMPLEMENTATION - (semi-option) makes it so source code is included
	#define RGFW_PRINT_ERRORS - (optional) makes it so RGFW prints errors when they're found
	#define RGFW_OSMESA - (optional) use OSmesa as backend (instead of system's opengl api + regular opengl)
	#define RGFW_EGL - (optional) use EGL for loading an OpenGL context (instead of the system's opengl api)
	#define RGFW_OPENGL_ES - (optional) use EGL to load and use Opengl ES for backend rendering (instead of the system's opengl api)
	#define VULKAN - (optional) use vulkan for the rendering backend (rather than opengl)

	#define RGFW_LINK_EGL (optional) (windows only) if EGL is being used, if EGL functions should be defined dymanically (using GetProcAddress)
	#define RGFW_LINK_OSMESA (optional) (windows only) if EGL is being used, if OS Mesa functions should be defined dymanically  (using GetProcAddress)

	#define RGFW_X11 (optional) (unix only) if X11 should be used. This option is turned on by default by unix systems except for MacOS
	#define RGFW_WGL_LOAD (optional) (windows only) if WGL should be loaded dynamically during runtime
*/

#if defined(__unix__) && !defined(__APPLE__) && !defined(RGFW_X11)
#define RGFW_X11
#endif

#if defined(__APPLE__) && defined(RGFW_X11)
#define RGFW_MACOS_X11
#endif

#if defined(RGFW_OPENGL_ES) && !defined(RGFW_EGL)
#define RGFW_EGL
#endif
#if defined(RGFW_EGL) && defined(__APPLE__)
#warning  EGL is not supported for Cocoa, switching back to the native opengl api
#undef RGFW_EGL
#endif

#if !defined(RGFW_OSMESA) && !defined(RGFW_EGL) && !defined(RGFW_GL) && !defined (RGFW_VULKAN)
#define RGFW_GL
#endif

/*! Optional arguments for making a windows */
#define RGFW_TRANSPARENT_WINDOW		(1L<<9) /*!< If the window is transparent*/
#define RGFW_NO_BORDER		(1L<<3) /*!< If the window doesn't have border*/
#define RGFW_NO_RESIZE		(1L<<4) /*!< If the window cannot be resized  by the user*/
#define RGFW_ALLOW_DND     (1L<<5) /*!< if the window supports drag and drop*/
#define RGFW_HIDE_MOUSE (1L<<6) /* if the window should hide the mouse or not (can be toggled later on)*/
#define RGFW_OPENGL (1L<<7) /* use normal opengl (if another version is also selected) */
#define RGFW_FULLSCREEN (1L<<8) /* if the window should be fullscreen by default or not */
/*! event codes */
#define RGFW_keyPressed 2 /*!< a key has been pressed*/
#define RGFW_keyReleased 3 /*!< a key has been released*/
#define RGFW_mouseButtonPressed 4 /*!< a mouse button has been pressed (left,middle,right)*/
#define RGFW_mouseButtonReleased 5 /*!< a mouse button has been released (left,middle,right)*/
#define RGFW_mousePosChanged 6 /*!< the position of the mouse has been changed*/
#define RGFW_jsButtonPressed 7 /*!< a joystick button was pressed */
#define RGFW_jsButtonReleased 8 /*!< a joystick button was released */
#define RGFW_jsAxisMove 9 /*!< an axis of a joystick was moved*/
#define RGFW_quit 33 /*!< the user clicked the quit button*/
#define RGFW_dnd 34 /*!< a file has been dropped into the window*/

/*! mouse button codes */
#define RGFW_mouseLeft  1 /*!< left mouse button is pressed*/
#define RGFW_mouseMiddle  2 /*!< mouse-wheel-button is pressed*/
#define RGFW_mouseRight  3 /*!< right mouse button is pressed*/
#define RGFW_mouseScrollUp  4 /*!< mouse wheel is scrolling up*/
#define RGFW_mouseScrollDown  5 /*!< mouse wheel is scrolling down*/

/*! joystick button codes (based on xbox/playstation), you may need to change these values per controller */
unsigned char RGFW_JS_A = 0; /* or PS X button */
unsigned char RGFW_JS_B = 1; /* or PS circle button */
unsigned char RGFW_JS_Y = 2; /* or PS triangle button */
unsigned char RGFW_JS_X = 3; /* or PS square button */
unsigned char RGFW_JS_START = 9; /* start button */
unsigned char RGFW_JS_SELECT = 8; /* select button */
unsigned char RGFW_JS_HOME = 10; /* home button */
unsigned char RGFW_JS_UP = 13; /* dpad up */
unsigned char RGFW_JS_DOWN = 14; /* dpad down*/
unsigned char RGFW_JS_LEFT = 15; /* dpad left */
unsigned char RGFW_JS_RIGHT = 16; /* dpad right */
unsigned char RGFW_JS_L1 = 4; /* left bump */
unsigned char RGFW_JS_L2 = 5; /* left trigger*/
unsigned char RGFW_JS_R1 = 6; /* right bumper */
unsigned char RGFW_JS_R2 = 7; /* right trigger */

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE: some parts of the data can represent different things based on the event (read comments in RGFW_Event struct) */
typedef struct RGFW_Event {
    unsigned char type; /*!< which event has been sen t?*/
    unsigned char button; /*!< which mouse button has been clicked (0) left (1) middle (2) right OR which joystick button was pressed OR which joystick axis was moved*/
    int x, y; /*!< mouse x, y of event */

    unsigned char ledState; /*!< 0 : numlock, 1 : caps lock, 3 : small lock*/

    unsigned keyCode; /*!< keycode of event*/

	#ifdef _WIN32
	char keyName[16]; /* key name of event*/
	#else
	char* keyName; /*!< key name of event */
	#endif

	/*! drag and drop data */
	int droppedFilesCount; /*!< house many files were dropped */
    char** droppedFiles; /*!< dropped files*/

	/*! joystick*/
	unsigned short joystick; /* which joystick this event applies to (if applicable to any) */
	unsigned char axisesCount; /* number of axises */
	char axis[4][2]; /* x, y of axises (-100 to 100) */
} RGFW_Event; /*!< Event structure for checking/getting events */

typedef struct RGFW_window {
    void* display; /*!< source display */
    void* window; /*!< source window */
    void* glWin; /*!< source opengl context */

	char* name; /*!< window's name*/
	int x, y, w, h; /*!< window size, x, y*/

	unsigned int fpsCap; /*!< the fps cap of the window should run at (change this var to change the fps cap, 0 = no limit)*/
	/*[the fps is capped when events are checked]*/

	int srcX, srcY, srcW, srcH; /* source size (for resizing, do not change these values directly) */
	char* srcName; /*!< source name, for chaning the name (do not change these values directly) */

	unsigned int fps, /*the current fps of the window [the fps is checked when events are checked]*/
				inFocus; /*if the window is in focus or not*/

	unsigned char valid; /* the final net for checking if a window is*/

	unsigned char dnd; /*!< if dnd is enabled or on (based on window creating args) */

	void* cursor;
	unsigned char cursorChanged;

	unsigned short joystickCount;
	int joysticks[4]; /* limit of 4 joysticks at a time */
	unsigned char jsPressed[4][16]; /* if a key is currently pressed or not (per joystick) */

	#ifdef RGFW_OSMESA
	unsigned char* buffer; /*OSMesa buffer*/
	unsigned char render; /* if OSMesa should render on the screen or not (set by window args by default but it can be changed in runtime if you want) */
	#endif

	#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
	void* view; /*apple viewpoint thingy*/
	unsigned int hideMouse; /*if the mouse is hidden or not*/
	#endif

	#ifdef RGFW_EGL
	void* EGL_surface;
	void* EGL_display;
	#endif

	RGFW_Event event; /*!< current event */
} RGFW_window; /*!< Window structure for managing the window */

typedef unsigned long RGFW_thread; /*thread type (to be defined per (unix/windows))*/

RGFW_window* RGFW_createWindowPointer(
	char* name, /* name of the window */
	int x, /*!< x */
	int y,  /*!< y */
	int width, /*!< width */
	int height, /*!< height */
	unsigned long args /* extra arguments (NULL / (unsigned long)0 means no args used)*/
); /*!< function to create a window struct (pointer)*/

/*! create window object (non-pointer) */
#define RGFW_createWindow(name, x, y, w, h, args)  *RGFW_createWindowPointer(name, x, y, w, h, args);

/*! initializes a vulkan rendering context for the RGFW window, you still need to load your own vulkan instance, ect, ect
	this outputs the vulkan surface into win->glWin
	RGFW_VULKAN must be defined for this function to be defined

*/
void RGFW_initVulkan(RGFW_window* win, void* inst);
/* returns how big the screen is (for fullscreen support, ect, ect)
   [0] = width
   [1] = height
*/
unsigned int* RGFW_getScreenSize(RGFW_window* win);

RGFW_Event* RGFW_checkEvents(RGFW_window* win); /*!< check events (returns a pointer to win->event or NULL if there is no event)*/

/*! window managment functions*/
void RGFW_closeWindow(RGFW_window* win); /*!< close the window and free leftover data */

void RGFW_setIcon(RGFW_window* win, /*!< source window */
				 unsigned char* icon /*!< icon bitmap */,
				 int width /*!< width of the bitmap*/,
				 int height, /*!< height of the bitmap*/
				 int channels /*!< how many channels the bitmap has (rgb : 3, rgba : 4) */
			); /*!< image resized by default */

void RGFW_defaultIcon(RGFW_window* win); /* sets the mouse to the default mouse image */

void RGFW_setMouse(RGFW_window* win, unsigned char* image, int width, int height, int channels); /*!< sets mouse to bitmap (very simular to RGFW_setIcon)*/
/*!< image NOT resized by default */
void RGFW_setMouseDefault(RGFW_window* win); /* sets the mouse to the default mouse image */

/* where the mouse is on the screen, x = [0], y = [1] */
int* RGFW_getGlobalMousePoint(RGFW_window* win);

#ifdef __APPLE__
void RGFW_hideMouse(RGFW_window* win);
#else
unsigned char blank[] = {0, 0, 0, 0};
#define RGFW_hideMouse(win) RGFW_setMouse(win, blank, 1, 1, 4);
#endif

void RGFW_makeCurrent(RGFW_window* win); /*!< make the window the current opengl drawing context */

/*error handling*/
unsigned char RGFW_Error(); /* returns true if an error has occurred (doesn't print errors itself) */

/*!< if window == NULL, it checks if the key is pressed globally. Otherwise, it checks only if the key is pressed while the window in focus.*/
unsigned char RGFW_isPressedI(RGFW_window* win, unsigned int key); /*!< if key is pressed (key code)*/

/*
	!!Keycodes defined at the bottom of the header file!!
*/
unsigned int RGFW_keyStrToKeyCode(char* key); /*!< converts a string of a key to it's key code */
#define RGFW_isPressedS(win, key) RGFW_isPressedI(win, RGFW_keyStrToKeyCode(key)) /*!< if key is pressed (key string) */

/*! clipboard functions*/
char* RGFW_readClipboard(RGFW_window* win); /*!< read clipboard data */
void RGFW_writeClipboard(RGFW_window* win, char* text); /*!< write text to the clipboard */

/*! threading functions*/

/*! NOTE! (for X11/linux) : if you define a window in a thread, it must be run after the original thread's window is created or else there will be a memory error */
RGFW_thread RGFW_createThread(void* (*function_ptr)(void*), void* args); /*!< create a thread*/
void RGFW_cancelThread(RGFW_thread thread); /*!< cancels a thread*/
void RGFW_joinThread(RGFW_thread thread); /*!< join thread to current thread */
void RGFW_setThreadPriority(RGFW_thread thread, unsigned char priority); /*!< sets the priority priority  */

/*! gamepad/joystick functions */

/*! joystick count starts at 0*/
unsigned short RGFW_registerJoystick(RGFW_window* win, int jsNumber); /*!< register joystick to window based on a number (the number is based on when it was connected eg. /dev/js0)*/
unsigned short RGFW_registerJoystickF(RGFW_window* win, char* file);

unsigned char RGFW_isPressedJS(RGFW_window* win, unsigned short controller, unsigned char button);

/*! Set OpenGL version hint */
void RGFW_setGLVersion(int major, int minor);

/*! native opengl functions */
static void* RGFW_getProcAddress(const char* procname); /* get native proc address */
void RGFW_swapBuffers(RGFW_window* win); /* swap the opengl buffer */
void RGFW_swapInterval(RGFW_window* win, int swapInterval);

/*! Supporting functions */
void RGFW_checkFPS(RGFW_window* win); /*!< updates fps / sets fps to cap (ran by RGFW_checkEvents)*/
unsigned char RGFW_ValidWindowCheck(RGFW_window* win, char* event); /*!< returns true if the window is valid (and prints an error and where it took place if it can)*/

/*
(Notes on how to manage Silicon (macos) included)

Example to get you started :

linux : gcc main.c -lX11 -lXcursor -lGL
windows : gcc main.c -lopengl32 -lshell32 -lgdi32
macos:
	<Silicon> can be replaced to where you have the Silicon headers stored
	<libSilicon.a> can be replaced to wherever you have libSilicon.a
	clang main.c -I<Silicon> <libSilicon.a> -framework Foundation -framework AppKit -framework OpenGL -framework CoreVideo

	NOTE(EimaMei): If you want the MacOS experience to be fully single header, then I'd be best to install Silicon (after compiling)
	by going to the `Silicon` folder and running `make install`. After this you can easily include Silicon via `#include <Silicon/silicon.h>'
	and link it by doing `-lSilicon`

#define RGFW_IMPLEMENTATION
#include "RGFW.h"

unsigned char icon[4 * 3 * 3] = {0xFF, 0x00, 0x00, 0xFF,    0xFF, 0x00, 0x00, 0xFF,     0xFF, 0x00, 0x00, 0xFF,   0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,     0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};

int main() {
    RGFW_window* w = RGFW_createWindowPointer("name", 500, 500, 500, 500, (unsigned long)0);

    RGFW_setIcon(win, icon, 3, 3, 4);

    for (;;) {
        RGFW_checkEvents(win);
        RGFW_swapBuffers(win);

		glClearColor(0xFF, 0XFF, 0xFF, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);

        if (win->event.type == RGFW_quit || RGFW_isPressedS(win, (char*)"Escape"))
            break;
    }

    RGFW_closeWindow(win);
}

	compiling :

	if you wish to compile the library all you have to do is create a new file with this in it

	rgfw.c
	#define RGFW_IMPLEMENTATION
	#include "RGFW.h"

	then you can use gcc (or whatever compile you wish to use) to compile the library into object file

	ex. gcc -c RGFW.c -fPIC

	after you compile the library into an object file, you can also turn the object file into an static or shared library

	(commands ar and gcc can be replaced with whatever equivalent your system uses)
	static : ar rcs RGFW.a RGFW.o
	shared :
		windows:
			gcc -shared RGFW.o -lopengl32 -lshell32 -lgdi32 -o RGFW.dll
		linux:
			gcc -shared RGFW.o -lX11 -lXcursor -lGL -o RGFW.so
		macos:
			<Silicon/include> can be replaced to where you have the Silicon headers stored
			<libSilicon.a> can be replaced to wherever you have libSilicon.a
			gcc -shared RGFW.o -framework Foundation <libSilicon.a> -framework AppKit -framework OpenGL -framework CoreVideo -I<Silicon/include>

	installing/building silicon (macos)

	Silicon does not need to be installde per se.
	I personally recommended that you use the Silicon included using RGFW

	to build this version of Silicon simplly run

	cd Silicon && make

	you can then use Silicon/include and libSilicon.a for building RGFW projects

	ex.
	gcc main.c -framework Foundation -lSilicon -framework AppKit -framework OpenGL -framework CoreVideo -ISilicon/include

	I also suggest you compile Silicon (and RGFW if applicable)
	per each time you compile your application so you know that everything is compiled for the same architecture.
*/

#ifdef RGFW_IMPLEMENTATION

#ifdef RGFW_PRINT_ERRORS
#include <stdio.h>
#endif

#include <time.h> /* time header (for  and drop functions / other functions that need time info)*/
#include <math.h>
#include <string.h> /* for strcmp */

#ifdef RGFW_OSMESA
#ifndef __APPLE__
#include <GL/osmesa.h>
#else
#include <OpenGL/osmesa.h>
#endif
#endif

#ifdef RGFW_EGL
#include <EGL/egl.h>
#include <GL/gl.h>
#endif

#ifdef RGFW_VULKAN
#ifdef RGFW_X11
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifdef __APPLE__
#define VK_USE_PLATFORM_MACOS_MVK
#endif

#include <vulkan/vulkan.h>

void RGFW_initVulkan(RGFW_window* win, void* inst) {
	#ifdef RGFW_X11
	VkXlibSurfaceCreateInfoKHR x11 = { VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR, 0, win->display, win->window };

	vkCreateXlibSurfaceKHR(inst, &x11, NULL, win->glWin);
	#endif
	#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR win32 = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, 0, win->display, win->window };

	vkCreateWin32SurfaceKHR(inst, &win32, NULL, win->glWin);
	#endif
	#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
	VkMacOSSurfaceCreateFlagsMVK macos = { VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_KHR, 0, win->display, win->window };

	vkCreateMacOSSurfaceMVK(inst, &macos, NULL, win->glWin);
	#endif
}

#endif

#ifdef RGFW_X11
#include <X11/Xlib.h>
#include <dlfcn.h>
#endif
#ifdef _WIN32

#include <windows.h>
#ifdef RGFW_GL
static void* RGFW_getProcAddress(const char* procname) { return (void*)wglGetProcAddress(procname); }
#endif
#endif
#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
const char* RGFW_keyStrings[128] = {"a", "s", "d", "f", "h", "g", "z", "x", "c", "v", "0", "b", "q", "w", "e", "r", "y", "t", "1", "2", "3", "4", "6", "5", "Equals", "9", "7", "Minus", "8", "0", "CloseBracket", "o", "u", "Bracket", "i", "p", "Return", "l", "j", "Apostrophe", "k", "Semicolon", "BackSlash", "Comma", "Slash", "n", "m", "Period", "Tab", "Space", "Backtick", "BackSpace", "0", "Escape", "0", "Super", "Shift", "CapsLock", "Alt", "Control", "0", "0", "0", "0", "0", "KP_Period", "0", "KP_Minus", "0", "0", "0", "0", "Numlock", "0", "0", "0", "KP_Multiply", "KP_Return", "0", "0", "0", "0", "KP_Slash", "KP_0", "KP_1", "KP_2", "KP_3", "KP_4", "KP_5", "KP_6", "KP_7", "0", "KP_8", "KP_9", "0", "0", "0", "F5", "F6", "F7", "F3", "F8", "F9", "0", "F11", "0", "F13", "0", "F14", "0", "F10", "0", "F12", "0", "F15", "Insert", "Home", "PageUp", "Delete", "F4", "End", "F2", "PageDown", "Left", "Right", "Down", "Up", "F1"};
unsigned char RGFW_keyMap[128] = { 0 };
#endif

unsigned int RGFW_keyStrToKeyCode(char* key) {
#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
	unsigned char i;
	for (i = 0; i < 128; i++)
		if (!strcmp(RGFW_keyStrings[i], key))
			return i;
	printf("%i\n", i);
#endif
#ifdef RGFW_X11
	if (key == "Space") key = (char*)"space";

    return XStringToKeysym(key);
#endif
#ifdef _WIN32
	if (sizeof(key)/sizeof(char) > 1) {
		if (!strcmp(key, "Super_L")) return VK_LWIN;
		else if (!strcmp(key, "Super_R")) return VK_RWIN;
		else if (!strcmp(key, "Space")) return VK_SPACE;
		else if (!strcmp(key, "Return")) return VK_RETURN;
		else if (!strcmp(key, "Caps_Lock")) return VK_CAPITAL;
		else if (!strcmp(key, "Tab")) return VK_TAB;
		else if (!strcmp(key, "Right")) return VK_RIGHT;
		else if (!strcmp(key, "Left")) return VK_LEFT;
		else if (!strcmp(key, "Up")) return VK_UP;
		else if (!strcmp(key, "Down")) return VK_DOWN;
		else if (!strcmp(key, "ShiftL")) return VK_LSHIFT;
		else if (!strcmp(key, "ShiftR")) return VK_RSHIFT;
		else if (!strcmp(key, "SuperL")) return VK_LSHIFT;
		else if (!strcmp(key, "SuperR")) return VK_RSHIFT;
		else if (!strcmp(key, "ControlL")) return VK_LSHIFT;
		else if (!strcmp(key, "ControlR")) return VK_RSHIFT;
	}

	int vKey = VkKeyScan(key[0]);

    return vKey;
#endif

	return 0;
}

#ifndef M_PI
#define M_PI		3.14159265358979323846	/* pi */
#endif

typedef struct RGFW_Timespec {
	time_t tv_sec;		/* Seconds.  */
	unsigned int tv_nsec;	/* Nanoseconds.  */
} RGFW_Timespec; /*time struct for fps functions*/

unsigned char RGFW_isPressedJS(RGFW_window* win, unsigned short c, unsigned char button) { return win->jsPressed[c][button]; }

unsigned char RGFW_error = 0;

int RGFW_majorVersion, RGFW_minorVersion;

void RGFW_setGLVersion(int major, int minor) {
	RGFW_majorVersion = major; 
	RGFW_minorVersion = minor;
}

#ifdef RGFW_EGL

#if defined(RGFW_LINK_EGL)
typedef EGLBoolean (EGLAPIENTRY * PFN_eglInitialize)(EGLDisplay,EGLint*,EGLint*);

PFNEGLINITIALIZEPROC eglInitializeSource;
PFNEGLGETCONFIGSPROC eglGetConfigsSource;
PFNEGLCHOOSECONFIGPROC eglChooseConfigSource;
PFNEGLCREATEWINDOWSURFACEPROC eglCreateWindowSurfaceSource;
PFNEGLCREATECONTEXTPROC eglCreateContextSource;
PFNEGLMAKECURRENTPROC eglMakeCurrentSource;
PFNEGLGETDISPLAYPROC eglGetDisplaySource;
PFNEGLSWAPBUFFERSPROC eglSwapBuffersSource;
PFNEGLSWAPINTERVALPROC eglSwapIntervalSource;
PFNEGLBINDAPIPROC eglBindAPISource;
PFNEGLDESTROYCONTEXTPROC eglDestroyContextSource;
PFNEGLTERMINATEPROC eglTerminateSource;
PFNEGLDESTROYSURFACEPROC eglDestroySurfaceSource;

#define eglInitialize eglInitializeSource
#define eglGetConfigs eglGetConfigsSource
#define eglChooseConfig eglChooseConfigSource
#define eglCreateWindowSurface eglCreateWindowSurfaceSource
#define eglCreateContext eglCreateContextSource
#define eglMakeCurrent eglMakeCurrentSource
#define eglGetDisplay eglGetDisplaySource
#define eglSwapBuffers eglSwapBuffersSource
#define eglSwapInterval eglSwapIntervalSource
#define eglBindAPI eglBindAPISource
#define eglDestroyContext eglDestroyContextSource
#define eglTerminate eglTerminateSource
#define eglDestroySurface eglDestroySurfaceSource;
#endif


#define EGL_CONTEXT_MAJOR_VERSION_KHR 0x3098
#define EGL_CONTEXT_MINOR_VERSION_KHR 0x30fb

void RGFW_createOpenGLContext(RGFW_window* win) {
	#if defined(RGFW_LINK_EGL)
    eglInitializeSource = (PFNEGLINITIALIZEPROC)eglGetProcAddress("eglInitialize");
    eglGetConfigsSource = (PFNEGLGETCONFIGSPROC)eglGetProcAddress("eglGetConfigs");
    eglChooseConfigSource = (PFNEGLCHOOSECONFIGPROC) eglGetProcAddress("eglChooseConfig");
    eglCreateWindowSurfaceSource = (PFNEGLCREATEWINDOWSURFACEPROC) eglGetProcAddress( "eglCreateWindowSurface");
    eglCreateContextSource = (PFNEGLCREATECONTEXTPROC) eglGetProcAddress("eglCreateContext");
    eglMakeCurrentSource = (PFNEGLMAKECURRENTPROC) eglGetProcAddress("eglMakeCurrent");
    eglGetDisplaySource = (PFNEGLGETDISPLAYPROC) eglGetProcAddress("eglGetDisplay");
	eglSwapBuffersSource = (PFNEGLSWAPBUFFERSPROC)  eglGetProcAddress("eglSwapBuffers");
	eglSwapIntervalSource = (PFNEGLSWAPINTERVALPROC)  eglGetProcAddress("eglSwapInterval");
	eglBindAPISource = (PFNEGLBINDAPIPROC)  eglGetProcAddress("eglBindAPI");
	eglDestroyContextSource = (PFNEGLDESTROYCONTEXTPROC)  eglGetProcAddress("eglDestroyContext");
	eglTerminateSource = (PFNEGLTERMINATEPROC)  eglGetProcAddress("eglTerminate");
	eglDestroySurfaceSource = (PFNEGLDESTROYSURFACEPROC)  eglGetProcAddress("eglDestroySurface");
	#endif

    win->EGL_display = eglGetDisplay((EGLNativeDisplayType)win->display);

    EGLint major, minor;
	
    eglInitialize(win->EGL_display, &major, &minor);

    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,
		#ifdef RGFW_OPENGL_ES
		EGL_OPENGL_ES2_BIT,
		#else
		EGL_OPENGL_BIT,
		#endif
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs;
    eglChooseConfig(win->EGL_display, config_attribs, &config, 1, &num_configs);

	#ifdef RGFW_OPENGL_ES
    eglBindAPI(EGL_OPENGL_ES_API);
	#else
	eglBindAPI(EGL_OPENGL_API);
	#endif
  
	win->glWin = eglCreateContext(win->EGL_display, config, EGL_NO_CONTEXT, NULL);
    win->EGL_surface = eglCreateWindowSurface(win->EGL_display, config, (EGLNativeWindowType)win->window, NULL);

    eglMakeCurrent(win->EGL_display, win->EGL_surface, win->EGL_surface, win->glWin);
	eglSwapBuffers(win->EGL_display, win->EGL_surface);

	eglSwapInterval(win->EGL_display, 1);
}

void* RGFW_getProcAddress(const char* procname) { return (void*)eglGetProcAddress(procname); }

void RGFW_closeEGL(RGFW_window* win) {
    eglDestroySurface(win->EGL_display, win->EGL_surface);
    eglDestroyContext(win->EGL_display, win->glWin);

    eglTerminate(win->EGL_display);
}

#endif

unsigned char RGFW_ValidWindowCheck(RGFW_window* win, char* event) {
	/*
		if this part gives you a seg fault, there is a good chance your window is not valid

		Make sure you're creaing the window and using the window structure

		accidently writing (RGFW_window type)->window is a common mistake too
	*/

	if (win->valid != 245 || win == (RGFW_window*)0
		#ifdef _WIN32
		|| !IsWindow((HWND)win->display)
		#endif
		#ifdef RGFW_X11
		|| XConnectionNumber((Display*)win->display) == -1 || win->window == (Window)0
		#endif
	) {
		#ifdef RGFW_PRINT_ERRORS
		printf("Error %s : invalid window structure \n", event);
		RGFW_error = 1;
		return 0;
		#endif
	}

	return 1;
}

#ifdef RGFW_X11

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

#ifdef RGFW_GL

#ifndef GLX_MESA_swap_control
#define  GLX_MESA_swap_control
#endif
#include <GL/glx.h> /* GLX defs, xlib.h, gl.h */
#endif
#include <X11/XKBlib.h> /* for converting keycode to string */
#include <X11/cursorfont.h> /* for hiding */
#include <stdlib.h> /* realloc, malloc and other memory functions */

#include <limits.h> /* for data limits (mainly used in drag and drop functions) */
#include <string.h> /* strlen and other char* managing functions */
#include <fcntl.h>
#include <assert.h>

#ifdef __linux__
#include <linux/joystick.h>
#endif

/*atoms needed for drag and drop*/
Atom XdndAware, XdndTypeList,     XdndSelection,    XdndEnter,        XdndPosition,     XdndStatus,       XdndLeave,        XdndDrop,         XdndFinished,     XdndActionCopy,   XdndActionMove,   XdndActionLink,   XdndActionAsk, XdndActionPrivate;

#ifdef RGFW_OSMESA
XImage* RGFW_omesa_ximage;
#endif
#ifndef RGFW_NO_X11_CURSOR
typedef XcursorImage* (* PFN_XcursorImageCreate)(int,int);
typedef void (* PFN_XcursorImageDestroy)(XcursorImage*);
typedef Cursor (* PFN_XcursorImageLoadCursor)(Display*,const XcursorImage*);
#endif
#ifdef RGFW_GL
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
#endif

#ifndef RGFW_NO_X11_CURSOR
PFN_XcursorImageLoadCursor XcursorImageLoadCursorSrc = NULL;
PFN_XcursorImageCreate XcursorImageCreateSrc = NULL;
PFN_XcursorImageDestroy XcursorImageDestroySrc = NULL;

#define XcursorImageLoadCursor XcursorImageLoadCursorSrc
#define XcursorImageCreate XcursorImageCreateSrc
#define XcursorImageDestroy XcursorImageDestroySrc

void* X11Cursorhandle = NULL;
#endif

unsigned int RGFW_windowsOpen = 0;

#ifdef RGFW_GL
static void* RGFW_getProcAddress(const char* procname) { return (void*)glXGetProcAddress((GLubyte*)procname); }

#define SET_ATTRIB(a, v) { \
    assert(((size_t) index + 1) < sizeof(attribs) / sizeof(attribs[0])); \
    attribs[index++] = a; \
    attribs[index++] = v; \
}
#endif

RGFW_window* RGFW_createWindowPointer(char* name, int x, int y, int w, int h, unsigned long args) {
	#ifndef RGFW_NO_X11_CURSOR
	if (X11Cursorhandle == NULL) {
		#if defined(__CYGWIN__)
			X11Cursorhandle = dlopen("libXcursor-1.so", RTLD_LAZY | RTLD_LOCAL);
		#elif defined(__OpenBSD__) || defined(__NetBSD__)
			X11Cursorhandle = dlopen("libXcursor.so", RTLD_LAZY | RTLD_LOCAL);
		#else
			X11Cursorhandle = dlopen("libXcursor.so.1", RTLD_LAZY | RTLD_LOCAL);
		#endif

		XcursorImageCreateSrc = (PFN_XcursorImageCreate)dlsym(X11Cursorhandle, "XcursorImageCreate");
		XcursorImageDestroySrc = (PFN_XcursorImageDestroy)dlsym(X11Cursorhandle, "XcursorImageDestroy");
        XcursorImageLoadCursorSrc = (PFN_XcursorImageLoadCursor)dlsym(X11Cursorhandle, "XcursorImageLoadCursor");
	}
	#endif

	RGFW_window* win = (RGFW_window*)malloc(sizeof(RGFW_window)); /* make a new RGFW struct */

    XInitThreads(); /* init X11 threading*/

    /* init the display*/
	win->display = XOpenDisplay(0);
	if (RGFW_FULLSCREEN & args) {
		unsigned int* r = RGFW_getScreenSize(win);
		x = 0;
		y = 0;
		w = r[0];
		h = r[1];
	}

	/* set and init the new window's data */

	win->srcX = win->x = x;
	win->srcY = win->y = y;
	win->srcW = win->w = w;
	win->srcH = win->h = h;

	win->srcName = win->name = name;
	win->fpsCap = 0;
	win->inFocus = 1;
	win->event.droppedFilesCount = 0;
	win->joystickCount = 0;
	win->dnd = 0;
	win->cursor = NULL;
	win->cursorChanged = 0;
	win->valid = 245;

	if ((Display *)win->display == NULL)
		return win;

    long event_mask =  KeyPressMask | KeyReleaseMask  | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | FocusChangeMask; /* X11 events accepted*/

	#ifdef RGFW_GL
	#if defined(RGFW_OSMESA) || defined(RGFW_EGL)
	if (RGFW_OPENGL & args)
	#endif
	{
		static int visual_attribs[] = {   GLX_X_RENDERABLE    , True,   GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,  GLX_RENDER_TYPE     , GLX_RGBA_BIT,   GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,   GLX_RED_SIZE        , 8,   GLX_GREEN_SIZE      , 8,   GLX_BLUE_SIZE       , 8,   GLX_ALPHA_SIZE      , 8,   GLX_DEPTH_SIZE      , 24,   GLX_STENCIL_SIZE    , 8,   GLX_DOUBLEBUFFER    , True,    None   };
		
		int fbcount;
		GLXFBConfig* fbc = glXChooseFBConfig((Display*)win->display, DefaultScreen(win->display), visual_attribs, &fbcount);

		int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

		int i;
		for (i = 0; i < fbcount; i++) {
			XVisualInfo *vi = glXGetVisualFromFBConfig((Display*)win->display, fbc[i]);
			if (vi) {
				int samp_buf, samples;
				glXGetFBConfigAttrib((Display*)win->display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
				glXGetFBConfigAttrib((Display*)win->display, fbc[i], GLX_SAMPLES, &samples );

				if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
					best_fbc = i, best_num_samp = samples;
				if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
					worst_fbc = i, worst_num_samp = samples;
			}
			XFree(vi);
		}

		GLXFBConfig bestFbc = fbc[best_fbc];

		XFree(fbc);

		/* Get a visual */
		XVisualInfo *vi = glXGetVisualFromFBConfig((Display*)win->display, bestFbc);

		/* make X window attrubutes*/
		XSetWindowAttributes swa;
		Colormap cmap;

		swa.colormap = cmap = XCreateColormap( (Display*)win->display,
												RootWindow( win->display, vi->screen ), 
												vi->visual, AllocNone );	

		swa.background_pixmap = None ;	
		swa.border_pixel = 0;
		swa.event_mask = event_mask;

		/* create the window*/
		win->window = (void*)XCreateWindow((Display *)win->display, RootWindow((Display *)win->display, vi->screen), x, y, w, h,
							0, vi->depth, InputOutput, vi->visual,
							CWBorderPixel|CWColormap|CWEventMask, &swa);

		if (RGFW_TRANSPARENT_WINDOW & args)
			XMatchVisualInfo((Display *)win->display, DefaultScreen((Display *)win->display), 32, TrueColor, vi); /* for RGBA backgrounds*/

	
		int context_attribs[5] = {0};

		if (RGFW_majorVersion || RGFW_minorVersion) {
			context_attribs[0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
			context_attribs[1] = RGFW_majorVersion;
			context_attribs[2] = GLX_CONTEXT_MINOR_VERSION_ARB;
			context_attribs[3] = RGFW_minorVersion;
		}

		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
		glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB" );

		win->glWin = glXCreateContextAttribsARB((Display*)win->display, bestFbc, 0, True, context_attribs);
	}

	#endif

	#if defined(RGFW_OSMESA) || defined(RGFW_EGL)  || defined(RGFW_VULKAN)
	#ifdef RGFW_GL
	else
	#endif
	{
		XSetWindowAttributes wa;
		wa.event_mask = event_mask;

		win->window = (void*)XCreateWindow((Display*)win->display, XDefaultRootWindow((Display*)win->display), x, y, w, h, 0, 0, InputOutput,
										DefaultVisual((Display*)win->display, XDefaultScreen((Display*)win->display)), CWEventMask, &wa);

		#ifdef RGFW_OSMESA
		win->glWin = OSMesaCreateContext(OSMESA_RGBA, NULL);
		win->buffer = malloc(w * h * 4);
		OSMesaMakeCurrent(win->glWin, win->buffer, GL_UNSIGNED_BYTE, w, h);
		#ifndef RGFW_GL
		win->render = 1;
		#endif
		#else
		#ifdef RGFW_EGL
		RGFW_createOpenGLContext(win);
		#endif
		#endif
	}
	#endif

    if (RGFW_NO_RESIZE & args) { /* make it so the user can't resize the window*/
        XSizeHints* sh = XAllocSizeHints();
        sh->flags = (1L << 4) | (1L << 5);
        sh->min_width = sh->max_width = w;
        sh->min_height = sh->max_height = h;

        XSetWMSizeHints((Display *)win->display, (Drawable)win->window, sh, ((Atom)40));
    }


    if (RGFW_NO_BORDER & args) {
		/* Atom vars for no-border*/
        Atom window_type = XInternAtom((Display *)win->display, "_NET_WM_WINDOW_TYPE", False);
        Atom value = XInternAtom((Display *)win->display, "_NET_WM_WINDOW_TYPE_DOCK", False);

        XChangeProperty((Display *)win->display, (Drawable)win->window, window_type, ((Atom)4), 32, PropModeReplace, (unsigned char *)&value, 1); /* toggle border*/
    }

    XSelectInput((Display *)win->display, (Drawable)win->window, event_mask); /* tell X11 what events we want*/

    /* make it so the user can't close the window until the program does*/
    Atom wm_delete = XInternAtom((Display *)win->display, "WM_DELETE_WINDOW", 1);
    XSetWMProtocols((Display *)win->display, (Drawable)win->window, &wm_delete, 1);

    /* connect the context to the window*/
    #ifdef RGFW_GL
	#ifdef RGFW_OSMESA
	if (RGFW_OPENGL & args) {
	#endif
		glXMakeCurrent((Display *)win->display, (Drawable)win->window, (GLXContext)win->glWin);

		RGFW_swapInterval(win, 1);
	#ifdef RGFW_OSMESA
		render = 0;
	}
	#endif
	#endif

    /* set the background*/
    XStoreName((Display *)win->display, (Drawable) win->window, name); /* set the name*/

    XMapWindow((Display *)win->display, (Drawable)win->window);						  /* draw the window*/
    XMoveWindow((Display *)win->display, (Drawable)win->window, x, y); /* move the window to it's proper cords*/

	if (RGFW_HIDE_MOUSE & args)
		RGFW_hideMouse(win);

	if (RGFW_ALLOW_DND & args) { /* init drag and drop atoms and turn on drag and drop for this window */
		win->dnd = 1;

		XdndAware         = XInternAtom((Display*)win->display, "XdndAware",         False);
		XdndTypeList      = XInternAtom((Display*)win->display, "XdndTypeList",      False);
		XdndSelection     = XInternAtom((Display*)win->display, "XdndSelection",     False);

		/* client messages */
		XdndEnter         = XInternAtom((Display*)win->display, "XdndEnter",         False);
		XdndPosition      = XInternAtom((Display*)win->display, "XdndPosition",      False);
		XdndStatus        = XInternAtom((Display*)win->display, "XdndStatus",        False);
		XdndLeave         = XInternAtom((Display*)win->display, "XdndLeave",         False);
		XdndDrop          = XInternAtom((Display*)win->display, "XdndDrop",          False);
		XdndFinished      = XInternAtom((Display*)win->display, "XdndFinished",      False);

		/* actions */
		XdndActionCopy    = XInternAtom((Display*)win->display, "XdndActionCopy",    False);
		XdndActionMove    = XInternAtom((Display*)win->display, "XdndActionMove",    False);
		XdndActionLink    = XInternAtom((Display*)win->display, "XdndActionLink",    False);
		XdndActionAsk     = XInternAtom((Display*)win->display, "XdndActionAsk",     False);
		XdndActionPrivate = XInternAtom((Display*)win->display, "XdndActionPrivate", False);
		const Atom version = 5;

		XChangeProperty((Display*)win->display, (Window)win->window,
                        XdndAware, 4, 32,
                        PropModeReplace, (unsigned char*) &version, 1); /* turns on drag and drop */
	}

	RGFW_setMouseDefault(win);

	RGFW_windowsOpen++;

    return win; /*return newly created window*/
}

unsigned int* RGFW_getScreenSize(RGFW_window* win) {
	static unsigned int RGFWScreen[2];

	if (RGFW_ValidWindowCheck) (unsigned int[2]) {0, 0};
	Screen* scrn = DefaultScreenOfDisplay((Display*)win->display);

	RGFWScreen[0] = scrn->height;
	RGFWScreen[1] = scrn->width;

	return RGFWScreen;
}

int* RGFW_getGlobalMousePoint(RGFW_window* win) {
	static int RGFWMouse[2];

	int x, y;
	unsigned int z;
	Window window1, window2;
    XQueryPointer((Display*)win->display, XDefaultRootWindow((Display*)win->display), &window1, &window2, &x, &RGFWMouse[0], &RGFWMouse[1], &y, &z);

	return RGFWMouse;
}

typedef struct XDND {
	long source, version;
	int format;
} XDND; /* data structure for xdnd events */
XDND xdnd;

int xAxis = 0, yAxis = 0;

RGFW_Event* RGFW_checkEvents(RGFW_window* win) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_checkEvents")) return NULL;

	XEvent E; /* raw X11 event */

	/* if there is no unread qued events, get a new one */
	if (XEventsQueued((Display*)win->display, QueuedAlready) + XEventsQueued((Display*)win->display, QueuedAfterReading))
		XNextEvent((Display*)win->display, &E);

	unsigned int i;

	if (win->event.droppedFilesCount && win->event.droppedFiles != (char**)0) {
		for (i = 0; i < win->event.droppedFilesCount; i++)
			free(win->event.droppedFiles[i]);

		free(win->event.droppedFiles);

		win->event.droppedFiles = (char**)0;
	}

	win->event.droppedFilesCount = 0;
	win->event.type = 0;

	switch (E.type) {
		case KeyPress:
		case KeyRelease:
			/* check if it's a real key release */
			if (E.type == KeyRelease && XEventsQueued((Display *)win->display, QueuedAfterReading)) { /* get next event if there is one*/
				XEvent NE;
				XPeekEvent((Display *)win->display, &NE);

				if (E.xkey.time == NE.xkey.time && E.xkey.keycode == NE.xkey.keycode) /* check if the current and next are both the same*/
					break;
			}

			/* set event key data */
			win->event.keyCode = XkbKeycodeToKeysym((Display *)win->display, E.xkey.keycode, 0, E.xkey.state & ShiftMask ? 1 : 0); /* get keysym*/
			win->event.keyName = XKeysymToString(win->event.keyCode); /* convert to string */

			/* get keystate data */
			XKeyboardState keystate;
			XGetKeyboardControl((Display *)win->display, &keystate);
			win->event.ledState = keystate.led_mask;
			win->event.keyCode = E.xkey.keycode;
			win->event.type = (E.type == KeyPress) ? RGFW_keyPressed : RGFW_keyReleased;
			break;

		case ButtonPress:
		case ButtonRelease:
			win->event.type = (E.type == ButtonPress) ? RGFW_mouseButtonPressed : RGFW_mouseButtonReleased;
			win->event.button = E.xbutton.button;
			break;

		case MotionNotify:
			win->event.x = E.xmotion.x;
			win->event.y = E.xmotion.y;
			win->event.type = RGFW_mousePosChanged;
			break;

		case ClientMessage:
			/* if the client closed the window*/
			if (E.xclient.data.l[0] == (long int)XInternAtom((Display *)win->display, "WM_DELETE_WINDOW", 1))
				win->event.type = RGFW_quit;

			/*
				much of this event (drag and drop code) is source from glfw
			*/

			else if (win->dnd) {
				unsigned char formFree = 0;

				if (E.xclient.message_type == XdndEnter) {
					unsigned long count;
					Atom* formats = (Atom*)0;
					Bool list = E.xclient.data.l[1] & 1;

					xdnd.source  = E.xclient.data.l[0];
					xdnd.version = E.xclient.data.l[1] >> 24;
					xdnd.format  = None;

					if (xdnd.version > 5)
						break;

					if (list) {
						Atom actualType;
						int actualFormat;
						unsigned long bytesAfter;

						XGetWindowProperty((Display*)win->display,
										xdnd.source,
										XdndTypeList,
										0,
										LONG_MAX,
										False,
										4,
										&actualType,
										&actualFormat,
										&count,
										&bytesAfter,
										(unsigned char**) &formats);
					}
					else {
						formats = (Atom*)malloc(E.xclient.data.l[2] + E.xclient.data.l[3] + E.xclient.data.l[4]);
						formFree = 1;

						count = 0;

						if (E.xclient.data.l[2] != None)
							formats[count++] = E.xclient.data.l[2];
						if (E.xclient.data.l[3] != None)
							formats[count++] = E.xclient.data.l[3];
						if (E.xclient.data.l[4] != None)
							formats[count++] = E.xclient.data.l[4];
					}

					unsigned int i;
					for (i = 0;  i < count;  i++) {
						char* name = XGetAtomName((Display*)win->display, formats[i]);
						if ((strcmp("text/uri-list", name) == 0) || (strcmp("text/plain", name) == 0))
							xdnd.format = formats[i];
					}

					if (list && formats) {
						XFree(formats);
						formats = (Atom*)0;
					}
					else if (formFree && formats != (Atom*)0) {
						free(formats);

						formats = (Atom*)0;
						formFree = 1;
					}
				}
				else if (E.xclient.message_type == XdndDrop) {
					win->event.type = RGFW_dnd;

					Time time = CurrentTime;

					if (xdnd.version > 5)
						break;

					if (xdnd.format) {
						if (xdnd.version >= 1)
							time = E.xclient.data.l[2];

						XConvertSelection((Display*)win->display,
										XdndSelection,
										xdnd.format,
										XdndSelection,
										(Window)win->window,
										time);


					}
					else if (xdnd.version >= 2) {
						XEvent reply = { ClientMessage };
						reply.xclient.window = xdnd.source;
						reply.xclient.message_type = XdndFinished;
						reply.xclient.format = 32;
						reply.xclient.data.l[0] = (long)win->window;
						reply.xclient.data.l[1] = 0;
						reply.xclient.data.l[2] = None;

						XSendEvent((Display*)win->display, xdnd.source,
								False, NoEventMask, &reply);
						XFlush((Display*)win->display);
					}
				}

				else if (E.xclient.message_type == XdndPosition) {
					const int xabs = (E.xclient.data.l[2] >> 16) & 0xffff;
					const int yabs = (E.xclient.data.l[2]) & 0xffff;
					Window dummy;
					int xpos, ypos;

					if (xdnd.version > 5)
						break;

					XTranslateCoordinates((Display*)win->display,
										XDefaultRootWindow((Display*)win->display),
										(Window)win->window,
										xabs, yabs,
										&xpos, &ypos,
										&dummy);

					win->event.x = xpos;
					win->event.y = ypos;

					XEvent reply = { ClientMessage };
					reply.xclient.window = xdnd.source;
					reply.xclient.message_type = XdndStatus;
					reply.xclient.format = 32;
					reply.xclient.data.l[0] = (long)win->window;
					reply.xclient.data.l[2] = 0;
					reply.xclient.data.l[3] = 0;

					if (xdnd.format)
					{
						reply.xclient.data.l[1] = 1;
						if (xdnd.version >= 2)
							reply.xclient.data.l[4] = XdndActionCopy;
					}

					XSendEvent((Display*)win->display, xdnd.source,
							False, NoEventMask, &reply);
					XFlush((Display*)win->display);
				}
			}

			break;

        case SelectionNotify:
			if (E.xselection.property == XdndSelection && win->dnd) {
				char* data;
                unsigned long result;

				Atom actualType;
				int actualFormat;
				unsigned long bytesAfter;

				XGetWindowProperty((Display*)win->display, E.xselection.requestor, E.xselection.property, 0, LONG_MAX, False, E.xselection.target, &actualType, &actualFormat, &result, &bytesAfter, (unsigned char**) &data);

                if (result) {
					/*
					SOURCED FROM GLFW _glfwParseUriList
					Copyright (c) 2002-2006 Marcus Geelnard
					Copyright (c) 2006-2019 Camilla Löwy
					*/

					const char* prefix = "file://";
					win->event.droppedFiles = NULL;
					char* line;

					win->event.droppedFilesCount = 0;

					while ((line = strtok(data, "\r\n"))) {
						char* path;

						data = NULL;

						if (line[0] == '#')
							continue;

						if (strncmp(line, prefix, strlen(prefix)) == 0) {
							line += strlen(prefix);
							while (*line != '/')
								line++;
						}

						win->event.droppedFilesCount++;

						path = (char*)calloc(strlen(line) + 1, 1);
						win->event.droppedFiles  = (char**)realloc(win->event.droppedFiles , win->event.droppedFilesCount * sizeof(char*));
						win->event.droppedFiles[win->event.droppedFilesCount - 1] = path;

						while (*line) {
							if (line[0] == '%' && line[1] && line[2]) {
								const char digits[3] = { line[1], line[2], '\0' };
								*path = (char) strtol(digits, NULL, 16);
								line += 2;
							}
							else
								*path = *line;

							path++;
							line++;
						}
					}
				}

                if (data)
                    XFree(data);

                if (xdnd.version >= 2) {
                    XEvent reply = { ClientMessage };
                    reply.xclient.window = xdnd.source;
                    reply.xclient.message_type = XdndFinished;
                    reply.xclient.format = 32;
                    reply.xclient.data.l[0] = (long)win->display;
                    reply.xclient.data.l[1] = result;
                    reply.xclient.data.l[2] = XdndActionCopy;

                    XSendEvent((Display*)win->display, xdnd.source, False, NoEventMask, &reply);
                    XFlush((Display*)win->display);
                }
            }

			break;

		case FocusIn:
			win->inFocus = 1;
			break;
		case FocusOut:
			win->inFocus = 0;
			RGFW_setMouseDefault(win);
			break;
		case ConfigureNotify:
			#ifndef RGFW_NO_X11_WINDOW_ATTRIB
			XWindowAttributes a;
			XGetWindowAttributes((Display *)win->display, (Window)win->window, &a);

			win->srcX = win->x = a.x;
			win->srcY = win->y = a.y;
			win->srcW = win->w = a.width;
			win->srcH = win->h = a.height; 
			#endif
			break;
		default: {
			#ifdef __linux__
			unsigned char i;
			for (i = 0; i < win->joystickCount; i++) {
				struct js_event e;

				if (!win->joysticks[i])
					continue;

				int flags = fcntl(win->joysticks[i], F_GETFL, 0);
				fcntl(win->joysticks[i], F_SETFL, flags | O_NONBLOCK);

				ssize_t bytes;
				while (bytes = read(win->joysticks[i], &e, sizeof(e)) > 0) {
						switch (e.type) {
							case JS_EVENT_BUTTON:
								win->event.type = e.value ? RGFW_jsButtonPressed : RGFW_jsButtonReleased;
								win->event.button = e.number;

								win->jsPressed[i][e.number] = e.value;
								break;
							case JS_EVENT_AXIS:
								ioctl(win->joysticks[i], JSIOCGAXES, &win->event.axisesCount);

								if (e.number == 0 || e.number % 2 && e.number != 1)
									xAxis = e.value;
								else
									yAxis = e.value;

								win->event.axis[e.number / 2][0] = xAxis / 327.67;
								win->event.axis[e.number / 2][1] = yAxis / 327.67;
								win->event.type = RGFW_jsAxisMove;
								win->event.button = e.number / 2;
								break;

							default: break;
						}
				}
			}
			#endif

			break;
		}
	}

	if ((win->srcX != win->x) || (win->srcY != win->y)) {
		XMoveWindow((Display *)win->display, (Drawable)win->window, win->x, win->y);
		win->srcX = win->x;
		win->srcY = win->y;
	}

	else if ((win->srcW != win->w) || (win->srcH != win->h)) {
		XResizeWindow((Display *)win->display, (Drawable)win->window, (Drawable)win->w, win->h);
		win->srcW = win->w;
		win->srcH = win->h;
	}

	if (win->srcName != win->name) {
		XStoreName((Display *)win->display, (Window)win->window, win->name);
		win->srcName = win->name;
	}

	if (win->inFocus && win->cursorChanged) {
		XDefineCursor((Display*)win->display, (Window)win->window, (Cursor)win->cursor);

		win->cursorChanged = 0;
	}

	XFlush((Display*)win->display);

	RGFW_checkFPS(win);

	if (win->event.type)
		return &win->event;
	else
		return NULL;
}

void RGFW_closeWindow(RGFW_window* win) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_closeWindow")) return;

	#ifdef RGFW_EGL
	RGFW_closeEGL(win);
	#endif

	XFreeCursor((Display*)win->display, (Cursor)win->cursor);

	if ((Display*)win->display) {
		if ((Drawable)win->window)
			XDestroyWindow((Display *)win->display, (Drawable)win->window); /* close the window*/

		if (win->display)
			XCloseDisplay((Display *)win->display); /* kill the display*/
	}

	#ifndef RGFW_NO_X11_CURSOR
	if (X11Cursorhandle != NULL && !RGFW_windowsOpen) {
		dlclose(X11Cursorhandle);

		X11Cursorhandle = NULL;
	}
	#endif

	/* set cleared display / window to NULL for error checking */
	win->display = (Display*)0;
	win->window = (Window)0;

	if (win->dnd && win->event.droppedFilesCount) {
		free(win->event.droppedFiles);
	}

	unsigned char i = 0;
	for (i; i < win->joystickCount; i++)
		close(win->joysticks[i]);

	#ifdef RGFW_OSMESA
	if (win->buffer != NULL) {
		free(win->buffer);
		win->render = 0;
	}
	#endif

	free(win); /* free collected window data */

	RGFW_windowsOpen--;
}

/*
	the majority function is sourced from GLFW
*/
void RGFW_setIcon(RGFW_window* win, unsigned char* src, int width, int height, int channels) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_setIcon")) return;

	int longCount = 2 + width * height;

    unsigned long* icon = (unsigned long*)malloc(longCount * sizeof(unsigned long));
    unsigned long* target = icon;

    *target++ = width;
    *target++ = height;

	int i;

    for (i = 0;  i < width * height;  i++) {
        if (channels == 3)
            *target++ = ((src[i * 3 + 0]) << 16) |
                        ((src[i * 3 + 1]) <<  8) |
                        ((src[i * 3 + 2]) <<  0) |
                        (0xFF << 24);

        else if (channels == 4)
            *target++ = ((src[i * 4 + 0]) << 16) |
                        ((src[i * 4 + 1]) <<  8) |
                        ((src[i * 4 + 2]) <<  0) |
                        ((src[i * 4 + 3]) << 24);
    }

    Atom NET_WM_ICON = XInternAtom((Display*)win->display, "_NET_WM_ICON", False);

    XChangeProperty((Display*)win->display, (Window)win->window,
                    NET_WM_ICON,
                    6, 32,
                    PropModeReplace,
                    (unsigned char*) icon,
                    longCount);

    free(icon);

    XFlush((Display*)win->display);
}

void RGFW_setMouse(RGFW_window* win, unsigned char* image, int width, int height, int channels) {
	#ifndef RGFW_NO_X11_CURSOR
	/* free the previous cursor */
	if (win->cursor != NULL && win->cursor != (void*)-1)
		XFreeCursor((Display*)win->display, (Cursor)win->cursor);

	XcursorImage* native = XcursorImageCreate(width, height);
    native->xhot = 0;
    native->yhot = 0;

    unsigned char* source = (unsigned char*) image;
    XcursorPixel* target = native->pixels;

	int i;
    for (i = 0;  i < width * height;  i++, target++, source += 4) {
        unsigned char alpha = 0xFF;
        if (channels == 4)
            alpha = source[3];

        *target = (alpha << 24) | (((source[0] * alpha) / 255) << 16) | (((source[1] * alpha) / 255) <<  8) | (((source[2] * alpha) / 255) <<  0);
    }

	win->cursorChanged = 1;
    win->cursor = (void*)XcursorImageLoadCursor((Display*)win->display, native);
	XcursorImageDestroy(native);
	#endif
}

void RGFW_setMouseDefault(RGFW_window* win) {
	/* free the previous cursor */
	if (win->cursor != NULL && win->cursor != (void*)-1)
		XFreeCursor((Display*)win->display, (Cursor)win->cursor);

	win->cursorChanged = 1;
	win->cursor = (void*)XCreateFontCursor((Display*)win->display, XC_left_ptr);
}

/*
	the majority function is sourced from GLFW
*/
char* RGFW_readClipboard(RGFW_window* win) {
  if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_readClipboard")) return (char*)"";

  char* result;
  unsigned long ressize, restail;
  int resbits;
  Atom bufid = XInternAtom((Display*)win->display, "CLIPBOARD", False),
       fmtid = XInternAtom((Display*)win->display, "STRING", False),
       propid = XInternAtom((Display*)win->display, "XSEL_DATA", False),
       incrid = XInternAtom((Display*)win->display, "INCR", False);
  XEvent event;

  XSelectInput ((Display*)win->display, (Window)win->window, PropertyChangeMask);
  XConvertSelection((Display*)win->display, bufid, fmtid, propid, (Window)win->window, CurrentTime);
  do {
    XNextEvent((Display*)win->display, &event);
  } while (event.type != SelectionNotify || event.xselection.selection != bufid);

  if (event.xselection.property) {
    XGetWindowProperty((Display*)win->display, (Window)win->window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
      &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

    if (fmtid == incrid)
      do {
        while (event.type != PropertyNotify || event.xproperty.atom != propid || event.xproperty.state != PropertyNewValue) XNextEvent((Display*)win->display, &event);

        XGetWindowProperty((Display*)win->display, (Window)win->window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
          &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
      } while (ressize > 0);
  }

  return result;
}

/*
	almost all of this function is sourced from GLFW
*/
void RGFW_writeClipboard(RGFW_window* win, char* text) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_writeClipboard")) return;
    Atom CLIPBOARD, UTF8_STRING, SAVE_TARGETS, TARGETS, MULTIPLE, ATOM_PAIR, PRIMARY, CLIPBOARD_MANAGER;

    CLIPBOARD = XInternAtom((Display*)win->display, "CLIPBOARD", False);
    UTF8_STRING = XInternAtom((Display*)win->display, "UTF8_STRING", False);
    SAVE_TARGETS = XInternAtom((Display*)win->display, "SAVE_TARGETS", False);
    TARGETS = XInternAtom((Display*)win->display, "TARGETS", False);
    MULTIPLE = XInternAtom((Display*)win->display, "MULTIPLE", False);
    ATOM_PAIR = XInternAtom((Display*)win->display, "ATOM_PAIR", False);
    PRIMARY = XInternAtom((Display*)win->display, "PRIMARY", False);
    CLIPBOARD_MANAGER = XInternAtom((Display*)win->display, "CLIPBOARD_MANAGER", False);

    XSetSelectionOwner((Display*)win->display, CLIPBOARD, (Window)win->window, CurrentTime);

    XConvertSelection((Display*)win->display, CLIPBOARD_MANAGER, SAVE_TARGETS, None, (Window)win->window, CurrentTime);

	unsigned char XA_STRING = 31;

    for (;;) {
        XEvent event;

        XNextEvent((Display*)win->display, &event);
        switch (event.type) {
            case SelectionRequest: {
                const XSelectionRequestEvent* request = &event.xselectionrequest;

                XEvent reply = { SelectionNotify };

                char* selectionString = NULL;
                const Atom formats[] = { UTF8_STRING, XA_STRING };
                const int formatCount = sizeof(formats) / sizeof(formats[0]);

                selectionString = text;

                if (request->target == TARGETS) {
                    const Atom targets[] = { TARGETS,
                                            MULTIPLE,
                                            UTF8_STRING,
                                            XA_STRING };

                    XChangeProperty((Display*)win->display,
                                    request->requestor,
                                    request->property,
                                    4,
                                    32,
                                    PropModeReplace,
                                    (unsigned char*) targets,
                                    sizeof(targets) / sizeof(targets[0]));

                    reply.xselection.property = request->property;
                }

                if (request->target == MULTIPLE) {

                    Atom* targets;

                    Atom actualType;
                    int actualFormat;
                    unsigned long count, bytesAfter;

                    XGetWindowProperty((Display*)win->display, request->requestor, request->property, 0, LONG_MAX, False, ATOM_PAIR,  &actualType, &actualFormat, &count, &bytesAfter, (unsigned char**) &targets);

                    unsigned long i;
                    for (i = 0;  i < count;  i += 2) {
                        int j;

                        for (j = 0;  j < formatCount;  j++) {
                            if (targets[i] == formats[j])
                                break;
                        }

                        if (j < formatCount)
                        {
                            XChangeProperty((Display*)win->display,
                                            request->requestor,
                                            targets[i + 1],
                                            targets[i],
                                            8,
                                            PropModeReplace,
                                            (unsigned char *) selectionString,
                                            strlen(selectionString));
                        }
                        else
                            targets[i + 1] = None;
                    }

                    XChangeProperty((Display*)win->display,
                                    request->requestor,
                                    request->property,
                                    ATOM_PAIR,
                                    32,
                                    PropModeReplace,
                                    (unsigned char*) targets,
                                    count);

                    XFree(targets);

                    reply.xselection.property = request->property;
                }

                reply.xselection.display = request->display;
                reply.xselection.requestor = request->requestor;
                reply.xselection.selection = request->selection;
                reply.xselection.target = request->target;
                reply.xselection.time = request->time;

                XSendEvent((Display*)win->display, request->requestor, False, 0, &reply);
                break;
			}

            default: return;
        }
    }
}

unsigned short RGFW_registerJoystick(RGFW_window* win, int jsNumber) {
	#ifdef __linux__
	char file[15];
	sprintf(file, "/dev/input/js%i", jsNumber);

	return RGFW_registerJoystickF(win, file);
	#endif
}

unsigned short RGFW_registerJoystickF(RGFW_window* win, char* file) {
	#ifdef __linux__
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_registerJoystickF")) return 0;

	int js = open(file, O_RDONLY);

	if (js && win->joystickCount < 4) {
		win->joystickCount++;

		win->joysticks[win->joystickCount - 1] = open(file, O_RDONLY);

		unsigned char i = 0;
		for (i; i < 16; i++)
			win->jsPressed[win->joystickCount - 1][i] = 0;

	}

	else {
		#ifdef RGFW_PRINT_ERRORS
		RGFW_error = 1;
		printf("Error RGFW_registerJoystickF : Cannot open file %s\n", file);
		#endif
	}

	return win->joystickCount - 1;
	#endif
}

char keyboard[32];
Display* RGFWd = (Display*)0;

unsigned char RGFW_isPressedI(RGFW_window* win, unsigned int key) {
	if (RGFWd == (Display*)0) RGFWd = XOpenDisplay(0);

	Display* d;
	if (win == (RGFW_window*)0)
		d = RGFWd;
	else if (!win->inFocus)
		return 0;
	else
		d = (Display*)win->display;

	XQueryKeymap(d, keyboard); /* query the keymap */


	KeyCode kc2 = XKeysymToKeycode(d, key); /* convert the key to a keycode */
	return !!(keyboard[kc2 >> 3] & (1 << (kc2 & 7)));				/* check if the key is pressed */
}

unsigned char RGFW_Error() { return RGFW_error; }

#endif

#ifdef _WIN32
#include <GL/gl.h>

char* createUTF8FromWideStringWin32(const WCHAR* source);

#define GL_FRONT				0x0404
#define GL_BACK					0x0405
#define GL_LEFT					0x0406
#define GL_RIGHT				0x0407

#if defined(RGFW_OSMESA) && defined(RGFW_LINK_OSMESA)

typedef void (GLAPIENTRY * PFN_OSMesaDestroyContext)(OSMesaContext);
typedef int (GLAPIENTRY * PFN_OSMesaMakeCurrent)(OSMesaContext,void*,int,int,int);
typedef OSMesaContext (GLAPIENTRY * PFN_OSMesaCreateContext)(GLenum, OSMesaContext);

PFN_OSMesaMakeCurrent OSMesaMakeCurrentSource;
PFN_OSMesaCreateContext OSMesaCreateContextSource;
PFN_OSMesaDestroyContext OSMesaDestroyContextSource;

#define OSMesaCreateContext OSMesaCreateContextSource
#define OSMesaMakeCurrent OSMesaMakeCurrentSource
#define OSMesaDestroyContext OSMesaDestroyContextSource
#endif

#ifdef RGFW_WGL_LOAD
typedef PROC (*PFN_wglGetProcAddress)(LPCSTR);
typedef HGLRC (*PFN_wglCreateContext)(HDC);
typedef BOOL (*PFN_wglMakeCurrent)(HDC,HGLRC);
typedef BOOL (*PFN_wglDeleteContext)(HGLRC);

PFN_wglGetProcAddress wglGetProcAddressSrc = NULL;
PFN_wglCreateContext wglCreateContextSrc = NULL;
PFN_wglMakeCurrent wglMakeCurrentSrc = NULL;
PFN_wglDeleteContext wglDeleteContextSrc = NULL;

#define wglGetProcAddress wglGetProcAddressSrc
#define wglCreateContext wglCreateContextSrc
#define wglMakeCurrent wglMakeCurrentSrc
#define wglDeleteContext wglDeleteContextSrc
HINSTANCE wglinstance = NULL;
#endif

typedef BOOL (*PFN_wglSwapIntervalEXT)(int);
PFN_wglSwapIntervalEXT wglSwapIntervalEXTSrc = NULL;
#define wglSwapIntervalEXT wglSwapIntervalEXTSrc

void* RGFWjoystickApi = NULL;

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
        const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023
#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

void init_opengl(RGFW_window* win) {
	/* create/load dummy window for loading ARB version */
	WNDCLASSA window_class;
	
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = DefWindowProcA;
	window_class.hInstance = GetModuleHandle(0);
	window_class.lpszClassName = "Dummy_WGL_djuasiodwa";

    if (!RegisterClassA(&window_class));

	HWND dummy_window = CreateWindowA(window_class.lpszClassName, "Dummy OpenGL Window", 0, 0, 0, 0, 0, 0, 0, window_class.hInstance, 0);
	
	HDC dummy_dc = GetDC(dummy_window);

	PIXELFORMATDESCRIPTOR pfd = {sizeof(pfd), 1, PFD_TYPE_RGBA, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 32, 8, PFD_MAIN_PLANE, 24, 8};

	int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);

	SetPixelFormat(dummy_dc, pixel_format, &pfd);

	HGLRC dummy_context = wglCreateContext(dummy_dc);

	wglMakeCurrent(dummy_dc, dummy_context);

	wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress("wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress("wglChoosePixelFormatARB");

	wglMakeCurrent(dummy_dc, 0);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummy_window, dummy_dc);
	DestroyWindow(dummy_window);
	

	/* use dummy window to load ARB version */
    int pixel_format_attribs[] = {WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,           WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,           WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,             WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,             WGL_COLOR_BITS_ARB,         32,                     WGL_DEPTH_BITS_ARB,         24,                 WGL_STENCIL_BITS_ARB,       8,            0                       };

    UINT num_formats;
    wglChoosePixelFormatARB((HDC)win->window, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);

    PIXELFORMATDESCRIPTOR npfd;
    DescribePixelFormat((HDC)win->window, pixel_format, sizeof(npfd), &npfd);
    SetPixelFormat((HDC)win->window, pixel_format, &npfd);

	int context_attribs[5] = {0, 0, 0, 0, 0};

	if (RGFW_majorVersion || RGFW_minorVersion) {
		context_attribs[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		context_attribs[1] = RGFW_majorVersion;
		context_attribs[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
		context_attribs[3] = RGFW_minorVersion;
	}

    win->glWin = wglCreateContextAttribsARB((HDC)win->window, 0, context_attribs);

    wglMakeCurrent((HDC)win->window, (HGLRC)win->glWin);
}

RGFW_window* RGFW_createWindowPointer(char* name, int x, int y, int w, int h, unsigned long args) {
	static char RGFW_trashed = 0;

    #ifdef RGFW_WGL_LOAD
	if (wglinstance == NULL) { 
		wglinstance = LoadLibraryA("opengl32.dll");

		wglCreateContext = (PFN_wglCreateContext) GetProcAddress(wglinstance, "wglCreateContext");
		wglDeleteContext = (PFN_wglDeleteContext) GetProcAddress(wglinstance, "wglDeleteContext");
		wglGetProcAddress = (PFN_wglGetProcAddress) GetProcAddress(wglinstance, "wglGetProcAddress");
		wglMakeCurrent = (PFN_wglMakeCurrent) GetProcAddress(wglinstance, "wglMakeCurrent");
	}
	#endif

    if (name == "") name = (char*)" ";

	RGFW_window* win = (RGFW_window*)malloc(sizeof(RGFW_window));

    int         pf;

	if (RGFW_FULLSCREEN & args) {
		unsigned int* r = RGFW_getScreenSize(win);
		x = 0;
		y = 0;
		w = r[0];
		h = r[1];
	}

	win->srcX = win->x = x;
	win->srcY = win->y = y;
	win->srcW = win->w = w;
	win->srcH = win->h = h;
	win->valid = 245;

	win->srcName = win->name = name;
	win->fpsCap = 0;
	win->inFocus = 1;
	win->joystickCount = 0;
	win->event.droppedFilesCount = 0;

    HINSTANCE inh = GetModuleHandle(NULL);

    WNDCLASSA Class = {0}; /* Setup the Window class. */
	Class.lpszClassName = name;
	Class.hInstance = inh;
	Class.hCursor = LoadCursor(NULL, IDC_ARROW);
	Class.lpfnWndProc = DefWindowProc;

    RegisterClassA(&Class);

	DWORD window_style = WS_MAXIMIZEBOX | WS_MINIMIZEBOX | window_style;

	if (!(RGFW_NO_BORDER & args))
		window_style |= WS_CAPTION | WS_SYSMENU | WS_BORDER;
	else
		window_style |= WS_POPUP | WS_VISIBLE;

	if (!(RGFW_NO_RESIZE & args))
		window_style |= WS_SIZEBOX;

    win->display = CreateWindowA( Class.lpszClassName, name, window_style, x, y, w, h, 0, 0, inh, 0);

	if (RGFW_TRANSPARENT_WINDOW & args)
		SetWindowLong((HWND)win->display, GWL_EXSTYLE, GetWindowLong((HWND)win->display, GWL_EXSTYLE) | WS_EX_LAYERED);

	if (RGFW_ALLOW_DND & args)
		DragAcceptFiles((HWND)win->display, TRUE);

    win->window = GetDC((HWND)win->display);

 	#ifdef RGFW_GL
	init_opengl(win);
	#endif

	#ifdef RGFW_OSMESA

	#ifdef RGFW_LINK_OSMESA
	OSMesaMakeCurrentSource = (PFN_OSMesaMakeCurrent) GetProcAddress(win->display, "OSMesaMakeCurrent");
	OSMesaCreateContextSource = (PFN_OSMesaCreateContext) GetProcAddress(win->display, "OSMesaCreateContext");
	OSMesaDestroyContextSource = (PFN_OSMesaDestroyContext) GetProcAddress(win->display, "OSMesaDestroyContext");
	#endif

	if (RGFW_OPENGL & args) {
	#endif
		ReleaseDC((HWND)win->display, (HDC)win->window);
		win->window = GetDC((HWND)win->display);
		wglMakeCurrent((HDC)win->window, (HGLRC)win->glWin);
	
		if (wglSwapIntervalEXT == NULL)
			wglSwapIntervalEXT = (PFN_wglSwapIntervalEXT)wglGetProcAddress("wglSwapIntervalEXT");
	#ifdef RGFW_OSMESA
		win->buffer = NULL;
	#endif
	#ifdef RGFW_OSMESA
	}
	else {
		win->glWin = (void*)OSMesaCreateContext(OSMESA_RGBA, NULL);
		win->buffer = malloc(w * h * 4);

		OSMesaMakeCurrent(win->glWin, win->buffer, GL_UNSIGNED_BYTE, w, h);
	}
	#endif

	#ifdef RGFW_EGL
	RGFW_createOpenGLContext(win);
	#endif

    ShowWindow((HWND)win->display, SW_SHOWNORMAL);

	if (!RGFW_trashed) { /* a throw away window needs to be created for some reason because of wgl's ARB loading */
		RGFW_trashed = 1;

		RGFW_window* trash = RGFW_createWindowPointer((char*)"", 0, 0, 0, 0, 0);
		RGFW_closeWindow(trash);
	}

    return win;
}


unsigned int* RGFW_getScreenSize(RGFW_window* win) {
	static unsigned int RGFW_ScreenSize[2];

	RGFW_ScreenSize[0] = GetDeviceCaps(GetDC(NULL), HORZRES);
	RGFW_ScreenSize[1] = GetDeviceCaps(GetDC(NULL), VERTRES);

	return RGFW_ScreenSize;
}

int* RGFW_getGlobalMousePoint(RGFW_window* win) {
	int RGFWMouse[2];

	POINT p; 
	GetCursorPos(&p);

	RGFWMouse[0] = p.x;
	RGFWMouse[1] = p.y;
	
	return RGFWMouse;
}

RGFW_Event* RGFW_checkEvents(RGFW_window* win) {
	RGFW_checkFPS(win);

	MSG msg = {};

	int setButton = 0;

	if (win->event.droppedFilesCount && win->event.droppedFiles != (char**)0) {
		unsigned int i;

		for (i = 0; i < win->event.droppedFilesCount; i++)
			free(win->event.droppedFiles[i]);

		free(win->event.droppedFiles); /* free dropped file data from the last event */

		win->event.droppedFiles = (char**)0;
	}

	win->event.droppedFilesCount = 0;


	while (PeekMessage(&msg, (HWND)win->display, 0u, 0u, PM_REMOVE)) {
		switch (msg.message) {
			case WM_CLOSE:
			case WM_QUIT:
				win->event.type = RGFW_quit;
				break;

			case WM_KEYUP:
				win->event.keyCode = msg.wParam;
				GetKeyNameTextA(msg.lParam, win->event.keyName, 16);
				win->event.type = RGFW_keyReleased;
				break;

			case WM_KEYDOWN:
				win->event.keyCode = msg.wParam;
				GetKeyNameTextA(msg.lParam, win->event.keyName, 16);
				win->event.type = RGFW_keyPressed;
				break;

			case WM_MOUSEMOVE:
				win->event.x = msg.pt.x - win->x;
				win->event.x = msg.pt.x - win->x;
				win->event.type = RGFW_mousePosChanged;
				break;

			case WM_LBUTTONDOWN:
				win->event.button = RGFW_mouseLeft;
				win->event.type = RGFW_mouseButtonPressed;
				break;
			case WM_RBUTTONDOWN:
				win->event.button = RGFW_mouseRight;
				win->event.type = RGFW_mouseButtonPressed;
				break;
			case WM_MBUTTONDOWN:
				win->event.button = RGFW_mouseMiddle;
				win->event.type = RGFW_mouseButtonPressed;
				break;

			case WM_MOUSEWHEEL:
				if (msg.wParam > 0)
					win->event.button = RGFW_mouseScrollUp;
				else
					win->event.button = RGFW_mouseScrollDown;

				win->event.type = RGFW_mouseButtonPressed;
				break;

			case WM_LBUTTONUP:
				win->event.button = RGFW_mouseLeft;
				win->event.type = RGFW_mouseButtonReleased;
				break;
			case WM_RBUTTONUP:
				win->event.button = RGFW_mouseRight;
				win->event.type = RGFW_mouseButtonReleased;
				break;
			case WM_MBUTTONUP:
				win->event.button = RGFW_mouseMiddle;
				win->event.type = RGFW_mouseButtonReleased;
				break;

			/*
				much of this event is source from glfw
			*/
			case WM_DROPFILES: {
					win->event.type = RGFW_dnd;

					HDROP drop = (HDROP)msg.wParam;
					POINT pt;
					int i;

					win->event.droppedFilesCount = DragQueryFileW(drop, 0xffffffff, NULL, 0);
					win->event.droppedFiles = (char**)calloc(win->event.droppedFilesCount, sizeof(char*));

					/* Move the mouse to the position of the drop */
					DragQueryPoint(drop, &pt);

					win->event.x = pt.x;
					win->event.y = pt.y;

					for (i = 0;  i < win->event.droppedFilesCount;  i++) {
						const UINT length = DragQueryFileW(drop, i, NULL, 0);
						WCHAR* buffer = (WCHAR*)calloc((size_t) length + 1, sizeof(WCHAR));

						DragQueryFileW(drop, i, buffer, length + 1);
						win->event.droppedFiles[i] = createUTF8FromWideStringWin32(buffer);

						free(buffer);
					}

					DragFinish(drop);
				}
				break;
			default:
				win->event.type = 0;
				break;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if ((win->srcX != win->x) || (win->srcY != win->y) || (win->srcW != win->w) || (win->srcH != win->h)) {
		SetWindowPos((HWND)win->display, (HWND)win->display, win->x, win->y, win->w, win->h, 0);
		win->srcX = win->x;
		win->srcY = win->y;
		win->srcW = win->w;
		win->srcH = win->h;
	} else {
		/* make sure the window attrubutes are up-to-date*/
		RECT a;

		if (GetWindowRect((HWND)win->display, &a)) {
			win->srcX = win->x = a.left;
			win->srcY = win->y = a.top;
			win->srcW = win->w = a.right - a.left;
			win->srcH = win->h = a.bottom - a.top;
		}
	}

	if (win->srcName != win->name) {
		SetWindowTextA((HWND)win->display, win->name);
		win->srcName = win->name;
	}

	win->event.keyCode = 0;

	if ((GetKeyState(VK_CAPITAL) & 0x0001)!=0)
		win->event.keyCode |= 1;
	if ((GetKeyState(VK_NUMLOCK) & 0x0001)!=0)
		win->event.keyCode |= 2;
	if ((GetKeyState(VK_SCROLL) & 0x0001)!=0)
		win->event.keyCode |= 3;


	if (!IsWindow((HWND)win->display))
		win->event.type = RGFW_quit;

    win->inFocus = (GetForegroundWindow() == win->display);

	if (win->inFocus)
		SetCursor((HCURSOR)win->cursor);
	else
		RGFW_setMouseDefault(win);

	if (win->event.type)
		return &win->event;
	else
		return NULL;
}

unsigned char RGFW_isPressedI(RGFW_window* win, unsigned int key) {
	if (win != NULL && !win->inFocus)
		return 0;

	if (GetAsyncKeyState(key) & 0x8000)
		return 1;
	else return 0;
}

HICON RGFW_loadHandleImage(RGFW_window* win, unsigned char* src, int width, int height, BOOL icon) {
    if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_loadHandleImage")) return NULL;
    int i;
    HDC dc;
    HICON handle;
    HBITMAP color, mask;
    BITMAPV5HEADER bi;
    ICONINFO ii;
    unsigned char* target = NULL;
    unsigned char* source = src;

    ZeroMemory(&bi, sizeof(bi));
    bi.bV5Size        = sizeof(bi);
    bi.bV5Width       = width;
    bi.bV5Height      = -height;
    bi.bV5Planes      = 1;
    bi.bV5BitCount    = 32;
    bi.bV5Compression = BI_BITFIELDS;
    bi.bV5RedMask     = 0x00ff0000;
    bi.bV5GreenMask   = 0x0000ff00;
    bi.bV5BlueMask    = 0x000000ff;
    bi.bV5AlphaMask   = 0xff000000;

    dc = GetDC(NULL);
    color = CreateDIBSection(dc,
                             (BITMAPINFO*) &bi,
                             DIB_RGB_COLORS,
                             (void**) &target,
                             NULL,
                             (DWORD) 0);
    ReleaseDC(NULL, dc);

    mask = CreateBitmap(width, height, 1, 1, NULL);

    for (i = 0;  i < width * height;  i++)
    {
        target[0] = source[2];
        target[1] = source[1];
        target[2] = source[0];
        target[3] = source[3];
        target += 4;
        source += 4;
    }

    ZeroMemory(&ii, sizeof(ii));
    ii.fIcon    = icon;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask  = mask;
    ii.hbmColor = color;

    handle = CreateIconIndirect(&ii);

    DeleteObject(color);
    DeleteObject(mask);

    return handle;
}

void RGFW_setMouse(RGFW_window* win, unsigned char* image, int width, int height, int channels) {
	win->cursor = (HCURSOR)RGFW_loadHandleImage(win, image, width, height, FALSE);
}

void RGFW_setMouseDefault(RGFW_window* win) {
	win->cursor = LoadCursor(NULL, IDC_ARROW);
}

void RGFW_closeWindow(RGFW_window* win) {
	#ifdef RGFW_EGL
	RGFW_closeEGL(win);
	#endif

	#ifdef RGFW_GL
	wglDeleteContext((HGLRC)win->glWin); /* delete opengl context */
	#endif
	DeleteDC((HDC)win->window); /* delete window */
	DestroyWindow((HWND)win->display); /* delete display */

	if (win->event.droppedFilesCount && win->event.droppedFiles != (char**)0) {
		unsigned int i;

		for (i = 0; i < win->event.droppedFilesCount; i++)
			free(win->event.droppedFiles[i]);

		free(win->event.droppedFiles); /* free dropped file data from the last event */

		win->event.droppedFiles = (char**)0;
	}

	#ifdef RGFW_OSMESA
	if (win->buffer != NULL)
		free(win->buffer);
	#endif

	free(win);
}

/* much of this function is sourced from GLFW */
void RGFW_setIcon(RGFW_window* win, unsigned char* src, int width, int height, int channels) {
    if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_setIcon")) return;

    HICON handle = RGFW_loadHandleImage(win, src, width, height, TRUE);

    SendMessageW((HWND)win->display, WM_SETICON, ICON_BIG, (LPARAM) handle);
    SendMessageW((HWND)win->display, WM_SETICON, ICON_SMALL, (LPARAM) handle);
}

char* RGFW_readClipboard(RGFW_window* win) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_readClipboard")) return (char*)"";

    /* Open the clipboard */
    if (!OpenClipboard(NULL))
        return (char*)"";

    /* Get the clipboard data as a Unicode string */
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        CloseClipboard();
        return (char*)"";
    }

	char* text = (char*)malloc(7);
	text = (char*)GlobalLock(hData);

    /* Release the clipboard data */
    GlobalUnlock(hData);
    CloseClipboard();

	return text;
}

void RGFW_writeClipboard(RGFW_window* win, char* text) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_writeClipboard")) return;

    int characterCount;
    HANDLE object;
    WCHAR* buffer;

    characterCount = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    if (!characterCount)
        return;

    object = GlobalAlloc(GMEM_MOVEABLE, characterCount * sizeof(WCHAR));
    if (!object)
        return;

    buffer = (WCHAR*)GlobalLock(object);
    if (!buffer) {
        GlobalFree(object);
        return;
    }

    MultiByteToWideChar(CP_UTF8, 0, text, -1, buffer, characterCount);
    GlobalUnlock(object);

    if (!OpenClipboard((HWND)win->display)) {
        GlobalFree(object);
        return;
    }

    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, object);
    CloseClipboard();
}

unsigned short RGFW_registerJoystick(RGFW_window* win, int jsNumber) {
	return RGFW_registerJoystickF(win, (char*)"");
}

unsigned short RGFW_registerJoystickF(RGFW_window* win, char* file) {

	return win->joystickCount - 1;
}

char* createUTF8FromWideStringWin32(const WCHAR* source) {
    char* target;
    int size;

    size = WideCharToMultiByte(CP_UTF8, 0, source, -1, NULL, 0, NULL, NULL);
    if (!size) {
        return NULL;
    }

    target = (char*)calloc(size, 1);

    if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, size, NULL, NULL)) {
        free(target);
        return NULL;
    }

    return target;
}

RGFW_thread RGFW_createThread(void* (*function_ptr)(void*), void* args) { return (long unsigned int)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)*function_ptr, args, 0, NULL);  }
void RGFW_cancelThread(RGFW_thread thread) { CloseHandle((HANDLE)thread);  }
void RGFW_joinThread(RGFW_thread thread) { WaitForSingleObject((HANDLE)thread, INFINITE); }
void RGFW_setThreadPriority(RGFW_thread thread, unsigned char priority) { SetThreadPriority((HANDLE)thread, priority); }
#endif

#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
#define GL_SILENCE_DEPRECATION
#include <Silicon/silicon.h>
#include <OpenGL/gl.h>
	
void* RGFWnsglFramework = NULL; 

#ifdef RGFW_GL
static void* RGFW_getProcAddress(const char* procname) {
	if (RGFWnsglFramework == NULL)
		RGFWnsglFramework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));

    CFStringRef symbolName = CFStringCreateWithCString(kCFAllocatorDefault, procname, kCFStringEncodingASCII);

    void* symbol = CFBundleGetFunctionPointerForName(RGFWnsglFramework, symbolName);

    CFRelease(symbolName);

    return symbol;
}
#endif
	
CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext) { return kCVReturnSuccess; }

RGFW_window** RGFW_windows;
unsigned int RGFW_windows_size = 0;

bool OnClose(void* self) {
	unsigned int i;
	for (i = 0; i < RGFW_windows_size; i++)
		if (RGFW_windows[i] && RGFW_windows[i]->window == self)
			break;

	RGFW_windows[i]->event.type = RGFW_quit;
	return true;
}

/* NOTE(EimaMei): Fixes the constant clicking when the app is running under a terminal. */
bool acceptsFirstResponder() { return true; }
bool performKeyEquivalent(NSEvent* event) { return true; }

NSDragOperation draggingEntered(NSDraggingInfo* sender) { return NSDragOperationCopy; }
NSDragOperation draggingUpdated(NSDraggingInfo* sender) { return NSDragOperationCopy; }
bool prepareForDragOperation(NSDraggingInfo* sender) { return true; }

/* NOTE(EimaMei): Usually, you never need 'id self, SEL cmd' for C -> Obj-C methods. This isn't the case. */
bool performDragOperation(id self, SEL cmd, NSDraggingInfo* sender) {
	NSWindow* window = NSDraggingInfo_draggingDestinationWindow(sender);

	unsigned int i;
	bool found = false;

	for (i = 0; i < RGFW_windows_size; i++)
		if (RGFW_windows[i]->window == window){
			found = true;
			break;
		}

	if (!found)
		i = 0;


	siArray(Class) array = si_array_init((Class[]){class(objctype(NSURL))}, sizeof(*array), 1);
	siArray(char*) droppedFiles = (siArray(char*))NSPasteboard_readObjectsForClasses(NSDraggingInfo_draggingPasteboard(sender), array, NULL);

	RGFW_windows[i]->event.droppedFiles = malloc(si_array_len(droppedFiles) * sizeof(*droppedFiles));
	RGFW_windows[i]->event.droppedFilesCount = si_array_len(droppedFiles);

	si_array_free(array);

	unsigned int x, y;

	for (y = 0; y < RGFW_windows[i]->event.droppedFilesCount; y++)
		RGFW_windows[i]->event.droppedFiles[y] = strdup(droppedFiles[y]);

	RGFW_windows[i]->event.type = RGFW_dnd;

	NSPoint p = NSDraggingInfo_draggingLocation(sender);
	RGFW_windows[i]->event.x = p.x;
	RGFW_windows[i]->event.x = p.y;

    return true;
}


RGFW_window* RGFW_createWindowPointer(char* name, int x, int y, int w, int h, unsigned long args){
	static unsigned char RGFW_loaded = 0;
	
	RGFW_window* win = malloc(sizeof(RGFW_window));

	if (RGFW_FULLSCREEN & args){
		unsigned int* r = RGFW_getScreenSize(win);
		x = 0;
		y = 0;
		w = r[0];
		h = r[1];
	}

	win->srcX = win->x = x;
	win->srcY = win->y = y;
	win->srcW = win->w = w;
	win->srcH = win->h = h;
	win->srcName = win->name = name;
	win->fpsCap = 0;
	win->inFocus = 0;
	win->hideMouse = 0;
	win->event.type = 0;
	win->event.droppedFiles = NULL;
	win->event.droppedFilesCount = 0;
	win->valid = 245;

	NSBackingStoreType macArgs = NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSBackingStoreBuffered | NSWindowStyleMaskTitled;

	if (!(RGFW_NO_RESIZE & args))
		macArgs |= NSWindowStyleMaskResizable;
	if (!(RGFW_NO_BORDER & args))
		macArgs |= NSWindowStyleMaskTitled;
	else
		macArgs |= NSWindowStyleMaskBorderless;


	win->window = NSWindow_init(NSMakeRect(x, y, w, h), macArgs, false, NULL);
	NSWindow_setTitle(win->window, name);

	NSOpenGLPixelFormatAttribute attributes[] = {
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		0, 0, 0
	};

	if (RGFW_majorVersion >= 4 || RGFW_majorVersion  >= 3) {
		attributes[11] = NSOpenGLPFAOpenGLProfile;
        attributes[12] = (RGFW_majorVersion  >= 4) ? NSOpenGLProfileVersion4_1Core : NSOpenGLProfileVersion3_2Core;
	}

	#ifdef RGFW_GL
	NSOpenGLPixelFormat* format = NSOpenGLPixelFormat_initWithAttributes(attributes);
	win->view = NSOpenGLView_initWithFrame(NSMakeRect(0, 0, w, h), format);
	NSOpenGLView_prepareOpenGL(win->view);

	RGFW_swapInterval(win, 1);
	#endif

	if (RGFW_ALLOW_DND & args) {
		siArray(NSPasteboardType) array = si_array_init((NSPasteboardType[]){NSPasteboardTypeURL, NSPasteboardTypeFileURL, NSPasteboardTypeString}, sizeof(*array), 3);
	    NSView_registerForDraggedTypes(win->window, array);
		si_array_free(array);
	}

    if (RGFW_TRANSPARENT_WINDOW & args) {
		#ifdef RGFW_GL
		int opacity = 0;
		NSOpenGLContext_setValues(win->glWin, &opacity, NSOpenGLContextParameterSurfaceOpacity);
		#endif
		NSWindow_setOpaque(win->window, false);
		NSWindow_setBackgroundColor(win->window, NSColor_colorWithSRGB(0, 0, 0, 0));
		NSWindow_setAlphaValue(win->window, 0x00);
	}

	#ifdef RGFW_GL
	NSOpenGLContext_makeCurrentContext(win->glWin);
	#endif
	#ifdef RGFW_OSMESA
	win->glWin = OSMesaCreateContext(OSMESA_RGBA, NULL);
	win->buffer = malloc(w * h * 4);
	OSMesaMakeCurrent(win->glWin, win->buffer, GL_UNSIGNED_BYTE, w, h);

	#ifdef RGFW_GL
    win->render = 0;
	#endif
	#endif

	NSWindow_setContentView(win->window, (NSView*)win->view);
	NSWindow_setIsVisible(win->window, true);

	if (!RGFW_loaded) {
		NSWindow_makeMainWindow(win->window);

		RGFW_loaded = 1;
    }

	/* NOTE(EimaMei): Why does Apple hate good code? Like wtf, who thought of methods being a great idea???
	Imagine a universe, where MacOS had a proper system API (we would probably have like 20% better performance).
	*/
	si_func_to_SEL_with_name(SI_DEFAULT, "windowShouldClose", OnClose);

	/* NOTE(EimaMei): Fixes the 'Boop' sfx from constantly playing each time you click a key. Only a problem when running in the terminal. */
	si_func_to_SEL("NSWindow", acceptsFirstResponder);
	si_func_to_SEL("NSWindow", performKeyEquivalent);

	/* NOTE(EimaMei): Drag 'n Drop requires too many damn functions for just a Drag 'n Drop event. */
	si_func_to_SEL("NSWindow", draggingEntered);
	si_func_to_SEL("NSWindow", draggingUpdated);
	si_func_to_SEL("NSWindow", prepareForDragOperation);
	si_func_to_SEL("NSWindow", performDragOperation);


	RGFW_windows_size++;
	RGFW_windows = realloc(RGFW_windows, RGFW_windows_size);
	RGFW_windows[RGFW_windows_size - 1] = NULL;

	unsigned int i;
	for (i = 0; i < RGFW_windows_size; i++)
		if (!RGFW_windows[i]){
			RGFW_windows[i] = win;
			break;
		}

	NSApplication_sharedApplication(NSApp);
	NSApplication_setActivationPolicy(NSApp, NSApplicationActivationPolicyRegular);
	NSApplication_finishLaunching(NSApp);

	return win;
}

unsigned int* RGFW_getScreenSize(RGFW_window* win){
	static unsigned int RGFW_SreenSize[2];

	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_getScreenSize")) return (unsigned int[2]){0, 0};

	NSRect r = NSScreen_frame(NSScreen_mainScreen());

	RGFW_SreenSize[0] = r.size.width;
	RGFW_SreenSize[1] =  r.size.height;

	return (unsigned int[2]){r.size.width, r.size.height};
}

int* RGFW_getGlobalMousePoint(RGFW_window* win) {
	int RGFW_mousePoint[2];
`	RGFW_mousePoint[0] = win->event.x;
`	RGFW_mousePoint[1] = win->event.y;

	return RGFW_mousePoint; /* the point is loaded during event checks */
}

unsigned int RGFW_keysPressed[10]; /*10 keys at a time*/

RGFW_Event* RGFW_checkEvents(RGFW_window* win) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_checkEvents")) return NULL;

	if (win->event.droppedFiles != NULL){
		free(win->event.droppedFiles);
		win->event.droppedFiles = NULL;
	}
	win->event.droppedFilesCount = 0;

	win->inFocus = NSWindow_isKeyWindow(win->window);

	/* NOTE(EimaMei): This is super janky code, THANKS APPLE. For some reason it takes a few frames AFTER becoming focused to allow setting the cursor. */
	if (win->inFocus && win->cursor != NULL && win->cursor != NULL && (win->cursorChanged != 2 || NSCursor_currentCursor() != win->cursor)) {
		if (win->cursorChanged != 2)
			win->cursorChanged++;

		if (win->cursorChanged != 2 || NSCursor_currentCursor() != win->cursor)
			NSCursor_set(win->cursor);
	}

	NSEvent* e = NSApplication_nextEventMatchingMask(NSApp, NSEventMaskAny, NULL, 0, true);

	NSPoint point = NSEvent_mouseLocation(e);

	if (NSEvent_window(e) == win->window) {
		unsigned char button = 0, i;

		switch(NSEvent_type(e)){
			case NSEventTypeKeyDown:
				win->event.type = RGFW_keyPressed;
				win->event.keyCode = (unsigned short)NSEvent_keyCode(e);
				win->event.keyName = (char*)NSEvent_characters(e);

				RGFW_keyMap[win->event.keyCode] = 1;
				break;

			case NSEventTypeKeyUp:
				win->event.type = RGFW_keyReleased;
				win->event.keyCode = (unsigned short)NSEvent_keyCode(e);
				win->event.keyName = (char*)NSEvent_characters(e);

				RGFW_keyMap[win->event.keyCode] = 0;

				break;

			case NSEventTypeLeftMouseDown:
				button = 1;
				win->event.button = 2;
			case NSEventTypeOtherMouseDown:
				button = 1;
				if (!button)
				win->event.button = 1;
			case NSEventTypeRightMouseDown:
				if (!button)
				win->event.button = 0;

				win->event.type = RGFW_mouseButtonPressed;
				break;

			case NSEventTypeLeftMouseUp:
				button = 1;
				if (NSEvent_type(e) == NSEventTypeLeftMouseUp)
					win->event.button = RGFW_mouseLeft;
			case NSEventTypeOtherMouseUp:
				if (!button && NSEvent_type(e) == NSEventTypeOtherMouseUp)
				win->event.button = RGFW_mouseMiddle;
				button = 1;
			case NSEventTypeScrollWheel:
				if (!button && NSEvent_type(e) == NSEventTypeScrollWheel){
					double deltaY = NSEvent_deltaY(e);

					if (deltaY > 0)
						win->event.button = RGFW_mouseScrollUp;

					else if (deltaY < 0)
						win->event.button = RGFW_mouseScrollDown;
				}
				button = 1;
			case NSEventTypeRightMouseUp:
				if (!button)
				win->event.button = RGFW_mouseRight;

				win->event.type = RGFW_mouseButtonReleased;
				break;

			case NSEventTypeMouseMoved:
				win->event.type = RGFW_mousePosChanged;

				NSPoint p = NSEvent_locationInWindow(e);
				win->event.x = p.x;
				win->event.y = p.y;
				break;


			default: break;
		}

		if (win->cursorChanged && NSPointInRect(NSEvent_mouseLocation(e), NSWindow_frame(win->window))) {
			if (win->cursor == NULL)
				CGDisplayHideCursor(kCGDirectMainDisplay);
			else {
				CGDisplayShowCursor(kCGDirectMainDisplay);

				if (win->cursor == NULL) 
					NSCursor_set(NSCursor_arrowCursor());
				else 
					NSCursor_set(win->cursor);
			}

			win->cursorChanged = false;
		}
	}

	NSApplication_sendEvent(NSApp, e);

	NSRect r = NSWindow_frame(win->window);

	if (r.origin.x != win->srcX || r.origin.y != win->srcY || r.size.width != win->srcW || r.size.height != win->srcH){
		win->srcX = win->x = r.origin.x;
		win->srcY = win->y = r.origin.y;
		win->srcW = win->w = r.size.width;
		win->srcH = win->h = r.size.height;
	}

	else if (win->x != win->srcX || win->y != win->srcY ||
			win->w != win->srcW || win->h != win->srcH){

		NSWindow_setFrameAndDisplay(win->window, NSMakeRect(win->x, win->y, win->w, win->h), true, true);

		win->srcX = win->x;
		win->srcY = win->y;
		win->srcW = win->w;
		win->srcH = win->h;
	}

	if (win->srcName != win->name){
		win->srcName = win->name;
		NSWindow_setTitle(win->window, win->name);
	}

	NSApplication_updateWindows(NSApp);

	RGFW_checkFPS(win);

	if (win->event.type)
		return &win->event;
	else
		return NULL;
}

void RGFW_setIcon(RGFW_window* win, unsigned char* data, int width, int height, int channels) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_setIcon")) return;

	/* code by EimaMei  */
    // Make a bitmap representation, then copy the loaded image into it.
    NSBitmapImageRep* representation = NSBitmapImageRep_initWithBitmapData(NULL, width, height, 8, channels, (channels == 4), false, "NSCalibratedRGBColorSpace", NSBitmapFormatAlphaNonpremultiplied, width * channels, 8 * channels);
    memcpy(NSBitmapImageRep_bitmapData(representation), data, width * height * channels);

    // Add ze representation.
    NSImage* dock_image = NSImage_initWithSize(NSMakeSize(width, height));
    NSImage_addRepresentation(dock_image, (NSImageRep*)representation);

    // Finally, set the dock image to it.
    NSApplication_setApplicationIconImage(NSApp, dock_image);

    // Free the garbage.
    release(dock_image);
    release(representation);
}

void RGFW_setMouse(RGFW_window* win, unsigned char* image, int width, int height, int channels) {
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_setMouse")) return;

	if (image == NULL) {
		NSCursor_set(NSCursor_arrowCursor());
		win->cursor = NULL;

		return ;
	}

	if (win->cursor != NULL && win->cursor != NULL)
		release(win->cursor);

	/* NOTE(EimaMei): Code by yours truly. */
    // Make a bitmap representation, then copy the loaded image into it.
    NSBitmapImageRep* representation = NSBitmapImageRep_initWithBitmapData(NULL, width, height, 8, channels, (channels == 4), false, "NSCalibratedRGBColorSpace", NSBitmapFormatAlphaNonpremultiplied, width * channels, 8 * channels);
    memcpy(NSBitmapImageRep_bitmapData(representation), image, width * height * channels);

    // Add ze representation.
    NSImage* cursor_image = NSImage_initWithSize(NSMakeSize(width, height));
    NSImage_addRepresentation(cursor_image, (NSImageRep*)representation);

    // Finally, set the cursor image.
    NSCursor* cursor = NSCursor_initWithImage(cursor_image, NSMakePoint(0, 0));

	win->cursor = cursor;
	win->cursorChanged = true;

    // Free the garbage.
    release(cursor_image);
    release(representation);
}

void RGFW_hideMouse(RGFW_window* win) {
	if (win->cursor != NULL && win->cursor != NULL)
		release(win->cursor);
	
	win->cursor = -1;
	win->cursorChanged = true;
}

void RGFW_setMouseDefault(RGFW_window* win) {
	if (win->cursor != NULL && win->cursor != NULL)
		release(win->cursor);
	
	win->cursor = NULL;
	win->cursorChanged = true;
}

unsigned char RGFW_isPressedI(RGFW_window* win, unsigned int key) {
	if (key >= 128){
		#ifdef RGFW_PRINT_ERRORS
		printf("RGFW_isPressedI : invalid keycode\n");
		#endif
		RGFW_error = 1;
	}

	return RGFW_keyMap[key];
}

char* RGFW_readClipboard(RGFW_window* win){ return (char*)NSPasteboard_stringForType(NSPasteboard_generalPasteboard(), NSPasteboardTypeString); }

void RGFW_writeClipboard(RGFW_window* win, char* text) {
	siArray(NSPasteboardType) array = si_array_init((NSPasteboardType[]){NSPasteboardTypeString}, sizeof(*array), 1);
	NSPasteBoard_declareTypes(NSPasteboard_generalPasteboard(), array, NULL);

	NSPasteBoard_setString(NSPasteboard_generalPasteboard(), text, NSPasteboardTypeString);
	si_array_free(array);
}

unsigned short RGFW_registerJoystick(RGFW_window* win, int jsNumber){


	return RGFW_registerJoystickF(win, (char*)"");
}

unsigned short RGFW_registerJoystickF(RGFW_window* win, char* file){
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_registerJoystick")) return 0;

	return win->joystickCount - 1;
}

void RGFW_closeWindow(RGFW_window* win){
	if (!RGFW_ValidWindowCheck(win, (char*)"RGFW_closeWindow")) return;

	release(win->view);

	if (win->cursor != NULL && win->cursor != NULL)
		release(win->cursor);

	unsigned int i;
	for (i = 0; i < RGFW_windows_size; i++)
		if (RGFW_windows[i]->window == win->window){
			RGFW_windows[i] = NULL;
			break;
		}

	if (!i){
		RGFW_windows_size = 0;
		free(RGFW_windows);
	}
	

	NSApplication_terminate(NSApp, (id)win->window);

	if (win->event.droppedFiles != NULL){
		free(win->event.droppedFiles);
		win->event.droppedFiles = NULL;
	}
}
#endif

#if defined(__unix__) || defined(__APPLE__)

#include <pthread.h>

RGFW_thread RGFW_createThread(void* (*function_ptr)(void*), void* args) {
	RGFW_thread t;
	pthread_create((pthread_t*)&t, NULL, *function_ptr, NULL);
	return t;
}
void RGFW_cancelThread(RGFW_thread thread) { pthread_cancel((pthread_t)thread); }
void RGFW_joinThread(RGFW_thread thread) { pthread_join((pthread_t)thread, NULL); }
#ifdef __linux__
void RGFW_setThreadPriority(RGFW_thread thread, unsigned char priority) { pthread_setschedprio(thread, priority); }
#endif
#endif


void RGFW_makeCurrent_OpenGL(RGFW_window* win) {
	#ifdef RGFW_GL
		#ifdef RGFW_X11
			glXMakeCurrent((Display *)win->display, (Drawable)win->window, (GLXContext)win->glWin);
		#endif
		#ifdef _WIN32
			wglMakeCurrent((HDC)win->window, (HGLRC)win->glWin);
		#endif
		#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
		NSOpenGLContext_makeCurrentContext(win->glWin);
		#endif
	#else
	#ifdef RGFW_EGL
	eglMakeCurrent(win->EGL_display, win->EGL_surface, win->EGL_surface, win->glWin);
	#endif
	#endif

}

void RGFW_makeCurrent(RGFW_window* win) {
    #ifdef RGFW_OSMESA
	#ifdef RGFW_GL
	if (!win->render)
	#endif
	#endif
	RGFW_makeCurrent_OpenGL(win);
}

void RGFW_swapInterval(RGFW_window* win, int swapInterval) { 
	#ifdef RGFW_GL
	#ifdef RGFW_X11
	((PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress((GLubyte*)"glXSwapIntervalEXT"))((Display*)win->display, (Window)win->window, swapInterval); 
	#endif
	#ifdef _WIN32
	wglSwapIntervalEXT(swapInterval);
	#endif
	#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
	win->glWin = NSOpenGLView_openGLContext(win->view);
	NSOpenGLContext_setValues(win->glWin, &swapInterval, NSOpenGLContextParameterSwapInterval);
	#endif
	#endif

	#ifdef RGFW_EGL
	eglSwapInterval(win->EGL_display, swapInterval);
	#endif
}

void RGFW_swapBuffers(RGFW_window* win) {
	RGFW_makeCurrent(win);

	#ifdef RGFW_EGL
	eglSwapBuffers(win->EGL_display, win->EGL_surface);
	#else
	#if defined(RGFW_X11) && defined(RGFW_GL)
	glXSwapBuffers((Display*)win->display, (Window)win->window);
	#endif
	#ifdef _WIN32
	SwapBuffers((HDC)win->window);
	#endif
	#if defined(__APPLE__) && !defined(RGFW_MACOS_X11)
	NSOpenGLContext_flushBuffer(win->glWin);
	#endif
	#endif



	/* clear the window*/


	#ifdef RGFW_OSMESA
    if (win->render) {
		unsigned char* row = (unsigned char*) malloc(win->srcW * 4);
		int half_height = win->srcH / 2;
		int stride = win->srcW * 4;

		int y;
		for (y = 0; y < half_height; ++y) {
			int top_offset = y * stride;
			int bottom_offset = (win->srcH - y - 1) * stride;
			memcpy(row, win->buffer + top_offset, stride);
			memcpy(win->buffer + top_offset, win->buffer + bottom_offset, stride);
			memcpy(win->buffer + bottom_offset, row, stride);
		}

		free(row);
		#ifdef RGFW_X11
			RGFW_omesa_ximage = XCreateImage(win->display, DefaultVisual(win->display, XDefaultScreen(win->display)), DefaultDepth(win->display, XDefaultScreen(win->display)),
								ZPixmap, 0, (char*)win->buffer, win->srcW, win->srcH, 32, 0);

			XPutImage(win->display, win->window, XDefaultGC(win->display, XDefaultScreen(win->display)), RGFW_omesa_ximage, 0, 0, 0, 0, win->srcW, win->srcH);
		#endif
		#ifdef _WIN32
			HBITMAP hbitmap = CreateBitmap(500, 500, 1, 32, (void*)win->buffer);
			HDC hdcMem = CreateCompatibleDC(win->window);
			HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hbitmap);
			BitBlt(win->window, 0, 0, 500, 500, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, hOld);
			DeleteDC(hdcMem);
		#endif
		#ifdef defined(__APPLE__) && !defined(RGFW_MACOS_X11)
		CGRect rect = CGRectMake (0, 0, win->srcW, win->srcH);// 2
		struct CGColorSpace* colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

		CGContextRef bitmapContext = CGBitmapContextCreate (win->buffer,
										win->srcW,
										win->srcH,
										4,
										(win->srcW * 4),
										colorSpace,
										kCGImageAlphaPremultipliedLast);

		CGColorSpaceRelease( colorSpace );

		struct CGImage* myImage = CGBitmapContextCreateImage(bitmapContext);

		CGContextDrawImage(NSGraphicsContext_currentContext(), rect, myImage);

		CGContextRelease(bitmapContext);
		CGImageRelease(myImage);
		#endif
	}
	#endif

	#ifdef RGFW_VULKAN
	#ifdef RGFW_PRINT_ERRORS
	printf("RGFW_swapBuffers %s\n", "RGFW_swapBuffers is not yet supported for Vulkan");
	RGFW_error = 1;
	#endif
	#endif
}

time_t startTime[2];
int frames = 0;

void RGFW_checkFPS(RGFW_window* win) {
	/* get current fps*/
	frames++;

	unsigned int seconds = time(0) - startTime[0];

	if (seconds) {
		win->fps = frames / seconds;

		frames = 0;
		startTime[0] = time(0);
	}


	/*slow down to the set fps cap*/
	if (win->fpsCap) {
		time_t currentTime = time(0);
		time_t elapsedTime = currentTime - startTime[1];

		int sleepTime = 1000/(win->fpsCap) - elapsedTime;
		if (sleepTime > 0) {
			RGFW_Timespec sleep_time = { sleepTime / 1000, (unsigned int)((sleepTime % 1000) * 1000000) };
			nanosleep((struct timespec*)&sleep_time, NULL);
		}

		startTime[1] = time(0);
	}
}

#endif /*RGFW_IMPLEMENTATION*/

#ifdef __cplusplus
}
#endif

#define RGFW_Escape RGFW_keyStrToKeycode("Escape")
#define RGFW_F1 RGFW_keyStrToKeycode("F1")
#define RGFW_F2 RGFW_keyStrToKeycode("F2")
#define RGFW_F3 RGFW_keyStrToKeycode("F3")
#define RGFW_F4 RGFW_keyStrToKeycode("F4")
#define RGFW_F5 RGFW_keyStrToKeycode("F5")
#define RGFW_F6 RGFW_keyStrToKeycode("F6")
#define RGFW_F7 RGFW_keyStrToKeycode("F7")
#define RGFW_F8 RGFW_keyStrToKeycode("F8")
#define RGFW_F9 RGFW_keyStrToKeycode("F9")
#define RGFW_F10 RGFW_keyStrToKeycode("F10")
#define RGFW_F11 RGFW_keyStrToKeycode("F11")
#define RGFW_F12 RGFW_keyStrToKeycode("F12")
#define RGFW_F13 RGFW_keyStrToKeycode("F13")
#define RGFW_F14 RGFW_keyStrToKeycode("F14")
#define RGFW_F15 RGFW_keyStrToKeycode("F15")
#define RGFW_Backtick RGFW_keyStrToKeycode("Backtick")
#define RGFW_0 RGFW_keyStrToKeycode("0")
#define RGFW_1 RGFW_keyStrToKeycode("1")
#define RGFW_2 RGFW_keyStrToKeycode("2")
#define RGFW_3 RGFW_keyStrToKeycode("3")
#define RGFW_4 RGFW_keyStrToKeycode("4")
#define RGFW_5 RGFW_keyStrToKeycode("5")
#define RGFW_6 RGFW_keyStrToKeycode("6")
#define RGFW_7 RGFW_keyStrToKeycode("7")
#define RGFW_8 RGFW_keyStrToKeycode("8")
#define RGFW_9 RGFW_keyStrToKeycode("9")
#define RGFW_Minus RGFW_keyStrToKeycode("Minus")
#define RGFW_Equals RGFW_keyStrToKeycode("Equals")
#define RGFW_BackSpace RGFW_keyStrToKeycode("BackSpace")
#define RGFW_Tab RGFW_keyStrToKeycode("Tab")
#define RGFW_CapsLock RGFW_keyStrToKeycode("CapsLock")
#define RGFW_ShiftL RGFW_keyStrToKeycode("ShiftL")
#define RGFW_ControlL RGFW_keyStrToKeycode("ControlL")
#define RGFW_AltL RGFW_keyStrToKeycode("AltL")
#define RGFW_SuperL RGFW_keyStrToKeycode("SuperL")
#define RGFW_ShiftR RGFW_keyStrToKeycode("ShiftR")
#define RGFW_ControlR RGFW_keyStrToKeycode("ControlR")
#define RGFW_AltR RGFW_keyStrToKeycode("AltR")
#define RGFW_SuperR RGFW_keyStrToKeycode("SuperR")
#define RGFW_Space RGFW_keyStrToKeycode("Space")
#define RGFW_A RGFW_keyStrToKeycode("A")
#define RGFW_B RGFW_keyStrToKeycode("B")
#define RGFW_C RGFW_keyStrToKeycode("C")
#define RGFW_D RGFW_keyStrToKeycode("D")
#define RGFW_E RGFW_keyStrToKeycode("E")
#define RGFW_F RGFW_keyStrToKeycode("F")
#define RGFW_G RGFW_keyStrToKeycode("G")
#define RGFW_H RGFW_keyStrToKeycode("H")
#define RGFW_I RGFW_keyStrToKeycode("I")
#define RGFW_J RGFW_keyStrToKeycode("J")
#define RGFW_K RGFW_keyStrToKeycode("K")
#define RGFW_L RGFW_keyStrToKeycode("L")
#define RGFW_M RGFW_keyStrToKeycode("M")
#define RGFW_N RGFW_keyStrToKeycode("N")
#define RGFW_O RGFW_keyStrToKeycode("O")
#define RGFW_P RGFW_keyStrToKeycode("P")
#define RGFW_Q RGFW_keyStrToKeycode("Q")
#define RGFW_R RGFW_keyStrToKeycode("R")
#define RGFW_S RGFW_keyStrToKeycode("S")
#define RGFW_T RGFW_keyStrToKeycode("T")
#define RGFW_U RGFW_keyStrToKeycode("U")
#define RGFW_V RGFW_keyStrToKeycode("V")
#define RGFW_W RGFW_keyStrToKeycode("W")
#define RGFW_X RGFW_keyStrToKeycode("X")
#define RGFW_Y RGFW_keyStrToKeycode("Y")
#define RGFW_Z RGFW_keyStrToKeycode("Z")
#define RGFW_Period RGFW_keyStrToKeycode("Period")
#define RGFW_Comma RGFW_keyStrToKeycode("Comma")
#define RGFW_Slash RGFW_keyStrToKeycode("Slash")
#define RGFW_Bracket RGFW_keyStrToKeycode("Bracket")
#define RGFW_CloseBracket RGFW_keyStrToKeycode("CloseBracket")
#define RGFW_Semicolon RGFW_keyStrToKeycode("Semicolon")
#define RGFW_Return RGFW_keyStrToKeycode("Return")
#define RGFW_Quote RGFW_keyStrToKeycode("Quote")
#define RGFW_BackSlash RGFW_keyStrToKeycode("BackSlash")
#define RGFW_Up RGFW_keyStrToKeycode("Up")
#define RGFW_Down RGFW_keyStrToKeycode("Down")
#define RGFW_Left RGFW_keyStrToKeycode("Left")
#define RGFW_Right RGFW_keyStrToKeycode("Right")
#define RGFW_Delete RGFW_keyStrToKeycode("Delete")
#define RGFW_Insert RGFW_keyStrToKeycode("Insert")
#define RGFW_End RGFW_keyStrToKeycode("End")
#define RGFW_Home RGFW_keyStrToKeycode("Home")
#define RGFW_PageUp RGFW_keyStrToKeycode("PageUp"1)
#define RGFW_PageDown RGFW_keyStrToKeycode("PageDown")
#define RGFW_Numlock RGFW_keyStrToKeycode("Numlock")
#define RGFW_KP_Slash RGFW_keyStrToKeycode("KP_Slash")
#define RGFW_Multiply RGFW_keyStrToKeycode("KP_Multiply")
#define RGFW_KP_Minus RGFW_keyStrToKeycode("KP_Minus")
#define RGFW_KP_1 RGFW_keyStrToKeycode("KP_1")
#define RGFW_KP_2 RGFW_keyStrToKeycode("KP_2")
#define RGFW_KP_3 RGFW_keyStrToKeycode("KP_3")
#define RGFW_KP_4 RGFW_keyStrToKeycode("KP_4")
#define RGFW_KP_5 RGFW_keyStrToKeycode("KP_5")
#define RGFW_KP_6 RGFW_keyStrToKeycode("KP_6")
#define RGFW_KP_7 RGFW_keyStrToKeycode("KP_7")
#define RGFW_KP_8 RGFW_keyStrToKeycode("KP_8")
#define RGFW_KP_9 RGFW_keyStrToKeycode("KP_9")
#define RGFW_KP_0 RGFW_keyStrToKeycode("KP_0")
#define RGFW_KP_Period RGFW_keyStrToKeycode("KP_Period")
#define RGFW_KP_Return RGFW_keyStrToKeycode("KP_Return")
