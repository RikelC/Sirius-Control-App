#!/bin/bash

cmd=$1
boardList=$2
channelList=$3
polarity=$4
value=$5
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
#while read -n1 character; do
#check_numerical $character
#if [ !$channel_is ]; then
#if [ "$character" = "-" ]; then
#format1=true
#elif [ "$character" = "," ]; then
#format2=true
#fi
#fi
#done < <(echo -n "$str")

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

function fill_list(){
#Board arrays
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

if [ "$one_check" == "true" -a "$two_check" = "true" ]; then
 for i in $(seq ${ONE} ${TWO}) 
 do
  echo "Board: $i"
  set_host_port $i
  read_channel
 done
fi
fi
#-------
if [ "$format" = "format2" ]; then
 IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Board: $i "
  set_host_port $i
  read_channel
 done
else
echo "not a proper format"
fi
fi

}


function write_formatted_bList(){
local var="$1"
check_numerical $var
one_check=false
if [ $channel_is = "true" ]; then
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

if [ "$one_check" == "true" -a "$two_check" = "true" ]; then
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
      read_specific_ch $channelList $polarity 
   fi
else

if [ "${channelList^^}" = "ALL" ]
 then
  echo "Reading all channels"
    if [ "${polarity^^}" = "POSITIVE" ]
      then
         for j in $(seq 1 16); do
           echo "channel: $j"
           SC_client read /V6 TRIGGER_THRES_POS_$j
         done
   elif [ "${polarity^^}" = "NEGATIVE" ]
      then
        for j in $(seq 1 16); do
          echo "channel: $j"
          SC_client read /V6 TRIGGER_THRES_NEG_$j
       done
   fi
else
 read_formatted_chList $channelList $polarity

fi
fi
}

function write_channel(){
check_numerical $channelList
if [ "$channel_is" = "true" ]; then
  if [ "$channelList" -gt 0 -a "$channelList" -lt 17 ]
   then
      write_specific_ch $channelList $polarity $value 
   fi
else

if [ "${channelList^^}" = "ALL" ]
 then
    if [ "${polarity^^}" = "POSITIVE" ]
      then
        for i in $(seq 1 16); do
          echo "channel: $i"
          SC_client write /V6 TRIGGER_THRES_POS_$i $value
        done
    elif [ "${polarity^^}" = "NEGATIVE" ]
      then
        for i in $(seq 1 16); do
          echo "channel: $i"
          SC_client write /V6 TRIGGER_THRES_NEG_$i $value
        done
  fi
else 
   write_formatted_chList $channelList $polarity $value
fi
fi
}



function read_specific_ch(){

local ch="$1"
local pol="$2"
if [ "${pol^^}" = "POSITIVE" ]
 then
  case $ch in
          "1") SC_client read /V6 TRIGGER_THRES_POS_1;;
          "2") SC_client read /V6 TRIGGER_THRES_POS_2;;
          "3") SC_client read /V6 TRIGGER_THRES_POS_3;;
          "4") SC_client read /V6 TRIGGER_THRES_POS_4;;
          "5") SC_client read /V6 TRIGGER_THRES_POS_5;;
          "6") SC_client read /V6 TRIGGER_THRES_POS_6;;
          "7") SC_client read /V6 TRIGGER_THRES_POS_7;;
          "8") SC_client read /V6 TRIGGER_THRES_POS_8;;
          "9") SC_client read /V6 TRIGGER_THRES_POS_9;;
          "10") SC_client read /V6 TRIGGER_THRES_POS_10;;
          "11") SC_client read /V6 TRIGGER_THRES_POS_11;;
          "12") SC_client read /V6 TRIGGER_THRES_POS_12;;
          "13") SC_client read /V6 TRIGGER_THRES_POS_13;;
          "14") SC_client read /V6 TRIGGER_THRES_POS_14;;
          "15") SC_client read /V6 TRIGGER_THRES_POS_15;;
          "16") SC_client read /V6 TRIGGER_THRES_POS_16;;
          "*") echo "Not a valid Channel Number";;
 esac
elif [ "${pol^^}" = "NEGATIVE" ]
 then
  case $ch in
          "1") SC_client read /V6 TRIGGER_THRES_NEG_1;;
          "2") SC_client read /V6 TRIGGER_THRES_NEG_2;;
          "3") SC_client read /V6 TRIGGER_THRES_NEG_3;;
          "4") SC_client read /V6 TRIGGER_THRES_NEG_4;;
          "5") SC_client read /V6 TRIGGER_THRES_NEG_5;;
          "6") SC_client read /V6 TRIGGER_THRES_NEG_6;;
          "7") SC_client read /V6 TRIGGER_THRES_NEG_7;;
          "8") SC_client read /V6 TRIGGER_THRES_NEG_8;;
          "9") SC_client read /V6 TRIGGER_THRES_NEG_9;;
          "10") SC_client read /V6 TRIGGER_THRES_NEG_10;;
          "11") SC_client read /V6 TRIGGER_THRES_NEG_11;;
          "12") SC_client read /V6 TRIGGER_THRES_NEG_12;;
          "13") SC_client read /V6 TRIGGER_THRES_NEG_13;;
          "14") SC_client read /V6 TRIGGER_THRES_NEG_14;;
          "15") SC_client read /V6 TRIGGER_THRES_NEG_15;;
          "16") SC_client read /V6 TRIGGER_THRES_NEG_16;;
          "*") echo "Not a valid Channel Number";;
 esac
else
 echo "Polarity is not valid."
fi

}

function write_specific_ch(){
local ch="$1"
local pol="$2"
local val="$3"
if [ "${pol^^}" = "POSITIVE" ]
 then
  case $ch in
          "1") SC_client write /V6 TRIGGER_THRES_POS_1 $val;;
          "2") SC_client write /V6 TRIGGER_THRES_POS_2 $val;;
          "3") SC_client write /V6 TRIGGER_THRES_POS_3 $val;;
          "4") SC_client write /V6 TRIGGER_THRES_POS_4 $val;;
          "5") SC_client write /V6 TRIGGER_THRES_POS_5 $val;;
          "6") SC_client write /V6 TRIGGER_THRES_POS_6 $val;;
          "7") SC_client write /V6 TRIGGER_THRES_POS_7 $val;;
          "8") SC_client write /V6 TRIGGER_THRES_POS_8 $val;;
          "9") SC_client write /V6 TRIGGER_THRES_POS_9 $val;;
          "10") SC_client write /V6 TRIGGER_THRES_POS_10 $val;;
          "11") SC_client write /V6 TRIGGER_THRES_POS_11 $val;;
          "12") SC_client write /V6 TRIGGER_THRES_POS_12 $val;;
          "13") SC_client write /V6 TRIGGER_THRES_POS_13 $val;;
          "14") SC_client write /V6 TRIGGER_THRES_POS_14 $val;;
          "15") SC_client write /V6 TRIGGER_THRES_POS_15 $val;;
          "16") SC_client write /V6 TRIGGER_THRES_POS_16 $val;;
          "*") echo "Not a valid Channel Number";;
 esac
elif [ "${pol^^}" = "NEGATIVE" ]
 then
  case $ch in
          "1") SC_client write /V6 TRIGGER_THRES_NEG_1 $val;;
          "2") SC_client write /V6 TRIGGER_THRES_NEG_2 $val;;
          "3") SC_client write /V6 TRIGGER_THRES_NEG_3 $val;;
          "4") SC_client write /V6 TRIGGER_THRES_NEG_4 $val;;
          "5") SC_client write /V6 TRIGGER_THRES_NEG_5 $val;;
          "6") SC_client write /V6 TRIGGER_THRES_NEG_6 $val;;
          "7") SC_client write /V6 TRIGGER_THRES_NEG_7 $val;;
          "8") SC_client write /V6 TRIGGER_THRES_NEG_8 $val;;
          "9") SC_client write /V6 TRIGGER_THRES_NEG_9 $val;;
          "10") SC_client write /V6 TRIGGER_THRES_NEG_10 $val;;
          "11") SC_client write /V6 TRIGGER_THRES_NEG_11 $val;;
          "12") SC_client write /V6 TRIGGER_THRES_NEG_12 $val;;
          "13") SC_client write /V6 TRIGGER_THRES_NEG_13 $val;;
          "14") SC_client write /V6 TRIGGER_THRES_NEG_14 $val;;
          "15") SC_client write /V6 TRIGGER_THRES_NEG_15 $val;;
          "16") SC_client write /V6 TRIGGER_THRES_NEG_16 $val;;
          "*") echo "Not a valid Channel Number";;
 esac
else
 echo "Polarity is not valid."
fi

}


function read_formatted_chList(){
local var="$1"
local pol="$2"

check_format $var
if [ "$format" = "format1" ]; then

one_check=false
two_chek=false
IFS="-" read -r ONE TWO <<< $var

if [ ${ONE} -gt 0 -a ${ONE} -lt 17 ]; then
one_check=true  
fi
if [ ${TWO} -gt 0 -a ${TWO} -lt 17 ]; then
two_check=true
fi

if [ "$one_check" == "true" -a "$two_check" = "true" ]; then

if [ ${ONE} -ge ${TWO} ]; then
r1=${TWO}
r2=${ONE}
else
r1=${ONE}
r2=${TWO}
fi


for i in $(seq ${r1} ${r2}) 
do
 echo "Channel: $i"
if [ "${pol^^}" = "POSITIVE" ]; then
           SC_client read /V6 TRIGGER_THRES_POS_$i
elif [ "${pol^^}" = "NEGATIVE" ]; then
           SC_client read /V6 TRIGGER_THRES_NEG_$i
fi

done
fi
fi

if [ "$format" = "format2" ]; then
  IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Channel: $i "
if [ ${pol^^} = "POSITIVE" ]; then
           SC_client read /V6 TRIGGER_THRES_POS_$i
elif [ ${pol^^} = "NEGATIVE" ]; then
           SC_client read /V6 TRIGGER_THRES_NEG_$i
fi
 done

fi

}


function write_formatted_chList(){
local var="$1"
local pol="$2"
local val="$3"
check_format $var
if [ "$format" = "format1" ]; then
one_check=false
two_chek=false
IFS="-" read -r ONE TWO <<< $var

if [ ${ONE} -gt 0 -a ${ONE} -lt 17 ]; then
one_check=true  
fi
if [ ${TWO} -gt 0 -a ${TWO} -lt 17 ]; then
two_check=true
fi

if [ "$one_check" == "true" -a "$two_check" = "true" ]; then

if [ ${ONE} -ge ${TWO} ]; then
r2=${ONE}
r1=${TWO}
else
r1=${ONE}
r2=${TWO}
fi


for i in $(seq ${r1} ${r2}) 
do
 echo "Channel: $i"
if [ "${pol^^}" = "POSITIVE" ]; then
           SC_client write /V6 TRIGGER_THRES_POS_$i $val
elif [ "${pol^^}" = "NEGATIVE" ]; then
           SC_client write /V6 TRIGGER_THRES_NEG_$i $val
fi

done
fi
fi


if [ "$format" = "format2" ]; then
  IFS="," read -r -a array <<< $var
 for i in ${array[@]}
 do
  echo "Channel: $i "
if [ "${pol^^}" = "POSITIVE" ]; then
           SC_client write /V6 TRIGGER_THRES_POS_$i $val
elif [ "${pol^^}" = "NEGATIVE" ]; then
           SC_client write /V6 TRIGGER_THRES_NEG_$i $val
fi
 done

fi

}

function set_host_port(){
 local BOARD="$1"
 export SC_HOST="numexo2-$BOARD"
 export SC_PORT="8061"
}

function usage(){

if [ "$argc" -lt 4 ]
then
echo "Illegal number of parameters."
echo 'MODE:READ/WRITE     BOARD:ALL/FRONT/BACK/65,67,../65-70      CHANNEL:ALL/1,5,9,../1-10     POLARITY:POSITIVE/NEGATIVE       VALUE TO WRITE:0x300'
exit 0
 fi
}

trap 'sig_int' INT
trap 'sig_hup' HUP 
trap 'sig_term' TERM 


usage
echo "command: $cmd, boardList: $boardList, channelList: $channelList, Polarity: $polarity, value: $value"
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
# Script to set the thresholds of the numexo2 boards
# Author: Rikel CHAKMA
# Date :30/07/2022
#---------------------------------------------------------------

