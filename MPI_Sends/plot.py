import subprocess
import matplotlib.pyplot as plt
import os
import sys

comparePath = os.path.join(os.path.dirname(__file__), 'res/compare.txt')
plotPath = os.path.join(os.path.dirname(__file__), 'res/compare.png')

sizes = []
send = []
ssend = []
rsend = []
bsend = []

with open(comparePath, 'r') as file:
    lines = file.readlines()

for line in lines:
    string = line.strip().split()
    sizes.append(float(string[0]))
    send.append(float(string[1]))
    ssend.append(float(string[2]))
    rsend.append(float(string[3]))
    bsend.append(float(string[4]))

plt.figure(figsize=(8, 6))
plt.plot(sizes, send, label='MPI_Send')
plt.plot(sizes, ssend, label='MPI_Ssend')
plt.plot(sizes, rsend, label='MPI_Rsend')
plt.plot(sizes, bsend, label='MPI_Bsend')
plt.xlabel('size')
plt.ylabel('Time, sec')
plt.legend()
plt.title('Compare plots for MPI send')
plt.grid(True)
plt.savefig(plotPath)
