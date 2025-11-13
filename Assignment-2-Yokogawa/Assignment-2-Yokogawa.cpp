#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>

using namespace std;

// Function to add a line of text
int add_line(std::vector<std::string>& lines) {
	std::cout << "Enter a line of text to add: ";
	std::string line; 
	std::getline(std::cin, line);
	lines.push_back(line);
	std::cout << "Line added successfully. Total words " << line.size() << std::endl;
	return 0; 
}

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

int output_data(const std::vector<std::string>& lines) {
	if (lines.empty()) {
		std::cout << "No lines stored." << std::endl; 
	}
	else {
		std::cout << "Stored lines:" << std::endl;
		for (size_t i = 0; i < lines.size(); ++i) {
			std::cout << i + 1 << ": " << lines[i] << std::endl;
		}
	}
	return 0;
}

int read_from_file(std::vector<std::string>& lines, const std::string& filename) {
	std::ifstream infile(filename);
	if (!infile) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return 1;
	}
	std::string line;
	while (std::getline(infile, line)) {
		lines.push_back(line);
		cout << line << endl;
	}
	
	std::cout << "Lines read from " << filename << " successfully." << std::endl;
	infile.close();
	return 0;
}

int write_to_file(const std::vector<std::string>& lines, const std::string& filename) {
	std::cout << "Please enter a filename: ";
	std::string filename_input;
	std::cin >> filename_input;
	std::cin.ignore();
	filename_input += ".txt";
	std::ofstream outfile(filename_input);

	if (!outfile) {
		std::cerr << "Error opening file for writing: " << filename_input << std::endl;
		return 1;
	}

	for (const auto& line : lines) {
		outfile << line << std::endl;
	}

	outfile.close();
	std::cout << "Lines saved to " << filename_input << " successfully." << std::endl;
	return 0;
}

int insert_line_before(std::vector<std::string>& lines) {
	std::cout << "Please enter a line number to insert before: ";
	std::string input;
	std::getline(std::cin, input);

	if (input.empty()) {
		std::cout << "Invalid input. Please enter a number." << std::endl;
		return 1;
	}

	for (char c : input) {
		if (!isdigit(c)) {
			std::cout << "Invalid input. Please enter a valid number." << std::endl;
			return 1;
		}
	}

	int line_number = std::stoi(input);
	if (line_number < 1 || line_number > static_cast<int>(lines.size()) + 1) {
		std::cout << "Invalid line number" << std::endl;
		return 1;
	}

	std::cout << "Enter the line of text to insert: ";
	std::string new_line;
	std::getline(std::cin, new_line);
	lines.insert(lines.begin() + line_number - 1, new_line);
	std::cout << "Line inserted successfully before line " << line_number << "." << std::endl;
	return 0;
}

int delete_line(std::vector<std::string>& lines) {

	if (lines.empty()) {
		std::cout << "No lines stored." << std::endl;
		return 0;
	}

	std::cout << "Please enter a line number to delete: ";
	std::string input;
	std::getline(std::cin, input);

	if (input.empty()) {
		std::cout << "Invalid input. Please enter a number." << std::endl;
		return 1;
	}

	for (char c : input) {
		if (!isdigit(c)) {
			std::cout << "Invalid input. Please enter a valid number." << std::endl;
			return 1;
		}
	}

	int line_number = std::stoi(input);

	if (line_number < 1 || line_number > static_cast<int>(lines.size())) {
		std::cout << "Invalid line number" << std::endl;
		return 1;
	}
	
	lines.erase(lines.begin() + line_number - 1);
	std::cout << "Line " << line_number << " deleted successfully." << std::endl;
	return 0;
}

int main()
{
	std::vector<std::string> lines; 

	while (true) {
		std::cout << "Enter a string (or 'exit' to quit): ";
		std::cout << "\nOptions: (a)dd line, (p)rint lines, (r)ead file, (i)nsert line before, (w)rite to file, (d)elete line, (e)xit" << std::endl;
		std::string input;
		std::getline(std::cin, input);
		input = trim(input);

		for (auto& c : input) c = tolower(c); {

			if (input == "a") {
				add_line(lines);
			}

			if (input == "p") {
				output_data(lines);
			}

			if (input == "i") {
				insert_line_before(lines);
			}

			if (input == "w") {
				write_to_file(lines, "output.txt");
			}

			if (input == "r") {
				read_from_file(lines, "output.txt");
			}

			if (input == "d") {
				delete_line(lines);
			}

			if (input == "e") {
				break;
			}
		}
	}
}