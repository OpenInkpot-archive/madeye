#include <Evas.h>
#include <Efreet.h>
#include <stdio.h>

#include <Evas.h>

#include <libkeys.h>

#include "keyboard.h"

#define KEYMAP_FILE "/usr/share/madeye/keymap.ini"

static keys_t* keys;
static _op* operations;

void key_handler(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Key_Up *e = (Evas_Event_Key_Up*)event_info;

    const char* action = keys_lookup_by_event(keys, "default", e);
    if(!action) return;

    for(_op* i = operations; i->opname; ++i)
    {
        if(!strcmp(action, i->opname))
            i->op();
    }
}

int read_keymap(_op* operations_)
{
    keys = keys_alloc("madeye");
    operations = operations_;
}
