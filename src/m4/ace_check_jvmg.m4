
# Copyright 2008-2019 Douglas Wikstrom
#
# This file is part of Verificatum Elliptic Curve library (VEC).
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

AC_DEFUN([ACE_CHECK_JVMG],[
AC_REQUIRE([ACE_PROG_JAVA])

AC_ARG_ENABLE([check_jvmg],
     [  --disable-check_jvmg    Skip checking that JVMG is installed.],
     [],[# Check for non-standard library we need.
ace_res=$($JAVA $JAVAFLAGS -classpath ../jvmg/jvmg.jar:$CLASSPATH:checks TestLoadJVMG)

echo -n "checking for jvmg.jar... "
if test "x$ace_res" = x;
then
   echo "yes"
else
   echo "no"
   AC_MSG_ERROR([$ace_res Please make sure that JVMG is installed (found at www.verificatum.org) and that your \$CLASSPATH points to the proper location. You can check your JVMG installation using 'java jvmg.Test'. This should give you a usage description. If you get a NoClassDefFoundError, then the proper jvmg.jar file can not be found. You may use the option --disable-check_jvmg to ignore this error if you know that you will install this package later.])
fi
])
])
