#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <climits>
#include <limits>

using namespace std;

// Track whether buffer has unsaved changes
static bool IS_DIRTY = false;

// Forward declaration
int write_to_file(const std::vector<std::string>& lines, const std::string& filename);

// Function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

// Function to confirm choices
int dialogue_comfirm(const std::string& stringmessage) {
	std::cout << stringmessage << " (y/n): ";
	std::string response;
	std::getline(std::cin, response);
	std::transform(response.begin(), response.end(), response.begin(), ::tolower);

	return response == "y";
}

// Print statistics: number of lines and shortest/longest line lengths
void print_line_stats(const std::vector<std::string>& lines, const std::string& filename) {
	size_t count = lines.size();
	if (count == 0) {
		std::cout << "File '" << filename << "': 0 lines. Shortest = 0, Longest = 0" << std::endl;
		return;
	}

	size_t min_len = std::numeric_limits<size_t>::max();
	size_t max_len = 0;

	for (const auto& l : lines) {
		size_t len = l.size();
		if (len < min_len) min_len = len;
		if (len > max_len) max_len = len;
	}

	std::cout << "File '" << filename << "': " << count
		<< " lines. Shortest = " << min_len
		<< ", Longest = " << max_len << std::endl;
}

// Function to add a line of text
int add_line(std::vector<std::string>& lines) {
	std::string line = "";
	// Use validate_input to get a non-empty line
	std::cout << "Enter a line of text to add: ";
	std::getline(std::cin, line);
	line = trim(line);
	if (line.empty()) {
		std::cout << "Error: Input cannot be empty. Line not added." << std::endl;
		return 1;
	}
	lines.push_back(line);
	IS_DIRTY = true; // mark unsaved changes
	std::cout << "Line added successfully. Total letters " << line.size() << std::endl;
	return 0;
}

bool file_exists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}

// Centralized input validation functions
std::string validate_input(const std::string& prompt, bool allow_empty = false) {
	std::cout << prompt;
	std::string input;
	std::getline(std::cin, input);
	input = trim(input);

	if (input.empty() && !allow_empty) {
		std::cout << "Error: Input cannot be empty. Please try again." << std::endl;
	}
	return input;
}

int validateIntInput(const std::string& prompt, int minValue = INT_MIN, int max_value = INT_MAX) {
		std::string input = validate_input(prompt, false);

		// Check if input is numeric
		if (!input.empty()) {
			bool valid = true;
			for (char c : input) {
				if (!std::isdigit(static_cast<unsigned char>(c))) {
					valid = false;
					break;
				}
			}
			if (!valid) {
				std::cout << "Error: Please enter a valid number." << std::endl;
			}
		}
		try {
			long value = std::stol(input);
			if (value < minValue || value > max_value) {
				std::cout << "Error: Number must be between " << minValue << " and " << max_value << "." << std::endl;
			}
			return static_cast<int>(value);
		}
		catch (const std::out_of_range&) {
			std::cout << "Error: Number is out of range. Please try again." << std::endl;
		}
		catch (const std::invalid_argument&) {
			std::cout << "Error: Invalid number format. Please try again." << std::endl;
		}
}

/*
* Name: output_data
* Purpose: Outputs all stored lines to the console.
* Inputs: lines - std::vector<std::string> - A vector of strings containing the stored lines.
* Returns: int - 0 on success.
*/
void output_data(const std::vector<std::string>& lines) {
	if (lines.empty()) {
		std::cout << "No lines stored." << std::endl;
	}
	else {
		std::cout << "Stored lines:" << std::endl;
		for (size_t i = 0; i < lines.size(); ++i) {
			std::cout << i + 1 << ": " << lines[i] << std::endl;
		}
	}
}

// Function to read lines from a file
void read_from_file(std::vector<std::string>& lines, const std::string& default_file) {
	std::string filename_input = validate_input("Please enter a filename (leave empty to use default): ", true);

	std::string file = filename_input.empty() ? default_file : (filename_input + ".txt");

	std::ifstream infile(file);
	if (!infile) {
		std::cerr << "Error opening file: " << file << std::endl;
		return;
	}
	std::string line;
	std::vector<std::string> file_lines;
	while (std::getline(infile, line)) {
		file_lines.push_back(line);
		std::cout << line << std::endl;
	}

	infile.close();
	// append read lines into buffer
	lines.insert(lines.end(), file_lines.begin(), file_lines.end());
	// reading from disk means buffer matches disk => not dirty
	IS_DIRTY = false;

	std::cout << "Lines read from " << file << " successfully." << std::endl;

	// Print stats for the file that was read
	print_line_stats(file_lines, file);

	if (dialogue_comfirm("Would you like to write to this file?")) {
		write_to_file(lines, file);
	}
}

// Function to write lines to a file
int write_to_file(const std::vector<std::string>& lines, const std::string& filename) {
	std::string prompt = "Please enter a filename (leave empty to use default '" + filename + "'): ";
	std::string filename_input = validate_input(prompt, true);

	std::string final_name = filename_input.empty() ? filename : (trim(filename_input) + ".txt");

	// Check if file exists BEFORE opening it
	if (file_exists(final_name)) {
		if (!dialogue_comfirm("File already exists. Would you like to overwrite it?")) {
			std::cout << "File overwrite cancelled, no changes made." << std::endl;
			return 0; // Exit early
		}
	}

	// Now open the file for writing
	std::ofstream outfile(final_name);

	if (!outfile) {
		std::cerr << "Error opening file for writing: " << final_name << std::endl;
		return 1;
	}

	// Write all lines to the file
	for (const auto& line : lines) {
		outfile << line << std::endl;
	}

	outfile.close();
	// successful save -> buffer no longer dirty
	IS_DIRTY = false;

	std::cout << "Lines saved to " << final_name << " successfully." << std::endl;

	// Print stats for the file that was written
	print_line_stats(lines, final_name);

	return 0;
}

// Function to insert a line before a specified line number
int insert_line_before(std::vector<std::string>& lines) {
	int max_line = static_cast<int>(lines.size()) + 1;
	int line_number = validateIntInput("Please enter a line number to insert before: ", 1, max_line);

	std::string new_line = validate_input("Enter the line of text to insert: ", false);
	lines.insert(lines.begin() + line_number - 1, new_line);
	IS_DIRTY = true;
	std::cout << "Line inserted successfully before line " << line_number << "." << std::endl;
	return 0;
}

// Function to delete a specified line
int delete_line(std::vector<std::string>& lines) {
	if (lines.empty()) {
		std::cout << "No lines stored." << std::endl;
		return 0;
	}

	int line_number = validateIntInput("Please enter a line number to delete: ", 1, static_cast<int>(lines.size()));

	lines.erase(lines.begin() + line_number - 1);
	IS_DIRTY = true;
	std::cout << "Line " << line_number << " deleted successfully." << std::endl;
	return 0;
}

// Main function
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

			if (input == "e" || input == "exit") {
				if (IS_DIRTY) {
					if (!dialogue_comfirm("You have unsaved changes. Are you sure you want to exit?")) {
						continue;
					}
				}
				break;
			}
		}
	}
}