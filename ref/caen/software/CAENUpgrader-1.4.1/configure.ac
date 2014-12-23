#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.5])
AC_INIT([CAENUpgrader], [1.3.0], [])
AM_INIT_AUTOMAKE([CAENUpgrader], [1.3.0])
AC_OUTPUT([Makefile cvUpgrade/Makefile cvUpgrade/src/Makefile CAENBridgeUpgrade/Makefile CAENBridgeUpgrade/src/Makefile CAENUpgraderGUI/Makefile])
AC_CONFIG_SRCDIR([cvUpgrade/include/flash.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([CAENVME],  [CAENVME_Init])
AC_CHECK_LIB([CAENComm], [CAENComm_OpenDevice])
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

