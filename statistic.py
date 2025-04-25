import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

SIZES = {100, 200, 300, 400, 500, 1000, 2000}


def checking_multiplication_correctness(path: str) -> bool:
    A = np.loadtxt(f"result\\{path}\\A.txt", dtype=int)
    B = np.loadtxt(f"result\\{path}\\B.txt", dtype=int)
    result = np.loadtxt(f"result\\{path}\\result.txt", dtype=int)

    current_result = np.dot(A, B)
    return (current_result == result).all()


def write_compare_res(path: str):
    for i in SIZES:
        with open(path, "a", encoding="utf-8", newline="\n") as file:
            if checking_multiplication_correctness(str(i)):
                file.write(f"Size: {i}*{i} - true\n")
            else:
                file.write(f"Size: {i}*{i} - false\n")


def plot_results(path):
    df = pd.read_csv(path, sep=r"\s+")

    sizes = df["Sizes"].tolist()
    times = df["Times"].tolist()

    plt.plot(sizes, times, marker="o")
    plt.xlabel("Sizes")
    plt.ylabel("Times")
    plt.title("Зависимость Times от Sizes")
    plt.grid(True)
    plt.show()


def main():
    plot_results("result\\statistic.txt")


if __name__ == "__main__":
    main()
