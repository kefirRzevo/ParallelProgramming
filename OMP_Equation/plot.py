#!/usr/bin/python3

import numpy as np
from matplotlib import pyplot as plt
from sys import argv
import pandas as pd
from pathlib import Path

repo_path = Path(__file__).parent.parent
data_path = repo_path / "build" / "OMP_Equation" / "data.txt"

plot_solution = True
plot_efficiency = True

executors = [ 1, 2, 3, 4, 5, 6, 7, 8 ]
times_of_exec = [ 3.34794, 1.84636, 1.56328, 1.47320, 1.56026, 1.54534, 1.69580, 1.91949 ]

def plotSpeedup(ax, times, label):
    speedup_of_executors      = []
    efficiency_of_executors   = []
    
    for i in range(0, len(times)):
        speedup_of_executors.append(times[0] / times[i])
        efficiency_of_executors.append(times[0] / times[i] / executors[i])

    ax[0].plot(executors[0:len(times)], speedup_of_executors,    label=label, linewidth=2.5)
    ax[1].plot(executors[0:len(times)], efficiency_of_executors, label=label, linewidth=2.5)


def main():
    if plot_solution:        
        fig = plt.figure(figsize=[12, 8])
        plt.title("Solutions", fontsize=28)

        df = pd.read_csv(data_path, sep=" ")
        x = np.array(df.iloc[:, 0].tolist())
        bVarianceNum = 11
        for i in range(0, bVarianceNum):
            y = np.array(df.iloc[:, 1 + i].tolist())
            plt.plot(x, y, label=f'b = {i / 10}', linewidth=2)

        plt.legend(loc="best", fontsize=15)
        plt.grid()

        plt.xlabel(r"$x$", fontsize=24)
        plt.ylabel(r"$y$", fontsize=24)
        plt.xticks(fontsize=18)
        plt.yticks(fontsize=18)

        plt.show()


    if plot_efficiency:
        fig, ax = plt.subplots(nrows=2, figsize=[12, 10]) 
        fig.suptitle(r"Speedup $S$ and Efficiency $E$ as functions of $n$", fontsize=20)

        plotSpeedup(ax, times_of_exec, r"$N = 2^{20} - 1$; $b = 0.7$")
        
        ax[0].grid()
        ax[0].set_xlabel(r"Executors $n$", fontsize=18)
        ax[0].set_ylabel(r"Speedup $S$", fontsize=18)
        ax[0].legend(loc="best", fontsize=14)

        ax[1].grid()
        ax[1].set_xlabel(r"Executors $n$", fontsize=18)
        ax[1].set_ylabel(r"Efficiency $E$", fontsize=18)

        plt.show()

if __name__ == '__main__':
    main()
