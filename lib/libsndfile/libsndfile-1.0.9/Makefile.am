## Process this file with automake to produce Makefile.in

SUBDIRS = man doc Win32 MacOS9 Octave src examples tests
DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = reconfigure.mk acinclude.m4 libsndfile.spec.in sndfile.pc.in \
			check_libsndfile.py libsndfile_version_convert.py Make.bat
			
			 
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sndfile.pc

m4datadir = $(datadir)/aclocal

test: check-recursive



## Do not edit or modify anything in this comment block.
## The arch-tag line is a file identity tag for the GNU Arch 
## revision control system.
##
## arch-tag: e40c569e-8020-4e95-b774-6b0703614526

