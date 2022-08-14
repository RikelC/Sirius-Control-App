#--------------------------------------------------------------
#  An app to plot the temperature and data flow in each board
#  Author : Rikel CHAKMA
#  Date : 07/06/2022
#--------------------------------------------------------------
#!/usr/bin/python
# importing the required module 
import os
import commands
import numpy as np
import datetime as dt
import matplotlib.pyplot as plt
from matplotlib.dates import AutoDateLocator, AutoDateFormatter, date2num
from matplotlib.animation import FuncAnimation
import sys



print ("App for displaying Temperature of the Numexo boards as a function of time\nauthor: Rikel Chakma\n")
print 'Animation interval:', int(str(sys.argv[1])) , str(sys.argv[2])
date_datetime = dt.datetime.strptime(dt.datetime.now().strftime('%H:%M:%S'), '%H:%M:%S')
start_date = date2num( date_datetime)

#---------------------------------------
#function to check if n is a number
def checkNumberability(n):
    try:
        int(n)
        return True
    except ValueError:
        # Not a number
        return False

#---------------------------------------
#function to get interval
def getInterval():
   animationInterval = 1000
   if len(sys.argv) == 3 :
        intv = int(sys.argv[1])
	unit = str(sys.argv[2])
	if unit == "sec" :
	   animationInterval = intv * 1000
	elif unit == "min" : 
	   animationInterval = intv * 1000 * 60
	elif unit == "hr" :
	   animationInterval = intv * 1000 * 60 * 60

   return animationInterval

#-------------------------------------
# Function to get the list of active numexo boards
def getListOfActiveBoards():
 boardList = []
 bList = commands.getoutput('source /home/sirius/bin/numexo2_list.sh')
 while len(bList) > 0:
  pos = bList.find(" ")
  temp = bList[0:pos]
  bList = bList.replace(bList[:pos+1], '', 1)
  if checkNumberability(temp):
   boardList.append(temp)
  else:
   st = ""
   for c in temp:
      if checkNumberability(c):
         st+=str(c)
   if checkNumberability(st):
      boardList.append(st)

 return boardList


# get temperature of the active boards
fboardList = getListOfActiveBoards()
#print(fboardList)
noOfBoards = len(fboardList)
#print('no of active boards:{}'.format(noOfBoards))

#-------------------------------------------------
# create plots:
figure,axes = plt.subplots(2, figsize=(15,10), sharex=True, sharey=True)
figure.suptitle('Temperature over Time', fontsize=16)
# add a big axes, hide frame
figure.add_subplot(111, frameon=False)
# hide tick and tick label of the big axes
plt.tick_params(labelcolor='none', top=False, bottom=False, left=False, right=False)
plt.grid(False)
plt.xlabel("Time")
plt.ylabel("Temperature (deg C)")
# data:
colors = [ 'green', 'red' , 'black', 'blue', 'cyan', 'magenta', 'pink', 'violet', 'orange', 'darkblue', 'coral', 'maroon', 'salmon', 'springgreen', 'aqua', 'purple', 'orchid', 'plum', 'darkcyan', 'gold','seagreen','olive','lime','teal','lightcoral','firebrick','sandybrown','tan','saddlebrown','tomato']
x = [] #time is same for all
y = [[] for i in range(noOfBoards)] # 2D array for temperature values of V5
z = [[] for i in range(noOfBoards)] # 2D array for temperature values of V6
#vector for Line2D
line_V5 = []
line_V6 = []
for i in range(noOfBoards): 
  temp, = axes[0].plot_date(x, y[i])
  line_V5.append(temp)
  temp2, = axes[1].plot_date(x, z[i])
  line_V6.append(temp2)


boardId_vec = []
temp_V5_vec = []
temp_V6_vec = []
def get_temp():
 del boardId_vec[:]
 del temp_V5_vec[:]
 del temp_V6_vec[:]
 board = "numexo2-{id}"
 for current_board in fboardList:
     #print(board.format(id =current_board))
     boardId_vec.append(current_board)
     # Set environment variables
     os.environ['SC_HOST'] = board.format(id =current_board)
     os.environ['SC_PORT'] = '8061'
     #send command to get V5 temperature
     output = commands.getoutput('SC_client read /SENSOR_V5 TEMPERATURE')
     # get the temp value in C
     x1 = output.find("=")
     res = output.replace(output[:x1+1], '', 1)
     x2 = res.find("(")
     res = res.replace(res[x2:len(res)], '', 1)
     temp_v5 = float(res)
     temp_V5_vec.append(temp_v5)
     #print ("V5 temperature =", temp_v5)
     #send command to get V6 temperature
     output = commands.getoutput('SC_client read /SENSOR_V6 TEMPERATURE')
     # get the temp value in C
     x1 = output.find("=")
     res = output.replace(output[:x1+1], '', 1)
     x2 = res.find("(")
     res = res.replace(res[x2:len(res)], '', 1)
     temp_v6 = float(res)
     temp_V6_vec.append(temp_v6)
     #print ("V6 temperature =", temp_v6)    
   #return boardId_vec, temp_V5_vec, temp_V6_vec


#-----------------------------------------------------
# animation function

def init():
  for n in range(noOfBoards): 
      line_V5[n].set_data([], [])
      line_V6[n].set_data([], [])
  return line_V5, line_V6,

def animate(i, x, y, z):
#-----------------
# Get temp
 get_temp()
 date_datetime = dt.datetime.strptime(dt.datetime.now().strftime('%H:%M:%S'), '%H:%M:%S')
 int_date = date2num( date_datetime)
 x.append(int_date)
# add v5 and v6 temp values for all
 axes[0].clear()
 axes[1].clear()
 for n in range(noOfBoards): 
    y[n].append(temp_V5_vec[n])
    line_V5[n].set_data(x, y[n])
    z[n].append(temp_V6_vec[n])
    line_V6[n].set_data(x, z[n])
    axes[0].plot(x, y[n], color=colors[n] , label='{N}'.format(N = boardId_vec[n]))
    axes[1].plot(x, z[n], color=colors[n], label='{N}'.format(N = boardId_vec[n]))
    axes[0].title.set_text('V5')
    axes[1].title.set_text('V6')

 locator = AutoDateLocator()
 axes[0].xaxis.set_major_locator(locator)
 axes[0].xaxis.set_major_formatter( AutoDateFormatter(locator) )
 axes[0].set_xlim([start_date, int_date])
 axes[0].set_ylim([30, 60])
 #figure.autofmt_xdate()
 #axes[0].legend()
 axes[1].xaxis.set_major_locator(locator)
 axes[1].xaxis.set_major_formatter( AutoDateFormatter(locator) )
 axes[1].set_xlim([start_date, int_date])
 axes[1].set_ylim([30, 60])
 figure.autofmt_xdate()

 #axes[1].legend()
# Format plot
 plt.xticks(rotation=45, ha='right')
 axes[0].legend(bbox_to_anchor =(1.1, 0.7))
 #plt.subplots_adjust(bottom=0.30)
 return line_V5, line_V6,

#main function
def main():
  animationInterval = getInterval()
  anim = FuncAnimation(figure, animate, fargs=(x, y, z), init_func = init, frames = 1000, interval = animationInterval, repeat=True)
  plt.show()

if __name__ == "__main__":
    main()


