# import csv
# import pandas 
import re
import matplotlib.pyplot as plt
import numpy as np
import gmplot
import sys

def pretty_print(data):
    print("ADD  MESS  TIME  LAT LON ALT SPD BATT RSSI")
    for k in data:
        print(k)

def plot_latlon(data, file_name):
    lats_all = data[:,3]/10E6
    lons_all = data[:,4]/10E6
    alts_all = data[:,5]
    speeds_all = data[:,6]
    lats = lats_all[lats_all != 0]#Cleans all zeros out of array
    lons = lons_all[lons_all != 0]#Cleans all zeros out of array
    alts = alts_all[alts_all != 0]
    speeds = speeds_all[lats_all != 0]
    # print(alts)
    plt.close(1)
    plt.figure(1)
    plt.scatter(lons, lats, c = speeds, s=50, cmap='jet')
    plt.colorbar()
    # plt.gray()
    # plt.ylim(51.85, 51.856)
    # plt.xlim(-111.67, -111.64)
    plt.show()

    # plt.figure(2)
    # plt.plot(alts_all)
    # plt.show()
    gmap = gmplot.GoogleMapPlotter(lats[0], lons[0], 14, map_type = 'hybrid' )
    for i in range(len(lats)-1):
        gmap.marker(lats[i], lons[i])
    gmap.draw("owen_ride.html")

# print("File to parse: ")
# file_name = input()
file_name = sys.argv[1] #Take file name from command line
# file_name = "20240418-170637.log"
raw_data = []
raw_data = open(file_name, 'r')

# print(raw_data.read())
raw_str = raw_data.readlines()
# print(raw_str)
lines_list = []
row_num = 0
data_arr = []

for line in raw_str:
    if line != '\n':
        int_line = []
        strip_line = line.strip()
        line.split(',')
        lines_list.append(strip_line.split(','))
        for element in lines_list[row_num]:
            int_line.append(int(element))
        data_arr.append(int_line)
        row_num +=1
    # print(line)
    # parsed_str = line.strip('\n')
# print(lines_list[0:5])
# print(data_arr[0:5])
# pretty_print(data_arr)
# Convert data array to numpy data array:
data_arr = np.array(data_arr)
plot_latlon(data_arr, file_name)
# print(data_arr)


#Data array structure:
# 1: address
# 2: message type
# 3: time
# 4: lat
# 5: lon
# 6: alt
# 7: spd
# 8: batt_volt
# 9: RSSI

