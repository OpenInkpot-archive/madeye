#ifndef MADEYE_H_
#define MADEYE_H_

#define FIT_WIDTH 0
#define FIT_HEIGHT 1
#define FIT_BEST 2
#define FIT_STRETCH 3
#define FIT_NO 4


char *get_theme_file(void);
int get_win_width(void);
int get_win_height(void);
double get_zoom_inc(void);
void set_zoom_inc(double newzoominc);
double get_hpan_inc(void);
void set_hpan_inc(double newhpaninc);
double get_vpan_inc(void);
void set_vpan_inc(double newvpaninc);
int get_lefttrim(void);
void set_lefttrim(int newlefttrim);
int get_righttrim(void);
void set_righttrim(int newrighttrim);
int get_toptrim(void);
void set_toptrim(int newtoptrim);
int get_bottomtrim(void);
void set_bottomtrim(int newbottomtrim);
int get_fit_mode(void);
void set_fit_mode(int newfitmode);
int get_reader_mode(void);
int get_antialias_mode(void);
void set_antialias_mode(int newantialiasmode);
void set_reader_mode(int newreadermode);
int get_num_pages(void);
void goto_page(int newpage);
int get_cur_page(void);

void render_cur_page(void);
void prerender_next_page(void);
void reset_cur_panning(void);
#endif
