#!/bin/bash

tmux has-session -t numexo2
if [ $? == 0 ]; then
	echo "numexo2 session exits.."
 tmux kill-session -t numexo2
 echo "numexo2 session closed.."
else
 echo "no numexo2 tmux session exits.."
fi

#-----------------------------------------------
# This script is closing telnet viewer
# Author: Rikel CHAKMA
# Date: 19/05/2022
# ---------------------------------------------- 

