import subprocess
import matplotlib.pyplot as plt
import os
import sys

execPath = os.path.join(os.path.dirname(__file__), './a.out')
comparePath = os.path.join(os.path.dirname(__file__), './res/compare.txt')

sizeMin = 1000000
sizeMax = 10000000
step = 1000000
data = []

for i in range(sizeMin, sizeMax, step):
    data.append([0]*3)
j = 0
for i in range(sizeMin, sizeMax, step):
    p1 = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '1', execPath, '{}'.format(i)], stdout = subprocess.PIPE)
    p2 = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '2', execPath, '{}'.format(i)], stdout = subprocess.PIPE)
    out1, err1 = p1.communicate()
    out2, err2 = p2.communicate()
    time1 = float(out1)
    time2 = float(out2)
    data[j][0] = i
    data[j][1] = time1
    data[j][2] = time2
    j = j + 1

with open(comparePath, 'w') as f:
    sys.stdout = f
    j = 0
    for i in range(sizeMin, sizeMax, step):
        s = str(data[j][0]) + '   ' + str(data[j][1]) + '   ' + str(data[j][2])
        print(s)
        j = j + 1