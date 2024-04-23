import subprocess
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

execPath = os.path.join(os.path.dirname(__file__), './a.out')
comparePath = os.path.join(os.path.dirname(__file__), './res/compare.txt')

data = []
maxThread = 10
average = 5

for i in range(1, maxThread):
    data.append(i)

for i in range(1, maxThread):
    average_times = []
    for j in range(average):
        p = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '{}'.format(i), execPath], stdout = subprocess.PIPE)
        out, err = p.communicate()
        average_times.append(float(out))
    data[i-1] = np.mean(average_times)

with open(comparePath, 'w') as f:
    sys.stdout = f
    for i in range(1, maxThread):
        s = str(i) + '   ' + str(data[i-1])
        print(s)


plotPath = os.path.join(os.path.dirname(__file__), './res/compare.png')

threads = []
times = []

with open(comparePath, 'r') as file:
    lines = file.readlines()

for line in lines:
    string = line.strip().split()
    threads.append(float(string[0]))
    times.append(float(string[1]))

plt.figure(figsize=(8, 6))
plt.plot(threads, times, label='TimeExec(N_Threads)')
plt.xlabel('N_Threads')
plt.ylabel('Time, msec')
plt.legend()
plt.title('Compare plot for diffrenet number of threads')
plt.grid(True)
plt.savefig(plotPath)
