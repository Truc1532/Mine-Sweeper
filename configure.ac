AC_INIT([Mine-sweeper], [1.0])
AM_INIT_AUTOMAKE([foreign])  # Using 'foreign' to avoid strict checks for files

# Check for the GNU Compiler Collection
AC_PROG_CC

# Check for SDL2
PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0], [have_sdl2=yes], [have_sdl2=no])
if test "x$have_sdl2" = "xno"; then
    AC_MSG_ERROR([SDL2 library is required but not found])
fi

# Check for SDL2_mixer
PKG_CHECK_MODULES([SDL2_MIXER], [SDL2_mixer >= 2.0], [have_sdl2_mixer=yes], [have_sdl2_mixer=no])
if test "x$have_sdl2_mixer" = "xno"; then
    AC_MSG_ERROR([SDL2_mixer library is required but not found])
fi

# Check for libjack
PKG_CHECK_MODULES([JACK], [jack >= 1.9], [have_jack=yes], [have_jack=no])
if test "x$have_jack" = "xno"; then
    AC_MSG_ERROR([libjack library is required but not found])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
