cmake_minimum_required(VERSION 3.10)
project(MyMatrixProject)

set(CMAKE_CXX_STANDARD 17)

# Указываем пути к заголовочным файлам
include_directories(include)

# Создаем исполняемый файл
add_executable(my_project main.cc src/matrix.cc)