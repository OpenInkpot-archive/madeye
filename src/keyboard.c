/*
 * Copyright (C) 2009 Tomasz Długosz <tomek3dgmail.com>
 * Copyright (C) 2009 Alexander Kerner <lunohod@openinkpot.org>
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

#include <stdio.h>
#include <string.h>
#include <Evas.h>

#include <libkeys.h>

#include "keyboard.h"

#define KEYMAP_FILE "/usr/share/madeye/keymap.ini"

#define __UNUSED__ __attribute__((__unused__))

static keys_t* keys;
static _op* operations;

void key_handler(void *data __UNUSED__, Evas *evas __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
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

void read_keymap(_op* operations_)
{
    keys = keys_alloc("madeye");
    operations = operations_;
}
