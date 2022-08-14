#!/bin/tcsh -f


    echo "building GUI app.............."

    rm -f CMakeCache.txt cmake_install.cmake Makefile

    rm -rf CMakeFiles

    sleep 1

    cmake ./

    sleep 1

    make

    echo "GUI app built at `date`"
