#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "include/matrix.h"
#include "include/stat.h"
#include "include/random_generator.h"

constexpr auto MIN_VALUE = 0;
constexpr auto MAX_VALUE = 1000;

static void create_directory(const std::string& dir_name) {
	try {
		if (std::filesystem::create_directories(dir_name)) {
			std::cout << "Directory " << dir_name << " created.\n";
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

static void chdir(std::string dir_name) {
	try {
		std::filesystem::current_path(dir_name);
		std::cout << "The current directory has been changed to: " << std::filesystem::current_path() << std::endl;
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

static void write_matrix(const std::string& filename, M::Matrix<int> matrix) {
	try {
		matrix.write_to_file(filename);
		std::cout << "The matrix has been successfully written to the file." << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}



int main() {
	
	std::vector<int> SIZES = { 100, 200, 300, 400, 500, 1000, 2000 }; 
	std::vector<double> TIMES(SIZES.size());
	chdir("C:\\Users\\user\\Desktop\\ALL\\University\\3 cours\\6 semester\\PP\\Labs");
	
	std::string dir_result = "result";
	create_directory(dir_result);
	chdir(dir_result);
	

	for (size_t i = 0; i < SIZES.size(); i++) {
		M::Matrix<int> A(SIZES[i], SIZES[i]);
		M::Matrix<int> B(SIZES[i], SIZES[i]);
		M::Matrix<int> result(SIZES[i], SIZES[i]);

		A.fill_random(MIN_VALUE, MAX_VALUE);
		B.fill_random(MIN_VALUE, MAX_VALUE);

		ExecutionTimer timer;
		result = A * B;
		timer.stop();

		std::string dir_name = std::to_string(SIZES[i]);
		create_directory(dir_name);
		chdir(dir_name);
		write_matrix("A.txt", A);
		write_matrix("B.txt", B);
		write_matrix("result.txt", result);

		TIMES[i] = timer.get_duration();

		chdir("..");
	}

	std::ofstream file("statistic.txt");
	if (!file.is_open())
		throw std::runtime_error("[write]Couldn't open the file for writing.");
	file << "Sizes\tTimes\n";
	for (size_t i = 0; i < SIZES.size(); i++) {
		file << SIZES[i] << "\t" << TIMES[i] << std::endl;
	}

	return 0;
}