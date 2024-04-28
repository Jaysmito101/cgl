/*
MIT License

Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_NETWORKING
#define CGL_IMPLEMENTATION
#include "cgl.h"

#include "omp.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#define WINDOW_SIZE	800

#define SQ(x) ((x) * (x))

#define PREF(name, line) { \
	CGL_float start_time##__LINE__ = CGL_utils_get_time(); \
	line; \
	CGL_float end_time##__LINE__ = CGL_utils_get_time(); \
	CGL_info("%s: %f", name, end_time##__LINE__ - start_time##__LINE__); \
}

typedef struct
{
	CGL_int width;
    CGL_int height;
	CGL_float aspect_ratio;
    CGL_ubyte* data;
    CGL_texture* texture;
} ci_Image;


ci_Image ci_load_image(const char* filename)
{
	ci_Image image = {0};
	stbi_set_flip_vertically_on_load(CGL_TRUE);
	image.data = stbi_load(filename, &image.width, &image.height, NULL, 3);
	if (!image.data)
	{
		CGL_error("Failed to load image: %s", filename);
		return image;
	}
    CGL_image image_data = {0};
    image_data.width = image.width;
    image_data.height = image.height;
    image_data.data = image.data;
    image_data.channels = 3;
    image_data.bytes_per_channel = 8;
    image.texture = CGL_texture_create(&image_data);
	image.aspect_ratio = (CGL_float)image.width / (CGL_float)image.height;
	return image;
}

ci_Image ci_blank_image(CGL_int width, CGL_int height)
{
	ci_Image image = {0};
	image.width = width;
	image.height = height;
	image.data = (CGL_ubyte*)CGL_malloc(width * height * 3 * 4);
	if (!image.data)
	{
		CGL_error("Failed to allocate memory for image");
		return image;
	}
	memset(image.data, 0, width * height * 3 * 4);
	CGL_image image_data = {0};
	image_data.width = image.width;
	image_data.height = image.height;
	image_data.data = image.data;
	image_data.channels = 3;
	image_data.bytes_per_channel = 8;
	image.texture = CGL_texture_create(&image_data);
	image.aspect_ratio = (CGL_float)image.width / (CGL_float)image.height;
	return image;
}

void ci_upload_image(ci_Image* image)
{
	CGL_texture_set_data(image->texture, image->data);
}

void ci_recreate_texture(ci_Image* image)
{
	CGL_image image_data = {0};
	image_data.width = image->width;
	image_data.height = image->height;
	image_data.data = image->data;
	image_data.channels = 3;
	image_data.bytes_per_channel = 8;
	CGL_texture_destroy(image->texture);
	image->texture = CGL_texture_create(&image_data);
}

void ci_save_image(const char* filename, ci_Image image)
{
	stbi_write_png(filename, image.width, image.height, 3, image.data, image.width * 3);
}

void ci_free_image(ci_Image image)
{
	CGL_free(image.data);
	CGL_texture_destroy(image.texture);
}

CGL_void resize_normal(ci_Image* image, ci_Image* original_image, CGL_int target_width, CGL_int target_height);
typedef CGL_void(*ci_resize_func)(ci_Image*, ci_Image*, CGL_int, CGL_int);



struct {
	ci_Image original;
	ci_Image resized;
	ci_resize_func resize_func;
	CGL_bool loaded;

	CGL_int current;
} g_state;


CGL_void resize_normal(ci_Image* image, ci_Image* original_image, CGL_int target_width, CGL_int target_height)
{
	CGL_int new_size = target_width * target_height * 3;
	static CGL_ubyte tmp_buffer[6000 * 6000 * 3] = {0};

	CGL_ubyte* new_data = &tmp_buffer[0];

	#pragma omp parallel for collapse(2)
	for (CGL_int y = 0; y < target_height; y++)
	{
		for (CGL_int x = 0; x < target_width; x++)
		{
			CGL_float target_tx = (CGL_float)x / (CGL_float)target_width;
			CGL_float target_ty = (CGL_float)y / (CGL_float)target_height;

			CGL_int source_x = (CGL_int)(target_tx * original_image->width);
			CGL_int source_y = (CGL_int)(target_ty * original_image->height);


			CGL_ubyte* source_pixel = &original_image->data[(source_y * original_image->width  + source_x) * 3];

			new_data[(y * target_width + x) * 3 + 0] = source_pixel[0];
			new_data[(y * target_width + x) * 3 + 1] = source_pixel[1];
			new_data[(y * target_width + x) * 3 + 2] = source_pixel[2];
		}
	}

	memcpy(image->data, new_data, new_size);
	image->width = target_width;
	image->height = target_height;
	image->aspect_ratio = (CGL_float)target_width / (CGL_float)target_height;
	ci_recreate_texture(image);
}



CGL_float* ci_calculate_energy_map(ci_Image* image, CGL_float* energy_map, CGL_bool h_or_v)
{

	#pragma omp parallel for collapse(2)
	for (CGL_int y = 0; y < image->height; y++)
	{
		for (CGL_int x = 0; x < image->width; x++)
		{
			CGL_int left_x = x - 1;
			CGL_int right_x = x + 1;
			CGL_int up_y = y - 1;
			CGL_int down_y = y + 1;

			if (left_x < 0) left_x = 0;
			if (right_x >= image->width) right_x = image->width - 1;
			if (up_y < 0) up_y = 0;
			if (down_y >= image->height) down_y = image->height - 1;

			CGL_ubyte* left_pixel = NULL;
			CGL_ubyte* right_pixel = NULL;
			CGL_ubyte* center_pixel = NULL;

			if (h_or_v)
			{
				left_pixel = &image->data[(y * image->width + left_x) * 3];
				right_pixel = &image->data[(y * image->width + right_x) * 3];
				center_pixel = &image->data[(y * image->width + x) * 3];
			}
			else
			{
				left_pixel = &image->data[(up_y * image->width + x) * 3];
				right_pixel = &image->data[(down_y * image->width + x) * 3];
				center_pixel = &image->data[(y * image->width + x) * 3];
			}


			CGL_float lR = (CGL_float)left_pixel[0];
			CGL_float lG = (CGL_float)left_pixel[1];
			CGL_float lB = (CGL_float)left_pixel[2];

			CGL_float rR = (CGL_float)right_pixel[0];
			CGL_float rG = (CGL_float)right_pixel[1];
			CGL_float rB = (CGL_float)right_pixel[2];

			CGL_float mR = (CGL_float)center_pixel[0];
			CGL_float mG = (CGL_float)center_pixel[1];
			CGL_float mB = (CGL_float)center_pixel[2];

			CGL_float lumL = 0.299f * lR + 0.587f * lG + 0.114f * lB;
			CGL_float lumR = 0.299f * rR + 0.587f * rG + 0.114f * rB;
			CGL_float lumM = 0.299f * mR + 0.587f * mG + 0.114f * mB;

			CGL_float x_energy = sqrtf(
				SQ(lR - mR) + SQ(lG - mG) + SQ(lB - mB) +
				SQ(rR - mR) + SQ(rG - mG) + SQ(rB - mB)
			);
			x_energy += sqrtf(
				SQ(lumL - lumM) + SQ(lumR - lumM)
			);

			// for border pixels make energe very high
			if (x == 0 || x == image->width - 1 || y == 0 || y == image->height - 1)
			{
				x_energy = 10.0;
			}

			energy_map[y * image->width + x] = x_energy;
		}
	}

	return energy_map;
}

CGL_float* ci_calculate_h_energy_map(ci_Image* image)
{
	static CGL_float energy_map[6000 * 6000] = { 0 };
	return ci_calculate_energy_map(image, energy_map, CGL_TRUE);
}

CGL_float* ci_calculate_v_energy_map(ci_Image* image)
{
	static CGL_float energy_map[6000 * 6000] = { 0 };
	return ci_calculate_energy_map(image, energy_map, CGL_FALSE);
}


CGL_int* ci_calculate_seam_map(CGL_float* energy_map, CGL_int* seam, CGL_int width, CGL_int height, CGL_bool h_or_v)
{
	
	#define SAMPLE_EMAP(x, y)  h_or_v ? energy_map[(y) * width + x] : energy_map[(x) * width + y] 
	#define UPDATE_EMAP(x, y, value)  { if (h_or_v) energy_map[(y) * width + x] = value; else energy_map[(x) * width + y] = value; }

	#pragma omp parallel for collapse(2)
	for (CGL_int i = 1; i < (h_or_v ? height : width); i++)
	{
		for (CGL_int j = 0; j < (h_or_v ? width : height); j++)
		{
			CGL_float prev_row_left = SAMPLE_EMAP(CGL_utils_clamp(j - 1, 0, width - 1), i - 1);
			CGL_float prev_row_center = SAMPLE_EMAP(j, i - 1);
			CGL_float prev_row_right = SAMPLE_EMAP(CGL_utils_clamp(j + 1, 0, width - 1), i - 1);


			CGL_float current_energy = SAMPLE_EMAP(j, i);

			CGL_float min_energy = CGL_utils_min(CGL_utils_min(prev_row_left, prev_row_center), prev_row_right);
			current_energy += min_energy;
			UPDATE_EMAP(j, i, current_energy);

		}
	}

	CGL_float min_energy = FLT_MAX;
	CGL_int min_energy_index = 0;

	// find min energy in last row or column depending on h_or_v
	for (CGL_int i = 0; i < (h_or_v ? width : height); i++)
	{
		CGL_float current_energy = SAMPLE_EMAP(i, (h_or_v ? height : width) - 1);
		if (current_energy < min_energy)
		{
			min_energy = current_energy;
			min_energy_index = i;
		}
	}

	// backtrace seam
	CGL_int current_index = min_energy_index;
	for (CGL_int i = (h_or_v ? height : width) - 1; i >= 0; i--)
	{
		seam[i] = current_index;
		CGL_float current_energy = SAMPLE_EMAP(current_index, i);
		CGL_float left_energy = FLT_MAX;
		CGL_float center_energy = FLT_MAX;
		CGL_float right_energy = FLT_MAX;
		if (current_index > 0) left_energy = SAMPLE_EMAP(current_index - 1, i);
		center_energy = SAMPLE_EMAP(current_index, i);
		if (current_index < (h_or_v ? width : height) - 1) right_energy = SAMPLE_EMAP(current_index + 1, i);
		CGL_float min_energy = CGL_utils_min(CGL_utils_min(left_energy, center_energy), right_energy);
		if (min_energy == left_energy) current_index--;
		else if (min_energy == right_energy) current_index++;
	}

	return seam;
}

CGL_int* ci_calculate_h_seam_map(CGL_float* energy_map, CGL_int width, CGL_int height)
{
	static CGL_int seam[6000] = { 0 };
	return ci_calculate_seam_map(energy_map, seam, width, height, CGL_TRUE);
}

CGL_int* ci_calculate_v_seam_map(CGL_float* energy_map, CGL_int width, CGL_int height)
{
	static CGL_int seam[6000] = { 0 };
	return ci_calculate_seam_map(energy_map, seam, width, height, CGL_FALSE);
}

CGL_void resize_ci(ci_Image* image, ci_Image* original_image, CGL_int target_width, CGL_int target_height)
{
	(void)original_image;
	// NOTE: This is not at all a good idea to recalculate the energy maps every step, ideally reuse
	//       the energy maps and update them as needed. But for the sake of simplicity I did not do that here
	CGL_float* h_energy_map = ci_calculate_h_energy_map(image);
	CGL_float* v_energy_map = ci_calculate_v_energy_map(image);

	CGL_int* h_seam_map = ci_calculate_h_seam_map(h_energy_map, image->width, image->height);
	CGL_int* v_seam_map = ci_calculate_v_seam_map(v_energy_map, image->width, image->height);

	static CGL_byte new_data[6000 * 6000 * 3] = { 0 };

	CGL_int new_width = target_width == image->width ? image->width : (target_width < image->width ? image->width - 1 : image->width + 1);
	CGL_int new_height = target_height == image->height ? image->height : (target_height < image->height ? image->height - 1 : image->height + 1);

	CGL_bool add_h_seam = new_width > image->width;
	CGL_bool add_v_seam = new_height > image->height;
	CGL_bool remove_h_seam = new_width < image->width;
	CGL_bool remove_v_seam = new_height < image->height;

	// loop through each row and remove the seam
	// if new width is smaller than original width, remove vertical seam
	// if new height is smaller than original height, remove horizontal seam
	#pragma omp parallel for collapse(2)
	for (CGL_int i = 0; i < new_height; i++)
	{
		CGL_int h_seam = h_seam_map[i];
		for (CGL_int j = 0; j < new_width; j++)
		{
			CGL_int v_seam = v_seam_map[j];

			CGL_int tj = j;
			CGL_int ti = i;

			CGL_int source_x = tj;
			CGL_int source_y = ti;

			if (remove_h_seam && h_seam <= tj) source_x++;
			if (remove_v_seam && v_seam <= ti) source_y++;

			if (add_h_seam && h_seam < tj) source_x--;
			if (add_v_seam && v_seam < ti) source_y--;

			CGL_int source_index = (source_y * image->width + source_x) * 3;
			CGL_int dest_index = (ti * new_width + tj) * 3;

			CGL_ubyte pr = image->data[source_index + 0], pg = image->data[source_index + 1], pb = image->data[source_index + 2];

			if (add_h_seam) {
				// if its the seam then interpolate between the neighbour pixels
				if (h_seam == tj)
				{
					CGL_int left_index = (source_y * image->width + source_x - 1) * 3;
					CGL_int right_index = (source_y * image->width + source_x) * 3;

					CGL_ubyte lr = image->data[left_index + 0], lg = image->data[left_index + 1], lb = image->data[left_index + 2];
					CGL_ubyte rr = image->data[right_index + 0], rg = image->data[right_index + 1], rb = image->data[right_index + 2];

					pr = (lr + rr) / 2;
					pg = (lg + rg) / 2;
					pb = (lb + rb) / 2;
				}
			}
			if (add_v_seam) {
				// if its the seam then interpolate between the neighbour pixels
				if (v_seam == ti)
				{
					CGL_int top_index = ((source_y - 1) * image->width + source_x) * 3;
					CGL_int bottom_index = ((source_y) * image->width + source_x) * 3;

					CGL_ubyte tr = image->data[top_index + 0], tg = image->data[top_index + 1], tb = image->data[top_index + 2];
					CGL_ubyte br = image->data[bottom_index + 0], bg = image->data[bottom_index + 1], bb = image->data[bottom_index + 2];

					pr = (tr + br) / 2;
					pg = (tg + bg) / 2;
					pb = (tb + bb) / 2;
				}
			}

			new_data[dest_index + 0] = pr;
			new_data[dest_index + 1] = pg;
			new_data[dest_index + 2] = pb;
		}
	}

	memcpy(image->data, new_data, new_width * new_height * 3);
	image->width = new_width;
	image->height = new_height;
	image->aspect_ratio = (CGL_float)image->width / (CGL_float)image->height;
	ci_recreate_texture(image);
}


// a drag and drop callback
CGL_void CGL_window_drop_callback(CGL_window* window, const CGL_byte** paths, CGL_int count)
{
	if (count > 0)
	{
		if (g_state.original.data)
		{
			ci_free_image(g_state.original);
			ci_free_image(g_state.resized);

			g_state.loaded = CGL_FALSE;
		}

		g_state.original = ci_load_image(paths[0]);
		g_state.resized = ci_blank_image(g_state.original.width, g_state.original.height);
		memcpy(g_state.resized.data, g_state.original.data, g_state.original.width * g_state.original.height * 3);
		ci_upload_image(&g_state.resized);

		g_state.current = 1;
		g_state.loaded = CGL_TRUE;
		CGL_info("Loaded image: %s", paths[0]);
	}
}

CGL_int main() 
{
    if (!CGL_init()) return 1;
    CGL_window* window = CGL_window_create(WINDOW_SIZE, WINDOW_SIZE, "Content Aware Image Resize - Jaysmito Mukherjee");
    if (!window) return EXIT_FAILURE;
	CGL_window_make_context_current(window);
    CGL_gl_init();
	CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
	CGL_window_set_drag_n_drop_callback(window, CGL_window_drop_callback);

	// g_state.resize_func = resize_normal;
	g_state.resize_func = resize_ci;

	g_state.current = 1;
	g_state.loaded = CGL_FALSE;

	CGL_widgets_init();

    while (!CGL_window_should_close(window))
    {
		CGL_window_set_size(window, WINDOW_SIZE, WINDOW_SIZE);



		CGL_int delta = 1;
		// W + up arrow or down arrow to change width
		if (CGL_window_is_key_pressed(window, CGL_KEY_W) && CGL_window_is_key_pressed(window, CGL_KEY_UP)) {
			PREF("Resize", g_state.resize_func(&g_state.resized, &g_state.original, g_state.resized.width + delta, g_state.resized.height));
		}
		else if (CGL_window_is_key_pressed(window, CGL_KEY_W) && CGL_window_is_key_pressed(window, CGL_KEY_DOWN)) {
			PREF("Resize", g_state.resize_func(&g_state.resized, &g_state.original, g_state.resized.width - delta, g_state.resized.height));
		}

		// H + up arrow or down arrow to change height
		if (CGL_window_is_key_pressed(window, CGL_KEY_H) && CGL_window_is_key_pressed(window, CGL_KEY_UP)) {
			PREF("Resize", g_state.resize_func(&g_state.resized, &g_state.original, g_state.resized.width, g_state.resized.height + delta));
		}
		else if (CGL_window_is_key_pressed(window, CGL_KEY_H) && CGL_window_is_key_pressed(window, CGL_KEY_DOWN)) {
			PREF("Resize", g_state.resize_func(&g_state.resized, &g_state.original, g_state.resized.width, g_state.resized.height - delta));
		}

		CGL_framebuffer_bind(default_framebuffer);
		CGL_gl_clear(0.1f, 0.1f, 0.1f, 1.0f);

		CGL_widgets_begin();

		if (g_state.loaded)
		{
			ci_Image* image = g_state.current == 0 ? &g_state.original : &g_state.resized;

			CGL_widgets_set_texture(image->texture);

			CGL_float width = 1.0, height = 1.0, offset_x = 0.0, offset_y = 0.0;
			if ( image->aspect_ratio > 1.0)
			{
				height = 1.0 / image->aspect_ratio;
				offset_y = (1.0 - height) / 2.0;
			}
			else
			{
				width = image->aspect_ratio;
				offset_x = (1.0 - width) / 2.0;
			}

			offset_x = offset_x * 2.0 - 1.0;
			offset_y = offset_y * 2.0 - 1.0;

			height *= 2.0;
			width *= 2.0;

			CGL_widgets_add_rect2f(offset_x, offset_y, width, height);

			// Set texture details as titile of the window
			static CGL_byte title[256];
			sprintf(title, "Content Aware Image Resize - [Image : %dx%d] [%s] - Jaysmito Mukherjee", image->width, image->height, g_state.current == 0 ? "Original" : "Resized");
			CGL_window_set_title(window, title);
		}
		else {
			CGL_window_set_title(window, "Content Aware Image Resize - Jaysmito Mukherjee");
		}

		if (CGL_window_is_key_pressed(window, CGL_KEY_O)) {
			g_state.current = 0;
		} else if (CGL_window_is_key_pressed(window, CGL_KEY_R)) {
			g_state.current = 1;
		}

		// reset on space
		if (CGL_window_is_key_pressed(window, CGL_KEY_SPACE)) {
			g_state.resize_func = resize_normal;
			memcpy(g_state.resized.data, g_state.original.data, g_state.original.width * g_state.original.height * 3);
			g_state.resized.width = g_state.original.width;
			g_state.resized.height = g_state.original.height;
			ci_upload_image(&g_state.resized);
		}

		CGL_widgets_end();

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }


	CGL_framebuffer_destroy(default_framebuffer);
	CGL_widgets_shutdown();
    CGL_gl_shutdown();
	CGL_window_destroy(window);
    CGL_shutdown();
}
