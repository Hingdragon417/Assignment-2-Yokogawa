#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <climits>
#include <limits>
#include "Logger.h"

using namespace std;

/*
 * Name: isDirty
 * Purpose: Track whether buffer has unsaved changes.
 * Inputs: none
 * Returns: bool (true if buffer has unsaved changes)
 */
static bool isDirty = false;

/*
 * Name: writeToFile (forward declaration)
 * Purpose: Write lines to a file.
 * Inputs: lines - const std::vector<std::string>& - buffer to write
 *         filename - const std::string& - default filename
 * Returns: int - 0 on success, non-zero on error
 */
int writeToFile(const std::vector<std::string>& lines, const std::string& filename);

/*
 * Name: trim
 * Purpose: Trim whitespace from both ends of a string.
 * Inputs: str - const std::string& - string to trim
 * Returns: std::string - trimmed string
 */
std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

/*
 * Name: dialogConfirm
 * Purpose: Ask the user a yes/no confirmation.
 * Inputs: message - const std::string& - message to display
 * Returns: bool - true if user answered 'y'
 */
bool dialogConfirm(const std::string& message) {
	std::cout << message << " (y/n): ";
	std::string response;
	std::getline(std::cin, response);
	std::transform(response.begin(), response.end(), response.begin(), ::tolower);
	return response == "y";
}

/*
 * Name: printLineStats
 * Purpose: Print number of lines and shortest/longest line lengths for a vector of lines.
 * Inputs: lines - const std::vector<std::string>& - lines to analyze
 *         filename - const std::string& - filename for display
 * Returns: void
 */
void printLineStats(const std::vector<std::string>& lines, const std::string& filename) {
	size_t count = lines.size();
	if (count == 0) {
		std::cout << "File '" << filename << "': 0 lines. Shortest = 0, Longest = 0" << std::endl;
		return;
	}

	size_t minLen = std::numeric_limits<size_t>::max();
	size_t maxLen = 0;

	for (const auto& l : lines) {
		size_t len = l.size();
		if (len < minLen) minLen = len;
		if (len > maxLen) maxLen = len;
	}

	std::cout << "File '" << filename << "': " << count
		<< " lines. Shortest = " << minLen
		<< ", Longest = " << maxLen << std::endl;
}

/*
 * Name: addLine
 * Purpose: Prompt user to add a non-empty line to the buffer.
 * Inputs: lines - std::vector<std::string>& - buffer to append to
 * Returns: int - 0 on success, non-zero on error
 */
int addLine(std::vector<std::string>& lines) {
	std::string line;
	std::cout << "Enter a line of text to add: ";
	std::getline(std::cin, line);
	line = trim(line);
	if (line.empty()) {
		Logger::Log("Attempted to add empty line.");
		std::cout << "Error: Input cannot be empty. Line not added." << std::endl;
		return 1;
	}
	lines.push_back(line);
	isDirty = true;
	std::cout << "Line added successfully. Total letters " << line.size() << std::endl;
	return 0;
}

/*
 * Name: fileExists
 * Purpose: Check if a file exists.
 * Inputs: filename - const std::string& - file to check
 * Returns: bool - true if file exists and is readable
 */
bool fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}

/*
 * Name: validateInput
 * Purpose: Prompt user for input with optional empty-allowed behavior.
 * Inputs: prompt - const std::string& - prompt text
 *         allowEmpty - bool - whether empty input is accepted
 * Returns: std::string - the trimmed user input
 */
std::string validateInput(const std::string& prompt, bool allowEmpty = false) {

		std::cout << prompt;
		std::string input;
		std::getline(std::cin, input);
		input = trim(input);

		if (input.empty() && !allowEmpty) {
			std::cout << "Error: Input cannot be empty. Please try again." << std::endl;
		}
		return input;
	}
/*           
 * Name: validateIntInput
 * Purpose: Prompt the user for an integer within a specified range.
 * Inputs: prompt - const std::string& - prompt text
 *         minValue - int - inclusive minimum
 *         maxValue - int - inclusive maximum
 * Returns: int - the validated integer
 */
int validateIntInput(const std::string& prompt, int minValue = INT_MIN, int maxValue = INT_MAX) {
	while (true) {
		std::string input = validateInput(prompt, false);

		// Check if input is numeric (optional leading '-' allowed)
		bool valid = !input.empty();
		for (size_t i = 0; i < input.size() && valid; ++i) {
			if (i == 0 && input[i] == '-') {
				if (input.size() == 1) valid = false;
				continue;
			}
			if (!std::isdigit(static_cast<unsigned char>(input[i]))) {
				valid = false;
			}
		}
		if (!valid) {
			std::cout << "Error: Please enter a valid number." << std::endl;
			continue;
		}

		try {
			long value = std::stol(input);
			if (value < minValue || value > maxValue) {
				std::cout << "Error: Number must be between " << minValue << " and " << maxValue << "." << std::endl;
				continue;
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
}

/*
 * Name: outputData
 * Purpose: Outputs all stored lines to the console.
 * Inputs: lines - const std::vector<std::string>& - buffer to display
 * Returns: void
 */
void outputData(const std::vector<std::string>& lines) {
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

/*
 * Name: readFromFile
 * Purpose: Read lines from a file and append them to the buffer; print stats for the file read.
 * Inputs: lines - std::vector<std::string>& - buffer to append to
 *         defaultFile - const std::string& - default filename if user provides none
 * Returns: void
 */
void readFromFile(std::vector<std::string>& lines, const std::string& defaultFile) {
	std::string filenameInput = validateInput("Please enter a filename (leave empty to use default): ", true);
	std::string file = filenameInput.empty() ? defaultFile : (filenameInput + ".txt");

	std::ifstream infile(file);
	if (!infile) {
		std::cerr << "Error opening file: " << file << std::endl;
		return;
	}
	std::string line;
	std::vector<std::string> fileLines;
	while (std::getline(infile, line)) {
		fileLines.push_back(line);
		std::cout << line << std::endl;
	}
	infile.close();

	// append read lines into buffer
	lines.insert(lines.end(), fileLines.begin(), fileLines.end());
	// reading from disk means buffer matches disk => not dirty
	isDirty = false;

	std::cout << "Lines read from " << file << " successfully." << std::endl;
	printLineStats(fileLines, file);

	if (dialogConfirm("Would you like to write to this file?")) {
		writeToFile(lines, file);
	}
}

/*
 * Name: writeToFile
 * Purpose: Write buffer lines to a file and print stats for the file written.
 * Inputs: lines - const std::vector<std::string>& - buffer to write
 *         filename - const std::string& - default filename
 * Returns: int - 0 on success, non-zero on error
 */
int writeToFile(const std::vector<std::string>& lines, const std::string& filename) {
	std::string prompt = "Please enter a filename (leave empty to use default '" + filename + "'): ";
	std::string filenameInput = validateInput(prompt, true);

	std::string finalName = filenameInput.empty() ? filename : (trim(filenameInput) + ".txt");

	// Check if file exists BEFORE opening it
	if (fileExists(finalName)) {
		if (!dialogConfirm("File already exists. Would you like to overwrite it?")) {
			std::cout << "File overwrite cancelled, no changes made." << std::endl;
			return 0; // Exit early
		}
	}

	// Now open the file for writing
	std::ofstream outfile(finalName);
	if (!outfile) {
		std::cerr << "Error opening file for writing: " << finalName << std::endl;
		return 1;
	}

	// Write all lines to the file
	for (const auto& l : lines) {
		outfile << l << std::endl;
	}
	outfile.close();

	// successful save -> buffer no longer dirty
	isDirty = false;

	std::cout << "Lines saved to " << finalName << " successfully." << std::endl;
	printLineStats(lines, finalName);

	return 0;
}

/*
 * Name: insertLineBefore
 * Purpose: Insert a new line before a specified line number in the buffer.
 * Inputs: lines - std::vector<std::string>& - buffer to modify
 * Returns: int - 0 on success, non-zero on error
 */
int insertLineBefore(std::vector<std::string>& lines) {
	int maxLine = static_cast<int>(lines.size()) + 1;
	int lineNumber = validateIntInput("Please enter a line number to insert before: ", 1, maxLine);

	std::string newLine = validateInput("Enter the line of text to insert: ", false);
	lines.insert(lines.begin() + lineNumber - 1, newLine);
	isDirty = true;
	std::cout << "Line inserted successfully before line " << lineNumber << "." << std::endl;
	return 0;
}

/*
 * Name: deleteLine
 * Purpose: Delete a specified line from the buffer.
 * Inputs: lines - std::vector<std::string>& - buffer to modify
 * Returns: int - 0 on success
 */
int deleteLine(std::vector<std::string>& lines) {
	if (lines.empty()) {
		std::cout << "No lines stored." << std::endl;
		return 0;
	}

	int lineNumber = validateIntInput("Please enter a line number to delete: ", 1, static_cast<int>(lines.size()));
	lines.erase(lines.begin() + lineNumber - 1);
	isDirty = true;
	std::cout << "Line " << lineNumber << " deleted successfully." << std::endl;
	return 0;
}

/*
 * Name: main
 * Purpose: Entry point and command loop.
 * Inputs: none
 * Returns: int
 */
int main()
{
	std::vector<std::string> lines;

	while (true) {
		std::cout << "Enter a string (or 'exit' to quit): ";
		std::cout << "\nOptions: (a)dd line, (p)rint lines, (r)ead file, (i)nsert line before, (w)rite to file, (d)elete line, (e)xit" << std::endl;
		std::string input;
		std::getline(std::cin, input);
		input = trim(input);

		// normalize to lower-case (in-place)
		for (auto& c : input) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

		if (input == "a") {
			addLine(lines);
		}
		else if (input == "p") {
			outputData(lines);
		}
		else if (input == "i") {
			insertLineBefore(lines);
		}
		else if (input == "w") {
			writeToFile(lines, "output.txt");
		}
		else if (input == "r") {
			readFromFile(lines, "output.txt");
		}
		else if (input == "d") {
			deleteLine(lines);
		}
		else if (input == "e" || input == "exit") {
			if (isDirty) {
				if (!dialogConfirm("You have unsaved changes. Are you sure you want to exit?")) {
					continue;
				}
			}
			break;
		}
	}
	return 0;
} 