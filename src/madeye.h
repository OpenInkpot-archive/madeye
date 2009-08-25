#ifndef MADEYE_H_
#define MADEYE_H_

struct _op {
	char *opname;
	void (*op)();
};

extern struct _op operations[];

void quit();

#endif
