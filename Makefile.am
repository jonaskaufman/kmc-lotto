# Store macros locally
ACLOCAL_AMFLAGS = -I build-aux/m4

#===========================================================================#
# Values are filled by Makemodule.am files
lib_LTLIBRARIES=
noinst_LIBRARIES=
noinst_LTLIBRARIES=
bin_PROGRAMS=
noinst_PROGRAMS=
check_PROGRAMS=
man1_MANS=
dist_bin_SCRIPTS=
nobase_include_HEADERS=
TESTS=

BUILT_SOURCES=

# Completely overkill, but we want the copyright files too
EXTRA_DIST=submodules

AM_CPPFLAGS=\
						 -I$(srcdir)/include\
						 -I$(srcdir)/submodules/googletest/googletest\
						 -I$(srcdir)/submodules/googletest/googletest/include\
						 -DABS_SRCDIR=\"$(abs_srcdir)\"\
						 -DABS_TOP_BUILDDIR=\"$(abs_top_builddir)\"

#===========================================================================#

include $(srcdir)/include/Makemodule.am		# lotto headers
include $(srcdir)/tests/Makemodule.am 		# all tests

#===========================================================================#
