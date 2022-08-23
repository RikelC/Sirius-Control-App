# Sirius-Control-App

This is a simple GUI developed using gtk-2. The goal is to bring different tasks achieved using different scripts under one framework to ease the process for our collaborators and the future SIRIUS users. The users can perform:

1. Necassary Chack:
->Boards ON/OFF Status: This check must be performed first. It shows whether all the boards listed in "Board Config file" are ON or OFF.
->Open Board config file: To edit the "Board Config file". The format must be preserved as this file is read in several places in the program.
->Network Readiness: To check whether the boards are ready for initialization.

2.Setup Initialization:
->Configure Numexo2 boards: To initialize basaline, threshold and other registers.
->Setup GTS: To align the GTS tree.
->Auto initialization: It performs the above two operations sequentially.

3.Miscellaneous functions:
->Ping Network: To test the various connected IP addresses in the Network.
->Scan Network: To see who is who and their status.
->Show Driver State: To see the Narval actor state.
->Show Firmware versions: To check the versions of the firmwares installed in the boards.
->Show Embaded Software version: To see the embadded software installed in V5.
->Show Narval State: To check the status of Narval.

4.Reset Numexo2 Boards:
Resetting of the Numexo2 boards is required from time to time in order to bring the baseline to 0 value. Very essential for proper functiong of the thresholds set for each boards. You can Enter Board numbers in formats like: 65,66,67 or ALL or Front or Back (Note the entry is case insensitive). Then, chose the state of the Acquisition as the commands are different for ON and OFF state. Then do the reset. In the future version, this will be done automatically.

5.Baseline:
To read and write the baseline values of the numexo2 boards. You can Enter Board numbers in formats like: 65,66,67 or 64-70 or ALL or DSSD/Front/Back/Tunnel (Note the entry is case insensitive).Then, you can Enter Channel numbers in formats like: 1,10,16 or 1-4 or ALL (Note the entry is case insensitive too). Finally, chose whether to read or write. For writing you must give in Hexadecimal format 0x.....

6.Threshold:
To read and write the threshold levels of the numexo2 boards. Similarly to the baseline, you can Enter Board numbers in formats like: 65,66,67 or 64-70 or ALL or DSSD/Front/Back/Tunnel (Note the entry is case insensitive). Then, enter Channel numbers in formats like: 1,10,16 or 1-4 or ALL. Then, chose the Polarity of the threshold and finally, chose whether to read or write. For writing you must give in Hexadecimal format 0x.....

7.Telnet Viewer:
To view the real time flow of data for different boards. This is important to diagnose which board(s) is(are) behaving strangely.

8.Temperature of Numexo2:
Allows you to measure the temeratue of vertex5 and vertex6 of the numexo2 boards. You can enter ALL or by board numbers 65,70,....

9. Graphical Temperature as a function of time
Press 'Draw' to view graphically the temeratue of vertex5 and vertex6 of all the numexo2 boards. And, press 'Close' to kill the graphical viewer.

10.GRU:
You can edit/view the configurations set for Online/Offline analysis. Please read the configuration file properly as the input parameters for the data analysis software are provided in this file. Pressing the "Launch" will start the analysis software. Pressing "Kill" button will send a SIGTERM signal to the program. And the program will exit after cleaning the resouces.

11. VIGRU
To launch the Vigru.

12. GUser
->Open Workspace: To edit/view the GUser header and source files. Adding new histograms for example.
->Cmake: To do Cmake of the source codes, if you add new class in the user library.
->Clean Make: To do clean Make.
->Make: To Make.
