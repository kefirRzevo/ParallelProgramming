import subprocess
import matplotlib.pyplot as plt
import os
import sys

comparePath = os.path.join(os.path.dirname(__file__), './res/compare.txt')
plotPath = os.path.join(os.path.dirname(__file__), './res/compare.png')

sizes = []
thread1 = []
thread2 = []

with open(comparePath, 'r') as file:
    lines = file.readlines()

for line in lines:
    string = line.strip().split()
    sizes.append(float(string[0]))
    thread1.append(float(string[1]))
    thread2.append(float(string[2]))

plt.figure(figsize=(8, 6))
plt.plot(sizes, thread1, label='1 thread')
plt.plot(sizes, thread2, label='2 thread')
plt.xlabel('size')
plt.ylabel('Time, sec')
plt.legend()
plt.title('Compare plots for Merge sort send')
plt.grid(True)
plt.savefig(plotPath)
