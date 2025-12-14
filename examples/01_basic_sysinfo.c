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
