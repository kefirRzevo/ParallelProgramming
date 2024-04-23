import subprocess
import matplotlib.pyplot as plt
import os
import sys

execPath = os.path.join(os.path.dirname(__file__), './a.out')
comparePath = os.path.join(os.path.dirname(__file__), './res/compare2.txt')
expPath = os.path.join(os.path.dirname(__file__), './res/exp.txt')

with open(expPath, 'r') as file:
    expStr = file.read().replace(' ', '')

sizeMin = 10
sizeMax = 10000
step = 100

data = []
for i in range(sizeMax):
    data.append([0]*3)

j = 0
for i in range(sizeMin, sizeMax, step):
    p1 = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '1', execPath, '{}'.format(i), 'e'], stdout = subprocess.PIPE)
    p2 = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '2', execPath, '{}'.format(i), 'e'], stdout = subprocess.PIPE)
    out1, err1 = p1.communicate()
    str1 = str(out1).removeprefix('b\'')
    out2, err2 = p2.communicate()
    str2 = str(out2).removeprefix('b\'')
    data[i][0] = i
    data[i][1] = next((k for k in range(min(len(str1), len(expStr))) if str1[k]!=expStr[k]), None)
    data[i][2] = next((k for k in range(min(len(str2), len(expStr))) if str2[k]!=expStr[k]), None)
    j = j + 1

with open(comparePath, 'w') as f:
    sys.stdout = f
    j = 0
    for i in range(sizeMin, sizeMax, step):
        s = str(data[i][0]) + '   ' + str(data[i][1]) + '   ' + str(data[i][2])
        print(s)
        j = j + 1
