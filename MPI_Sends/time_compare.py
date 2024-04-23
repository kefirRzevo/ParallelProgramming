import subprocess
import matplotlib.pyplot as plt
import os
import sys

execPath = os.path.join(os.path.dirname(__file__), './a.out')
comparePath = os.path.join(os.path.dirname(__file__), 'res/compare.txt')

sizeMax = 15
sizes = [0] * sizeMax

data = []
for i in range(sizeMax):
    data.append([0]*4)

for i in range(sizeMax):
    size = 2**i
    sizes[i] = size
    for j in range(4):
        p = subprocess.Popen(['mpirun', '--map-by', ':oversubscribe', '-n', '2', execPath, '{}'.format(size), str(j)], stdout = subprocess.PIPE)
        out, err = p.communicate()
        time = float(out)
        data[i][j] = time

with open(comparePath, 'w') as f:
    sys.stdout = f
    for i in range(sizeMax):
        s = str(sizes[i])
        for j in range(4):
            s += ' {}'.format(data[i][j])
        print(s)
