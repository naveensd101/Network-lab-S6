import csv
#csv file headings - num, time, rtt

count = []
with open('packet_time.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        t = float(row['time'])
        t = t * 10;
        t = int(t)
        count.append(t/10)

#draw a frequency graph for the count array
import matplotlib.pyplot as plt
dict = {}
for i in count:
    if i in dict:
        dict[i] += 1
    else:
        dict[i] = 1

print(dict)

x = []
y = []
for key in dict:
    x.append(key)
    y.append(dict[key])

plt.xlabel('Time (s)')
plt.ylabel('Frequency')
plt.plot(x, y)
plt.show()

