#!/bin/tcsh
foreach BOARD ($argv)
  if ($BOARD != $0) then
     setenv SC_HOST numexo2-$BOARD
     sleep 0.1
     setenv SC_PORT 8061
     SC_client write /V6 CONFIG_V6 0x484
     sleep 0.1
     SC_client write /V6 CONFIG_V6 0x405
     sleep 0.1
  endif
end

