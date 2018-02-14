
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
