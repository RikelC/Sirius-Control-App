#!/bin/tcsh

foreach BOARD ($argv)
  if ($BOARD != $0) then
     setenv SC_HOST numexo2-$BOARD
     setenv SC_PORT 8061
     SC_client read /V5 FIRMWARE_VERSION_V5
     SC_client read /V6 FIRMWARE_VERSION_V6
  endif
end



#SC_client -p 8061 -h numexo2-83 read /V6 FIRMWARE_VERSION_V6
#SC_client -p 8061 -h numexo2-83 read /V5 FIRMWARE_VERSION_V5
#SC_client -p 8061 -h numexo2-83 getinfo
# SC_client -p 8061 -h numexo2-83 read /GTS carrier_status
#SC_client -p 8061 -h numexo2-83 read /GTS clockpath_status
#SC_client -p 8061 -h numexo2-83 read /V6 PRETRIG_BUFFER_1
#SC_client -p 8061 -h numexo2-83 read /V6 BASIC_TRIGGER_THRES_1
