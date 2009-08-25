#include <Evas.h>
#include <Efreet.h>
#include <stdio.h>
#include "madeye.h"
#include "keyboard.h"

#define KEYMAP_FILE "/usr/share/madeye/keymap.ini"

Eina_Hash *keymap;

void key_handler(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	Evas_Event_Key_Up *e = (Evas_Event_Key_Up*)event_info;

	void (*op)() = eina_hash_find(keymap, e->keyname);
	if(op)
		op();
	else
		fprintf(stderr,"keyname: %s -> NO action assigned.\n", e->keyname);
}

static Eina_Bool add_key(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
	for(struct _op *op = operations; op->opname != NULL; op++)
		if(!strncmp(op->opname, data, strlen(op->opname)))
			eina_hash_add(keymap, strdup(key), op->op);

	return EINA_TRUE;
}

int read_keymap(void)
{
	keymap = eina_hash_string_superfast_new(NULL);

	if(ecore_file_exists(KEYMAP_FILE)) {
		Efreet_Ini *read_k = efreet_ini_new(KEYMAP_FILE);
		eina_hash_foreach(eina_hash_find(read_k->data, "keymap"), add_key, NULL);
		efreet_ini_free(read_k);
	}

	eina_hash_add(keymap, "Escape", quit);
}
