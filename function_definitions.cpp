/* 
* General Function Definitions
* Contains function definitions for general use.
* Last Modified: 01/05/2020
*/
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>
#include <sstream>
#include <fstream>
#include <memory>
#include <cstdio>
#include <chrono>
#include <ctime>
#include "measurement_class.h"
#include "experiment_class.h"
#include "function_definitions.h"

const int non_number_flag{ -1 };
const int bad_split_position{ -2 };

bool is_non_zero_error(std::vector<double> variable_error_data)
{
	// Checks for any non-zero values in a list
	bool non_zero_error{ false };
	for (size_t i{ 0 }; i < variable_error_data.size(); i++) {
		if (variable_error_data[i] != 0) non_zero_error = true;
	}
	return non_zero_error;
}
std::string remove_zero_trail(std::string string_number)
{
	// uses stringstream to remove zeros at the end of a string double
	std::ostringstream reversed_number_stream;
	bool had_non_zero{ false };
	bool had_decimal{ false };
	for (size_t i{ string_number.length() - 1 }; i > 0; i--) {
		if (string_number[i] != '0' && string_number[i] != '.') had_non_zero = true;
		if (had_non_zero) reversed_number_stream << string_number[i];
		if (string_number[i] == '.') {
			had_non_zero = true;
			had_decimal = true;
		}
	}
	if (had_decimal) {
		reversed_number_stream << string_number[0];
		std::string reversed_number{ reversed_number_stream.str() };
		std::string corrected_string_number{ reversed_number.rbegin(), reversed_number.rend() };
		return corrected_string_number;
	} else return string_number;
}
std::string remove_trailing_spaces(std::string string_data)
{
	// Check string is not empty
	if (string_data.empty()) return string_data;
	// Check string is not only spaces
	bool only_spaces{ true };
	for (size_t i{ 0 }; i < string_data.length(); i++) {
		if (string_data[i] != ' ') only_spaces = false;
	}
	if (only_spaces) {
		std::string empty_string;
		return empty_string;
	}
	// scan backwards through string
	std::ostringstream reversed_stringstream;
	bool end_of_trail{ false };
	for (size_t i{ string_data.length() - 1 }; i > 0; i--) {
		if (string_data[i] != ' ') end_of_trail = true;
		if (end_of_trail) reversed_stringstream << string_data[i];
	}
	reversed_stringstream << string_data[0];
	std::string reversed_string{ reversed_stringstream.str() };
	std::string corrected_string{ reversed_string.rbegin(), reversed_string.rend() };
	return corrected_string;
}
bool is_data_point_a_number(std::string data_point)
{
	// Remove spaces in input
	data_point.erase(remove_if(data_point.begin(), data_point.end(), isspace), data_point.end());
	// Check if each char in data_point matches to a number
	std::string list_of_numbers{ "0123456789" };
	bool contains_non_digit{ false };
	int number_of_dots{ 0 };
	for (size_t i{ 0 }; i < data_point.length(); i++) {
		// Check for number
		bool is_current_char_a_number{ false };
		for (size_t j{ 0 }; j < list_of_numbers.length(); j++) {
			if (data_point[i] == list_of_numbers[j]) is_current_char_a_number = true;
		}
		if (!is_current_char_a_number) contains_non_digit = true;
		// Check for decimal point
		if (data_point[i] == '.') {
			number_of_dots += 1;
			contains_non_digit = false;
		}
		// Check for negative
		if (i == 0 && data_point[0] == '-') contains_non_digit = false;

	}
	if (number_of_dots > 1) {
		contains_non_digit = true;
	}
	return !contains_non_digit;
}
bool is_data_point_a_datestamp(std::string data_point)
{
	// remove spaces
	data_point.erase(remove_if(data_point.begin(), data_point.end(), isspace), data_point.end());
	// check for dd/mm/yyyy format
	if (data_point.length() != 10) return false;
	std::string list_of_numbers{ "0123456789" };
	bool is_datestamp{ true };
	for (size_t i{ 0 }; i < data_point.length(); i++) {
		if (i == 2 || i == 5) {
			if (data_point[i] != '/') is_datestamp = false;
		} else {
			bool contains_number{ false };
			for (size_t j{ 0 }; j < list_of_numbers.size(); j++) {
				if (data_point[i] == list_of_numbers[j]) contains_number = true;
			}
			if (!contains_number) is_datestamp = false;
		}
	}
	return is_datestamp;
}
std::vector<std::string> datestamps_from_single_date(std::string date, size_t length)
{
	std::vector<std::string> datestamps;
	for (size_t i{ 0 }; i < length; i++) datestamps.push_back(date);
	return datestamps;
}
std::vector<double> string_to_double_vector(std::vector<std::string> string_vector)
{
	// Convert a string vector to a double vector
	std::vector<double> double_vector;
	for (size_t i{ 0 }; i < string_vector.size(); i++) {
		if (!is_data_point_a_number(string_vector[i])) throw non_number_flag;
		else double_vector.push_back(stod(string_vector[i]));
	}
	return double_vector;
}
std::string choose_split(std::string string_to_split, char delimeter, int position)
{
	if (remove_trailing_spaces(string_to_split).empty()) {
		noexcept("ERROR: Attempted to split empty string");
	}
	std::stringstream stream(string_to_split);
	std::string item;
	std::vector<std::string> splitted_strings;
	while (getline(stream, item, delimeter)) {
		splitted_strings.push_back(item);
	}
	if (position >= static_cast<int>(splitted_strings.size())) throw bad_split_position;
	return splitted_strings[position];
}
std::string format_variables(const experiment& exp)
{
	std::ostringstream out_stream;
	// get the longest length inputs for later formatting
	size_t largest_count{ 0 };
	for (int i{ 0 }; i < exp.number_of_variables; i++) {
		// get the number of inputs and allow space for entry numbers
		std::vector<std::string> column_data{ exp.experiment_data[i]->get_string_data() };
		if (i == 0) largest_count = column_data.size();
		else if (column_data.size() > largest_count) largest_count = column_data.size();
	}
	size_t space_for_entry_numbers{ std::to_string(largest_count).length() + 3 };
	std::vector<size_t> longest_lengths_of_inputs;
	for (int i{ 0 }; i < exp.number_of_variables; i++) {
		std::string variable_name{ exp.experiment_data[i]->get_variable_name() };
		// get the legnth of the longest input for each column
		std::vector<std::string> column_data{ exp.experiment_data[i]->get_string_data() };
		size_t longest_length{ variable_name.length() };
		for (size_t j{ 0 }; j < column_data.size(); j++) {
			size_t current_length{ column_data[j].length() };
			if (current_length > longest_length) longest_length = current_length;
		}
		// make sure columns are divided so add 1 space either side
		longest_length += 2;
		// make sure heading can fit in the middle of the space
		size_t blank_heading_space{ longest_length - variable_name.length() };
		if (blank_heading_space % 2 != 0) longest_length++;
		longest_lengths_of_inputs.push_back(longest_length + 2);
	}
	// Format top lines
	out_stream << "+";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) out_stream << "-";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		out_stream << "+";
		for (size_t j{ 0 }; j < longest_lengths_of_inputs[i]; j++) out_stream << "-";
	}
	out_stream << "+" << std::endl;
	// Variable headings
	out_stream << "|";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) out_stream << " ";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		out_stream << "|";
		size_t heading_length{ exp.experiment_data[i]->get_variable_name().length() };
		size_t blank_space{ longest_lengths_of_inputs[i] - heading_length };
		for (size_t k{ 0 }; k < blank_space / 2; k++) out_stream << " ";
		out_stream << exp.experiment_data[i]->get_variable_name();
		for (size_t k{ 0 }; k < blank_space / 2; k++) out_stream << " ";
	}
	out_stream << "|" << std::endl;
	// Heading divider
	out_stream << "+";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) out_stream << "-";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		out_stream << "+";
		for (size_t j{ 0 }; j < longest_lengths_of_inputs[i]; j++) out_stream << "-";
	}
	out_stream << "+" << std::endl;
	// print data under headings
	for (int i{ 0 }; i < exp.experiment_data[0]->count(); i++) {
		for (int j{ 0 }; j < exp.number_of_variables; j++) {
			if (j == 0) {
				out_stream << "|";
				size_t space_for_current_number{ space_for_entry_numbers - std::to_string(i).length() - 2 };
				out_stream << "  " << i;
				for (size_t k{ 0 }; k < space_for_current_number; k++) out_stream << " ";
			}
			out_stream << "|";
			std::string current_value{ exp.experiment_data[j]->get_string_data()[i] };
			out_stream << " ";
			out_stream << current_value;

			// make sure formatting is okay between columns
			std::string column{ exp.experiment_data[j]->get_variable_name() };

			size_t space_to_fill{ longest_lengths_of_inputs[j] - current_value.length() };
			for (size_t k{ 0 }; k < space_to_fill - 1; k++) out_stream << " ";
		}
		out_stream << "|" << std::endl;
	}
	// Format bottom line
	out_stream << "+";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) out_stream << "-";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		out_stream << "+";
		for (size_t j{ 0 }; j < longest_lengths_of_inputs[i]; j++) out_stream << "-";
	}
	out_stream << "+" << std::endl;
	std::string formatted_variables{ out_stream.str()};

	return formatted_variables;
}
std::ostream& operator<<(std::ostream& os, const experiment& exp)
{
	// print experiment name
	os << exp.experiment_title << std::endl;
	// print formatted variables
	std::string formatted_variables{ format_variables(exp) };
	os << formatted_variables;
	return os;
}
bool is_trigger_used(std::string input, std::string trigger_word)
{
	if (trigger_word.length() >= input.length()) return false;
	bool trigger_used{ true };
	for (size_t i{ 0 }; i < input.length(); i++) {
		if (input[i] != trigger_word[i]) {
			if (i != trigger_word.length()) trigger_used = false; break;
		}
	}
	return trigger_used;
}
std::string get_selected_experiment_ID(std::string input, std::string trigger)
{
	int chars_to_erase{ static_cast<int>(trigger.length()) + 1 };
	std::string selected_experiment_number{ input.erase(0, chars_to_erase) };
	return selected_experiment_number;
}
bool check_selection_is_valid(std::string selected_experiment_number, std::vector<std::string> list_of_selections)
{
	bool selection_valid{ false };
	for (size_t i{ 0 }; i < list_of_selections.size(); i++)
	{
		if (selected_experiment_number == list_of_selections[i]) selection_valid = true;
	}
	if (selection_valid) return true;
	else return false;
}
experiment get_experiment_from_save_file(std::string experiment_filename)
{
	// Initialise member data to fill
	std::string exp_file_title;
	std::vector<std::string> exp_file_datestamps;
	std::vector<std::string> exp_file_variable_types;
	int exp_file_number_of_variables{ 0 };
	int exp_file_number_of_rows{ 0 };
	std::vector<std::shared_ptr<measurement>> exp_file_data;
	// Constructor for data from saved .txt files (specific format)
	std::ifstream file(experiment_filename);
	if (!file.good()) {
		std::cerr << "ERROR: File '" << experiment_filename << "' could not be opened" << std::endl;
		std::cin.get();
		exit(1);
	}
	// Read in all the data and headings
	std::string line;
	int line_number{ 0 };
	std::vector<std::string> variable_names;
	std::vector<std::string> all_data;
	while (getline(file, line)) {
		if (line_number == 0) {
			exp_file_title = line;
			line_number++;
			continue;
		} else if (line[0] == '|') {
			bool looking_for_data{ false };
			bool getting_data{ false };
			std::stringstream current_data;
			for (size_t i{ 1 }; i < line.length(); i++) {
				// If there has been a |, then if the next character is not a space the it is data
				if (looking_for_data) {
					if (line[i] != ' ') {
						getting_data = true;
					}
				}
				// Read in the data between the | and spaces 
				if (getting_data) {
					if (line[i] != '|')	current_data << line[i];
					else {
						getting_data = false;
						looking_for_data = false;
						if (line_number == 1) {
							// remove trailing spaces
							variable_names.push_back(remove_trailing_spaces(current_data.str()));
							current_data.str("");
							exp_file_number_of_variables++;
						} else {
							all_data.push_back(remove_trailing_spaces(current_data.str()));
							current_data.str("");
						}
					}
				}
				// Character shows that data will be after a few spaces
				if (line[i] == '|') {
					looking_for_data = true;
				}
			}
		} else continue;
		line_number++;
	}
	file.close();

	// Sort all the data with their variables
	std::vector<std::vector<std::string>> data_for_variables;
	for (int j{ 0 }; j < exp_file_number_of_variables; j++) {
		bool get_data_point{ false };
		int skipped_points{ 0 };
		std::vector<std::string> temp_variable_data;
		for (size_t i{ 0 }; i < all_data.size(); i++) {
			// First data point
			if (i == j) get_data_point = true;
			if (get_data_point) {
				temp_variable_data.push_back(all_data[i]);
				skipped_points = 0;
				get_data_point = false;
			}
			// Skip until next variable data point
			if (skipped_points == exp_file_number_of_variables - 1) get_data_point = true;
			else skipped_points++;
		}
		data_for_variables.push_back(temp_variable_data);
	}

	// Check first data point for each variable to identify its measurement type, then create variable objects
	std::string variable_type;
	for (int j{ 0 }; j < exp_file_number_of_variables; j++) {
		// check if variable is a datestamp
		size_t found_datestamp{ variable_names[j].find("Datestamp") };
		// if is a double variable
		if (is_data_point_a_number(data_for_variables[j][0])) {
			std::vector<double> double_variable_data;
			std::vector<std::string>::iterator data_start{ data_for_variables[j].begin() };
			std::vector<std::string>::iterator data_end{ data_for_variables[j].end() };
			std::vector<std::string>::iterator data_point;
			for (data_point = data_start; data_point < data_end; ++data_point) {
				double_variable_data.push_back(stod(*data_point));
			}
			// If the variable is an error set the variable type to 'error', otherwise keep as 'double'
			if (variable_names[j].find("Error") != std::string::npos) {
				exp_file_data.emplace_back(new double_data(variable_names[j], double_variable_data, datestamps_from_single_date("Date placeholder", double_variable_data.size())));
				exp_file_variable_types.push_back("error");
			} else {
				exp_file_data.emplace_back(new double_data(variable_names[j], double_variable_data, datestamps_from_single_date("Date placeholder", double_variable_data.size())));
				exp_file_variable_types.push_back("double");
			}
			exp_file_number_of_rows = static_cast<int>(double_variable_data.size());
		} else if (found_datestamp != std::string::npos || variable_names[j] == "Datestamp") {
			// if is a datestamp
			exp_file_data.emplace_back(new nominal_data(variable_names[j], data_for_variables[j], datestamps_from_single_date("Date placeholder", data_for_variables.size())));
			exp_file_variable_types.push_back("datestamp");
			exp_file_number_of_rows = static_cast<int>(data_for_variables[j].size());
			// check if it is the experiment datestamp
			if (variable_names[j] == "Datestamps") {
				exp_file_datestamps.clear();
				for (size_t i{ 0 }; i < data_for_variables[j].size(); i++) {
					exp_file_datestamps.push_back(data_for_variables[j][i]);
				}
			}
		} else {
			// if is a nominal variable
			exp_file_data.emplace_back(new nominal_data(variable_names[j], data_for_variables[j], datestamps_from_single_date("Date placeholder", data_for_variables.size())));
			exp_file_variable_types.push_back("nominal");
			exp_file_number_of_rows = static_cast<int>(data_for_variables[j].size());
		}
	}
	experiment exp_from_file{ exp_file_data, exp_file_number_of_variables, exp_file_number_of_rows, exp_file_title, exp_file_datestamps, exp_file_variable_types };
	return exp_from_file;
}
experiment get_experiment_from_csv(std::string experiment_filename)
{
	// Initialise member data to fill
	std::string exp_file_title;
	std::vector<std::string> exp_file_datestamps;
	std::vector<std::string> exp_file_variable_types;
	int exp_file_number_of_variables{ 0 };
	int exp_file_number_of_rows{ 0 };
	std::vector<std::shared_ptr<measurement>> exp_file_data;

	// Read data from a (general) csv file
	std::ifstream file(experiment_filename);
	if (!file.good()) {
		std::cerr << "ERROR: File '" << experiment_filename << "' could not be opened" << std::endl;
		std::cin.get();
		exit(1);
	}
	// Get number of variables
	std::string first_line;
	getline(file, first_line);
	exp_file_number_of_variables++;
	for (size_t i{ 0 }; i < first_line.length(); i++) {
		if (first_line[i] == ',') exp_file_number_of_variables++;
	}
	// Get the number of error variables
	int error_occurrences{ 0 };
	std::string::size_type pos{ 0 };
	std::string target = "Error";
	for (size_t i{ 0 }; i < first_line.length(); i++) {
		while ((pos = first_line.find(target, pos)) != std::string::npos) {
			++error_occurrences;
			pos += target.length();
		}
	}
	// Get the number of datestamp variables
	int datestamp_occurrences{ 0 };
	pos = 0;
	target = "Datestamps";
	for (size_t i{ 0 }; i < first_line.length(); i++) {
		while ((pos = first_line.find(target, pos)) != std::string::npos) {
			++datestamp_occurrences;
			pos += target.length();
		}
	}
	// Read in variables column by column as vectors of strings
	std::string line;
	// Variables to keep track of what's been read in
	int data_index{ 0 };
	int error_index{ 0 };
	int datestamp_index{ 0 };
	// Initialise containers
	std::vector<std::string> variable_names;
	std::vector<std::vector<std::string>> all_string_data(exp_file_number_of_variables - datestamp_occurrences - error_occurrences);
	std::vector<std::vector<std::string>> all_datestamp_data(datestamp_occurrences);
	std::vector<std::vector<double>> all_error_data(error_occurrences);
	std::vector<std::string> string_variable_types;  // Either string, error or datestamp
	int line_number{ 0 };
	for (int i{ 0 }; i < exp_file_number_of_variables; i++) {
		// Initialise variable parameters and file position	
		std::string variable_name;
		std::vector<std::string> string_variable_data;
		std::vector<std::string> variable_datestamps;
		std::vector<double> variable_errors;
		bool read_errors{ false };
		bool read_datestamps{ false };
		line_number = 0;
		file.clear();
		file.seekg(0, std::ios::beg);
		while (getline(file, line)) {
			// If it exists, get the data point
			std::string data_point;
			try {
				data_point = choose_split(line, ',', i);
			} catch (int error_flag) {
				if (error_flag == bad_split_position) {
					std::cerr << "ERROR: File contains empty data cell or imbalanced variable lengths" << std::endl;
					exit(error_flag);
				}
			}
			if (line_number == 0) {
				// Check if variable is an string, error or datestamp variable
				if (data_point.find("Error") != std::string::npos) {
					read_errors = true;
					string_variable_types.push_back("error");
				} else if (data_point.find("Datestamps") != std::string::npos || data_point == "Datestamps") {
					read_datestamps = true;
					string_variable_types.push_back("datestamp");
				} else string_variable_types.push_back("string");
				variable_name = data_point;
				variable_names.push_back(variable_name);
			} else {
				// Sort data into data, error, datestamp variables
				if (read_errors) {
					// If is an error variable assume that it is numerical
					try {
						if (!is_data_point_a_number(data_point)) throw non_number_flag;
						else all_error_data[error_index].push_back(stod(data_point));
					} catch (int error_flag) {
						if (error_flag == non_number_flag)	std::cerr << "ERROR: Cannot convert string to double" << std::endl;
						exit(error_flag);
					}
				} else if (read_datestamps) {
					all_datestamp_data[datestamp_index].push_back(data_point);
				} else all_string_data[data_index].push_back(data_point);
			}
			line_number++;
		}
		// Reset read bools and update indices
		if (read_errors) {
			read_errors = false;
			error_index++;
		} else if (read_datestamps) {
			read_datestamps = false;
			datestamp_index++;
		} else data_index++;
	}
	if (all_string_data[0].size() > 0)	exp_file_number_of_rows = static_cast<int>(all_string_data[0].size());
	else if (all_error_data[0].size() > 0)	exp_file_number_of_rows = static_cast<int>(all_error_data[0].size());
	else if (all_datestamp_data[0].size() > 0)	exp_file_number_of_rows = static_cast<int>(all_datestamp_data[0].size());
	// Check whether string data is numerical or not
	std::vector<std::string> data_variable_types;	// Either double or nominal
	for (size_t i{ 0 }; i < all_string_data.size(); i++) {
		bool contains_string{ false };
		for (size_t j{ 0 }; j < all_string_data[0].size(); j++) {
			if (!is_data_point_a_number(all_string_data[i][j])) {
				contains_string = true;
				break;
			}
		}
		if (contains_string) data_variable_types.push_back("nominal");
		else data_variable_types.push_back("double");
	}
	file.close();
	// Get full list of variable types
	int index{ 0 };
	for (size_t i{ 0 }; i < string_variable_types.size(); i++) {
		if (string_variable_types[i] == "string") {
			exp_file_variable_types.push_back(data_variable_types[index]);
			index++;
		} else exp_file_variable_types.push_back(string_variable_types[i]);
	}
	// If there are datestamps, set as experiment datestamps
	bool add_extra_variable{ false };  // Account for the extra new datestamp variable
	if (datestamp_occurrences > 0) {
		int index{ 0 };
		for (size_t i{ 0 }; i < variable_names.size(); i++) {
			if (variable_names[i] == "Datestamps") index = i;
		}
		exp_file_datestamps = all_datestamp_data[index];
	} else {
		// Otherwise set as blank dates
		add_extra_variable = true;
		for (int i{ 0 }; i < exp_file_number_of_rows; i++) exp_file_datestamps.push_back("00/00/0000");
	}
	// Add experiment dates
	exp_file_data.emplace_back(new nominal_data("Datestamps", exp_file_datestamps, exp_file_datestamps));
	// Create variables for experiment data
	int index_correction{ 0 };
	for (size_t i{ 0 }; i < exp_file_variable_types.size(); i++) {
		std::string type{ exp_file_variable_types[i] };
		if (type == "double") {
			// Initialise variable member data
			std::vector<double> variable_data;
			try {
				variable_data = string_to_double_vector(all_string_data[i - index_correction]);
			} catch (int error_flag) {
				if (error_flag == non_number_flag) {
					std::cerr << "Attempted to convert a non-numerical string to double" << std::endl;
					exit(error_flag);
				}
			}
			std::vector<double> variable_errors;
			std::vector<std::string> variable_datestamps;
			bool found_errors{ false };
			bool found_datestamps{ false };
			// Search for associated variables
			for (size_t j{ 0 }; j < variable_names.size(); j++) {
				if (variable_names[j].find(variable_names[i]) != std::string::npos) {
					if (variable_names[j].find("Error") != std::string::npos) {
						// Count error variables before it
						found_errors = true;
						int errors_before{ 0 };
						for (size_t k{ 0 }; k < j; k++) {
							if (variable_names[k].find("Error") != std::string::npos) {
								errors_before++;
							}
						}
						variable_errors = all_error_data[errors_before];
					} else if (variable_names[j].find("Datestamps") != std::string::npos) {
						// Count datestamp variables before it
						found_datestamps = true;
						int datestamps_before{ 0 };
						for (size_t k{ 0 }; k < j; k++) {
							if (variable_names[k].find("Datestamps") != std::string::npos) datestamps_before++;
						}
						variable_datestamps = all_datestamp_data[datestamps_before];
					}
				}
			}
			if (!found_errors) {
				for (int i{ 0 }; i < exp_file_number_of_rows; i++) variable_errors.push_back(0);
			}
			if (!found_datestamps) {
				variable_datestamps = exp_file_datestamps;
			}
			// Create variable and add to experiment
			exp_file_data.emplace_back(new double_data(variable_names[i], variable_data, variable_errors, variable_datestamps));
			if (found_errors) exp_file_data.emplace_back(new double_data(variable_names[i] + " Error", variable_errors, variable_datestamps));
			if (found_datestamps) exp_file_data.emplace_back(new nominal_data(variable_names[i] + " Datestamps", variable_datestamps, variable_datestamps));
		} else if (type == "nominal") {
			// Initialise variable member data
			std::vector<std::string> variable_data{ all_string_data[i - index_correction] };
			std::vector<std::string> variable_datestamps;
			bool found_datestamps{ false };
			// Search for associated variables
			for (size_t j{ 0 }; j < variable_names.size(); j++) {
				if (variable_names[j].find(variable_names[i]) != std::string::npos) {
					if (variable_names[j].find("Datestamps") != std::string::npos) {
						// Count datestamp variables before it
						found_datestamps = true;
						int datestamps_before{ 0 };
						for (size_t k{ 0 }; k < j; k++) {
							if (variable_names[k].find("Datestamps") != std::string::npos) datestamps_before++;
						}
						variable_datestamps = all_datestamp_data[datestamps_before];
					}
				}
			}
			if (!found_datestamps) {
				variable_datestamps = exp_file_datestamps;
			}
			// Create variable and add to experiment
			exp_file_data.emplace_back(new nominal_data(variable_names[i], variable_data, variable_datestamps));
			if (found_datestamps) exp_file_data.emplace_back(new nominal_data(variable_names[i], variable_datestamps, variable_datestamps));
		} else index_correction++;
	}
	// If a new datestamp (blank) variable was created, update variable types and variable number
	if (add_extra_variable) {
		exp_file_variable_types.insert(exp_file_variable_types.begin(), "datestamp");
		exp_file_number_of_variables++;
	}
	experiment exp_from_file{ exp_file_data, exp_file_number_of_variables, exp_file_number_of_rows, exp_file_title, exp_file_datestamps, exp_file_variable_types };
	return exp_from_file;
}
