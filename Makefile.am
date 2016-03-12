ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src test
EXTRA_DIST = ItzamDocumentation.html LicenseOpenSource.txt LicenseClosedSource.txt reconf cleanup

dist-hook:

	if test -d ItzamC; then \
		mkdir $(distdir)/ItzamC; \
		mkdir $(distdir)/ItzamC/ThreadTest; \
		cp -p ItzamC/* $(distdir)/ItzamC; \
		cp -p ItzamC/ThreadTest/* $(distdir)/ItzamC/ThreadTest; \
	fi

uninstall-local:
	rm -rf $(distdir)/ItzamC
