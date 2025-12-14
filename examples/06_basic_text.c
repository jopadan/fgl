#include "fgl.h"

static fgl_vec2i  size  = {1024, 600};

int main()
{
	fgl_open_window(size.x, size.y, "fgl example 06 - basic text");
	fgl_font fnt = fgl_load_font("res/font.ttf");
	fgl_font fnt2 = fgl_load_font("res/font2.ttf");
	int val1 = fgl_random_int(-3, 9);
	int val2 = 0;
	fgl_color color = (fgl_color){ fgl_random_int(0, 255), fgl_random_int(0, 255), fgl_random_int(0, 255), 255 };

	while (fgl_update_window()) {
		if (fgl_is_window_resized()) size = fgl_get_window_size();
		if (fgl_is_key_pressed(KEY_V))      fgl_window_flip_vsync();

		// updating contents
		if ((fgl_window_fps() != 0 ? fgl_window_frames() % fgl_window_fps() : fgl_window_frames() % 60) == 0) {
			val1 = fgl_random_int(-3, 9);
			color = (fgl_color){ fgl_random_int(0, 255), fgl_random_int(0, 255), fgl_random_int(0, 255), 255 };
		}

		// drawing
		fgl_start_drawing();
			fgl_set_background(FGL_WHITE);
			fgl_draw_text(fnt, "dealing with text", 10, 10, 30, FGL_DARKGRAY);
			fgl_draw_text(fnt, "this is a normal text, and it can have any font previously specified (ttf only)", 10, 40, 30, FGL_DARKGRAY);
			fgl_draw_text(fnt2, "quite 8-bit, isn't it?", 10, 70, 30, color);
			fgl_draw_text(fnt, fgl_format_text("you can also have values updating: %d", fgl_window_frames()), 10, 110, 30, FGL_DARKGRAY);
			fgl_draw_text(fnt, fgl_format_text("and random values: %d", val1), 10, 140, 30, FGL_DARKGRAY);
			fgl_draw_text(fnt, "with colorful texts as well", 40, 200, 80, color);
			fgl_draw_text(fnt, "and big texts!", 40, size.y - 200, 120, FGL_BLACK);
			fgl_draw_text(fnt, fgl_window_fps_string(), size.x - 60, 3, 13, FGL_BLACK);
		fgl_stop_drawing();
	}

	fgl_close_window();
	exit(EXIT_SUCCESS);
}
