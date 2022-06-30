#!/bin/csh

gnome-terminal --title="Temp view" -- csh -c "python numexo2_temperature.py $1 $2"

#for full screen view replave --maximize by --full-screen
#-----------------------------------------------
# This script is for executing telnet_numexo2_tmux_view.sh in a new terminal
# Author: Rikel CHAKMA
# Date: 19/05/2022
# ---------------------------------------------- 