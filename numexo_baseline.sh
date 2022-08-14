#!/bin/bash

cmd=$1
boardList=$2
channelList=$3
value=$4
argc=$#

function sig_int(){
echo 'SIGINT caught. Exiting the current process.'
cd $current_dir
exit 0
}

function sig_hup(){
echo 'SIGHUP caught. Exiting the current process.'
cd $current_dir
exit 0
}

function sig_term(){
echo 'SIGTERM caught. Exiting the current process.'
cd $current_dir
exit 0
}

#[[:alnum:]]+$ #alpha numeric

function check_numerical(){
local val="$1"
channel_is=false
if [[ $val =~ ^[[:digit:]]+$ ]]; then
channel_is=true
else
channel_is=false
fi
}

function check_format(){
local str="$1"

format="" 
for (( i=0; i<${#str}; i++ )); do
if [ "${str:i:1}" == "-" ];then
 format="format1" 
fi
done

for (( i=0; i<${#str}; i++ )); do
if [ "${str:i:1}" == "," ]; then
format="format2"
fi
done
}


#Board arrays
function fill_list(){

numexoList=''
frontList=''
backList=''
dssdList=''
tunnelList=''
while IFS= read -r line; do
IFS="=" read -r ONE TWO <<< $line
if [ "${ONE^^}" = "FRONT" ]; then
    frontList+=" "
    frontList+=${TWO}
    dssdList+=" "
    dssdList+=${TWO}
    numexoList+=" "
    numexoList+=${TWO}
  elif [ "${ONE^^}" = "BACK" ]; then
    backList+=" "
    backList+=${TWO}
    dssdList+=" "
    dssdList+=${TWO}
    numexoList+=" "
    numexoList+=${TWO}
elif [ "${ONE^^}" = "TUNNEL" ]; then
    tunnelList+=" "
    tunnelList+=${TWO}
    numexoList+=" "
    numexoList+=${TWO}


else
echo "Not a good format" 
 fi
done </home/sirius/ganacq_manip/test/GRU_SIRIUS/GUser-build/ConfigFiles/numexo2Boards.config

}

function read_boards(){
if [ "${boardList^^}" = "ALL" ]
 then
for i in $numexoList
do
echo "Board: $i"
  set_host_port $i
  read_channel
sleep 0.5s
done
elif [ "${boardList^^}" = "FRONT" ]
 then
 for i in $frontList
do
echo "Board: $i"
  set_host_port $i
  read_channel
sleep 0.5s
done
elif [ "${boardList^^}" = "BACK" ]
 then
  for i in $backList
do
echo "Board: $i"
  set_host_port $i
  read_channel
sleep 0.5s 
done
elif [ "${boardList^^}" = "DSSD" ]
 then
  for i in $dssdList
do
echo "Board: $i"
  set_host_port $i
  read_channel
sleep 0.5s 
done

else
 echo "Board: $boardList"
 read_formatted_bList $boardList
fi

}

function write_boards(){
if [ "${boardList^^}" = "ALL" ]
 then
for i in $numexoList
do
echo "Board: $i"
  set_host_port $i
  write_channel
sleep 0.5s
done
elif [ "${boardList^^}" = "FRONT" ]
 then
 for i in $frontList
do
echo "Board: $i"
  set_host_port $i
  write_channel
sleep 0.5s
done
elif [ "${boardList^^}" = "BACK" ]
 then
  for i in $backList
do
echo "Board: $i"
  set_host_port $i
  write_channel
sleep 0.5s 
done
elif [ "${boardList^^}" = "DSSD" ]
 then
  for i in $dssdList
do
echo "Board: $i"
  set_host_port $i
  write_channel
sleep 0.5s 
done

else
 echo "Board $boardList"
  write_formatted_bList $boardList
fi

}

function read_formatted_bList(){
local var="$1"
check_numerical $var
one_check=false
if [ "$channel_is" = "true" ]; then
  for i in $numexoList 
 do
 if [ "${var}" -eq "$i" ]; then
   one_check=true  
fi
 done


if [ "$one_check" = "true" ]; then

  echo "Board: $var"
  set_host_port $var
  read_channel
else
echo "No Numexo2-$var Board is active"
fi
else
 check_format $var
if [ "$format" = "format1" ]; then

one_check=false
two_chek=false

IFS="-" read -r ONE TWO <<< $var
for i in $numexoList
do
 if [ "${ONE}" -eq "$i" ]; then
   one_check=true  
 fi
 if [ "${TWO}" -eq "$i" ]; then
   two_check=true  
 fi
done

if [ "$one_check" = "true" -a "$two_check" = "true" ]; then
 for i in $(seq ${ONE} ${TWO}) 
 do
  echo "Board: $i"
  set_host_port $i
  read_channel
 done
fi
fi

if [ "$format" = "format2" ]; then
 IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Board: $i "
  set_host_port $i
  read_channel
 done
fi
fi
}

function write_formatted_bList(){
local var="$1"
check_numerical $var
one_check=false
if [ "$channel_is" = "true" ]; then
  for i in $numexoList 
 do
 if [ "${var}" -eq "$i" ]; then
   one_check=true  
fi
 done


if [ "$one_check" = "true" ]; then

  echo "Board: $var"
  set_host_port $var
  write_channel
else
echo "No Numexo2-$var Board is active"
fi
else
check_format $var
if [ "$format" = "format1" ]; then


one_check=false
two_chek=false

IFS="-" read -r ONE TWO <<< $var
for i in $numexoList
do
 if [ "${ONE}" -eq "$i" ]; then
   one_check=true  
 fi
 if [ "${TWO}" -eq "$i" ]; then
   two_check=true  
 fi
done

if [ "$one_check" = "true" -a "$two_check" = "true" ]; then
 for i in $(seq ${ONE} ${TWO}) 
 do
  echo "Board: $i"
  set_host_port $i
  write_channel
 done
fi
fi

if [ "$format" = "format2" ]; then
 IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Board: $i "
  set_host_port $i
  write_channel
 done
fi
fi
}


function read_channel(){
check_numerical $channelList
if [ "$channel_is" = "true" ]; then
  if [ "$channelList" -gt 0 -a "$channelList" -lt 17 ]
   then
      read_specific_ch $channelList 
   fi
else
  if [ "${channelList^^}" = "ALL" ]
    then
     echo "Reading all channels"
     SC_client read /MEZ_1/DAC_1 DAC_A
     SC_client read /MEZ_1/DAC_1 DAC_B
     SC_client read /MEZ_1/DAC_2 DAC_A
     SC_client read /MEZ_1/DAC_2 DAC_B
     SC_client read /MEZ_2/DAC_1 DAC_A
     SC_client read /MEZ_2/DAC_1 DAC_B
     SC_client read /MEZ_2/DAC_2 DAC_A
     SC_client read /MEZ_2/DAC_2 DAC_B
     SC_client read /MEZ_3/DAC_1 DAC_A
     SC_client read /MEZ_3/DAC_1 DAC_B
     SC_client read /MEZ_3/DAC_2 DAC_A
     SC_client read /MEZ_3/DAC_2 DAC_B
     SC_client read /MEZ_4/DAC_1 DAC_A
     SC_client read /MEZ_4/DAC_1 DAC_B
     SC_client read /MEZ_4/DAC_2 DAC_A
     SC_client read /MEZ_4/DAC_2 DAC_B   
   else
    read_formatted_chList $channelList 
  fi
fi
}

function write_channel(){
check_numerical $channelList
if [ "$channel_is" = "true" ]; then
  if [ "$channelList" -gt 0 -a "$channelList" -lt 17 ]
   then
      write_specific_ch $channelList $value 
   fi
else

if [ "${channelList^^}" = "ALL" ]
 then
  SC_client write /MEZ_1/DAC_1 DAC_A $value
  SC_client write /MEZ_1/DAC_1 DAC_B $value
  SC_client write /MEZ_1/DAC_2 DAC_A $value
  SC_client write /MEZ_1/DAC_2 DAC_B $value
  SC_client write /MEZ_2/DAC_1 DAC_A $value
  SC_client write /MEZ_2/DAC_1 DAC_B $value
  SC_client write /MEZ_2/DAC_2 DAC_A $value
  SC_client write /MEZ_2/DAC_2 DAC_B $value
  SC_client write /MEZ_3/DAC_1 DAC_A $value
  SC_client write /MEZ_3/DAC_1 DAC_B $value
  DC_client write /MEZ_3/DAC_2 DAC_A $value
  SC_client write /MEZ_3/DAC_2 DAC_B $value
  SC_client write /MEZ_4/DAC_1 DAC_A $value
  SC_client write /MEZ_4/DAC_1 DAC_B $value
  SC_client write /MEZ_4/DAC_2 DAC_A $value
  SC_client write /MEZ_4/DAC_2 DAC_B $value  
  
else 
   write_formatted_chList $channelList $value
fi
fi
}



function read_specific_ch(){
local ch="$1"

case $ch in
            "1") SC_client read /MEZ_1/DAC_1 DAC_A;;
            "2") SC_client read /MEZ_1/DAC_1 DAC_B;;
            "3") SC_client read /MEZ_1/DAC_2 DAC_A;;
            "4") SC_client read /MEZ_1/DAC_2 DAC_B;;
            "5") SC_client read /MEZ_2/DAC_1 DAC_A;;
            "6") SC_client read /MEZ_2/DAC_1 DAC_B;;
            "7") SC_client read /MEZ_2/DAC_2 DAC_A;;
            "8") SC_client read /MEZ_2/DAC_2 DAC_B;;
            "9") SC_client read /MEZ_3/DAC_1 DAC_A;;
           "10") SC_client read /MEZ_3/DAC_1 DAC_B;;
           "11") SC_client read /MEZ_3/DAC_2 DAC_A;;
           "12") SC_client read /MEZ_3/DAC_2 DAC_B;;
           "13") SC_client read /MEZ_4/DAC_1 DAC_A;;
           "14") SC_client read /MEZ_4/DAC_1 DAC_B;;
           "15") SC_client read /MEZ_4/DAC_2 DAC_A;;
           "16") SC_client read /MEZ_4/DAC_2 DAC_B;;
            "*") echo "Not a valid Channel Number";;
 esac
}

function write_specific_ch(){
local ch="$1"
local val="$2"

case $ch in
            "1") SC_client write /MEZ_1/DAC_1 DAC_A $val;;
            "2") SC_client write /MEZ_1/DAC_1 DAC_B $val;;
            "3") SC_client write /MEZ_1/DAC_2 DAC_A $val;;
            "4") SC_client write /MEZ_1/DAC_2 DAC_B $val;;
            "5") SC_client write /MEZ_2/DAC_1 DAC_A $val;;
            "6") SC_client write /MEZ_2/DAC_1 DAC_B $val;;
            "7") SC_client write /MEZ_2/DAC_2 DAC_A $val;;
            "8") SC_client write /MEZ_2/DAC_2 DAC_B $val;;
            "9") SC_client write /MEZ_3/DAC_1 DAC_A $val;;
           "10") SC_client write /MEZ_3/DAC_1 DAC_B $val;;
           "11") SC_client write /MEZ_3/DAC_2 DAC_A $val;;
           "12") SC_client write /MEZ_3/DAC_2 DAC_B $val;;
           "13") SC_client write /MEZ_4/DAC_1 DAC_A $val;;
           "14") SC_client write /MEZ_4/DAC_1 DAC_B $val;;
           "15") SC_client write /MEZ_4/DAC_2 DAC_A $val;;
           "16") SC_client write /MEZ_4/DAC_2 DAC_B $val;;
            "*") echo "Not a valid Channel Number";;
  esac
}


function read_formatted_chList(){
local var="$1"
check_numerical $var
if [ "$channel_is" = "true" ]; then
 if [ "${var}" -gt 0 -a "${var}" -lt 17 ]; then
  read_specific_ch $var
else
echo "Please choose a channel between 1-16"
fi
else


check_format $var
if [ "$format" = "format1" ]; then

one_check=false
two_chek=false
IFS="-" read -r ONE TWO <<< $var

if [ "${ONE}" -gt 0 -a "${ONE}" -lt 17 ]; then
one_check=true  
fi
if [ "${TWO}" -gt 0 -a "${TWO}" -lt 17 ]; then
two_check=true
fi

if [ "$one_check" = "true" -a "$two_check" = "true" ]; then

if [ "${ONE}" -ge "${TWO}" ]; then
r1="${TWO}"
r2="${ONE}"
else
r1="${ONE}"
r2="${TWO}"
fi


for i in $(seq ${r1} ${r2}) 
do
 echo "Channel: $i"
 read_specific_ch $i
done
fi
fi

if [ "$format" = "format2" ]; then
  IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Channel: $i "
  read_specific_ch $i
 done

fi
fi

}


function write_formatted_chList(){
local var="$1"
local val="$2"

check_numerical $var
if [ "$channel_is" = "true" ]; then
 if [ "${var}" -gt 0 -a "${var}" -lt 17 ]; then
  write_specific_ch $var $val
else
echo "Please choose a channel between 1-16"
fi
else


check_format $var
if [ "$format" = "format1" ]; then

one_check=false
two_chek=false
IFS="-" read -r ONE TWO <<< $var

if [ "${ONE}" -gt 0 -a "${ONE}" -lt 17 ]; then
one_check=true  
fi
if [ "${TWO}" -gt 0 -a "${TWO}" -lt 17 ]; then
two_check=true
fi

if [ "$one_check" = "true" -a "$two_check" = "true" ]; then

if [ "${ONE}" -ge "${TWO}" ]; then
r2="${ONE}"
r1="${TWO}"
else
r1="${ONE}"
r2="${TWO}"
fi


for i in $(seq ${r1} ${r2}) 
do
 echo "Channel: $i"
 write_specific_ch $i $val
done

fi
fi

if [ "$format" = "format2" ]; then
  IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Channel: $i "
  write_specific_ch $i $val
 done

fi
fi
}


function set_host_port(){
 local BOARD="$1"
 export SC_HOST="numexo2-$BOARD"
 export SC_PORT="8061"
}

function usage(){
if [ "$argc" -lt 3 ]; then
  echo "Illegal number of parameters."
  echo 'MODE:READ/WRITE     BOARD:ALL/FRONT/BACK/65,67,../65-70      CHANNEL:ALL/1,5,9,../1-10     VALUE TO WRITE:0x300'
  exit 0
fi
}

trap 'sig_int' INT
trap 'sig_hup' HUP 
trap 'sig_term' TERM 


usage
echo "command: $cmd, boardList: $boardList, channelList: $channelList, value: $value"

fill_list
if [ "${cmd^^}" = "READ" ]
 then
  read_boards
elif [ "${cmd^^}" = "WRITE" ]
  then
   write_boards
else
  echo "No mode chosen"
fi


#------------------------------------------------------------
# Script to set the baselines of the numexo2 boards
# Author: Rikel CHAKMA
# Date :30/07/2022
#---------------------------------------------------------------

