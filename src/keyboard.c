#include <Evas.h>
#include <Efreet.h>
#include <stdio.h>
#include "keyboard.h"

Eina_Hash* keymap;

static void key_handler(void *data, Evas *evas, Evas_Object *obj, void *event_info) {
	Evas_Event_Key_Up* e = (Evas_Event_Key_Up*)event_info;

	const char* k = e->keyname;
	fprintf(stderr,"key pressed: %s, ",k);

	void* pointer;

	pointer = eina_hash_find(keymap, k);
	if (pointer!=NULL)
		fprintf(stderr,"action assigned: %s.\n", pointer);
	else
		fprintf(stderr,"NO action assigned.\n");
}

int read_keymap(void){
	if(!ecore_file_exists(KEYMAP_FILE)) {
		fprintf(stderr,"No keymap file!\n");
		return 1;
	}
	Efreet_Ini* read_k = efreet_ini_new(KEYMAP_FILE);
	efreet_ini_section_set(read_k,"foo");
	keymap = eina_hash_string_superfast_new(NULL);

	eina_hash_add(keymap,"Escape",(char*)efreet_ini_string_get(read_k,"Escape"));

	//fprintf(stderr,"%s\n",efreet_ini_string_get(read_k,"Escape"));

	efreet_ini_free(read_k);
}

