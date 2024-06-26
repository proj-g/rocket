#! /bin/bash
#Useful References:
# https://www.linux.com/training-tutorials/writing-simple-bash-script/
#https://unix.stackexchange.com/questions/42376/reading-from-serial-from-linux-command-line/42377

#Terminal commands:
# cat /dev/ttyACM0 > /home/field1/Documents/data_log/test_in_1.log
#  File name sequencing from:
# https://unix.stackexchange.com/questions/340010/how-do-i-create-sequentially-numbered-file-names-in-bash

today=$(date +%Y%m%d)-$(date +%H%M%S)
fname=$today.log
# machineName = uname -n

printf 'Logging data to: "%s" \n' "$fname"

SOURCEPORT=/dev/ttyACM0
# WRITE_DEST=/home/fiel1d/Documents/data_log/"$fname"
WRITE_DEST=/home/$USER/Documents/data_log/"$fname"
cat $SOURCEPORT | tee $WRITE_DEST #tee sends stdout to display and destination file
# https://stackoverflow.com/questions/418896/how-to-redirect-output-to-a-file-and-stdout
# cat $SOURCEPORT
