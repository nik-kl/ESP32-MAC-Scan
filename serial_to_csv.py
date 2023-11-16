#! /usr/bin/python3

import serial
import csv

ser = serial.Serial('/dev/ttyUSB0', 115200)

filename = "mac_data.csv"
header = ["MAC", "RSSI"]

with open(filename, 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(header)

    scanning = False
    flag = False

    while True:
        line = ser.readline().decode().strip()

        if line == "=====":
            scanning = True
            flag = True
            continue

        if line == "-----":
            if flag == True:
                scanning = False
                break

        if scanning:
            try:
                mac, rssi = line.split('/')
                writer.writerow([mac, rssi])
            except ValueError:
                print(f"Invalid line: {line}")
                continue
