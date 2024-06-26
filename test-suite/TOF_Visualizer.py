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
from threading import Thread
import signal
import sys

colors = [(255/255, 0, 0), (255/255, 165/255, 0), (0, 255/255, 0,)]
bounds = [0, 1000, 2000, 4000]

#Image Resolution of TOF
N=8

global s1
s1= -np.ones((N,N))
global s2
s2 = -np.ones((N,N))

### Select Serial Port!
SERIAL_PORT = 'COM6'
#SERIAL_PORT = "/dev/tty.usbserial-110"
SERIAL_RATE = 115200

### DEBUG Mode - Random Data generated
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

def getdata(ser):

    global s1
    global s2
    global thread_stop
    print('GETDATA')


    while thread_stop==False:
        if DEBUG==True:
            s1= np.random.randint(0,4000,(N,N))
            s2= np.random.randint(0,4000,(N,N))
            time.sleep(0.1)
        else:
            reading = ser.readline().decode('utf-8')

            if 'ID: 1' in reading:
                s1=reading.split('[')[1].split(']')[0].split(',')[0:-1]
                s1=np.asarray(s1).astype(int)

                # Drop corrupt or incorrect packets
                if np.size(s1) != 64:
                    continue

                s1=np.reshape(s1,(N,N))
                s1=np.flip(s1)
                s1=np.flip(s1,axis=0)
                print('s1 Done')

            elif 'ID: 0' in reading:
                s2=reading.split('[')[1].split(']')[0].split(',')[0:-1]
                s2=np.asarray(s2).astype(int)

                # Drop corrupt or incorrect packets
                if np.size(s2) != 64:
                    continue

                s2=np.reshape(s2,(N,N))
                s2=np.flip(s2)
                s2=np.flip(s2,axis=0)
                print('s2 Done')
            else:
                print(reading)

def update(i):
    print('Update Visualization')
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

thread_stop=False

def shutdown(sig, frame):
  global thread_stop
  thread_stop = True
  
  if DEBUG==False:
    try:
        serial.Serial(SERIAL_PORT, SERIAL_RATE).close()
    except:
        print('COMPORT NOT ACCESIBLE')

  plt.close()
  print("Shutting down...")
  sys.exit(0)

signal.signal(signal.SIGINT, shutdown)

if __name__ == "__main__":
    #MAIN CODE
    fig, (ax1,ax2) = plt.subplots(1,2)
    im1 = ax1.imshow(s1, cmap=newcmp,norm=norm)
    im2 = ax2.imshow(s2, cmap=newcmp,norm=norm)

    
    try:
        if DEBUG==False:
            ser = serial.Serial(SERIAL_PORT, SERIAL_RATE)
        else:
            print('Debug Mode:')
            ser=''
        
        thread=Thread(target=getdata, args=(ser,))
        thread.start()

        ani = FuncAnimation(plt.gcf(), update, interval=100)
        plt.show()

    except:
        print(f"Serial connection to board not detected on port: {SERIAL_PORT}.\nPlease connect the board, or change the port on line 27.")


    



