# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = src themes

install-data-hook:
	$(mkdir_p) $(DESTDIR)$(xsession_dir)
	$(INSTALL) -D madeye.desktop $(DESTDIR)/usr/share/applications/madeye.desktop
