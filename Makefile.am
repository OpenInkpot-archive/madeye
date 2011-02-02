SUBDIRS = src data po themes help

install-exec-hook:
	mkdir -p ${DESTDIR}/usr/share/help/madeye
	oi-help-compiler -u $(srcdir)/help ${DESTDIR}/usr/share/help/madeye
