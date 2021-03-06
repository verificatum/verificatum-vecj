# Verificatum Elliptic Curve Library for Java (VECJ)


## Overview

VECJ allows calling [Verificatum Elliptic Curve library
(VEC)](https://github.com/verificatum/verificatum-vec) from Java
applications. This drastically improves the speed of such operations
compared to pure Java implementations. The following assumes that you
are using a release. Developers should also read `README_DEV.md`.


## Building

The source consists of a single Java class `com.verificatum.vecj.VEC`
and C code that basically provides a wrapper of VEC.

The `LIBRARY_PATH` must point to `libgmp.la` and `libvec.la` and
`C_INCLUDE_PATH` must point to `gmp.h` and `vec.h`. This is usually
the case automatically after installing GMP and VEC. Then use

        ./configure
        make

to build the library. If you prefer to use the
[Clang compiler](https://clang.llvm.org) in place of GCC for the native code,
then you may use `./configure CC=clang` instead of the above to enable
it.

**Caution: Please understand that although it seems that Clang works
as well as GCC, switching compiler is a large change for mature
software.**


# Installing

1. Use

        make install

   to install the library `libvecj-<VERSION>.{la,a,so}` and the
   jar-file `verificatum-vecj-<VERSION>.jar` in the standard
   locations.

2. You should also make sure that the newly installed jar-file is
   found by `java` by updating your CLASSPATH. On Ubuntu you can use
   something like the following snippet in your init script.

        export CLASSPATH=/usr/local/share/java/verificatum-vecj-<VERSION>.jar:${CLASSPATH}

3. You need to tell the JVM where your native library, i.e.,
   `libvecj-<VERSION>.{la,a,so}`, can be found. You may either pass the
   location using the `java.library.path` property, e.g.,

        java -Djava.library.path=/usr/local/lib/

   or you can set the shell variable `LD_LIBRARY_PATH` once and for
   all in an init file, e.g.,

        export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}

4. You can test if you managed to build correctly by executing:

        make check

   This runs a set of tests.


## Usage

The software is supposed to be used by other applications, but you can
use

        make bench

to benchmark the group operations.


## API Documentation

You may use
 
        make api

to invoke Javadoc to build the API. The API is not installed
anywhere. You can copy it to any location.


## Reporting Bugs

Minor bugs should be reported in the repository system as issues or
bugs. Security critical bugs, vulnerabilities, etc should be reported
directly to the Verificatum Project. We will make best effort to
disclose the information in a responsible way before the finder gets
proper credit.
