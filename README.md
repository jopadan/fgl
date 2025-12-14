![](./resources/logo.png)

# Funky Graphics Library
an orthographic [OpenGL][3] 2D/3D graphics platform framework [C23][1] library following the [KISS][5] design principle using [glfw][4]
## features
- orthographic 2D [OpenGL][3] graphics
- intervall timer based fps performance counter
- [glfontstash][6] and [stbtruetype][7] based font text rendering
- simple shapes drawing such as pixels, triangles, lines and rectangles
- easy sprite rendering and loading
- simplified HID device input based on [glfw][4]
## dependencies
- [C23][1]
- [CMake][2]
- [OpenGL][3]
- [glfw][4]
## building and installing library
```sh
git clone --recurse-submodules --recursive https://github.com/mrfunkdude/fgl.git
cd fgl
cmake . --install-prefix=/usr
make && sudo make install
```
## example application
this example will create a resizable window with a 2D HUD system information display.
Pressing 'V' enables/disables vsync, causing effect artifacts of the rotating blue
quad time string clock hand.

```c
#include "fgl.h"

fgl_vec2i size = {1024, 600};
fgl_font fnt;

int main()
{
	const char* sysinfo = fgl_sysinfo_string();
	fgl_open_window(size.x, size.y, "fgl example 01 - basic sysinfo");
	fnt = fgl_load_font("res/font.ttf");

	while (fgl_update_window()) {
		if (fgl_is_window_resized()) size = fgl_get_window_size();
		if (fgl_is_key_pressed(KEY_V))      fgl_window_flip_vsync();

		fgl_start_drawing();
			fgl_set_background(FGL_BLACK);

			// rotating blue quad with time string as clock hand
			glPushMatrix();
				glTranslatef((float)(size.x>>1),(float)(size.y>>1),0.0f);
				glRotatef(fgl_window_frames(),0.0f,0.0f,1.0f);
				fgl_draw_rectangle(-20,-20,40,40,FGL_BLUE);
				fgl_draw_text(fnt, fgl_time_string(), 0, -4, 13, FGL_WHITE);
			glPopMatrix();

			// ortho 2d text hud
			glPushMatrix();
				fgl_draw_text(fnt, fgl_time_string(), 0, 3, 13, FGL_WHITE);
				fgl_draw_text(fnt, fgl_window_fps_string(), size.x - 60, 3, 13, FGL_WHITE);
				fgl_draw_text(fnt, sysinfo, 0, size.y - 13, 13, FGL_WHITE);
			glPopMatrix();
		fgl_stop_drawing();
	}

	fgl_close_window();
	exit(EXIT_SUCCESS);
}
```
once installed the library, run the following command
```sh
gcc your_source.c -lFGL -o your_binary
```
[1]: https://www.open-std.org/JTC1/SC22/WG14/
[2]: https://cmake.org/
[3]: https://www.opengl.org/
[4]: https://www.glfw.org/
[5]: https://en.wikipedia.org/wiki/KISS_principle/
[6]: https://github.com/memononen/fontstash
[7]: https://github.com/nothings/stb
