# import csv
# import pandas 
import re

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
print(data_arr[0:5])
# parsed_lines = lines_list.split(',')
# print(parsed_lines)
# parsed_str = raw_str.split()
# print(parsed_str)
# raw_list  = list(re.split(',', raw_str))
# clean_str = re.sub(',', '\n', raw_str)
# # print(raw_list)
# raw_list = list(clean_str.split("\n"))
# print(raw_list)
# itemized_list = []

# removal_list = []

# for w in len(raw_list):
#     if raw_list[w] == ',':
#         itemized_list[w] = ','
# #     else:
# #         itemized_list[w] = int(raw_list[w])

# itemized_list = list(map(int, raw_list))
# print(itemized_list)


# # itemized_list = [w.replace('\n\n', ',') for w in raw_list]
# # itemized_list = raw_list.remove(",")

# # with open (file_name) as csv_file:
# #     csv_reader = csv.reader(csv_file, delimiter=',')
# #     line_count = 0
# #     for row in csv_reader:
# #         print(f)

# # import numpy as np
# # df = pandas.read_csv(file_name)
# # print(df)

