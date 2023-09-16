#!/bin/bash

#echo -e "\\n\\n\\n\\n\\n" > /dev/oled

while true
do
    STATE=$(cat /sys/class/thermal/cooling_device0/cur_state)
    TEMP=$(cat /sys/class/thermal/thermal_zone0/temp)
    DATE=$(date +'%H:%M:%S')
    #MEM=$(free -m | head -2 | tail -1 | awk '{print "m: "$3"/"$2}')
    MEM=$(free -m | head -2 | tail -1 | awk '{print $3"/"$2}' | tr -d '\n')
    CPU=$(mpstat  | tail -1 | awk '{print 100-$13}' | tr -d '\n')
    if [[ $STATE -le 1 ]]; then
        STATE="off"
    elif [[ $STATE -gt 1 ]]; then
        STATE="run"
    fi

    let TEMP=TEMP/1000
    MSG=$(printf '%s\\nfan: %s (%s C)\\ncpu %s\\nm: %s' ${DATE} ${STATE} ${TEMP} ${CPU} ${MEM} )
    #echo -e ${MSG}
    echo -e ${MSG} > /dev/oled
    sleep 1
done
