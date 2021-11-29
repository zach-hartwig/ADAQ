#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_DEFUN(AC_CHECK_SYMBOL,
[AC_MSG_CHECKING([for $1 in $2])
AC_CACHE_VAL(ac_cv_func_$1,
[AC_REQUIRE_CPP()dnl
changequote(, )dnl
symbol="[^a-zA-Z_0-9]$1[^a-zA-Z_0-9]"
changequote([, ])dnl
ac_found=no
for ac_header in $2 ; do
  ac_safe=`echo "$ac_header" | sed 'y%./+-%__p_%' `
  if test $ac_found != "yes" ; then
      if eval "test \"`echo '$ac_cv_header_'$ac_safe`\" = yes"; then
            AC_EGREP_HEADER( $symbol, $ac_header, [ac_found="$ac_header"] )
      fi
  fi
done
if test "$ac_found" != "no" ; then
  AC_MSG_RESULT($ac_found)
  ifelse([$3], , :, [$3])
else
  AC_MSG_RESULT(no)
  ifelse([$4], , , [$4
])dnl
fi
])])

AC_PREREQ([2.67])
AC_INIT([CAENUpgrader], [1.7.1], [])
AM_INIT_AUTOMAKE([CAENUpgrader], [1.7.1])
AC_OUTPUT([Makefile cvUpgrade/Makefile cvUpgrade/src/Makefile CAENBridgeUpgrade/Makefile CAENBridgeUpgrade/src/Makefile HVUpgrade/Makefile HVUpgrade/src/Makefile DeskBoot/Makefile DeskBoot/src/Makefile CAENUpgraderGUI/Makefile])
AC_CONFIG_SRCDIR([cvUpgrade/include/flash.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_DEFINE(LINUX,[1],"Define LINUX")
# Checks for libraries.
AC_CHECK_HEADER(CAENVMElib.h,[a=0],[a=1])
AC_CHECK_HEADER(CAENVMEtypes.h,[a=$a],[a=1])
AC_CHECK_SYMBOL([cvUSB_A4818], CAENVMEtypes.h, [a=$a], [a=1])
AC_CHECK_LIB(CAENVME,CAENVME_Init,[a=$a], [a=1])
if test $a == 1; then
        AC_MSG_ERROR([
                CAENVMElib >= 3.2.0 must be installed on your system.])
fi
AC_CHECK_HEADER(CAENComm.h,[b=0],[b=1])
AC_CHECK_SYMBOL([CAENComm_USB_A4818], CAENComm.h, [b=$b], [b=1])
AC_CHECK_LIB(CAENComm, CAENComm_OpenDevice,[b=$b], [b=1])
if test $b == 1; then
        AC_MSG_ERROR([
                CAENComm >= 1.4.0 must be installed on your system.])
fi

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h sys/time.h sys/timeb.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([atexit gettimeofday memset pow select sqrt])

AC_OUTPUT

