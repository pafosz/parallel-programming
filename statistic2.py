import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

SIZES = {100, 200, 300, 400, 500, 1000, 2000}

def checking_multiplication_correctness(path: str) -> bool:
    A = np.loadtxt(f"result_for_lab2\\{path}\\A.txt", dtype=int, skiprows=1)
    B = np.loadtxt(f"result_for_lab2\\{path}\\B.txt", dtype=int, skiprows=1)
    result = np.loadtxt(f"result_for_lab2\\{path}\\result.txt", dtype=int, skiprows=1)

    current_result = np.dot(A, B)
    return (current_result == result).all()

def write_compare_res(output_path: str):
    with open(output_path, "w", encoding="utf-8") as file:
        for size in sorted(SIZES):
            is_correct = checking_multiplication_correctness(str(size))
            status = "correct" if is_correct else "incorrect"
            file.write(f"Matrix {size}x{size} multiplication: {status}\n")


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

    plt.xlabel('Size')
    plt.ylabel('Time (s)')
    plt.title('Зависимость Times от Sizes')
    plt.grid(True)
    plt.legend()
    plt.savefig('stat2.png', dpi=300, bbox_inches='tight')
    plt.show()



def main():
    plot_results("result_for_lab2\\statistic.csv")
    write_compare_res("compare_result2.txt")


if __name__ == "__main__":
    main()
