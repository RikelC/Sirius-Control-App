#!/bin/csh

tmux has-session -t numexo2
if ($? == 0)then
 echo "numexo2 session exits.."
else
    tmux new-session -s numexo2 -n boards -d 
    #divide the window into 16 equqlize panes
    #First create 4 panes numbered as 0--3 by default
    tmux split-window -h
    tmux select-pane -t 0
    tmux split-window -v
    tmux select-pane -t 2
    tmux split-window -v
    #Now divide each pane into 4 panes 
    tmux select-pane -t 0
    tmux split-window -h
    tmux select-pane -t 0
    tmux split-window -v
    tmux select-pane -t 2
    tmux split-window -v
    #0-6
    tmux select-pane -t 4
    tmux split-window -h
    tmux select-pane -t 4
    tmux split-window -v
    tmux select-pane -t 6
    tmux split-window -v
    #0--9
    tmux select-pane -t 8
    tmux split-window -h
    tmux select-pane -t 8
    tmux split-window -v
    tmux select-pane -t 10
    tmux split-window -v
    #0--12
    tmux select-pane -t 12
    tmux split-window -h
    tmux select-pane -t 12
    tmux split-window -v
    tmux select-pane -t 14
    tmux split-window -v
    tmux select-layout -t numexo2 tiled
    #0--15
    #DSSD-FRONT
    #Numexo2-80
    tmux send-keys -t numexo2:0.0 'telnet gansterm28 10008' C-m
    #Numexo2-81
    tmux send-keys -t numexo2:0.1 'telnet gansterm28 10007' C-m
    #Numexo2-69
    tmux send-keys -t numexo2:0.2 'telnet gansterm28 10006' C-m
    #Numexo2-65
    tmux send-keys -t numexo2:0.3 'telnet gansterm28 10005' C-m
    #Numexo2-75
    tmux send-keys -t numexo2:0.4 'telnet gansterm28 10004' C-m
    #Numexo2-73
    tmux send-keys -t numexo2:0.5 'telnet gansterm28 10003' C-m
    #Numexo2-74
    tmux send-keys -t numexo2:0.6 'telnet gansterm28 10002' C-m
    #Numexo2-71
    tmux send-keys -t numexo2:0.7 'telnet gansterm28 10001' C-m
    #DSSD-BACk
    #Numexo2-72
    tmux send-keys -t numexo2:0.8 'telnet gansterm28 10017' C-m
    #Numexo2-77
    tmux send-keys -t numexo2:0.9 'telnet gansterm28 10018' C-m
    #Numexo2-70
    tmux send-keys -t numexo2:0.10 'telnet gansterm28 10019' C-m
    #Numexo2-79
    tmux send-keys -t numexo2:0.11 'telnet gansterm28 10020' C-m
    #Numexo2-78
    tmux send-keys -t numexo2:0.12 'telnet gansterm28 10021' C-m
    #Numexo2-76
    tmux send-keys -t numexo2:0.13 'telnet gansterm28 10022' C-m
    #Numexo2-64
    tmux send-keys -t numexo2:0.14 'telnet gansterm28 10023' C-m
    #Numexo2-92
    tmux send-keys -t numexo2:0.15 'telnet gansterm28 10024' C-m
    #
    tmux select-window -t numexo2:0
    #

endif
    tmux attach -t numexo2

#-----------------------------------------------
# This script is for surveying the numexo2 boards through telnet
# Author: Rikel CHAKMA
# Date: 19/05/2022
# ----------------------------------------------
