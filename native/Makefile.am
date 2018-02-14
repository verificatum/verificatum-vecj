
# Copyright 2008-2018 Douglas Wikstrom
#
# This file is part of Verificatum Elliptic Curve library (VEC).
#
# VEC is free software: you can redistribute it and/or modify it
# under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# VEC is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General
# Public License for more details.
#
# You should have received a copy of the GNU Affero General Public
# License along with VEC. If not, see <http://www.gnu.org/licenses/>.

ACLOCAL_AMFLAGS=-I m4

# Extracts the flags used by GMP.
GMP_CFLAGS := $(shell ./extract_GMP_CFLAGS)

# We use pedantic flags, strip the optimization flag of GMP, and
# insert our own level of optimization.
AM_CFLAGS := -O3 -Wall -W -Werror $(shell echo ${GMP_CFLAGS} | sed -e "s/-O[O12345]//")

libvecj_la_LIBADD = -lgmp -lvec

# We use -release to glue the native code and Java code together. We
# are aware that this violate common practice for library versioning.
libvecj_la_LDFLAGS = -release $(VERSION)

# This is generated from a Java file and copied to this directory by
# the parent directory.
# include_HEADERS = com_verificatum_vecj_VEC.h

lib_LTLIBRARIES = libvecj.la
libvecj_la_SOURCES = com_verificatum_vecj_VEC.c convert.c convert.h

dist_noinst_DATA = extract_GMP_CFLAGS.c

clean-local:
	find . -name "*~" -delete
	rm -rf compile
