SUBDIRS = src data po themes help

desktopdir = $(datarootdir)/applications
desktop_DATA = madeye.desktop

install-exec-hook:
	mkdir -p ${DESTDIR}/usr/share/help/madeye
	oi-help-compiler -u $(srcdir)/help ${DESTDIR}/usr/share/help/madeye
