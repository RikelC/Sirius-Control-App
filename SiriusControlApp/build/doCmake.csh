#!/bin/tcsh -f


    echo "building GUI app.............."

    rm -f CMakeCache.txt cmake_install.cmake Makefile

    rm -rf CMakeFiles

    sleep 1

    cmake ../sources/

    sleep 1

    make

    echo "GUI app built at `date`"

#------------------------------------------------------------------------
# This script is to simplify repeated CMake process
# Author: Rikel CHAKMA
# Date: 19/05/2022
# ------------------------------------------------------------------------
