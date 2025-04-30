import os
import re
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple


def parse_output_file(filepath: str) -> Tuple[int, List[Tuple[int, float]]]:
    """
    Улучшенный парсер, поддерживающий разные форматы вывода.
    """
    with open(filepath, 'r') as f:
        content = f.read()

    # Ищем количество процессов (разные возможные варианты)
    proc_match = re.search(r'(?:Running|Processes|MPI processes)[^\d]*(\d+)', content)
    if not proc_match:
        raise ValueError(f"Не удалось найти количество процессов в {filepath}")
    proc_count = int(proc_match.group(1))

    results = []

    # Поддержка разных форматов времени:
    # 1. Size 100x100 Time: 12.345 ms
    # 2. Matrix 100 Time: 1.234 s
    # 3. 100x100: 0.123 sec
    time_patterns = [
        r'Size (\d+)x\d+ Time: ([\d.]+)\s*(?:ms|s|sec|seconds?)',
        r'Matrix (\d+).*?Time: ([\d.]+)\s*(?:ms|s|sec|seconds?)',
        r'(\d+)x\d+.*?(?:time|took):?\s*([\d.]+)\s*(?:ms|s|sec|seconds?)'
    ]

    for pattern in time_patterns:
        for match in re.finditer(pattern, content, re.IGNORECASE):
            size = int(match.group(1))
            time = float(match.group(2))

            # Конвертируем в миллисекунды если время в секундах
            if 's' in match.group(0).lower() and 'ms' not in match.group(0).lower():
                time *= 1000

            results.append((size, time))

    if not results:
        raise ValueError(f"Не найдены данные о размерах матриц и времени в {filepath}")

    return proc_count, results


def collect_data_from_folder(folder: str) -> Dict[int, List[Tuple[int, float]]]:
    """Собирает данные из всех файлов в папке."""
    data: Dict[int, List[Tuple[int, float]]] = {}

    for filename in os.listdir(folder):
        if filename.startswith(("slurm-", "output_")) and filename.endswith((".out", ".log")):
            filepath = os.path.join(folder, filename)
            try:
                proc_count, results = parse_output_file(filepath)
                if proc_count not in data:
                    data[proc_count] = []
                data[proc_count].extend(results)
                print(f"Обработан {filename}: {len(results)} записей")
            except Exception as e:
                print(f"Ошибка в {filename}: {str(e)}")

    return data


def plot_data(data: Dict[int, List[Tuple[int, float]]]) -> None:
    """Строит график производительности."""
    if not data or all(not v for v in data.values()):
        print("Нет данных для построения графика")
        return

    plt.figure(figsize=(12, 7))

    for proc_count in sorted(data.keys()):
        sizes, times = zip(*sorted(data[proc_count]))
        plt.plot(sizes, times, 'o-', label=f"{proc_count} процессов")

    plt.xlabel('Размер матрицы (NxN)')
    plt.ylabel('Время выполнения (мс)')
    plt.title('Производительность умножения матриц')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()

    # Сохраняем график
    plot_path = os.path.join(folder, "matrix_performance.png")
    plt.savefig(plot_path)
    print(f"График сохранён в {plot_path}")
    plt.show()


if __name__ == "__main__":
    folder = input("Введите путь к папке с результатами: ").strip() or "."
    data = collect_data_from_folder(folder)

    if data:
        print("\nСобранные данные:")
        for proc_count, results in sorted(data.items()):
            print(f"{proc_count} процессов: {len(results)} измерений")
        plot_data(data)
    else:
        print("Не удалось собрать данные. Проверьте:")
        print(f"1. Папка {os.path.abspath(folder)} существует")
        print("2. Файлы имеют правильный формат")
        print("3. Файлы содержат данные о размерах матриц и времени выполнения")