/*
mrfunkdude (c) 2021
fgl v1.0: text
Text functions implementation

Contents:
- loading fonts
- drawing text
- embedding variables to a renderable text
*/

#include "fgl.h"

#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"

#define GLFONTSTASH_IMPLEMENTATION
#include "glfontstash.h"

#define MAX_BUFFERS         4
#define BUFFER_LEN       1024

// -------------------
// Module-specific functions and variables
extern int load_text_environment();         // Load fontstash env
extern int unload_text_environment();       // Close fontstash env
static FONScontext *fs = NULL;

// -------------------
// Implementations

// Load a font into memory
int fgl_load_font(const char *path)
{
    int fn = FONS_INVALID;
    fn = fonsAddFont(fs, "font", path);
    if (fn == FONS_INVALID) {
        printf("[FGL][ERR] [FNT][LDR] %s\n", path);
    }

    printf("[FGL][NFO] [FNT][LDR] %s\n", path);

    return fn;
}

// Draw a piece of text
void fgl_draw_text(fgl_font font, const char *text, int x, int y, float size, fgl_color color)
{
    unsigned int c = glfonsRGBA(color.r, color.g, color.b, color.a);

    fonsClearState(fs);
    fonsSetFont(fs, font);

    fonsSetSize(fs, size);
    fonsSetColor(fs, c);
    fonsDrawText(fs, x, y+size-6, text, NULL);
}

// Embed variables to a text
const char* fgl_format_text(const char *text, ...)
{
    static char buffers[MAX_BUFFERS * BUFFER_LEN] = {};

    va_list args;
    va_start(args, text);
    buffers[vsnprintf(buffers, sizeof(buffers), text, args)] = '\0';
    va_end(args);
    return buffers;
}

// -------------------
// Load fontstash env
extern int load_text_environment()
{
    fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
    if (fs == NULL) {
        printf("[FGL][ERR] [FNT][BEG] fontstash\n");
        return -1;
    }

    printf("[FGL][NFO] [FNT][BEG] fontstash\n");
}

// Close fontstash env
extern int unload_text_environment()
{
    glfonsDelete(fs);
    printf("[FGL][NFO] [FNT][END] fontstash\n");
}
