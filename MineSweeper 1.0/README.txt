# Mine-Sweeper
------DEPENDENCIES-------
Here are all the dependencies you will need to compile this game:
    libjack
    SDL2 (development library)
    SDL2_mixer 
    GNU Compiler Collection (gcc)
    make

------COMPILING------
Compiling is really straight forward. First go into the games directory:
    cd path/to/game/directory
Once inside enter the command below to allow autogen.sh to run as a program. In some cases you'll need to run the command as root:
    chmod +x autogen.sh
After that run the autogen.sh file:
    ./autogen.sh
If the autogen.sh file completed successfully then you can run the makefile:
    make
Optionally you can run:
    sudo make install

The last commmand is not really important. It will just make a copy of the executable over to /usr/local/bin. It will allow you to run the game just by entering 'Mine-sweeper' in the terminal (works only if you're in the game directory).

   