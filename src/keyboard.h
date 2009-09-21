#ifndef MADEYE_KEYBOARD_H_
#define MADEYE_KEYBOARD_H_

typedef struct {
	char *opname;
	void (*op)();
} _op;

void key_handler(void *data, Evas *evas, Evas_Object *obj, void *event_info);
int read_keymap(_op* operations);

#endif
