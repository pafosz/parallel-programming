import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

SIZES = {100, 200, 300, 400, 500, 1000, 2000}

def checking_multiplication_correctness(path: str) -> bool:
    A = np.loadtxt(f"result_for_lab2\\{path}\\A.txt", dtype=int)
    B = np.loadtxt(f"result_for_lab2\\{path}\\B.txt", dtype=int)
    result = np.loadtxt(f"result_for_lab2\\{path}\\result.txt", dtype=int)

    current_result = np.dot(A, B)
    return (current_result == result).all()


def write_compare_res(path: str):
    for i in SIZES:
        with open(path, "a", encoding="utf-8", newline="\n") as file:
            if checking_multiplication_correctness(str(i)):
                file.write(f"Size: {i}*{i} - true\n")
            else:
                file.write(f"Size: {i}*{i} - false\n")


def plot_results(csv_path):
    # Читаем CSV файл
    df = pd.read_csv(csv_path)

    # Получаем список размеров матриц из заголовков (исключая первый столбец)
    sizes = [int(col) for col in df.columns[1:]]

    # Создаем отдельный график для каждого количества потоков
    plt.figure(figsize=(10, 6))

    for i in range(len(df)):
        thread_count = df.iloc[i, 0]  # Первый столбец - количество потоков
        times = df.iloc[i, 1:].values  # Остальные столбцы - времена

        plt.plot(sizes, times, marker='o', label=f'{thread_count} threads')

    plt.xlabel('Matrix Size')
    plt.ylabel('Time (ms)')
    plt.title('Matrix Multiplication Performance')
    plt.grid(True)
    plt.legend()
    plt.savefig('stat2.png', dpi=300, bbox_inches='tight')
    plt.show()



def main():
    plot_results("result_for_lab2\\statistic.csv")


if __name__ == "__main__":
    main()
