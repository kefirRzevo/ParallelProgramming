#!/usr/bin/python3

import numpy as np
from matplotlib import pyplot as plt
import pandas as pd
import os

dataPath = os.path.join(os.path.dirname(__file__), 'res/output.txt')

df = pd.read_csv(dataPath, sep=" ")
df.sort_values(by=['t', 'x'], inplace=True)

t = np.array(df.iloc[:, 0].tolist())
x = np.array(df.iloc[:, 1].tolist())
u = np.array(df.iloc[:, 2].tolist())

fig = plt.figure(figsize=[8, 6]) 
ax = fig.add_subplot(111, projection ='3d')

im = ax.scatter(x, t, u, cmap='coolwarm', c=u)
cbar = fig.colorbar(im, orientation='vertical')

ax.set_xlabel(r"$x$", fontsize=18)
ax.set_ylabel(r"$t$", fontsize=18)
ax.set_zlabel(r"$u$", fontsize=18)
ax.set_xlim([0, 1])
ax.set_ylim([0, 1])
ax.set_zlim([-1, 1.5])
plt.show()
