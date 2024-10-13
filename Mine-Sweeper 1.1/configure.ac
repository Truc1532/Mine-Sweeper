AC_INIT([Prototype_game], [1.0])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC

PKG_CHECK_MODULES([SDL2], [sdl2 >= 2.0], [have_sdl2=yes], [have_sdl2=0])
if test "x$have_sdl2" = "xno"; then
    AC_MSG_ERROR([SDL2 library is required but not found])
fi

PKG_CHECK_MODULES([SDL2_MIXER], [SDL2_mixer >= 2.0], [have_sdl2_mixer=yes], [have_sdl2_mixer=no])
if test "x$have_sdl2_mixer" = "xno"; then
    AC_MSG_ERROR([SDL2_mixer library is required but not found])
fi

PKG_CHECK_MODULES([JACK], [jack >= 1.9], [have_jack=yes], [have_jack=no])
if test "x$have_jack" = "xno"; then
    AC_MSG_ERROR([libjack library is required but not found])
fi

PKG_CHECK_MODULES([OPENGL], [gl >= 1.2], [have_opengl=yes], [have_opengl=no])
if test "x$have_opengl" = "xno"; then
    AC_MSG_ERROR([OpenGL 1.2 is required but not found])
fi

PKG_CHECK_MODULES([GLEW], [glew >= 2.2], [have_glew=yes], [have_glew=no])
if test "x$have_glew" = "xno"; then
    AC_MSG_ERROR([GLEW library is required but not found])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT