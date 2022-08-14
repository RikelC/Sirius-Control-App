#!/bin/tcsh
set BOARD = $1
set CHANNEL = $2
set STATE = $3




     setenv SC_HOST numexo2-$BOARD
     setenv SC_PORT 8061
if ($CHANNEL == "ALL" ) then
     SC_client write /MEZ_1/DAC_1 DAC_A 0xAA00  #fixe baseline #AA00 (rear) #AA00 (HT) #7E00 (TUNNEL) #8500 FEANICS
SC_client write /MEZ_1/DAC_1 DAC_B 0xAA00   #fixe baseline
SC_client write /MEZ_1/DAC_2 DAC_A 0xAA00   #fixe baseline
SC_client write /MEZ_1/DAC_2 DAC_B 0xAA00   #fixe baseline
SC_client write /MEZ_2/DAC_1 DAC_A 0xAA00 #fixe baseline
SC_client write /MEZ_2/DAC_1 DAC_B 0xAA00 #fixe baseline
SC_client write /MEZ_2/DAC_2 DAC_A 0xAA00 #fixe baseline
SC_client write /MEZ_2/DAC_2 DAC_B 0xAA00 #fixe baseline
SC_client write /MEZ_3/DAC_1 DAC_A 0xAA00 #fixe baseline
SC_client write /MEZ_3/DAC_1 DAC_B 0xAA00 #fixe baseline
SC_client write /MEZ_3/DAC_2 DAC_A 0xAA00 #fixe baseline
SC_client write /MEZ_3/DAC_2 DAC_B 0xAA00 #fixe baseline
SC_client write /MEZ_4/DAC_1 DAC_A 0xAA00 #fixe baseline
SC_client write /MEZ_4/DAC_1 DAC_B 0xAA00 #fixe baseline
SC_client write /MEZ_4/DAC_2 DAC_A 0xAA00 #fixe baseline
SC_client write /MEZ_4/DAC_2 DAC_B 0xAA00 #fixe baseline
endif  





#SC_client -p 8061 -h numexo2-83 read /V6 FIRMWARE_VERSION_V6
#SC_client -p 8061 -h numexo2-83 read /V5 FIRMWARE_VERSION_V5
#SC_client -p 8061 -h numexo2-83 getinfo
# SC_client -p 8061 -h numexo2-83 read /GTS carrier_status
#SC_client -p 8061 -h numexo2-83 read /GTS clockpath_status
#SC_client -p 8061 -h numexo2-83 read /V6 PRETRIG_BUFFER_1
#SC_client -p 8061 -h numexo2-83 read /V6 BASIC_TRIGGER_THRES_1
