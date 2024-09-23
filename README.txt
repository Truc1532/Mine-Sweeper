# Mine-Sweeper
------DEPENDENCIES-------
You will need the GNU Compiler Collection. 
-Ubuntu-
    sudo apt-get install gcc
-Fedora-
    sudo dnf install gcc
-CentOS-
    sudo yum install gcc
-openSUSE-
    sudo zypper install gcc

You'll also need the SDL2 library
-Ubuntu-
    sudo apt-get install libsdl2-dev
-Fedora-
    sudo dnf install SDL2-devel
-CentOS-
    sudo yum install SDL2-devel
-openSUSE-
    sudo zypper install libSDL2-devel

You also need the SDL_mixer header. Sometimes the SDL_mixer header doesn't come with the sdl2 library by default.
-Ubuntu-
    sudo apt-get install libsdl2-mixer-dev
-Fedora-
    sudo dnf install SDL2_mixer-devel
-CentOS-
    sudo yum install SDL2_mixer-devel
-openSUSE-
    Most likely the SDL_mixer header comes with the SDL2 library by default. If not then you'll have to install it obviously.
    sudo zypper install libSDL2_mixer-devel
------COMPILING------
Compiling is really straight forward. First go into the games directory:
    cd path/to/game/directory
Once inside just enter:
    make

