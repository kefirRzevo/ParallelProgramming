import subprocess
import matplotlib.pyplot as plt
import re
import json
from pathlib import Path

average = 5
max_threads = 16
repo_path = Path(__file__).parent.parent

mpi_base_path = repo_path / "build" / "OMP_Lab" / "MPI_Base"
omp_base_path = repo_path / "build" / "OMP_Lab" / "OMP_Base"
mpi_1c_path = repo_path / "build" / "OMP_Lab" / "MPI_1c"
omp_3b_path = repo_path / "build" / "OMP_Lab" / "OMP_3b"
data_path = repo_path / "OMP_Lab" / "data.json"
plot_path = repo_path / "OMP_Lab" / "plot.png"


class measure_type:
    threads: int
    time: float

    def __init__(self, threads: int, time: float):
        self.threads = threads
        self.time = time

    def __repr__(self) -> str:
        return f'"{self.threads}": {self.time}'


class measures_type:
    mpi_base: list[measure_type]
    omp_base: list[measure_type]
    mpi_1c: list[measure_type]
    omp_3b: list[measure_type]

    def __init__(self):
        self.mpi_base = []
        self.omp_base = []
        self.mpi_1c = []
        self.omp_3b = []

    def __repr__(self) -> str:
        mpi_base_repr = ", ".join(repr(m) for m in self.mpi_base)
        omp_base_repr = ", ".join(repr(m) for m in self.omp_base)
        mpi_1c_repr = ", ".join(repr(m) for m in self.mpi_1c)
        omp_3b_repr = ", ".join(repr(m) for m in self.omp_3b)
        return f'{{ "mpi_base": {{ {mpi_base_repr} }}, "omp_base": {{ {omp_base_repr} }}, "mpi_1c": {{ {mpi_1c_repr} }}, "omp_3b": {{ {omp_3b_repr} }} }}'


def count_time(args: list[str], prog: str) -> float:
    res: float = 0.0
    for i in range(average):
        p = subprocess.Popen(
            args=args, stdout=subprocess.PIPE, cwd=repo_path, encoding="UTF-8"
        )
        out, err = p.communicate()
        pattern = r"Elapsed time:\s*([0-9]*\.?[0-9]+(?:[eE][+-]?[0-9]+)?)\s*seconds"
        match = re.search(pattern, out)
        if match:
            elapsed_time = float(match.group(1))
            res = res + elapsed_time
        else:
            raise Exception(f"can not get time from '{prog}'")
    res = res / average
    return round(res, 4)


def measure_mpi(prog: str, threads: int) -> float:
    args = ["mpirun", "--map-by", ":oversubscribe", "-n", f"{threads}", f"{prog}"]
    return count_time(args, prog)


def measure_omp(prog: str, threads: int) -> float:
    args = [f"{prog}", f"{threads}"]
    return count_time(args, prog)


def write_data():
    data = measures_type()
    for i in range(1, max_threads + 1):
        mpi_base = measure_type(i, measure_mpi(mpi_base_path, i))
        omp_base = measure_type(i, measure_omp(omp_base_path, i))
        mpi_1c = measure_type(i, measure_mpi(mpi_1c_path, i))
        omp_3b = measure_type(i, measure_omp(omp_3b_path, i))
        data.mpi_base.append(mpi_base)
        data.omp_base.append(omp_base)
        data.mpi_1c.append(mpi_1c)
        data.omp_3b.append(omp_3b)
    with open(data_path, "w") as f:
        f.write(str(data))


def read_data() -> measures_type:
    data = measures_type()
    with open(data_path, "r") as f:
        res = json.load(f)
    mpi_base_measures = res["mpi_base"]
    for key, value in mpi_base_measures.items():
        data.mpi_base.append(measure_type(int(key), float(value)))
    omp_base_measures = res["omp_base"]
    for key, value in omp_base_measures.items():
        data.omp_base.append(measure_type(int(key), float(value)))
    mpi_1c_measures = res["mpi_1c"]
    for key, value in mpi_1c_measures.items():
        data.mpi_1c.append(measure_type(int(key), float(value)))
    omp_3b_measures = res["omp_3b"]
    for key, value in omp_3b_measures.items():
        data.omp_3b.append(measure_type(int(key), float(value)))
    return data


def plot_speedup(ax, measures: list[measure_type], label: str):
    speedup_of_executors = []
    efficiency_of_executors = []
    threads_list = [measure.threads for measure in measures]
    seq_time = next((measure for measure in measures if measure.threads == 1), None)
    for measure in measures:
        speedup_of_executors.append(seq_time.time / measure.time)
        efficiency_of_executors.append(seq_time.time / measure.time / measure.threads)

    ax[0].plot(threads_list, speedup_of_executors, label=label, linewidth=2.5)
    ax[1].plot(threads_list, efficiency_of_executors, label=label, linewidth=2.5)


def main():
    write_data()
    data = read_data()

    fig, ax = plt.subplots(nrows=2, figsize=[12, 10])

    fig.suptitle(r"Speedup $S$ and Efficiency $E$ as functions of $n$", fontsize=20)
    print(data)

    plot_speedup(ax, data.omp_base, "Baseline OMP")
    plot_speedup(ax, data.mpi_base, "Baseline MPI")
    plot_speedup(ax, data.mpi_1c, "Task 1C (MPI)")
    plot_speedup(ax, data.omp_3b, "Task 3B (OMP)")

    ax[0].grid()
    ax[0].set_xlabel(r"Executors $n$", fontsize=18)
    ax[0].set_ylabel(r"Speedup $S$", fontsize=18)
    ax[0].legend(loc="best", fontsize=14)

    ax[1].grid()
    ax[1].set_xlabel(r"Executors $n$", fontsize=18)
    ax[1].set_ylabel(r"Efficiency $E$", fontsize=18)

    plt.show()
    #plt.savefig(plot_path)


if __name__ == "__main__":
    main()
