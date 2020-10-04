import matplotlib.pyplot as plt
import csv

time,susceptible,infected,recovered=[],[],[],[]

with open('dataSet.csv', 'r') as csvfile:
    plots= csv.reader(csvfile, delimiter=',')
    for data in plots:
        time.append(int(data[0]))
        susceptible.append(int(data[1]))
        infected.append(int(data[2]))
        recovered.append(int(data[3]))

scaleX=20 #change x scale factor-> prints 20 days here 
xmin, xmax = plt.xlim()
plt.xlim(xmin*scaleX,xmax*scaleX)
plt.plot(time,infected,label='Infected',color='red')
plt.plot(time,recovered,label='Recovered',color='green')
plt.plot(time,susceptible,label='Susceptible',color='orange')

plt.title('Plot for number of vertices of a type v/s time')
plt.legend()

plt.xlabel('Time')
plt.ylabel('Number of vertices')

plt.show()
