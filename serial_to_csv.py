#! /usr/bin/python3

import csv
import time
import serial

#ser = serial.Serial('/dev/ttyUSB0', 115200)
ser = serial.Serial('COM5', 115200)

filename = "mac_data.csv"
header = ["TIME", "MAC", "RSSI"]

currentTime = time.time()

with open(filename, 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(header)

    scanning = False
    flag = False

file.close()

rows = []

while True:
    with open(filename, 'a', newline='') as file:
        writer = csv.writer(file)
        line = ser.readline().decode().strip()

        if line == "W=====":
            scanning = True
            flag = True
            print("Scanning...")
            currentTime = time.time()
            continue
        if line == "-----":
            if flag == True:
                scanning = False
                for row in rows:
                    writer.writerow([row.currentTime, row.mac, row.rssi])
        if scanning:
            try:
                mac, rssi = line.split('/')
                rows.append({
                    currentTime: currentTime,
                    mac: mac,
                    rssi: rssi
                })
            except ValueError:
                print(f"Invalid line: {line}")
                continue
