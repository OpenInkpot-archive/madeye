#ifndef MADEYE_KEYBOARD_H_
#define MADEYE_KEYBOARD_H_

/* FIXME */
#define KEYMAP_FILE "/home/tomek/OI/madeye/data/keymap.desktop.ini"

Eina_Hash* keymap;
static void key_handler(void *data, Evas *evas, Evas_Object *obj, void *event_info);
int read_keymap(void);

#endif
