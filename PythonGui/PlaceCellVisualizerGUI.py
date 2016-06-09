# -*- coding: utf-8 -*-
"""
Created on Fri Mar 18 17:06:59 2016

PLACE FIELAD VISUALIZER GUI

@author: alessio
"""

import socket,sys,struct,time, thread, threading
import matplotlib.pyplot as plt
from matplotlib.widgets import  Button, RadioButtons
import numpy as np

### DEFINE CONSTANTS AND PARAMS ###

MAX_BUFFER_SIZE = 4096 #needs to match open-ephys buffer length
HELLO_STR_SIZE = 23    #Hi from Open-Ephys GUI!
N_GRID = 20
# SpikePos Struct info
spikePos = struct.Struct('f f q h h f')  # x,y,timestamp,electrodeID,sortedId, +
SPIKE_POS_OBJ_SIZE = spikePos.size;
STD_WIDTH = 1.0
STD_HEIGTH = 1.0

def printciao():
    print 'ciao'

### Socket Setup function
def socketSetup(server, port, timeout):
    # Create socket
    server_address = (server, port) 
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print 'Socket created'

    # Set socket options
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.settimeout(timeout)
    
    print server_address
    # Bind socket to local host and port
    try:
        s.bind((server_address))
        print 'Socket bind complete'
        error = 0
    except socket.error as msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        error = 1 
    
        
    return s, error


### Streaming Thread function
def streaming():
    
    print "Streaming"
    
    global s, MAX_BUFFER_SIZE, HELLO_STR_SIZE, mapBoundariesReceived, ax, neuronButtons, radioax
    global neuronIDList, neuronMapIDList, countID, neuronNumber, axcolor
    
    while True:
        try:          
            (data, address) = s.recvfrom(MAX_BUFFER_SIZE);
          
            if len(data) == HELLO_STR_SIZE: # Test
              
              print "Testing Connection: ", data
               
            else:                          # Transmit
              
              spikeReceived = struct.unpack('h', data[0:2])[0]
              print "Spike received: ", spikeReceived
              (width, height) = struct.unpack('ff', data[2:10])
              print "Width: ", width, "Heigth: ", height
              
              if mapBoundariesReceived == False:
                  # Create boundary matrix
                  x_grid = np.linspace(0, width, N_GRID + 2)
                  y_grid = np.linspace(0, height, N_GRID + 2)
#                  plt.axis([0, width, 0, height])
                  mapBoundariesReceived == True
                  
                  
              
              spikeData = data[10:(SPIKE_POS_OBJ_SIZE*spikeReceived)+10]
              labels = []
              
              # Extract neurons
              for ii in range(0,spikeReceived):
                  
                  spikeRaw = spikeData[(ii)*(SPIKE_POS_OBJ_SIZE):(ii+1)*(SPIKE_POS_OBJ_SIZE)]
                  spikeStruct = spikePos.unpack(spikeRaw)
                  #print "i: ", ii, "spike info: ",spikeStruct
                  
                  spikeID = spikeStruct[4]
                  spikeX = spikeStruct[0]
                  spikeY = spikeStruct[1]
                  
                  #debug: add gaussian
#                  mu, sigma = 0 + (-1)^spikeID, 0.05*(spikeID+1) # mean and standard deviation
                  mu, sigma = 0, 0.05
                  spikeX = 0.5 + np.random.normal(mu, sigma, 1)
                  spikeY = 0.7 + np.random.normal(mu, 2*sigma, 1)
                  
                  
                  
                  if spikeX > width:
                      spikeX = width
                  if spikeY > height:
                      spikeY = height
                      
                  if spikeX < 0:
                      spikeX = 0
                  if spikeY < 0:
                      spikeY = 0
                      
                  #print spikeX,spikeY
                  

                  if spikeID in neuronIDList:
                      indexSpike = neuronIDList.index(spikeID)
                      # Find x and y indexes
                      x_ind = np.where(x_grid>=spikeX)[0][0]
                      y_ind = np.where(y_grid>=spikeY)[0][0]
                   
                      # Check boundaries
                      if x_ind == N_GRID+1:
                          x_ind = N_GRID-1;
                      if y_ind == N_GRID+1:
                          y_ind = N_GRID-1;
                          
                      if x_ind == 0:
                          x_ind = 1;
                      if y_ind == 0:
                          y_ind = 1;
                          
                      print x_ind, y_ind, spikeID, indexSpike
                          
                      # Update count
                      neuronMapIDList[indexSpike][x_ind-1][y_ind-1] = neuronMapIDList[indexSpike][x_ind-1][y_ind-1] + 1                      
                      countID[indexSpike] = countID[indexSpike]+1
                      
                  else:
                      neuronIDList.append(spikeID)
                      countID.append(0)
                      neuronMapIDList.append(np.zeros((N_GRID,N_GRID)))
                      
                      

                  #print spikeStruct

#            plt.sca(ax)
#            plt.imshow(neuronMapIDList[0],animated=True,label="Place Field")
               
               
            for neur in range(0, len(neuronIDList)-1):   
                #print str(neuronIDList[neur])
                labels.append(str(neuronIDList[neur]))
                neuronButtons = RadioButtons(radioax, labels)
  
            
        except socket.timeout:
            print 'Socket timeout'
            s.close()
            break



##### START #####
# Create neuron list and relative map list

neuronIDList = []
neuronMapIDList = []
countID = []

mapBoundariesReceived = False
x_grid = []
y_grid = []
neuronNumber = 0

### Create GUI widgets
fig, ax = plt.subplots()
plt.subplots_adjust(left=0.25, bottom=0.25)

mapSelected = np.zeros((N_GRID,N_GRID))
#plt.axis([0, STD_WIDTH , 0, STD_WIDTH])
ax.imshow(mapSelected,animated=True,label="Place Field")

axcolor = 'lightgoldenrodyellow'

(s,error) = socketSetup('localhost',9090,15)

# Create two threads as follows

t1 = threading.Thread(target=streaming)
t1.start()

#try:
#   thread.start_new_thread(streaming, ("STREAMING",) )
#except:
#   print "Error: unable to start STREAMING thread"


updateax = plt.axes([0.8, 0.025, 0.1, 0.04])
updateButton = Button(updateax, 'Update', color=axcolor, hovercolor='0.975')

selectax = plt.axes([0.5, 0.025, 0.1, 0.04])
selectButton = Button(selectax, 'Select', color=axcolor, hovercolor='0.975')

radioax = plt.axes([0.1, 0.3, 0.1, 0.5])
neuronButtons = RadioButtons(radioax, [])
   
   


def update(event):
    global mapSelected, ax, image, fig
    
#    image.set_data(mapSelected)
#    ax.figure.canvas.draw()    
    
    plt.sca(ax)
    plt.imshow(mapSelected,animated=True,label="Place Field")
    
def select(event):
    global neuronMapIDList, mapSelected
    
    mapSelected = neuronMapIDList[0]
    print 'ciao'
    
    
        
updateButton.on_clicked(update)
selectButton.on_clicked(select)


#
#rax = plt.axes([0.025, 0.5, 0.15, 0.15], axisbg=axcolor)
#radio = RadioButtons(rax, ('red', 'blue', 'green'), active=0)
#
#
#def colorfunc(label):
#    l.set_color(label)
#    fig.canvas.draw_idle()
#radio.on_clicked(colorfunc)
#
#plt.show()