import subprocess
import matplotlib.pyplot as plt
import os
import sys

execPath = os.path.join(os.path.dirname(__file__), './a.out')
comparePath = os.path.join(os.path.dirname(__file__), './res/compare1.txt')

sizeMax = 100000
data = []
for i in range(sizeMax):
    data.append([0]*3)

for i in range(10, sizeMax, 1000):
    p1 = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '1', execPath, '{}'.format(i), 't'], stdout = subprocess.PIPE)
    p2 = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '2', execPath, '{}'.format(i), 't'], stdout = subprocess.PIPE)
    out1, err1 = p1.communicate()
    out2, err2 = p2.communicate()
    time1 = float(out1)
    time2 = float(out2)
    data[i][0] = i
    data[i][1] = time1
    data[i][2] = time2

with open(comparePath, 'w') as f:
    sys.stdout = f
    for i in range(10, sizeMax, 1000):
        s = str(data[i][0]) + '   ' + str(data[i][1]) + '   ' + str(data[i][2])
        print(s)
