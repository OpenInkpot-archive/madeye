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

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include "keyhandler.h"
#include "dialogs.h"
#include "madeye.h"
#include "database.h"
#define REL_THEME "themes/themes_oitheme.edj"

#define ROUND(f) (int)floor(f + 0.5)

//pthread_t thread;

Evas *evas;
Evas_Object *image;
//char          *filename;

//int numpages;
//int curpage=0;
//bool active_image=1;
//int prerendering=0;
int fitmode=FIT_WIDTH;
int readermode=0;
double zoom=1.0;
double zoominc=0.1;
double hpaninc=0.5;
double vpaninc=0.5;

int lefttrim=0;
int righttrim=0;
int toptrim=0;
int bottomtrim=0;

int winwidth=600;
int winheight=800;

/*
 * Returns edje theme file name.
 */
char *get_theme_file(void) {
	char *rel_theme;
	asprintf(&rel_theme, "%s/%s", "/usr/share/madeye", REL_THEME);
	return rel_theme;
}

int get_win_width(void) {
    return winwidth;    
}

int get_win_height(void) {
    return winheight;    
}

double get_zoom_inc(void) {
    return zoominc;    
}

void set_zoom_inc(double newzoominc) {
    zoominc=newzoominc;    
}

double get_hpan_inc(void) {
    return hpaninc;    
}

void set_hpan_inc(double newhpaninc) {
    hpaninc=newhpaninc;    
}

double get_vpan_inc(void) {
    return vpaninc;    
}

void set_vpan_inc(double newvpaninc) {
    vpaninc=newvpaninc;    
}

int get_lefttrim(void) {
    return lefttrim;    
}

void set_lefttrim(int newlefttrim) {
    lefttrim=newlefttrim;    
}

int get_righttrim(void) {
    return righttrim;    
}

void set_righttrim(int newrighttrim) {
    righttrim=newrighttrim;    
}

int get_toptrim(void) {
    return toptrim;    
}

void set_toptrim(int newtoptrim) {
    toptrim=newtoptrim;    
}

int get_bottomtrim(void) {
    return bottomtrim;    
}

void set_bottomtrim(int newbottomtrim) {
    bottomtrim=newbottomtrim;    
}

int get_fit_mode(void) {
    return fitmode;    
}

void set_fit_mode(int newfitmode) {
    fitmode=newfitmode;
}

int get_reader_mode(void) {
    return readermode;    
}

void set_reader_mode(int newreadermode) {
    readermode=(newreadermode!=0);    
    
}

/*
int get_num_pages(void) {
    return numpages;
}

void goto_page(int newpage) {
    curpage=newpage;
    reset_cur_panning();
    render_cur_image();
    //prerender_next_page();
}

int get_cur_page(void) {
    return curpage;    
}
*/

void render_cur_image() {
    int width,height;
	evas_object_image_size_get(image,&width,&height);
    evas_object_resize(image, 100, 100);
    evas_object_image_fill_set(image, 0, 0, 100, 100);

#if 0
    double fitwidthzoom=((double)get_win_width())/((double)(width-lefttrim-righttrim))*zoom;
    double fitheightzoom=((double)get_win_height())/((double)(height-toptrim-bottomtrim))*zoom;
    
    double scalex;
    double scaley;
    
    if(fitmode==FIT_WIDTH)
    {
        scalex=fitwidthzoom;    
        scaley=fitwidthzoom;
    }
    else if(fitmode==FIT_HEIGHT)
    {
        scalex=fitheightzoom;
        scaley=fitheightzoom;
    }
    else if(fitmode==FIT_BEST)
    {
        if(fitwidthzoom<=fitheightzoom)
        {
            scalex=fitwidthzoom;
            scaley=fitwidthzoom;
        }
        else
        {
            scalex=fitheightzoom;
            scaley=fitheightzoom;
        }
        
    }
    else if(fitmode==FIT_STRETCH)
    {
        scalex=fitwidthzoom;
        scaley=fitheightzoom;
    
    }
    else if(fitmode==FIT_NO)
    {
        scalex=1.0;
        scaley=1.0;
        
    }
    
    //epdf_page_scale_set (page,scalex,scaley);
    //epdf_page_scale_set (page,1.0,1.0);
    //epdf_page_scale_set(page,zoom,zoom);
#endif
    if(!lefttrim && !righttrim && !toptrim && !bottomtrim)
    {
   		evas_object_show(image);
    }
    else
    {
        //epdf_page_render_slice (page,pdfobj,(int)(((double)lefttrim)*scalex),(int)(((double)toptrim)*scaley),(int)(((double)(width-lefttrim-righttrim))*scalex),(int)(((double)(height-toptrim-bottomtrim))*scaley));
                             
        
    }
    //fprintf(stderr,"\nwidth=%d,height=%d,ltrim=%d,rtrim=%d,ttrim=%d,btrim=%d,fwzoom=%f,fhzoom=%f\n",width,height,lefttrim,righttrim,toptrim,bottomtrim,fitwidthzoom,fitheightzoom);
}

#if 0
void *thread_func(void *vptr_args)
{
    if(curpage>=(numpages-1))
        return NULL;

    Evas_Object *pdfobj;
    if(active_image)
        pdfobj=evas_object_name_find(evas,"pdfobj2");
    else
        pdfobj=evas_object_name_find(evas,"pdfobj1");
    epdf_page_page_set(page,curpage+1);
    int width,height;
    epdf_page_size_get (page, &width, &height);
    //epdf_page_scale_set (page,((double)get_win_width())/((double)width)*zoom,((double)get_win_height())/((double)height)*zoom);
    double fitwidthzoom=((double)get_win_width())/((double)(width-lefttrim-righttrim))*zoom;
    double fitheightzoom=((double)get_win_height())/((double)(height-toptrim-bottomtrim))*zoom;
    
    double scalex;
    double scaley;
    
    if(fitmode==FIT_WIDTH)
    {
        scalex=fitwidthzoom;    
        scaley=fitwidthzoom;
    }
    else if(fitmode==FIT_HEIGHT)
    {
        scalex=fitheightzoom;
        scaley=fitheightzoom;
    }
    else if(fitmode==FIT_BEST)
    {
        if(fitwidthzoom<=fitheightzoom)
        {
            scalex=fitwidthzoom;
            scaley=fitwidthzoom;
        }
        else
        {
            scalex=fitheightzoom;
            scaley=fitheightzoom;
        }
        
    }
    else if(fitmode==FIT_STRETCH)
    {
        scalex=fitwidthzoom;
        scaley=fitheightzoom;
    
    }
    else if(fitmode==FIT_NO)
    {
        scalex=1.0;
        scaley=1.0;
        
    }
    
    //epdf_page_scale_set (page,scalex,scaley);
    //epdf_page_scale_set (page,1.0,1.0);
    //epdf_page_scale_set(page,zoom,zoom);
    if(!lefttrim && !righttrim && !toptrim && !bottomtrim)
    {
        epdf_page_render (page,pdfobj);
    }
    else
    {
        epdf_page_render_slice (page,pdfobj,(int)(((double)lefttrim)*scalex),(int)(((double)toptrim)*scaley),(int)(((double)(width-lefttrim-righttrim))*scalex),(int)(((double)(height-toptrim-bottomtrim))*scaley));
                             
        
    }
    //prerendering=0;
    return NULL;

}
#endif

int are_legal_coords(int x1,int y1,int x2,int y2) {
    int xs_in_range=((x1>0&&x1<get_win_width())||(x2>0&&x2<get_win_width()));
    int ys_in_range=((y1>0&&y1<get_win_height())||(y2>0&&y2<get_win_height()));
    int xs_opposite=(x1<=0&&x2>=get_win_width());
    int ys_opposite=(y1<=0&&y2>=get_win_height());
    if((ys_in_range && xs_in_range) || (ys_in_range&& xs_opposite) || (xs_in_range && ys_opposite) || (xs_opposite && ys_opposite))
        return 1;
    return 0;
    
    
}

void pan_cur_page(int panx,int pany) {
	/*
    Evas_Object *pdfobj;
    if(active_image)
        pdfobj=evas_object_name_find(evas,"pdfobj1");
    else
        pdfobj=evas_object_name_find(evas,"pdfobj2"); 
	*/
    int x,y,w,h;
    evas_object_geometry_get(image,&x,&y,&w,&h);
    
    
    if(are_legal_coords(x+panx,y+pany,x+w+panx,y+h+pany))
        evas_object_move (image,x+panx,y+pany);
}

void reset_cur_panning(void) {
	/*
    Evas_Object *pdfobj;
    if(active_image)
        pdfobj=evas_object_name_find(evas,"pdfobj1");
    else
        pdfobj=evas_object_name_find(evas,"pdfobj2"); 
    evas_object_move (pdfobj,0,0);    
	*/
}

void reset_next_panning(void) {
	/*
    Evas_Object *pdfobj;
    if(active_image)
        pdfobj=evas_object_name_find(evas,"pdfobj2");
    else
        pdfobj=evas_object_name_find(evas,"pdfobj1"); 
    evas_object_move (pdfobj,0,0);    
	*/
}

void ensure_thread_dead(void) {
    /*
    if(prerendering)
        pthread_join(thread, NULL);
    prerendering=0;
    */

}

void prerender_next_page(void) {
	//evas_object_image_preload(Evas_Object *obj, Evas_Bool cancel)
    //ensure_thread_dead();
    //prerendering=1;
    //pthread_create(&thread, NULL, thread_func, NULL);
}


void flip_pages(void) {
	/*
    Evas_Object *active,*inactive;
    if(active_image) {
        active=evas_object_name_find(evas,"pdfobj1");
        inactive=evas_object_name_find(evas,"pdfobj2");
        active_image=2;
    }
    else {
        active=evas_object_name_find(evas,"pdfobj2");
        inactive=evas_object_name_find(evas,"pdfobj1");
        active_image=1;
    }
    evas_object_hide(active);
    evas_object_show(inactive);
	*/
}

/*
void next_page(void) {
    if(curpage>=(numpages-1))
        return;
    curpage++;
    //pthread_join(thread, NULL);
    ensure_thread_dead();
    reset_next_panning();
    flip_pages();
    prerender_next_page();
}

void prev_page(void) {
    if(curpage<=0)
        return;
    curpage--;
    reset_cur_panning();
    render_cur_image();
    
    prerender_next_page();
}
*/

/* GUI */

/* Main key handler */

void main_esc(Evas *e, Evas_Object *obj) {
    ecore_main_loop_quit();
}

void main_ok(Evas *e, Evas_Object *obj) {
    Evas_Object *bgobj=evas_object_name_find(evas,"background");
    PreferencesDialog(evas,bgobj);
}

void main_shift(Evas *e, Evas_Object *obj) {
    
}

void main_nav_up(Evas *e, Evas_Object *obj) {
    
}

void main_nav_down(Evas *e, Evas_Object *obj) {
    
}

void main_nav_left(Evas *e, Evas_Object *obj) {
    
    //prev_page();
}

void main_nav_right(Evas *e, Evas_Object *obj) {
	/*
    if(readermode)
    {
        Evas_Object *pdfobj;
        int pan_amt=(-1)*ROUND(((double)get_win_height())*vpaninc);
        if(active_image)
            pdfobj=evas_object_name_find(evas,"pdfobj1");
        else
            pdfobj=evas_object_name_find(evas,"pdfobj2"); 
        int x,y,w,h;
        evas_object_geometry_get(pdfobj,&x,&y,&w,&h);
    
    
        if(are_legal_coords(x,y+pan_amt,x+w,y+h+pan_amt))
            pan_cur_page(0,pan_amt);
        //else
            //next_page();
    }
    //else
      //  next_page();
	*/
}

void main_nav_sel(Evas *e, Evas_Object *obj) {
    
    
}

void main_nav_menubtn(Evas *e, Evas_Object *obj) {
    
}

void main_item(Evas *e, Evas_Object *obj,int index, bool lp) {
    if(index==1)
    {
        pan_cur_page((-1)*ROUND(((double)get_win_width())*hpaninc),0);
    }
    else if(index==2)
    {
        pan_cur_page(ROUND(((double)get_win_width())*hpaninc),0);
        
    }
    else if(index==3)
    {
        pan_cur_page(0,ROUND(((double)get_win_height())*vpaninc));
        
    }
    else if(index==4)
    {
        pan_cur_page(0,(-1)*ROUND(((double)get_win_height())*vpaninc));
        
    }
    else if(index==5)
    {
        reset_cur_panning();
        
    }
    else if(index==6)
    {
        Evas_Object *bgobj=evas_object_name_find(evas,"background");
        GotoPageEntry(evas,bgobj);    
        
    }
    else if(index==7)
    {
		/*
        if((zoom-zoominc)>0)
        {
            Evas_Object *pdfobj;
            if(active_image)
                pdfobj=evas_object_name_find(evas,"pdfobj1");
            else
                pdfobj=evas_object_name_find(evas,"pdfobj2"); 
            int x,y,w,h;
            evas_object_geometry_get(pdfobj,&x,&y,&w,&h);
            int new_w=ROUND(((double)w)*(zoom-zoominc)/zoom);
            int new_h=ROUND(((double)h)*(zoom-zoominc)/zoom);
            if(are_legal_coords(x,y,x+new_w,y+new_h))
            {
                zoom-=zoominc;
                render_cur_image();
                //prerender_next_page();
            }
        }
		*/
        
    }
    else if(index==8)
    {
		/*
        Evas_Object *pdfobj;
        if(active_image)
            pdfobj=evas_object_name_find(evas,"pdfobj1");
        else
            pdfobj=evas_object_name_find(evas,"pdfobj2"); 
        int x,y,w,h;
        evas_object_geometry_get(pdfobj,&x,&y,&w,&h);
        int new_w=ROUND(((double)w)*(zoom+zoominc)/zoom);
        int new_h=ROUND(((double)h)*(zoom+zoominc)/zoom);
        if(are_legal_coords(x,y,x+new_w,y+new_h))
        {
            zoom+=zoominc;
            render_cur_image();
           // prerender_next_page();
        }
		*/
        
    }
    else if(index==9)
    {
        //prev_page();    
        
    }
    else if(index==0)
    {
		/*
        if(readermode)
        {
            Evas_Object *pdfobj;
            int pan_amt=(-1)*ROUND(((double)get_win_height())*vpaninc);
            if(active_image)
                pdfobj=evas_object_name_find(evas,"pdfobj1");
            else
                pdfobj=evas_object_name_find(evas,"pdfobj2"); 
            int x,y,w,h;
            evas_object_geometry_get(pdfobj,&x,&y,&w,&h);
    
    
            if(are_legal_coords(x,y+pan_amt,x+w,y+h+pan_amt))
                pan_cur_page(0,pan_amt);
            //else
              //  next_page();
        }
        //else
          //  next_page();
		*/
    }
}

static key_handler_info_t main_info =
{
    main_ok,
    main_esc,
    main_nav_up,
    main_nav_down,
    main_nav_left,
    main_nav_right,
    main_nav_sel,
    main_nav_menubtn,
    main_shift,
    main_item
};

void save_global_settings(char *filename) {
//    set_setting_INT(filename,"current_page",curpage);
    set_setting_DOUBLE(filename,"zoom_increment",zoominc);
    set_setting_DOUBLE(filename,"current_zoom",zoom);
    set_setting_DOUBLE(filename,"h_pan_increment",hpaninc);
    set_setting_DOUBLE(filename,"v_pan_increment",vpaninc);
    set_setting_INT(filename,"left_trim",lefttrim);
    set_setting_INT(filename,"right_trim",righttrim);
    set_setting_INT(filename,"top_trim",toptrim);
    set_setting_INT(filename,"bottom_trim",bottomtrim);
    set_setting_INT(filename,"fit_mode",fitmode);
    set_setting_INT(filename,"reader_mode",readermode);
}

void restore_global_settings(char *filename) {
    int temp11,temp12,temp13,temp14;
    double temp21,temp22,temp23,temp24;
    /*
    temp11=get_setting_INT(filename,"current_page");
    if(temp11>=0)
        curpage=temp11;
	*/
    
    
    temp21=get_setting_DOUBLE(filename,"zoom_increment");
    temp22=get_setting_DOUBLE(filename,"current_zoom");
    if(temp21>0 && temp22>0)
    {
        zoominc=temp21;
        zoom=temp22;
    }
    temp21=get_setting_DOUBLE(filename,"h_pan_increment");
    temp22=get_setting_DOUBLE(filename,"v_pan_increment");
    if(temp21>0 && temp22>0)
    {
        hpaninc=temp21;
        vpaninc=temp22;
        
    }
    temp11=get_setting_INT(filename,"left_trim");
    temp12=get_setting_INT(filename,"right_trim");
    temp13=get_setting_INT(filename,"top_trim");
    temp14=get_setting_INT(filename,"bottom_trim");
    if(temp11>=0 && temp12>=0 && temp13>=0 && temp14>=0)
    {
        lefttrim=temp11;
        righttrim=temp12;
        toptrim=temp13;
        bottomtrim=temp14;
        
    }
    temp11=get_setting_INT(filename,"reader_mode");
    if(temp11==0 || temp11==1)
    {
        readermode=temp11;    
        
    }
    temp11=get_setting_INT(filename,"fit_mode");
    if(temp11>=0)
    {
        fitmode=temp11;    
    }
}

int main(int argc, char *argv[]) {
    Ecore_Evas *ee;
    
    Evas_Object *bg; //background
	//Evas_Object *o1,*o2;

    /* initialize our libraries */
    evas_init();
    ecore_init();
    ecore_evas_init();
    edje_init();
    
	//char *cwd = get_current_dir_name();
	//free(cwd);
    
    const char *homedir=getenv("HOME");

    char *settingsdir;
    asprintf(&settingsdir,"%s/%s",homedir,".madeye/");
    if(!ecore_file_path_dir_exists(settingsdir))
        ecore_file_mkpath(settingsdir);
    free(settingsdir);

    char *dbfile;
    asprintf(&dbfile,"%s/%s",homedir,".madeye/files.db");
    int dbres=init_database(dbfile);
    free(dbfile);

    if(dbres!=(-1))
        restore_global_settings(argv[1]);
    /* create our Ecore_Evas and show it */
    ee = ecore_evas_software_x11_new(0, 0, 0, 0, 600, 800);
    
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);
    ecore_evas_title_set(ee, "madEYE");
    ecore_evas_show(ee);

    /* get a pointer our new Evas canvas */
    evas = ecore_evas_get(ee);

    /* create our white background */
    bg = evas_object_rectangle_add(evas);
    evas_object_color_set(bg, 255, 255, 255, 255);
    evas_object_move(bg, 0, 0);
    evas_object_resize(bg, 600, 800);
    evas_object_name_set(bg, "background");
    evas_object_focus_set(bg, 1);
    set_key_handler(bg,&main_info);
    evas_object_show(bg);
    
    
    //filename=argv[1];
	image = evas_object_image_filled_add(evas);
    evas_object_image_file_set(image, argv[1], NULL);
    if (!image) {
    // manage error here
        fprintf(stderr,"Error Opening %s\n", argv[1]);
	return 1;
    }
    //numpages=epdf_document_page_count_get(document);
    //page = epdf_page_new (document);
    //if (!page) {
    //    fprintf(stderr,"Error Processing Document");
    //}
    //active_image=1;

    //o2 = evas_object_image_add (evas);
    //evas_object_move (o2, 0, 0);
    //evas_object_name_set(o2, "pdfobj2");
    //evas_object_show (o2);

    //o1 = evas_object_image_add (evas);
    
    
	/*
    char *temp11,*temp12;
    if(dbres!=(-1))
    {
        temp11=get_setting(argv[1],"current_x");
        temp12=get_setting(argv[1],"current_y");
    }
    if(temp11 && temp12 && dbres!=(-1))
        evas_object_move (o1,(int)strtol(temp11,NULL,10),(int)strtol(temp12,NULL,10));
    else
        evas_object_move(o1,0,0);
    if(temp11)
        free(temp11);
    if(temp12)
        free(temp12);
    evas_object_name_set(o1, "pdfobj1");
    evas_object_show (o1);
	*/

    evas_object_name_set(image, "image");
	evas_object_move(image, 0, 0);
    render_cur_image();
    //prerender_next_page();
    

    /* start the main event loop */
    ecore_main_loop_begin();
    
    /* when the main event loop exits, shutdown our libraries */
    if(dbres!=(-1)) {
		/*
        save_global_settings(argv[1]);
        Evas_Object *pdfobj;
        if(active_image)
            pdfobj=evas_object_name_find(evas,"pdfobj1");
        else
            pdfobj=evas_object_name_find(evas,"pdfobj2"); 
        int x,y,w,h;
        evas_object_geometry_get(pdfobj,&x,&y,&w,&h);
        set_setting_INT(argv[1],"current_x",x);
        set_setting_INT(argv[1],"current_y",y);
		*/
        fini_database();
    }
    //evas_object_del (o1);
    //evas_object_del (o2);
    evas_object_del (bg);
    
    edje_shutdown();
    ecore_evas_shutdown();
    ecore_shutdown();
    evas_shutdown();

}
