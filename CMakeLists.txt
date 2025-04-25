cmake_minimum_required(VERSION 3.10)
project(MyMatrixProject)

set(CMAKE_CXX_STANDARD 17)

# Указываем пути к заголовочным файлам
include_directories(include)

# Создаем исполняемый файл, добавляя новый файл stats.cc
add_executable(my_project main.cc
src/file_manager.cc
src/matrix.cc
src/random_generator.cc
src/stat.cc
include/file_manager.h
include/matrix.h
include/random_generator.h
include/stat.h
)

add_executable(openmp
        openmp.cc
        include/stat.h
        include/matrix.h
        src/stat.cc
        src/matrix.cc
)

find_package(OpenMP REQUIRED)
target_link_libraries(openmp PRIVATE OpenMP::OpenMP_CXX)  # Прилинковать