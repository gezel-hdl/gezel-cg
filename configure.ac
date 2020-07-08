AC_INIT([gezel-cg], [2.5], [https://github.com/gezel-hdl/gezel/issues])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_MACRO_DIRS([m4])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
LT_INIT

# Check for gmp
AC_CHECK_LIB(gmp, __gmpz_init, , [AC_MSG_ERROR([GNU Multiprecision Library is required])])

dnl ----------------------------------------------
AX_ARG_DISABLE(vhdl,
    AS_HELP_STRING([--disable-vhdl], [build VHDL code generator]))

AM_CONDITIONAL([BUILDVHDL], [test "$enable_vhdl" = yes])


dnl ----------------------------------------------
AX_ARG_DISABLE(verilog,
    AS_HELP_STRING([--disable-verilog], [build Verilog code generator]))

AM_CONDITIONAL([BUILDVERILOG], [test "$enable_verilog" = yes])

dnl ----------------------------------------------
dnl TODO: REMOVE THIS!
AC_ARG_ENABLE(buildeval,
    AS_HELP_STRING([--enable-eval], [build evaluation version [default=no]]))

evaldefine=
if test "$enable_eval" = yes; then
  evaldefine=-DENABLEEVAL;
fi;

CPPFLAGS="$CPPFLAGS $evaldefine";

dnl ----------------------------------------------
AX_ARG_DISABLE(igc,
    AS_HELP_STRING([--disable-igc], [build incremental GEZEL compiler]))

AM_CONDITIONAL([BUILDIGC], [test "$enable_igc" = yes])

dnl ----------------------------------------------

gezeldir=
AC_ARG_WITH(gezel,
     AS_HELP_STRING([--with-gezel], [Specify location of GEZEL installation]),
     [gezeldir="$withval"])

dnl ----------------------------------------------
CPPFLAGS="$CPPFLAGS -I$gezeldir/include/gezel";
LDFLAGS="$LDFLAGS -L$gezeldir/lib";

needlibdl=no;
if test "$host_os" != "cygwin"; then
  needlibdl=yes;
fi;
AM_CONDITIONAL([LINKLIBDL], [test "$needlibdl" = yes])

dnl Checks for libraries.

dnl Checks for header files.

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug], [Compile with debug information [default=no]]))

XCXXFLAGS="-Wall -O3 -fomit-frame-pointer"
if test "$enable_debug" = "yes"; then
   XCXXFLAGS="-Wall -g -pg"
fi;

CXXFLAGS="$CXXFLAGS $XCXXFLAGS"

dnl Checks for typedefs, structures, and compiler characteristics.

dnl TODO: REMOVE THIS!
# see if we can use ostringstream and sstream
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <sstream>
using std::ostringstream;
],[ostringstream message; message << "Hello"; return 0;],
AC_DEFINE(HAVE_SSTREAM,,[define if the compiler has stringstream]), )

dnl Include checks for Simit-ARM (so that correct config.h is created)
AC_CHECK_HEADERS(fcntl.h unistd.h sys/types.h sys/stat.h sys/time.h sys/times.h sys/resource.h stdint.h inttypes.h)
AC_TYPE_UID_T
AC_C_INLINE
AC_C_BIGENDIAN 
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_STRUCT_ST_BLKSIZE
AC_STRUCT_ST_BLOCKS
AC_STRUCT_ST_RDEV
AC_TYPE_SIGNAL

AC_OUTPUT(Makefile vhdl/Makefile verilog/Makefile igc/Makefile)
