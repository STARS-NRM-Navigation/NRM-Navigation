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

colors = ['red', 'yellow', 'green']
bounds = [0, 1000, 2000, 4000]


# this port address is for the serial tx/rx pins on the GPIO header
SERIAL_PORT = 'COM3'
SERIAL_RATE = 115200


def getdata():
    print('Readout')
    s1_DONE=False
    s2_DONE=False


    ser = serial.Serial(SERIAL_PORT, SERIAL_RATE)

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


# Create a colormap
newcmp = plt.cm.colors.ListedColormap(colors)

# Define the normalization for the colormap
norm = plt.cm.colors.BoundaryNorm(bounds, newcmp.N)

fig, (ax1,ax2) = plt.subplots(1,2)

#create two image plots
s1,s2=getdata()
im1 = ax1.imshow(s1, cmap=newcmp,norm=norm)
im2 = ax2.imshow(s2, cmap=newcmp,norm=norm)




def update(i):
    s1,s2=getdata()

    im1.set_data(s1)
    im2.set_data(s2)
    
    write_text(s1,ax1)
    write_text(s2,ax2)

    ax1.set_title('TOF 1')
    ax2.set_title('TOF 2')

def write_text(data,ax):
    for artist in ax.get_children():
        if isinstance(artist, plt.Text):
            artist.set_text('')

    for (j,i),label in np.ndenumerate(data):
        ax.text(i,j,label,ha='center',va='center')


ani = FuncAnimation(plt.gcf(), update, interval=200)
plt.show()


