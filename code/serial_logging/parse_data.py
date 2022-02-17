# import csv
import pandas 

# print("File to parse: ")
# file_name = input()
file_name = "/home/field1/Documents/data_log/20210925-113542.log"

# with open (file_name) as csv_file:
#     csv_reader = csv.reader(csv_file, delimiter=',')
#     line_count = 0
#     for row in csv_reader:
#         print(f)

# import numpy as np
df = pandas.read_csv(file_name)
print(df)

