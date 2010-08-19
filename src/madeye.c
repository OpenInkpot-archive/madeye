/*
 * Copyright (C) 2009 Marc Lajoie <quickhand@openinkpot.org>
 * Copyright (C) 2009 Tomasz Długosz <tomek3dgmail.com>
 * Copyright (C) 2009, 2010 Alexander Kerner <lunohod@openinkpot.org>
 * Copyright © 2009 Mikhail Gusarov <dottedmag@dottedmag.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _GNU_SOURCE
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <libintl.h>
#include <locale.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Efreet_Mime.h>
#include <Edje.h>

#include <libchoicebox.h>
#include <libeoi.h>
#include <libeoi_help.h>
#include <libeoi_themes.h>
#include <libeoi_clock.h>
#include <libeoi_battery.h>

#include "keyboard.h"

Evas *evas;
Evas_Object *orig_image;
Evas_Object *image;

Eina_List *filelist;
Eina_List *cur_file;

#define THEME_EDJE "madeye"

struct mime_type_t {
    const char *type;
    const char *name;
} supported_types[] = {
    { "image/jpeg", "JPEG" },
    { "image/png", "PNG" },
    { "image/gif", "GIF" },
    { NULL, NULL },
};

typedef struct frame {
    int x, y, w, h;
} frame_t;

typedef struct file {
    const char *filename;
    unsigned int idx;
    struct mime_type_t *mtype;
} file_t;

int brightness_level = 0, contrast_level = 0;
bool preserve_settings = true;
bool dither = false;

void floyd_steinberg_dither();
void render_cur_image();
void next_image();
void prev_image();
void inc_brighness();
void dec_brighness();
void inc_contrast();
void dec_contrast();
void toggle_dithering();
void toggle_fullscreen();
void toggle_pres_set();
void show_help();
void quit();
void adjust_image();
void reload();
static void update_buttons();

_op operations[] = {
    {"NEXT_IMAGE", next_image},
    {"PREV_IMAGE", prev_image},
    {"INC_BRIGHTNESS", inc_brighness},
    {"DEC_BRIGHTNESS", dec_brighness},
    {"INC_CONTRAST", inc_contrast},
    {"DEC_CONTRAST", dec_contrast},
    {"DITHER", toggle_dithering},
    {"PRES_SET", toggle_pres_set},
    {"FULLSCREEN", toggle_fullscreen},
    {"RELOAD", reload},
    {"HELP", show_help},
    {"QUIT", quit},
    {NULL, NULL},
};

unsigned char lut[256];

#define R_VAL(__p__) *(__p__+2)
#define G_VAL(__p__) *(__p__+1)
#define B_VAL(__p__) *(__p__)
#define Y_VAL(__p__) (0xff&((306*(0xff&(int)R_VAL(__p__)) + 601*(0xff&(int)G_VAL(__p__)) + 117*(0xff&(int)B_VAL(__p__)))/1024))

#define UNUSED __attribute__ ((unused))

void
exit_all(void *param UNUSED)
{
    ecore_main_loop_quit();
}

void
quit()
{
    ecore_main_loop_quit();
}

static void
die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

frame_t
get_frame()
{
    frame_t f;

    Evas_Object *mw = evas_object_name_find(evas, "main-window");
    if (evas_object_visible_get(mw)) {
        Evas_Object *e = evas_object_name_find(evas, "main-edje");
        edje_object_part_geometry_get(e, "image-frame", &f.x, &f.y, &f.w,
                                      &f.h);

        int x, y;
        edje_object_part_geometry_get(mw, "contents", &x, &y, NULL, NULL);
        f.x += x;
        f.y += y;
    } else {
        f.x = 0;
        f.y = 0;
        evas_output_size_get(evas, &f.w, &f.h);
    }

    return f;
}

static void
main_win_resize_handler(Ecore_Evas *ee, int w, int h, void *param UNUSED)
{
    Evas *canvas = ecore_evas_get(ee);
    Evas_Object *mw = evas_object_name_find(canvas, "main-window");
    evas_object_resize(mw, w, h);

    Evas_Object *e = evas_object_name_find(canvas, "main-edje");
    const char *file;
    const char *collection;
    edje_object_file_get(e, &file, &collection);
    char *replacement = (h > 600) ? "vert_main_edje" : "hor_main_edje";
    if(strcmp(collection, replacement)) {
        edje_object_file_set(e, file, replacement);
        update_buttons();
    }

    Evas_Object *fs_i = evas_object_name_find(canvas, "fs-icon");
    if(h > 600) {
        eoi_main_window_footer_show(mw);
        edje_object_part_swallow(mw, "state-icons", fs_i);
    } else {
        eoi_main_window_footer_hide(mw);
        edje_object_part_unswallow(mw, fs_i);
        evas_object_hide(fs_i);
    }

    Evas_Object *bg = evas_object_name_find(canvas, "background");
    evas_object_resize(bg, w, h);

    render_cur_image();
}

static void
main_win_delete_handler(Ecore_Evas *main_win UNUSED)
{
    ecore_main_loop_quit();
}

static void
draw_toggle_button(Evas_Object *gui, const char *button, bool state)
{
    char *buf;
    asprintf(&buf, "%s,%s", button, state ? "pressed" : "default");
    edje_object_signal_emit(gui, buf, "");
    free(buf);
}

static void
draw_scale(const char *scale, int level)
{
    Evas_Object *o =
        edje_object_part_swallow_get(evas_object_name_find
                                     (evas, "main-window"),
                                     "contents");
    if (!o)
        return;

    char *buf;
    asprintf(&buf, "%s,%d", scale, level);
    edje_object_signal_emit(o, buf, "");
    free(buf);
}

static void
update_buttons()
{
    Evas_Object *o =
        edje_object_part_swallow_get(evas_object_name_find
                                     (evas, "main-window"),
                                     "contents");

    draw_toggle_button(o, "dither", dither);
    draw_toggle_button(o, "presset", preserve_settings);

    draw_scale("brightness-level", brightness_level);
    draw_scale("contrast-level", contrast_level);
}

void
reload()
{
    brightness_level = 0;
    contrast_level = 0;
    dither = false;
    update_buttons();
    render_cur_image();
}

void
update_info()
{
    Evas_Object *e = evas_object_name_find(evas, "main-edje");

    file_t *file = eina_list_data_get(cur_file);

    edje_object_part_text_set(e, "filename", ecore_file_strip_ext(ecore_file_file_get(file->filename)));

    if(file->mtype)
        edje_object_part_text_set(e, "extention", file->mtype->name);
    else
        edje_object_part_text_set(e, "extention", "");

    char *t;
    int width, height;
    evas_object_image_size_get(orig_image, &width, &height);
    asprintf(&t, gettext("%dpx x %dpx"), width, height);
    edje_object_part_text_set(e, "dimensions", t);
    free(t);

    float fsize = ecore_file_size(file->filename);
    fsize /= 1024;
    if(fsize > 1024) {
        fsize /= 1024;
        asprintf(&t, "%.2f %s", fsize, gettext("MB"));
    } else
        asprintf(&t, "%.2f %s", fsize, gettext("kB"));
    edje_object_part_text_set(e, "size", t);
    free(t);

    time_t time = ecore_file_mod_time(file->filename);
    t = (char*)malloc(200);
    strftime(t, 200, gettext("%c"), localtime(&time));
    edje_object_part_text_set(e, "date", t);
    free(t);
}

void
render_cur_image()
{
    double zoom = 1.0;
    int width, height;

    frame_t f = get_frame();

    evas_object_image_load_size_set(orig_image, f.w, f.h);

    file_t *file = eina_list_data_get(cur_file);
    evas_object_image_file_set(orig_image, file->filename, NULL);
    evas_object_image_size_get(orig_image, &width, &height);
    update_info();

    int stride = evas_object_image_stride_get(orig_image);

    if (dither)
        floyd_steinberg_dither();

    evas_object_image_size_set(image, width, height);
    evas_object_image_alpha_set(image,
                                evas_object_image_alpha_get(orig_image));

    if (brightness_level || contrast_level)
        adjust_image();
    else {
        char *s = evas_object_image_data_get(orig_image, EINA_FALSE);
        char *t = evas_object_image_data_get(image, EINA_TRUE);
        memcpy(t, s, stride * height * 4);

        evas_object_image_data_update_add(image, 0, 0, width, height);
    }

    if (width > f.w) {
        zoom = 1.0 * width / f.w;

        width = f.w;
        height /= zoom;
    }

    if (height > f.h) {
        zoom = 1.0 * height / f.h;

        width /= zoom;
        height = f.h;
    }

    evas_object_move(image, f.x + (f.w - width) / 2,
                     f.y + (f.h - height) / 2);
    evas_object_resize(image, width, height);
    evas_object_show(image);
}

void
fill_lut()
{
    int x;
    float k;

    if (contrast_level < 0) {
        k = pow(1.1, -contrast_level);
        k = 1 / k;
    } else if (contrast_level > 0)
        k = pow(1.1, contrast_level);
    else
        k = 1;

    for (int i = 0; i < 256; i++) {
        x = i + brightness_level * 25;
        x = x * k;

        if (x < 0)
            x = 0;
        else if (x > 255)
            x = 255;

        lut[i] = x;
    }
}

void
adjust_image()
{
    int w, h, stride;

    evas_object_image_size_get(image, &w, &h);
    stride = evas_object_image_stride_get(image);

    int c;
    char *p = evas_object_image_data_get(orig_image, EINA_FALSE);
    char *_p = evas_object_image_data_get(image, EINA_TRUE);

    Eina_Bool alpha = evas_object_image_alpha_get(orig_image);

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            if (!alpha || (int) *p) {
                if (!(R_VAL(p) == G_VAL(p) && G_VAL(p) == B_VAL(p))) {
                    c = Y_VAL(p);

                    if (c < 0)
                        c = 0;
                    else if (c > 255)
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

void
inc_brighness()
{
    if (brightness_level > 5)
        return;

    brightness_level++;

    draw_scale("brightness-level", brightness_level);

    fill_lut();
    adjust_image();
}

void
dec_brighness()
{
    if (brightness_level < -5)
        return;

    brightness_level--;
    draw_scale("brightness-level", brightness_level);
    fill_lut();
    adjust_image();
}

void
inc_contrast()
{
    if (contrast_level > 5)
        return;

    contrast_level++;
    draw_scale("contrast-level", contrast_level);
    fill_lut();
    adjust_image();
}

void
dec_contrast()
{
    if (contrast_level < -5)
        return;

    contrast_level--;
    draw_scale("contrast-level", contrast_level);
    fill_lut();
    adjust_image();
}

static void
update_footer()
{
    unsigned int lsize = eina_list_count(filelist);
    file_t *f = eina_list_data_get(cur_file);
    choicebox_aux_edje_footer_handler(evas_object_name_find
                                      (evas, "main-window"), "footer",
                                      f->idx, lsize);
}

void
next_image()
{
    cur_file = eina_list_next(cur_file);
    if (!cur_file)
        cur_file = filelist;

    update_footer();

    if (!preserve_settings) {
        brightness_level = 0;
        contrast_level = 0;
        dither = false;

        update_buttons();
    }

    render_cur_image();
}

void
prev_image()
{
    cur_file = eina_list_prev(cur_file);
    if (!cur_file)
        cur_file = eina_list_last(filelist);

    update_footer();

    if (!preserve_settings) {
        brightness_level = 0;
        contrast_level = 0;
        dither = false;

        update_buttons();
    }

    render_cur_image();
}

void
floyd_steinberg_dither()
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
    int factor_half = factor >> 1;

    x = c;
    // convert first line to grayscale
    for (int i = 0; i < w; i++) {
        if (!(R_VAL(x) == G_VAL(x) && G_VAL(x) == B_VAL(x))) {
            xi = Y_VAL(x);
            CHECK_BOUNDS(xi);
            G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
        }
        x += 4;
    }

    for (int j = 0; j < h; j++) {
        x = c + (j + 1) * stride * 4;

        // convert next line to grayscale
        for (int i = 0; j < (h - 1) && i < w; i++) {
            if (!(R_VAL(x) == G_VAL(x) && G_VAL(x) == B_VAL(x))) {
                xi = Y_VAL(x);
                CHECK_BOUNDS(xi);
                G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
            }
            x += 4;
        }

        for (int i = 0; i < w; i++) {
            x = c + j * stride * 4 + i * 4;

            oldpixel = 0xff & (int) R_VAL(x);

            newpixel = (oldpixel + factor_half) / factor;
            newpixel *= factor;
            CHECK_BOUNDS(newpixel);
            G_VAL(x) = B_VAL(x) = R_VAL(x) = newpixel;

            quant_error = oldpixel - newpixel;

            if (i < w - 1) {
                x += 4;
                xi = R_VAL(x);
                xi = xi + 7 * quant_error / 16;
                CHECK_BOUNDS(xi);
                G_VAL(x) = B_VAL(x) = R_VAL(x) = xi;
            }

            x = c + (j + 1) * stride * 4 + (i - 1) * 4;

            if (j < h - 1) {
                if (i > 0) {
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

                if (i < w - 1) {
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

void
toggle_fullscreen()
{
    Evas_Object *mw = evas_object_name_find(evas, "main-window");

    if (evas_object_visible_get(mw))
        evas_object_hide(mw);
    else
        evas_object_show(mw);

    render_cur_image();
}

void
toggle_pres_set()
{
    preserve_settings = !preserve_settings;

    draw_toggle_button(edje_object_part_swallow_get
                       (evas_object_name_find(evas, "main-window"),
                        "contents"), "presset", preserve_settings);
}


void
toggle_dithering()
{
    dither = !dither;

    draw_toggle_button(edje_object_part_swallow_get
                       (evas_object_name_find(evas, "main-window"),
                        "contents"), "dither", dither);

    render_cur_image();
}

void
init_filelist(const char *file)
{
    char filename[1024];
    char *f;
    Eina_List *l;
    int idx = 0, curidx = 0;

    char *path = ecore_file_dir_get(file);
    file = ecore_file_file_get(file);

    Eina_List *ls = ecore_file_ls(path);

    efreet_mime_init();

    EINA_LIST_FOREACH(ls, l, f) {
        snprintf(filename, 1024, "%s/%s", path, f);
        free(f);

        if (ecore_file_is_dir(filename))
            continue;

        const char *mime_type = efreet_mime_type_get(filename);
        if (!mime_type)
            continue;

        for (struct mime_type_t *t = supported_types; t->type; t++)
            if (!strncmp(mime_type, t->type, strlen(t->type))) {
                file_t *fdata = (file_t *) malloc(sizeof(file_t));
                fdata->filename = strdup(filename);
                fdata->idx = idx++;
                fdata->mtype = t;

                filelist = eina_list_append(filelist, fdata);

                if (!cur_file
                    && !strncmp(file, ecore_file_file_get(filename),
                                strlen(file))) {
                    cur_file = eina_list_last(filelist);
                    curidx = fdata->idx;
                }

                break;
            }
    }

    eina_list_free(ls);

    curidx = idx - curidx;
    file_t *fdata;
    EINA_LIST_FOREACH(filelist, l, fdata)
        fdata->idx = (fdata->idx + curidx) % idx;

    efreet_mime_shutdown();
}

void
show_help()
{
    eoi_help_show(evas,
                  "madeye", "index", gettext("Madeye: Help"), NULL, NULL);
}

int
main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "no image file given!\n");
        return 1;
    }

    Ecore_Evas *ee;
    Evas_Object *bg;            //background

    /* initialize our libraries */
    if (!evas_init())
        die("Unable to initialize Evas\n");
    if (!ecore_init())
        die("Unable to initialize Ecore\n");
    if (!ecore_evas_init())
        die("Unable to initialize Evas\n");
    if (!edje_init())
        die("Unable to initialize Edje\n");

    setlocale(LC_ALL, "");
    textdomain("madeye");

    init_filelist(argv[1]);
    if(!cur_file) {
        fprintf(stderr, "cannot open file %s\n", argv[1]);
        exit(1);
    }

    ecore_x_io_error_handler_set(exit_all, NULL);

    ee = ecore_evas_software_x11_8_new(0, 0, 0, 0, 600, 800);

    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);
    ecore_evas_title_set(ee, "madEYE");
    ecore_evas_show(ee);

    ecore_evas_callback_delete_request_set(ee, main_win_delete_handler);
    eoi_resize_callback_add(ee, main_win_resize_handler, NULL);

    /* get a pointer our new Evas canvas */
    evas = ecore_evas_get(ee);

    /* create our white background */
    bg = evas_object_rectangle_add(evas);
    evas_object_color_set(bg, 255, 255, 255, 255);
    evas_object_move(bg, 0, 0);
    evas_object_resize(bg, 600, 800);
    evas_object_name_set(bg, "background");
    evas_object_show(bg);

    Evas_Object *mw = eoi_main_window_create(evas);
    eoi_fullwindow_object_register(ee, mw);
    eoi_run_clock(mw);
    eoi_run_battery(mw);

    edje_object_part_text_set(mw, "title", "Madeye");
    edje_object_part_text_set(mw, "footer", "0/0");

    evas_object_name_set(mw, "main-window");
    evas_object_move(mw, 0, 0);
    evas_object_resize(mw, 600, 800);
    evas_object_show(mw);

    evas_object_focus_set(mw, true);
    evas_object_event_callback_add(mw, EVAS_CALLBACK_KEY_UP, &key_handler,
                                   NULL);

    Evas_Object *r = evas_object_rectangle_add(evas);
    evas_object_color_set(r, 0, 0, 255, 255);
    evas_object_show(r);

    Evas_Object *e = eoi_create_themed_edje(evas, THEME_EDJE, "vert_main_edje");
    evas_object_name_set(e, "main-edje");
    edje_object_part_swallow(mw, "contents", e);

    Evas_Object *fs_i = eoi_create_themed_edje(evas, THEME_EDJE, "fullscreen_icon");
    evas_object_name_set(fs_i, "fs-icon");
    edje_object_part_swallow(mw, "state-icons", fs_i);

    orig_image = evas_object_image_filled_add(evas);
    evas_object_name_set(orig_image, "orig_image");

    image = evas_object_image_filled_add(evas);
    evas_object_name_set(image, "image");
    evas_object_image_smooth_scale_set(image, EINA_TRUE);
    evas_object_image_smooth_scale_set(orig_image, EINA_TRUE);
    evas_object_show(image);

    read_keymap(operations);
    update_buttons();
    update_footer();
    render_cur_image();

    /* start the main event loop */
    ecore_main_loop_begin();

    evas_object_del(image);
    evas_object_del(bg);

    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();
    edje_shutdown();

    return 0;
}
