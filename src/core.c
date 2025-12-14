/*
mrfunkdude (c) 2021
fgl v1.0: core
Core functions implementation

Contents:
- window functions
- basic drawing functions
- input functions
- miscellaneous functions
*/

#include "fgl.h"

typedef struct fgl_timer {
	timer_t                      id;
	long long                  freq;
	struct {
		sigset_t           mask;
		struct sigevent   event;
		struct sigaction action;
	} signal;
	struct itimerspec           its;
	struct {
		unsigned long     count;
		unsigned long      last;
		unsigned long      rate;
		char         string[12];
	} frame;
	struct {
		time_t             last;
		char        string[128];
	} time;
} fgl_timer;

// ---------------------
// Module-specific functions and variables
static GLFWwindow *window;
static int    current_key_state[512] = {};
static int   previous_key_state[512] = {};
static int  current_mouse_state[3]   = {};
static int previous_mouse_state[3]   = {};
static bool size_changed = false;
static bool vsync = false;
static fgl_vec2i size = {};
static fgl_timer timer;

bool fgl_update_window()
{
	if(fgl_is_window_closed()) return false;
	timer.frame.count++;
	return true;
}

bool fgl_window_flip_vsync()
{
	glfwMakeContextCurrent(window);
	glfwSwapInterval((int)((!vsync)&0b1));
	return (vsync=(!vsync));
}

bool fgl_is_window_resized()
{
	if(!size_changed)
		return false;
	size_changed = false;
	return true;
}

fgl_vec2i fgl_get_window_size() { return size; }
fgl_vec2i fgl_resize_window(int w, int h)
{
    if(w > 0 && h > 0)
    {
	glfwSetWindowSize(window, w, h);
	glfwGetFramebufferSize(window, &size.x, &size.y);
    }
    return size;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

static void size_callback(GLFWwindow *window, int width, int height)
{
	size_changed = true;
	size.x = width;
	size.y = height;
	glViewport(0, 0, size.x, size.y);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);   // black by default

	glEnable(GL_BLEND);     // enable color blending; required to work with transparencies
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();   // reset the projection matrix
	glOrtho(0, size.x, size.y, 0, 0, 1);     // top left corner is 0,0

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   // reset the modelview matrix
	printf("[FGL][NFO] [WIN][RES] GLFW %dx%d\n", width, height);
}

// -------------------
// External functions
extern int load_text_environment();         // Load fontstash env
extern int unload_text_environment();       // Close fontstash env

// -------------------------
// Implementations

unsigned long fgl_window_frames()      { return timer.frame.count;  }
unsigned long fgl_window_fps()         { return timer.frame.rate;   }
const char*   fgl_sysinfo_string()
{
	static char dst[256] = {}; struct utsname         uts; uname(&uts);
	dst[snprintf(dst, 256, "%s %s %s", uts.sysname, uts.release, uts.machine)] = '\0';
	return dst;
}
const char*   fgl_time_string()        { return timer.time.string;  }
const char*   fgl_window_fps_string()  { return timer.frame.string; }

static void timer_callback(int sig, siginfo_t *si, void *uc)
{
	fgl_timer* timer     = ((fgl_timer*)si->si_value.sival_ptr);
	timer->frame.rate    = timer->frame.count - timer->frame.last;
	timer->frame.last    = timer->frame.count;
	timer->time.last     = time(NULL);
	char* temp           = ctime(&timer->time.last);
	temp[strlen(temp)-1] = '\0';
	strncpy(timer->frame.string, fgl_format_text("FPS: %6u", timer->frame.rate), 11);
	strncpy(timer->time.string, fgl_format_text("%s", temp), 127);
}

static bool init_timer(long nsec)
{
	bool result = false;
	timer.signal.action.sa_flags = SA_SIGINFO;
	timer.signal.action.sa_sigaction = timer_callback;
	sigemptyset(&timer.signal.action.sa_mask);
	if(sigaction(SIGRTMIN, &timer.signal.action, NULL) == -1)
		goto error;

	sigemptyset(&timer.signal.mask);
	sigaddset(&timer.signal.mask, SIGRTMIN); 
	if(sigprocmask(SIG_SETMASK, &timer.signal.mask, NULL) == -1)
		goto error;

	timer.signal.event.sigev_notify = SIGEV_SIGNAL;
	timer.signal.event.sigev_signo = SIGRTMIN;
	timer.signal.event.sigev_value.sival_ptr = &timer;
	if(timer_create(CLOCK_TAI, &timer.signal.event, &timer.id) == -1)
		goto error;

	timer.freq                    = 1000000000;
	timer.its.it_value.tv_sec     = nsec / timer.freq;
	timer.its.it_value.tv_nsec    = nsec % timer.freq;
	timer.its.it_interval.tv_sec  = timer.its.it_value.tv_sec;
	timer.its.it_interval.tv_nsec = timer.its.it_value.tv_nsec;
	if(timer_settime(timer.id, 0, &timer.its, NULL) == -1)
		goto error;

	sleep(1);
	if(sigprocmask(SIG_UNBLOCK, &timer.signal.mask, NULL) == -1)
		goto error;

	result = true; printf("[FGL][NFO] [CLK][FPS] %lu.%lus\n", timer.its.it_value.tv_sec, timer.its.it_value.tv_nsec);
error:  return result;
}

// Initialize OpenGL context
int fgl_open_window(int w, int h, const char *title)
{
    // check for glfw initialization
    if (!glfwInit()) {
        printf("[FGL][ERR] [SYS][BEG] GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, false);

    // check for window initialization
    window = glfwCreateWindow(w, h, title, NULL, NULL);
    if (!window) {
        printf("[FGL][ERR] [WIN][BEG] GLFW %dx%d\n", w, h);
        glfwTerminate();
        return -2;
    }
    glfwGetFramebufferSize(window, &size.x, &size.y);
    // callbacks
    glfwSetFramebufferSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
    fgl_window_flip_vsync();
    printf("[FGL][NFO] [WIN][BEG] GLFW %dx%d\n", w, h);

    // set up context
    size_changed = true;
    size.x = w;
    size.y = h;
    glViewport(0, 0, size.x, size.y);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);   // black by default

    glEnable(GL_BLEND);     // enable color blending; required to work with transparencies
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();   // reset the projection matrix
    glOrtho(0, size.x, size.y, 0, 0, 1);     // top left corner is 0,0

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();   // reset the modelview matrix
    srand(time(NULL));  // seed the random number generator
    load_text_environment();
    if(!init_timer(1000000000)) return -3;
    return 0;
}

// Close the OpenGL context
void fgl_close_window()
{
    timer_delete(timer.id);
    unload_text_environment();

    glfwDestroyWindow(window);
    glfwTerminate();
    printf("[FGL][NFO] [WIN][END] GLFW %dx%d\n", size.x, size.y);
}

// Check if the ESC key or close button is pressed
bool fgl_is_window_closed()
{
    return glfwWindowShouldClose(window);
}

// TODO: Returns a screenshot as RGBA8888 using the current resolution
fgl_vec4ub*   fgl_window_screenshot(const char *path)
{
	return NULL;
}

/* Drawing functions */

// Clear appropiate buffers
void fgl_start_drawing()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
}

// Swap buffers and poll events
void fgl_stop_drawing()
{
    glfwSwapBuffers(window);
    glfwPollEvents();

    glFlush();
}

// Set background to a color
void fgl_set_background(fgl_color color)
{
    // Clamp values from 0.0 to 1.0
    float r = (float)color.r / 255;
    float g = (float)color.g / 255;
    float b = (float)color.b / 255;
    float a = (float)color.a / 255;
    glClearColor(r, g, b, a);
}

/* Input functions: keyboard */

// Check if a key is not being held down
bool fgl_is_key_up(int key)
{
    if (glfwGetKey(window, key) == GLFW_RELEASE) return true;
    else return false;
}

// Check if a key is held down
bool fgl_is_key_down(int key)
{
    if (glfwGetKey(window, key) == GLFW_PRESS) return true;
    else return false;
}

// Check if a key is pressed once
bool fgl_is_key_pressed(int key)
{
    bool res = false;

    current_key_state[key] = fgl_is_key_down(key);
    if (current_key_state[key] != previous_key_state[key]) {
        if (current_key_state[key])
            res = true;
        previous_key_state[key] = current_key_state[key];
    } else {
        res = false;
    }

    return res;
}

// Check if a key was released
bool fgl_is_key_released(int key)
{
    bool res = false;

    current_key_state[key] = fgl_is_key_up(key);
    if (current_key_state[key] != previous_key_state[key]) {
        if (current_key_state[key])
            res = true;
        previous_key_state[key] = current_key_state[key];
    } else {
        res = false;
    }

    return res;
}

/* Input functions: mouse */

// Check if a mouse button is not being held down
bool fgl_is_mouse_up(int button)
{
    if (glfwGetMouseButton(window, button) == GLFW_RELEASE) return true;
    else return false;
}

// Check if a mouse button is held down
bool fgl_is_mouse_down(int button)
{
    if (glfwGetMouseButton(window, button) == GLFW_PRESS) return true;
    else return false;
}

// Check if a mouse button is pressed once
bool fgl_is_mouse_pressed(int button)
{
    bool res = false;

    current_mouse_state[button] = fgl_is_mouse_down(button);
    if (current_mouse_state[button] != previous_mouse_state[button]) {
        if (current_mouse_state[button])
            res = true;
        previous_mouse_state[button] = current_mouse_state[button];
    } else {
        res = false;
    }

    return res;
}

// Check if a mouse button was released
bool fgl_is_mouse_released(int button)
{
    bool res = false;

    current_mouse_state[button] = fgl_is_mouse_up(button);
    if (current_mouse_state[button] != previous_mouse_state[button]) {
        if (current_mouse_state[button])
            res = true;
        previous_mouse_state[button] = current_mouse_state[button];
    } else {
        res = false;
    }

    return res;
}

// Returns mouse X
int fgl_get_mouse_x()
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    return (int)x;
}

// Returns mouse Y
int fgl_get_mouse_y()
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    return (int)y;
}

// Returns mouse position
fgl_vec2d fgl_get_mouse_pos()
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    return (fgl_vec2d){ x, y };
}

/* Miscellaneous functions */

// Return a random integer
int fgl_random_int(int min, int max)
{
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }
    
    return (rand()%(abs(max-min)+1) + min);
}

