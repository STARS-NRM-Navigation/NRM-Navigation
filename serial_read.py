import serial
import random
import time
from matplotlib.pyplot import imshow, show,cm
from matplotlib.colors import ListedColormap, LinearSegmentedColormap
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import matplotlib
import numpy as np
import matplotlib as mpl
import time
import signal
import sys

colors = [(255/255, 0, 0), (255/255, 165/255, 0), (0, 255/255, 0,)]
bounds = [0, 1000, 2000, 4000]


# this port address is for the serial tx/rx pins on the GPIO header
SERIAL_PORT = 'COM4'
# SERIAL_PORT = 'COM3'
SERIAL_PORT = '/dev/tty.usbserial-10'
SERIAL_RATE = 115200

DEBUG=False

COLORMAP=0
#NORMAL COLOR CODE 0
#DISCRETE COLOR CODE 1
#GRADIENT COLOR CODE 2

if COLORMAP==0:
    newcmp=matplotlib.pyplot.get_cmap('RdYlGn')
    norm = matplotlib.colors.TwoSlopeNorm(vmin=0., vcenter=2000, vmax=4000)
elif COLORMAP==1:
    #DISCRETE COLOR CODE 1
    # Create a colormap
    newcmp = plt.cm.colors.ListedColormap(colors)
    # Define the normalization for the colormap
    norm = plt.cm.colors.BoundaryNorm(bounds, newcmp.N)
    #create two image plots

elif COLORMAP==2:
#GRADIENT COLOR CODE
    newcmp = LinearSegmentedColormap.from_list("",colors)
    norm = matplotlib.colors.TwoSlopeNorm(vmin=800., vcenter=1500, vmax=4000)

def getdata():
    if DEBUG==True:
        return np.random.randint(0,4000,(8,8)), np.random.randint(0,4000,(8,8))
    else:
        s1_DONE=False
        s2_DONE=False

        ser = serial.Serial(SERIAL_PORT, SERIAL_RATE)
        ser.flushInput()
        while True:
            reading = ser.readline().decode('utf-8')

            if 'ID: 0' in reading:
                s1=reading.split('[')[1].split(']')[0].split(',')[0:-1]
                s1=np.asarray(s1).astype(int)
                s1=np.reshape(s1,(8,8))
                s1_DONE=True
                print('s1 Done')

            if 'ID: 1' in reading:
                s2=reading.split('[')[1].split(']')[0].split(',')[0:-1]
                s2=np.asarray(s2).astype(int)
                s2=np.reshape(s2,(8,8))
                s2_DONE=True
                print('s2 Done')

            if ((s1_DONE==True) and (s2_DONE==True)):
                break

        return s1,s2

def update(i):
    s1,s2=getdata()

    im1.set_data(s1)
    im2.set_data(s2)

    write_text(s1,ax1)
    write_text(s2,ax2)

    ax1.set_title('TOF 1', fontsize=20)
    ax2.set_title('TOF 2', fontsize=20)

def write_text(data,ax):
    for artist in ax.get_children():
        if isinstance(artist, plt.Text):
            artist.set_text('')

    for (j,i),label in np.ndenumerate(data):
        ax.text(i,j,label,ha='center',va='center')

def shutdown(sig, frame):
  print("Server shutting down...")
  sys.exit(0)

#MAIN CODE
signal.signal(signal.SIGINT, shutdown)
fig, (ax1,ax2) = plt.subplots(1,2)
im1 = ax1.imshow(np.zeros((8,8)), cmap=newcmp,norm=norm)
im2 = ax2.imshow(np.zeros((8,8)), cmap=newcmp,norm=norm)

ani = FuncAnimation(plt.gcf(), update, interval=1)
plt.show()


