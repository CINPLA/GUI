# -*- coding: utf-8 -*-
"""
Created on Tue Mar 22 19:06:07 2016

@author: alessio
"""

import socket, sys, struct, threading, time
import numpy as np
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

if sys.version_info[0] < 3:
    import Tkinter as Tk
    from Tkinter import *
    # import ttk
else:
    import tkinter as Tk
    # from tkinter import ttk


''' DEFINE CONSTANTS AND PARAMS '''

MAX_BUFFER_SIZE = 4096  # needs to match open-ephys buffer length
HELLO_STR_SIZE = 23     # Hi from Open-Ephys GUI!
N_GRID = 10
# SpikePos Struct info
spikePos = struct.Struct('f f q h h f')  # x,y,timestamp,electrodeID,sortedId, +
SPIKE_POS_OBJ_SIZE = spikePos.size
STD_WIDTH = 1.0
STD_HEIGHT = 1.0

# Define colors
grey = "#aaaaaaaaa"

debug = False

'''Socket Setup function'''


def socketSetup(server, port, timeout):
    # Create socket
    server_address = (server, port)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print 'Socket created'

    # Set socket options
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.settimeout(timeout)

    print server_address
    # Bind socket to local host and port
    try:
        sock.bind(server_address)
        print 'Socket bind complete'
        err = 0
    except socket.error as msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        err = 1

    return sock, err


# Streaming Thread function
def streaming():

    print "Streaming"

    global s, MAX_BUFFER_SIZE, HELLO_STR_SIZE, mapBoundariesReceived, a, listbox, x_grid, y_grid
    global neuronIDList, cumulativeCountMapList, sumsID, firingRates, firingMaps, n_received, t_curr, t_pass
    global is_streaming, prevX, prevY, neuronNumber

    while is_streaming:
        try:
            (data, address) = s.recvfrom(MAX_BUFFER_SIZE)

            # window = []

            if len(data) == HELLO_STR_SIZE: # Test

                print "Testing Connection: ", data

            else:                          # Transmit

                # Update counter and time count
                n_received += 1
                t_curr = time.time()
                window = t_curr - t_pass
                t_pass = t_curr
                # Reset counter for each neuron
                # count of sorted spike received in each datagram
                countID = np.array([])
                # count of sorted spike received in each datagram for each grid element
                countMapList = []
                # Count n spikes per grid element to normalize over occupancy of grid
                spikePerGrid = np.zeros((N_GRID, N_GRID))

                occupancy_thresh = 0.1

                spikeReceived = struct.unpack('h', data[0:2])[0]
                print "Spike received: ", spikeReceived
                (width, height) = struct.unpack('ff', data[2:10])
                print "Width: ", width, "Heigth: ", height

                # if mapBoundariesReceived is False:
                #     # Create boundary matrix
                #     x_grid = np.linspace(0, width, N_GRID + 2)
                #     y_grid = np.linspace(0, height, N_GRID + 2)
                #     #                  plt.axis([0, width, 0, height])
                #     mapBoundariesReceived = True

                spikeData = data[10:(SPIKE_POS_OBJ_SIZE*spikeReceived)+10]

                # Extract neurons
                for ii in range(0, spikeReceived):

                    spikeRaw = spikeData[ii*SPIKE_POS_OBJ_SIZE:(ii+1)*SPIKE_POS_OBJ_SIZE]
                    spikeStruct = spikePos.unpack(spikeRaw)
                    # print "i: ", ii, "spike info: ",spikeStruct
                    spikeID = spikeStruct[4]
                    spikeX = spikeStruct[0]
                    spikeY = spikeStruct[1]
                    # print spikeX, spikeY

                    if np.isnan(spikeX):
                        spikeX = prevX
                    if np.isnan(spikeY):
                        spikeY = prevY

                    # debug: add gaussian
                    if debug:
                        if spikeID == 0:
                            mux, sigmax = 0.5, 0.05
                            muy, sigmay = 0.5, 0.05
                        elif spikeID == 1:
                            mux, sigmax = 0.2, 0.07
                            muy, sigmay = 0.6, 0.15
                        elif spikeID == 2:
                            mux, sigmax = 0.6, 0.1
                            muy, sigmay = 0.8, 0.05
                        elif spikeID == 3:
                            mux, sigmax = 0.3, 0.1
                            muy, sigmay = 0.2, 0.02
                        elif spikeID == 4:
                            mux, sigmax = 0.7, 0.08
                            muy, sigmay = 0.2, 0.05
                        else:
                            mux, sigmax = 0.5, 0.2
                            muy, sigmay = 0.5, 0.2

                        spikeX = np.random.normal(mux, sigmax, 1)
                        spikeY = np.random.normal(muy, sigmay, 1)

                    # Check boundaries

                    if spikeX > width:
                        spikeX = width
                    if spikeY > height:
                        spikeY = height

                    if spikeX < 0:
                        spikeX = 0
                    if spikeY < 0:
                        spikeY = 0

                    # print spikeX,spikeY
                    # print 'spikeid: ', spikeID

                    prevX = spikeX
                    prevY = spikeY

                    if spikeID in neuronIDList:
                        indexSpike = np.where(neuronIDList == spikeID)[0]
                        update_map_with_spike(x_grid, y_grid, spikeX, spikeY, indexSpike, cumulativeCountMapList,
                                              countID, countMapList, spikePerGrid)

                    else:
                        # Create new Map
                        neuronIDList = np.append(neuronIDList, spikeID)
                        countID = np.append(countID, [0])
                        countMapList.append(np.zeros((N_GRID, N_GRID)))
                        sumsID = np.append(sumsID, [0])
                        firingRates = np.append(firingRates, [0])
                        cumulativeCountMapList.append(np.zeros((N_GRID, N_GRID), dtype=np.uint64))
                        cumulativeFiringMaps.append(np.zeros((N_GRID, N_GRID)))
                        firingMaps.append(np.zeros((N_GRID, N_GRID)))

                        # update maps
                        indexSpike = np.where(neuronIDList == spikeID)[0]
                        update_map_with_spike(x_grid, y_grid, spikeX, spikeY, indexSpike, cumulativeCountMapList,
                                              countID, countMapList, spikePerGrid)

                        # when a new neuron is received: update listbox
                        labels = []
                        for neur in range(0, len(neuronIDList)):
                            labels.append("    " + str(int(neuronIDList[neur])))

                        listbox.delete(0, END)
                        for item in labels:
                            listbox.insert(END, "        " + item)

                # Update firing rates
                firingRates = (np.array(sumsID) + np.array(countID) / float(window)) / float(n_received)
                sumsID = sumsID + np.array(countID) / float(window)

                # update firing maps
                # compute grid occupancy (%) for current datagram
                gridOccupancy = spikePerGrid/np.sum(spikePerGrid)
                for neur in range(0, len(neuronIDList)):
                    # current Firing Map
                    newFM = np.zeros((N_GRID, N_GRID))
                    for ii in range(N_GRID):
                        for jj in range(N_GRID):
                            if gridOccupancy[ii][jj] > occupancy_thresh:
                                # update FM only if occupancy > 0.1 (robustness)
                                newFM[ii][jj] = cumulativeCountMapList[neur][ii][jj]/gridOccupancy[ii][jj] / \
                                                float(window)

                    firingMaps[neur][:][:] = (cumulativeFiringMaps + newFM)/float(n_received)

        except socket.timeout:
            print 'Socket timeout'
            s.close()
            break


def update_map_with_spike(x_ticks, y_ticks, xpos, ypos, ind, mapList, counts, countsMap, gridCount):
    # Find x and y indexes
    if xpos != 0 and xpos != 1:
        x_ind = np.where(x_ticks >= xpos)[0][0] - 2
    elif xpos != 0:
        x_ind = 0
    elif xpos != 1:
        x_ind = N_GRID - 1
    else:
        x_ind = 0  # Get rid of unreference warnings...

    if ypos != 0 and ypos != 1:
        y_ind = np.where(y_ticks >= ypos)[0][0] - 2
    elif ypos != 0:
        y_ind = 0
    elif ypos != 1:
        y_ind = N_GRID - 1
    else:
        y_ind = 0

    # Update count
    gridCount[x_ind][y_ind] += 1
    mapList[ind][x_ind][y_ind] += 1
    counts[ind] += 1
    countsMap[ind][x_ind][y_ind] += 1


''' START '''
# Create neuron list and relative map list

# list of sorted spike ID
neuronIDList = np.array([])
# cumulative count of spike per grid element
cumulativeCountMapList = []
# cumulative count of sorted spike (used to update firing rate)
sumsID = np.array([], dtype=np.uint64)
# mean firing rate of each sorted spike
firingRates = np.array([])
# cumulative firing maps (sum of firing rate for each grid element)
cumulativeFiringMaps = []
# mean firing map (normalized over time of occupancy) for each sorted neuron
firingMaps = []

mapBoundariesReceived = False
# x_grid = []
# y_grid = []
width = 1.
height = 1.
x_grid = np.linspace(0, width, N_GRID + 2)
y_grid = np.linspace(0, height, N_GRID + 2)
neuronNumber = 0

t_curr = time.time()
t_pass = time.time()
n_received = long(0)

prevX = 0
prevY = 0

# Create GUI widgets
root = Tk()
root.wm_title("Place Field Visualizer")
root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)

# CONTENT FRAME
content = Frame(root, bg=grey)
content.grid(column=0, row=0, sticky=N+S+E+W, columnspan=5, rowspan = 4)
# 4 x 5
for row in range(0, 3):
    content.rowconfigure(row, weight=1)
for col in range(0, 4):
    content.columnconfigure(col, weight=1)

# PLOT FRAME
plotframe = Frame(content, bg=grey)  # , padding="3 3 3 3"
plotframe.grid(column=1, row=0, columnspan=4, rowspan=4, sticky=N+S+E+W)
# 4 x 4
for row in range(0, 3):
    plotframe.rowconfigure(row, weight=1)
for col in range(0, 3):
    plotframe.columnconfigure(col, weight=1)

# USER FRAME
userframe = Frame(content, bg=grey)  # , padding="3 3 3 3"
userframe.grid(column=0, row=0, rowspan=4, sticky=N+W+S)

# 4 x 1
for row in range(0, 3):
    userframe.rowconfigure(row, weight=1)
for col in range(0, 1):
    userframe.columnconfigure(col, weight=1)


f = Figure()  # figsize=(5,5), dpi=100
a = f.add_subplot(111)
a.set_xlabel('Width', fontsize=20)
a.set_xlabel('Height', fontsize=20)

# a tk.DrawingArea
canvas = FigureCanvasTkAgg(f, plotframe)
canvas.get_tk_widget().grid(column=0, row=0, columnspan=4, rowspan=4, sticky=NSEW)
# canvas._tkcanvas.grid(column=0, row=0, sticky=N+E+W+S)

mapSelected = np.zeros((N_GRID,N_GRID))
im = a.imshow(mapSelected, animated=True, label="Place Field")
cbar_ax = f.add_axes([0.85, 0.15, 0.05, 0.7])
f.colorbar(im, cax=cbar_ax)
canvas.show()

'''GUI methods'''


def _update():
    global cumulativeCountMapList, mapSelected, a, f, canvas, im, cbar_ax, firingRateLabel, labelVar

    select = listbox.curselection()

    if len(select) != 0:
        # mapSelected = cumulativeCountMapList[select[0]]
        # max_norm = np.float(np.max(mapSelected))
        # if max_norm != 0:
        #     mapSelected = mapSelected/max_norm
        mapSelected = firingMaps[select[0]]
        im = a.imshow(mapSelected.T, animated=True, label="Place Field", cmap='hot', extent=[0, 1, 1, 0])  # , interpolation='nearest'
        cbar_ax = f.add_axes([0.85, 0.15, 0.05, 0.7])
        f.colorbar(im, cax=cbar_ax)
        canvas.show()

        newFR = 'FR = ' + str(round(firingRates[select[0]], 2))
        labelVar.set(newFR)

        print "Place field updated"
    else:
        print "No neuron selected!"


def _onselect(evt):
    global _update
    _update()


def _quit():
    global is_streaming, s
    is_streaming = False
    s.close()
    # block thread as well
    root.quit()     # stops mainloop
    root.destroy()  # this is necessary on Windows to prevent


listboxLabel = Label(userframe, text="Neuron ID", font=("Helvetica", 18), bd=5, relief=SUNKEN, width=12)  # , bg="blue"
listboxLabel.grid(column=0, row=1, sticky=N+W+S, padx=10)  # , columnspan=2, rowspan = 2,

listbox = Listbox(userframe, bd=5, font=("Helvetica", 15), relief=SUNKEN, width=15)
listbox.grid(column=0, row=2, sticky=N+W, padx=10, pady=40)  # , columnspan=2, rowspan = 2,
listbox.bind('<<ListboxSelect>>', _onselect)

listbox.insert(END, "    <Neurons>")

labelVar = StringVar()
labelVar.set('Firing Rate FR')

firingRateLabel = Label(userframe, textvariable=labelVar, font=("Helvetica", 18), bd=5, relief=SUNKEN, width=12)  # , bg="blue"
firingRateLabel.grid(column=0, row=3, sticky=N+W+S, padx=10)  # , columnspan=2, rowspan = 2,


buttonUpdate = Button(userframe, text='Update', font=("Helvetica", 18), command=_update, width=12)
buttonUpdate.grid(column=0, row=4, rowspan=1, sticky=N+W+S, padx=10, pady=40)  # ,columnspan=2, rowspan = 2

buttonQuit = Button(userframe, text='Quit', font=("Helvetica", 18), command=_quit, width=12)
buttonQuit.grid(column=0, row=5, rowspan=1, sticky=N+W+S, padx=10, pady=40)  # ,columnspan=2, rowspan = 8


# Create socket connection
(s, error) = socketSetup('localhost', 9090, 15)

# Create and start streaming thread
is_streaming = True
t1 = threading.Thread(target=streaming)
t1.start()
# t1.join()

mainloop()