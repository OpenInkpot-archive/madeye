/***************************************************************************
 *   Copyright (C) 2009 by Marc Lajoie, Tomasz Długosz                     *
 *   quickhand@openinkpot.org                                              *
 *   tomek3dgmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Efreet_Mime.h>

#include "keyboard.h"

Evas_Object *orig_image;
Evas_Object *image;
bool dither = false;

Eina_List *filelist;
Eina_List *cur_file;

char *supported_types[] = {
	"image/jpeg",
	"image/png",
	"image/gif",
	NULL
};

int winwidth = 600;
int winheight = 800;

int brightness_level = 0, contrast_level = 0;

void floyd_steinberg_dither();
void render_cur_image();
void next_image();
void prev_image();
void inc_brighness();
void dec_brighness();
void inc_contrast();
void dec_contrast();
void toggle_dithering();
void quit();

_op operations[] = {
	{ "NEXT_IMAGE", next_image },
	{ "PREV_IMAGE", prev_image },
	{ "INC_BRIGHTNESS", inc_brighness },
	{ "DEC_BRIGHTNESS", dec_brighness },
	{ "INC_CONTRAST", inc_contrast },
	{ "DEC_CONTRAST", dec_contrast },
	{ "DITHER", toggle_dithering },
	{ "RELOAD", render_cur_image },
    { "QUIT", quit },
	{ NULL, NULL},
};

unsigned char lut[256];

#define R_VAL(__p__) *(__p__+2)
#define G_VAL(__p__) *(__p__+1)
#define B_VAL(__p__) *(__p__)
#define Y_VAL(__p__) (0xff&((306*(0xff&(int)R_VAL(__p__)) + 601*(0xff&(int)G_VAL(__p__)) + 117*(0xff&(int)B_VAL(__p__)))/1024))

void exit_all(void *param) { ecore_main_loop_quit(); }

void quit() { ecore_main_loop_quit(); }

static void die(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

static void main_win_resize_handler(Ecore_Evas* main_win)
{
	int w, h;
	Evas *canvas = ecore_evas_get(main_win);
	evas_output_size_get(canvas, &w, &h);

	winwidth = w;
	winheight = h;

	Evas_Object *bg = evas_object_name_find(canvas, "background");
	evas_object_resize(bg, w, h);
	evas_object_image_load_size_set(orig_image, w, h);

	render_cur_image();
}

static void main_win_delete_handler(Ecore_Evas* main_win)
{
    ecore_main_loop_quit();
}

void render_cur_image()
{
	double zoom = 1.0;
	int width, height;

	brightness_level = 0;
	contrast_level = 0;

	evas_object_image_file_set(orig_image, eina_list_data_get(cur_file), NULL);
	evas_object_image_size_get(orig_image, &width, &height);
	int stride = evas_object_image_stride_get(orig_image);

	if(dither)
		floyd_steinberg_dither();

	evas_object_image_size_set(image, width, height);

	char *s = evas_object_image_data_get(orig_image, EINA_FALSE);
	char *t = evas_object_image_data_get(image, EINA_TRUE);
	memcpy(t, s, stride * height * 4);

	evas_object_image_data_update_add(image, 0, 0, width, height);
	evas_object_image_alpha_set(image, evas_object_image_alpha_get(orig_image));

	if(width > winwidth) {
		zoom = 1.0 * width / winwidth;

		width = winwidth;
		height /= zoom;
	}

	if(height > winheight) {
		zoom = 1.0 * height / winheight;

		width /= zoom;
		height = winheight;
	}

	evas_object_move(image, (winwidth - width) / 2, (winheight - height) / 2);
	evas_object_resize(image, width, height);
	evas_object_show(image);
}

void fill_lut()
{
	int x;
	float k;

	if(contrast_level < 0) {
		k = pow(1.1, -contrast_level);
		k = 1/k;
	} else if(contrast_level > 0)
		k = pow(1.1, contrast_level);
	else
		k = 1;

	for(int i = 0; i < 256; i++) {
		x = i + brightness_level * 25;
		x = x * k;

		if(x < 0)
			x = 0;
		else if(x > 255)
			x = 255;

		lut[i] = x;
	}
}

void adjust_image()
{
	int w, h, stride;

	evas_object_image_size_get(image, &w, &h);
	stride = evas_object_image_stride_get(image);

	int c;
	char *p = evas_object_image_data_get(orig_image, EINA_FALSE);
	char *_p = evas_object_image_data_get(image, EINA_TRUE);

	Eina_Bool alpha = evas_object_image_alpha_get(orig_image);

	for(int j = 0; j < h; j++) {
		for(int i = 0; i < w; i++) {
			if(!alpha || (int)*p) {
				if(!(R_VAL(p) == G_VAL(p) && G_VAL(p) == B_VAL(p))) {
					c = Y_VAL(p);

					if(c < 0)
						c = 0;
					else if(c > 255)
						c = 255;
				} else {
					c = R_VAL(p) & 0xff;
				}

				G_VAL(_p) = B_VAL(_p) = R_VAL(_p) = lut[c];
			}

			p += 4;
			_p += 4;
		}
		p += (stride - w) * 4;
		_p += (stride - w) * 4;
	}

	evas_object_image_data_update_add(image, 0, 0, w, h);
}

void inc_brighness()
{
	if(brightness_level > 5)
		return;

	brightness_level++;
	fill_lut();
	adjust_image();
}

void dec_brighness()
{
	if(brightness_level < -5)
		return;

	brightness_level--;
	fill_lut();
	adjust_image();
}

void inc_contrast()
{
	if(contrast_level > 5)
		return;

	contrast_level++;
	fill_lut();
	adjust_image();
}

void dec_contrast()
{
	if(contrast_level < -5)
		return;

	contrast_level--;
	fill_lut();
	adjust_image();
}

void next_image()
{
	cur_file = eina_list_next(cur_file);
	if(!cur_file)
		cur_file = filelist;

	render_cur_image();
}

void prev_image()
{
	cur_file = eina_list_prev(cur_file);
	if(!cur_file)
		cur_file = eina_list_last(filelist);

	render_cur_image();
}

void floyd_steinberg_dither()
{
	int w, h, stride;

#define CHECK_BOUNDS(__x__) if(__x__ < 0) __x__ = 0; else if(__x__ > 255) __x__ = 255;

	evas_object_image_size_get(orig_image, &w, &h);
	stride = evas_object_image_stride_get(orig_image);

	unsigned char *c = evas_object_image_data_get(orig_image, EINA_TRUE);

	int xi;
	unsigned char *x;

	int oldpixel, newpixel;
	int quant_error;

	int factor = 255 / (8 - 1);
	int factor_half = factor>>1;

	x = c;
	// convert first line to grayscale
	for(int i = 0; i < w; i++) {
		if(!(R_VAL(x) == G_VAL(x) && G_VAL(x) == B_VAL(x))) {
			xi = Y_VAL(x);
			CHECK_BOUNDS(xi);
			G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
		}
		x += 4;
	}

	for(int j = 0; j < h; j++) {
		x = c + (j+1) * stride * 4;

		// convert next line to grayscale
		for(int i = 0; j<(h-1) && i < w; i++) {
			if(!(R_VAL(x) == G_VAL(x) && G_VAL(x) == B_VAL(x))) {
				xi = Y_VAL(x);
				CHECK_BOUNDS(xi);
				G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
			}
			x += 4;
		}

		for(int i = 0; i < w; i++) {
			x = c + j * stride * 4 + i * 4;

			oldpixel = 0xff & (int)R_VAL(x);

			newpixel = (oldpixel + factor_half) / factor;
			newpixel *= factor;
			CHECK_BOUNDS(newpixel);
			G_VAL(x) = B_VAL(x) = R_VAL(x) = newpixel;

			quant_error = oldpixel - newpixel;

			if(i < w - 1) {
				x += 4;
				xi = R_VAL(x); 
				xi = xi + 7 * quant_error / 16;
				CHECK_BOUNDS(xi);
				G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
			}

			x = c + (j+1) * stride * 4 + (i - 1) * 4;

			if(j < h - 1) {
				if(i > 0) {
					xi = R_VAL(x); 
					xi = xi + 3 * quant_error / 16;
					CHECK_BOUNDS(xi);
					G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
				}

				x += 4;
				xi = R_VAL(x); 
				xi = xi + 5 * quant_error / 16;
				CHECK_BOUNDS(xi);
				G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;

				if(i < w - 1) {
					x += 4;
					xi = R_VAL(x); 
					xi = xi + quant_error / 16;
					CHECK_BOUNDS(xi);
					G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
				}
			}
		}
	}

	evas_object_image_data_update_add(orig_image, 0, 0, w, h);
}

void toggle_dithering()
{
	dither = !dither;
	render_cur_image();
}

void init_filelist(const char *file)
{
	char filename[1024];
	char *f;
	Eina_List *l;

	char *path = ecore_file_dir_get(file);
	file = ecore_file_file_get(file);

	Eina_List *ls = ecore_file_ls(path);

	efreet_mime_init();

	EINA_LIST_FOREACH(ls, l, f) {
		snprintf(filename, 1024, "%s/%s", path, f);
		free(f);

		if(ecore_file_is_dir(filename))
			continue;

		const char *mime_type = efreet_mime_type_get(filename);
		if(!mime_type)
			continue;

		for(char **t = supported_types; t && *t; t++)
			if(!strncmp(mime_type, *t, strlen(*t))) {
				filelist = eina_list_append(filelist, strdup(filename));

				if(!cur_file && !strncmp(file, ecore_file_file_get(filename), strlen(file)))
					cur_file = eina_list_last(filelist);

				break;
			}
	}

	eina_list_free(ls);

	efreet_mime_shutdown();
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		fprintf(stderr,"no image file given!\n");
		return 1;
	}

	Ecore_Evas *ee;
	Evas_Object *bg; //background

	/* initialize our libraries */
	if(!evas_init())
		die("Unable to initialize Evas\n");
	if(!ecore_init())
		die("Unable to initialize Ecore\n");
	if(!ecore_evas_init())
		die("Unable to initialize Evas\n");

	ecore_x_io_error_handler_set(exit_all, NULL);

	ee = ecore_evas_software_x11_new(0, 0, 0, 0, 600, 800);

	ecore_evas_borderless_set(ee, 0);
	ecore_evas_shaped_set(ee, 0);
	ecore_evas_title_set(ee, "madEYE");
	ecore_evas_show(ee);

	ecore_evas_callback_resize_set(ee, main_win_resize_handler);

	/* get a pointer our new Evas canvas */
	Evas *evas = ecore_evas_get(ee);

	/* create our white background */
	bg = evas_object_rectangle_add(evas);
	evas_object_color_set(bg, 255, 255, 255, 255);
	evas_object_move(bg, 0, 0);
	evas_object_resize(bg, 600, 800);
	evas_object_name_set(bg, "background");
	evas_object_focus_set(bg, 1);
	evas_object_event_callback_add(bg, EVAS_CALLBACK_KEY_UP, &key_handler, NULL);
	evas_object_show(bg);

	orig_image = evas_object_image_filled_add(evas);
	evas_object_name_set(orig_image, "orig_image");
	image = evas_object_image_filled_add(evas);
	evas_object_name_set(image, "image");
	evas_object_image_smooth_scale_set(image, EINA_TRUE);
	evas_object_image_smooth_scale_set(orig_image, EINA_TRUE);
	evas_object_image_load_size_set(orig_image, 600, 800);

	read_keymap(operations);
	init_filelist(argv[1]);
	render_cur_image();

	/* start the main event loop */
	ecore_main_loop_begin();

	evas_object_del(image);
	evas_object_del(bg);

	ecore_evas_shutdown();
	ecore_shutdown();
	evas_shutdown();

	return 0;
}
