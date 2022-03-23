# import csv
# import pandas 
import re
# import matplotlib.pyplot as plt

def pretty_print(data):
    print("ADD  MESS  TIME  LAT LON ALT SPD BATT RSSI")
    for k in data:
        print(k)


# print("File to parse: ")
# file_name = input()
file_name = "20210925-113542.txt"
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
pretty_print(data_arr)

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

