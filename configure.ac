
dnl Process this file with autoconf to produce a configure script.
AC_INIT(vhdl/fdlvhd.cxx)
AM_INIT_AUTOMAKE(gcode, 2.4)
AM_CONFIG_HEADER(config.h:config.h.in)

AC_PREFIX_DEFAULT("`pwd`/build")

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL

# Check for gmp
AC_CHECK_LIB(gmp, __gmpz_init, , [AC_MSG_ERROR([GNU Multiprecision Library is required])])

dnl ----------------------------------------------
AC_ARG_ENABLE(vhdl,
    [  --enable-vhdl            build VHDL code generator [default=no]],
    vhdlenable=yes,vhdlenable=no)

AM_CONDITIONAL([BUILDVHDL], [test "$vhdlenable" = yes])


dnl ----------------------------------------------
AC_ARG_ENABLE(verilog,
    [  --enable-verilog            build Verilog code generator [default=no]],
    verilogenable=yes,verilogenable=no)

AM_CONDITIONAL([BUILDVERILOG], [test "$verilogenable" = yes])

dnl ----------------------------------------------
AC_ARG_ENABLE(buildeval,
    [  --enable-eval            build evaluation version [default=no]],
    buildeval=yes,buildeval=no)

evaldefine=
if test "$buildeval" = yes; then
  evaldefine=-DENABLEEVAL;
fi;

CPPFLAGS="$CPPFLAGS $evaldefine";

dnl ----------------------------------------------
AC_ARG_ENABLE(igc,
    [  --enable-igc             build incremental GEZEL compiler [default=no]],
    igcenable=yes,igcenable=no)

AM_CONDITIONAL([BUILDIGC], [test "$igcenable" = yes])

dnl ----------------------------------------------

gezeldir=
AC_ARG_WITH(gezel,
     [  --with-gezel           Specify location of GEZEL installation],
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

CXX_COMP=`basename $CXX`

case "$CXX_COMP" in
	c++ | g++)
		CXXFLAGS="-Wall -g -pg"
#		CXXFLAGS="-Wall -O3 -fomit-frame-pointer"
		;;
	*)
		;;
esac

CXXFLAGS="$CXXFLAGS $XCXXFLAGS"

dnl Checks for typedefs, structures, and compiler characteristics.

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

# AC_OUTPUT(Makefile vhdl/Makefile verilog/Makefile po/Makefile.in m4/Makefile igc/Makefile)
AC_OUTPUT(Makefile vhdl/Makefile m4/Makefile igc/Makefile)
