#include "fgl.h"

fgl_vec2i size = {1024, 600};

int main()
{
	fgl_open_window(size.x, size.y, "fgl example 03 - basic mouse input");
	fgl_font fnt = fgl_load_font("res/font.ttf");
	fgl_circle circ = (fgl_circle){ fgl_get_mouse_x(), fgl_get_mouse_y(), 40 };
	fgl_color color = FGL_RED;
	int mode = 0;

	while (fgl_update_window()) {
		if (fgl_is_window_resized()) size = fgl_get_window_size();
		if (fgl_is_key_pressed(KEY_V))      fgl_window_flip_vsync();

		// updating ball movement
		circ.x = fgl_get_mouse_x();
		circ.y = fgl_get_mouse_y();

		// update ball's color depending on click
		if (fgl_is_mouse_pressed(MOUSE_LEFT))
			color = FGL_RED;
		if (fgl_is_mouse_pressed(MOUSE_RIGHT))
			color = FGL_BLUE;
		if (fgl_is_mouse_pressed(MOUSE_MIDDLE)) {
			mode++;

			if (mode > 1) {
				mode = 0;
			}
		}

		fgl_start_drawing();
			fgl_set_background(FGL_WHITE);
			mode == 0 ? fgl_draw_circle(circ.x, circ.y, circ.radius, color) : fgl_draw_circle_outline(circ.x, circ.y, circ.radius, color);
			fgl_draw_text(fnt, "press the mouse buttons to change the color of the ball", 10, 10, 30, FGL_DARKGRAY);
			fgl_draw_text(fnt, "press the middle mouse button to see the outline of the ball", 10, 50, 30, FGL_DARKGRAY);
		fgl_stop_drawing();
	}

	fgl_close_window();
	exit(EXIT_SUCCESS);
}
