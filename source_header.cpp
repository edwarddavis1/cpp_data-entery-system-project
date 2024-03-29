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

const int non_number_flag{ -1 };

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
		try {
			if (!is_data_point_a_number(string_vector[i])) throw non_number_flag;
			else double_vector.push_back(stod(string_vector[i]));
		}
		catch (int error_flag) {
			if (error_flag == non_number_flag)	std::cerr << "ERROR: Cannot convert string to double" << std::endl;
			exit(error_flag);
		}
	}
	return double_vector;
}
std::string choose_split(std::string string_to_split, char delimeter, int pos)
{
	std::stringstream stream(string_to_split);
	std::string item;
	std::vector<std::string> splitted_strings;
	while (getline(stream, item, delimeter))
	{
		splitted_strings.push_back(item);
	}

	return splitted_strings[pos];
}
std::ostream& operator<<(std::ostream& os, const experiment& exp)
{
	// print experiment name
	std::cout << exp.experiment_title << std::endl;

	// print out variable names as headings and get the longest length inputs for later formatting
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
	os << "+";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) os << "-";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		os << "+";
		for (size_t j{ 0 }; j < longest_lengths_of_inputs[i]; j++) os << "-";
	}
	os << "+" << std::endl;
	// Variable headings
	os << "|";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) os << " ";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		os << "|";
		size_t heading_length{ exp.experiment_data[i]->get_variable_name().length() };
		size_t blank_space{ longest_lengths_of_inputs[i] - heading_length };
		for (size_t k{ 0 }; k < blank_space / 2; k++) os << " ";
		os << exp.experiment_data[i]->get_variable_name();
		for (size_t k{ 0 }; k < blank_space / 2; k++) os << " ";
	}
	os << "|" << std::endl;
	// Heading divider
	os << "+";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) os << "-";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		os << "+";
		for (size_t j{ 0 }; j < longest_lengths_of_inputs[i]; j++) os << "-";
	}
	os << "+" << std::endl;
	// print data under headings
	for (int i{ 0 }; i < exp.experiment_data[0]->count(); i++) {
		for (int j{ 0 }; j < exp.number_of_variables; j++) {
			if (j == 0) {
				os << "|";
				size_t space_for_current_number{ space_for_entry_numbers - std::to_string(i).length() - 2 };
				os << "  " << i;
				for (size_t k{ 0 }; k < space_for_current_number; k++) os << " ";
			}
			os << "|";
			std::string current_value{ exp.experiment_data[j]->get_string_data()[i] };
			os << " ";
			os << current_value;

			// make sure formatting is okay between columns
			std::string column{ exp.experiment_data[j]->get_variable_name() };

			size_t space_to_fill{ longest_lengths_of_inputs[j] - current_value.length() };
			for (size_t k{ 0 }; k < space_to_fill - 1; k++) os << " ";
		}
		os << "|" << std::endl;
	}
	// Format bottom line
	os << "+";
	for (size_t i{ 0 }; i < space_for_entry_numbers; i++) os << "-";
	for (size_t i{ 0 }; i < longest_lengths_of_inputs.size(); i++) {
		os << "+";
		for (size_t j{ 0 }; j < longest_lengths_of_inputs[i]; j++) os << "-";
	}
	os << "+" << std::endl;
	return os;
}
void print_file(std::string filename)
{
	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "ERROR: File '" << filename << "' could not be opened" << std::endl;
		std::cin.get();
		exit(1);
	}
	std::string line;
	while (getline(file, line)) std::cout << line << std::endl;
	file.close();
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
		if (line_number == 0) exp_file_title = line;
		if (line[0] != '|') {
			line_number++;
			continue;
		} else {
			bool looking_for_data{ false };
			bool getting_data{ false };
			std::stringstream current_data;
			for (size_t i{ 1 }; i < line.length(); i++) {
				if (looking_for_data) {
					if (line[i] != ' ') {
						getting_data = true;
					}
				}
				if (getting_data) {
					if (line[i] != '|')	current_data << line[i];
					else {
						getting_data = false;
						looking_for_data = false;
						if (line_number == 2) {
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
				if (line[i] == '|') {
					looking_for_data = true;
				}
			}
		}
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
		// Initialise variable parameters		
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
			// Get the date point
			std::string data_point{ choose_split(line, ',', i) };
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
					}
					catch (int error_flag) {
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
	if (datestamp_occurrences > 0) {
		int index{ 0 };
		for (size_t i{ 0 }; i < variable_names.size(); i++) {
			if (variable_names[i] == "Datestamps") index = i;
		}
		exp_file_datestamps = all_datestamp_data[index];
	} else {
		// Otherwise set as blank dates
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
			std::vector<double> variable_data{ string_to_double_vector(all_string_data[i - index_correction]) };
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
	experiment exp_from_file{ exp_file_data, exp_file_number_of_variables, exp_file_number_of_rows, exp_file_title, exp_file_datestamps, exp_file_variable_types };
	return exp_from_file;
}

nominal_data::nominal_data(const std::string name_of_variable, const std::vector<std::string> variable_data, const std::vector<std::string> variable_datestamps)
{
	variable_name = name_of_variable;
	data = variable_data;
	datestamps = variable_datestamps;
}
void nominal_data::print_data()
{
	std::cout << "Variable: " << variable_name << std::endl;
	std::vector<std::string>::iterator data_start{ data.begin() };
	std::vector<std::string>::iterator data_end{ data.end() };
	std::vector<std::string>::iterator data_iterator;
	int i{ 0 };
	for (data_iterator = data_start; data_iterator < data_end; ++data_iterator)
	{
		std::cout << "  (" << i << ") " << *data_iterator << std::endl;
		i++;
	}
	std::cout << std::endl;
}
size_t nominal_data::get_length() const { return data.size(); }
std::string nominal_data::get_variable_name() const { return variable_name; }
std::vector<std::string> nominal_data::get_datestamps() const { return datestamps; }
std::vector<double> nominal_data::get_error_data() const
{
	std::vector<double> error_data;
	for (size_t i{ 0 }; i < data.size(); i++) {
		error_data.push_back(0);
	}
	return error_data;
}
std::string& nominal_data::operator[](size_t index)
{
	if (index < 0 || index >= data.size()) {
		std::cerr << "ERROR: element out of bounds" << std::endl;
		throw("out of bounds");
	}
	return data[index];
}
void nominal_data::summary() const { std::cout << "nominal data summary" << std::endl; }
std::vector<std::string> nominal_data::summary_measures() const
{
	std::vector<std::string> list_of_summary_measures;
	list_of_summary_measures.push_back(std::to_string(this->count()));
	list_of_summary_measures.push_back("n/a");
	list_of_summary_measures.push_back("n/a");
	list_of_summary_measures.push_back("n/a");
	list_of_summary_measures.push_back("n/a");
	list_of_summary_measures.push_back("n/a");
	list_of_summary_measures.push_back("n/a");
	list_of_summary_measures.push_back("n/a");
	return list_of_summary_measures;
}
std::string nominal_data::get_measurement_type() const { return "nominal"; }
std::string nominal_data::get_variable_name() { return variable_name; }
int nominal_data::count() const { return static_cast<int>(data.size()); }
std::string nominal_data::mode() {
	std::cout << "mode avg" << std::endl;
	return 0;
}
std::vector<std::string> nominal_data::get_string_data() const { return data; }
std::vector<std::string> nominal_data::get_data() const { return data; }
void nominal_data::add_data(std::vector<std::string> data_to_add)
{
	for (size_t i{ 0 }; i < data_to_add.size(); i++) {
		data.push_back(data_to_add[i]);
	}
}
void nominal_data::add_datestamps(std::vector<std::string> datestamps_to_add)
{
	for (size_t i{ 0 }; i < datestamps_to_add.size(); i++) {
		datestamps.push_back(datestamps_to_add[i]);
	}
}

double_data::double_data(const std::string name_of_variable, const std::vector<double> variable_data, std::vector<std::string> variable_datestamps)
{
	// Constructor for a variable with no error
	variable_name = name_of_variable;
	for (size_t i{ 0 }; i < variable_data.size(); i++) {
		data.push_back(variable_data[i]);
		error_data.push_back(0);
		datestamps.push_back(variable_datestamps[i]);
	}
}
double_data::double_data(const std::string name_of_variable, const std::vector<double> variable_data, const double systematic_error, std::vector<std::string> variable_datestamps)
{
	// Constructor for a variable with a systematic error
	variable_name = name_of_variable;
	for (size_t i{ 0 }; i < variable_data.size(); i++) {
		data.push_back(variable_data[i]);
		error_data.push_back(systematic_error);
		datestamps.push_back(variable_datestamps[i]);
	}
}
double_data::double_data(const std::string name_of_variable, const std::vector<double> variable_data, const std::vector<double> measurement_error, std::vector<std::string> variable_datestamps)
{
	// Constructor for a variable with individual measurement error
	variable_name = name_of_variable;
	for (size_t i{ 0 }; i < variable_data.size(); i++) {
		data.push_back(variable_data[i]);
		error_data.push_back(measurement_error[i]);
		datestamps.push_back(variable_datestamps[i]);
	}
}
void double_data::print_data()
{
	std::cout << "Variable: " << variable_name << std::endl;
	typename std::vector<double>::iterator data_start{ data.begin() };
	typename std::vector<double>::iterator data_end{ data.end() };
	typename std::vector<double>::iterator data_iterator;
	int i{ 0 };
	size_t largest_entry_number{ data.size() };
	std::string largest_entry_number_string{ std::to_string(largest_entry_number) };
	size_t length_of_longest_entry_number{ largest_entry_number_string.length() };
	for (data_iterator = data_start; data_iterator < data_end; ++data_iterator)
	{
		std::cout << "  (" << i << ") ";
		size_t legnth_of_entry{ std::to_string(i).length() };
		size_t extra_space{ length_of_longest_entry_number - legnth_of_entry };
		for (size_t i{ 0 }; i < extra_space; i++) std::cout << " ";
		std::cout << *data_iterator << std::endl;
		i++;
	}
	std::cout << std::endl;
}
size_t double_data::get_length() const { return data.size(); }
std::vector<std::string> double_data::get_datestamps() const { return datestamps; }
std::vector<double> double_data::get_error_data() const { return error_data; }
std::string double_data::get_variable_name() const { return variable_name; }
std::string double_data::get_measurement_type() const { return "double"; }
double& double_data::operator[](size_t index)
{
	if (index < 0 || index >= data.size()) {
		std::cerr << "ERROR: element out of bounds" << std::endl;
		throw("out of bounds");
	}
	return data[index];
}
std::vector<std::string> double_data::get_string_data() const {
	// find out the maximum number of decimal places for formatting
	int largest_num_decimal_points{ 0 };
	bool contains_decimal{ false };
	for (size_t i{ 0 }; i < data.size(); i++) {
		std::string temp_data_point{ remove_zero_trail(std::to_string(data[i])) };
		bool counting_decimals{ false };
		int current_decimal_points{ 0 };
		for (size_t j{ 0 }; j < temp_data_point.length(); j++) {
			if (counting_decimals) current_decimal_points++;
			if (temp_data_point[j] == '.') {
				counting_decimals = true;
				contains_decimal = true;
			}
		}
		if (current_decimal_points > largest_num_decimal_points) largest_num_decimal_points = current_decimal_points;
	}
	// return the data with same amount of decimal points
	std::vector<std::string> string_data;
	for (size_t i{ 0 }; i < data.size(); i++) {
		std::string temp_data_point{ std::to_string(data[i]) };
		if (contains_decimal) {
			bool counting_decimals{ false };
			int current_decimal_points{ 0 };
			std::stringstream data_point;
			for (size_t j{ 0 }; j < temp_data_point.length(); j++) {
				if (counting_decimals) current_decimal_points++;
				data_point << temp_data_point[j];
				if (current_decimal_points == largest_num_decimal_points) break;
				if (temp_data_point[j] == '.') counting_decimals = true;
			}
			string_data.push_back(data_point.str());
		} else string_data.push_back(remove_zero_trail(temp_data_point));
	}
	return string_data;
}
std::vector<double> double_data::get_data() const { return data; }
int double_data::count() const { return static_cast<int>(data.size()); }
double double_data::mean() const
{
	double sum{ 0 };
	int number_of_points{ 0 };
	for (size_t i{ 0 }; i < data.size(); i++) {
		sum += data[i];
		number_of_points++;
	}
	double mean{ sum / number_of_points };
	return mean;
}
double double_data::standard_deviation() const
{
	double sum_of_squared_deviation_from_mean{ 0 };
	int number_of_points{ 0 };
	for (size_t i{ 0 }; i < data.size(); i++) {
		sum_of_squared_deviation_from_mean += pow(data[i] - this->mean(), 2);
		number_of_points++;
	}
	double standard_deviation{ pow(sum_of_squared_deviation_from_mean / (number_of_points - 1.) , 0.5) };
	return standard_deviation;
}
double double_data::min() const
{
	double min{ data[0] };
	for (size_t i{ 1 }; i < data.size(); i++) {
		if (data[i] < min) min = data[i];
	}
	return min;
}
double double_data::max() const
{
	double max{ data[0] };
	for (size_t i{ 1 }; i < data.size(); i++) {
		if (data[i] > max) max = data[i];
	}
	return max;
}
double double_data::get_percentile_value(double percentile) const
{
	// Sort data numerically
	std::vector<double> sorted_data{ data };
	std::sort(sorted_data.begin(), sorted_data.end());
	int number_of_points{ static_cast<int>(data.size()) };
	// find the percentile both as int and double
	int int_index{ static_cast<int>((number_of_points * percentile) / 100) };
	double double_index{ (number_of_points * percentile) / 100. };
	// if percentile is a decimal, int and double will be different in which case round up
	if (static_cast<double>(int_index) != double_index && int_index + 1 < number_of_points) int_index++;
	double percentile_value{ sorted_data[int_index] };

	return percentile_value;
}
void double_data::summary() const
{
	int length_of_heading{ static_cast<int>(variable_name.length()) + 14 };
	for (int i{ 0 }; i < length_of_heading; i++) std::cout << "-";
	std::cout << "\n   " << variable_name << " Summary   " << std::endl;
	for (int i{ 0 }; i < length_of_heading; i++) std::cout << "-";
	std::cout << "\nCount: " << this->count() << std::endl
		<< "Mean: " << this->mean() << std::endl
		<< "Std: " << this->standard_deviation() << std::endl
		<< "Min: " << this->min() << std::endl
		<< "25%: " << this->get_percentile_value(25) << std::endl
		<< "50%: " << this->get_percentile_value(50) << std::endl
		<< "75%: " << this->get_percentile_value(75) << std::endl
		<< "Max: " << this->max() << std::endl;
	for (int i{ 0 }; i < length_of_heading; i++) std::cout << "-";
	std::cout << std::endl;
}
std::vector<std::string> double_data::summary_measures() const
{
	std::vector<std::string> list_of_summary_measures;
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->count())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->mean())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->standard_deviation())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->min())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->get_percentile_value(25))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->get_percentile_value(50))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->get_percentile_value(75))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->max())));
	return list_of_summary_measures;
}
void double_data::add_data(std::vector<double> data_to_add)
{
	for (size_t i{ 0 }; i < data_to_add.size(); i++) {
		data.push_back(data_to_add[i]);
	}
}
void double_data::add_datestamps(std::vector<std::string> datestamps_to_add)
{
	for (size_t i{ 0 }; i < datestamps_to_add.size(); i++) {
		datestamps.push_back(datestamps_to_add[i]);
	}
}
void double_data::add_errors(std::vector<double> errors_to_add)
{
	for (size_t i{ 0 }; i < errors_to_add.size(); i++) {
		error_data.push_back(errors_to_add[i]);
	}
}

experiment::experiment(std::vector<std::shared_ptr<measurement>> exp_data, int exp_number_of_variables, int exp_number_of_rows, std::string exp_title, std::vector<std::string> exp_datestamps, std::vector<std::string> exp_variable_types)
{
	experiment_data = exp_data;
	number_of_variables = exp_number_of_variables;
	number_of_rows = exp_number_of_rows;
	experiment_title = exp_title;
	experiment_datestamps = exp_datestamps;
	variable_types = exp_variable_types;
}
experiment::experiment(nominal_data variable, std::string title_of_experiment)
{
	// Constructor from existing nominal variable
	number_of_rows = variable.get_length();
	number_of_variables = 2;
	experiment_title = title_of_experiment;
	std::cout << "         Adding data to experiment" << std::endl;
	experiment_data.emplace_back(new nominal_data("Datestamps", variable.get_datestamps(), variable.get_datestamps()));
	variable_types.push_back("datestamp");
	experiment_datestamps = variable.get_datestamps();
	experiment_data.emplace_back(new nominal_data(variable.get_variable_name(), variable.get_string_data(), variable.get_datestamps()));
	variable_types.push_back("nominal");
}
experiment::experiment(double_data variable, std::string title_of_experiment)
{
	// Constructor from existing double variable
	number_of_rows = variable.get_length();
	number_of_variables = 2;
	experiment_title = title_of_experiment;
	std::cout << "         Adding data to experiment" << std::endl;
	experiment_data.emplace_back(new nominal_data("Datestamps", variable.get_datestamps(), variable.get_datestamps()));
	variable_types.push_back("datestamp");
	experiment_datestamps = variable.get_datestamps();
	experiment_data.emplace_back(new double_data(variable.get_variable_name(), variable.get_data(), variable.get_datestamps()));
	variable_types.push_back("double");
	std::vector<double> variable_error_data{ variable.get_error_data() };
	bool non_zero_error{ false };
	for (size_t i{ 0 }; i < variable_error_data.size(); i++) {
		if (variable_error_data[i] != 0) non_zero_error = true;
	}
	if (non_zero_error) {
		experiment_data.emplace_back(new double_data(variable.get_variable_name() + " Error", variable.get_error_data(), variable.get_datestamps()));
		variable_types.push_back("error");
	}
}
experiment::experiment(std::string experiment_filename)
{
	if (experiment_filename.find(".txt") != std::string::npos) {
		experiment exp_from_file{ get_experiment_from_save_file(experiment_filename) };
		experiment_data = exp_from_file.get_experiment_data();
		number_of_variables = exp_from_file.get_number_of_variables();
		number_of_rows = exp_from_file.get_number_of_rows();
		experiment_title = exp_from_file.get_title();
		experiment_datestamps = exp_from_file.get_datestamps();
		variable_types = exp_from_file.get_variable_types();
	} else if (experiment_filename.find(".csv") != std::string::npos) {
		experiment exp_from_file{ get_experiment_from_csv(experiment_filename) };
		experiment_data = exp_from_file.get_experiment_data();
		number_of_variables = exp_from_file.get_number_of_variables();
		number_of_rows = exp_from_file.get_number_of_rows();
		experiment_title = exp_from_file.get_title();
		experiment_datestamps = exp_from_file.get_datestamps();
		variable_types = exp_from_file.get_variable_types();
	} else {
		std::cerr << "ERROR: File type not supported; please use a (specified format) .txt or a .csv file" << std::endl;
	}
}
experiment& experiment::operator=(experiment& exp)
{
	// deal with self-assignment
	if (&exp == this) return *this;

	// reset this object
	experiment_data.clear();
	experiment_datestamps.clear();
	variable_types.clear();
	number_of_rows = 0;
	number_of_variables = 0;
	experiment_title = "My Experiment";

	// copy data over
	number_of_rows = exp.number_of_rows;
	number_of_variables = exp.number_of_variables;
	experiment_title = exp.experiment_title;
	// check if datestamps are listed
	bool datestamps_exist{ true };
	if (exp.experiment_datestamps.size() == 0) datestamps_exist = false;
	if (datestamps_exist) {
		for (size_t i{ 0 }; i < exp.experiment_datestamps.size(); i++) {
			experiment_datestamps.push_back(exp.experiment_datestamps[i]);
		}
	}
	for (int i{ 0 }; i < number_of_variables; i++) {
		experiment_data.push_back(exp.experiment_data[i]);
		variable_types.push_back(exp.variable_types[i]);
	}

	return *this;
}
measurement& experiment::operator[](int index)
{
	if (index < 0 || index >= number_of_variables) {
		std::cerr << "ERROR: element out of bounds" << std::endl;
		throw("out of bounds");
	}
	return *experiment_data[index];
}
std::vector<std::shared_ptr<measurement>> experiment::get_experiment_data() const { return experiment_data; }
void experiment::set_title(std::string title) { experiment_title = title; }
std::string experiment::get_title() const { return experiment_title; }
int experiment::get_number_of_variables() const { return number_of_variables; }
int experiment::get_number_of_rows() const { return number_of_rows; }
std::vector<std::string> experiment::get_list_of_variable_names() const
{
	std::vector<std::string> list_of_variable_names;
	for (size_t i{}; i < experiment_data.size(); i++) {
		list_of_variable_names.push_back(experiment_data[i]->get_variable_name());
	}
	return list_of_variable_names;
}
std::vector<std::string> experiment::get_datestamps() const { return experiment_datestamps; }
std::vector<std::string> experiment::get_variable_types() const { return variable_types; }
void experiment::add_datestamp(std::string datestamp) { experiment_datestamps.push_back(datestamp); }
void experiment::summary()
{
	std::vector<std::shared_ptr<measurement>> list_of_variables{ experiment_data };
	std::vector<std::shared_ptr<measurement>>::iterator first_variable{ experiment_data.begin() };
	std::vector<std::shared_ptr<measurement>>::iterator last_variable{ experiment_data.end() };
	std::vector<std::shared_ptr<measurement>>::iterator variable;

	// get all summary measures
	std::vector<std::vector<std::string>> all_summary_measures;
	// get length of measure values for formatting
	std::vector<size_t> longest_values;
	for (variable = first_variable; variable < last_variable; ++variable) {
		std::vector<std::string> temp_summary_measures{ (*variable)->summary_measures() };
		all_summary_measures.push_back(temp_summary_measures);
		size_t longest_value{ (*variable)->get_variable_name().length() };
		for (size_t i{ 0 }; i < temp_summary_measures.size(); i++) {
			if (temp_summary_measures[i].length() > longest_value) longest_value = temp_summary_measures[i].length();
		}
		longest_values.push_back(longest_value);
	}

	// print variable headings
	std::cout << "       ";  // space for longest measure name
	int i{ 0 };
	std::vector<std::string> measures{ "Count", "Mean", "Std", "Min", "25%", "50%", "75%", "Max" };
	size_t width_of_summary{ 7 };
	for (variable = first_variable; variable < last_variable; ++variable) {
		std::string variable_name{ (*variable)->get_variable_name() };
		std::cout << variable_name;
		width_of_summary += variable_name.length() + 1;
		// leave space for longest longest measures
		size_t space_to_leave{ longest_values[i] - variable_name.length() };
		for (size_t j{ 0 }; j < space_to_leave; j++) std::cout << " "; width_of_summary++;
		std::cout << "   ";
		width_of_summary += 3;
		i++;
	}
	std::cout << std::endl;
	for (size_t i{ 0 }; i < width_of_summary; i++) std::cout << "-";
	std::cout << std::endl;
	// print summary measures
	for (size_t i{ 0 }; i < all_summary_measures[0].size(); i++) {
		std::cout << measures[i];
		size_t space_to_leave{ 7 - measures[i].length() };
		for (size_t k{ 0 }; k < space_to_leave; k++) std::cout << " ";

		for (int j{ 0 }; j < this->get_number_of_variables(); j++) {
			std::cout << all_summary_measures[j][i];
			size_t space_between{ longest_values[j] - all_summary_measures[j][i].length() + 3 };
			for (size_t j{ 0 }; j < space_between; j++) std::cout << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
void experiment::save_experiment() const
{
	// save file
	std::ofstream outfile(experiment_title + ".txt");
	outfile << this->get_title() << std::endl;
	// enter the data
	outfile << *this;
	outfile.close();

	// get current list of available experiments
	std::vector<std::string> available_experiments;
	std::ifstream current_saved_experiments("list_of_experimentfiles.txt");
	if (!current_saved_experiments.good()) {
		std::cerr << "ERROR: File could not be opened" << std::endl;
		std::cin.get();
		exit(1);
	}
	std::string filename;
	while (getline(current_saved_experiments, filename))
	{
		if (filename != experiment_title) available_experiments.push_back(filename);
	}
	current_saved_experiments.close();
	// add to the list of available experiments and rewrite file
	available_experiments.push_back(experiment_title);
	std::ofstream new_saved_experiments("list_of_experimentfiles.txt");
	for (size_t i{ 0 }; i < available_experiments.size(); i++) new_saved_experiments << available_experiments[i] << std::endl;
	new_saved_experiments.close();
	std::cout << "saved experiment: " << experiment_title << " as " << experiment_title << ".txt" << std::endl;
}
void experiment::delete_experiment()
{
	// delete file
	std::string file_to_delete{ experiment_title + ".txt" };
	if (remove(file_to_delete.c_str()) != 0) std::cerr << "ERROR: failed to delete " << file_to_delete << std::endl;
	// get current list of available experiments
	std::vector<std::string> available_experiments;
	std::ifstream current_saved_experiments("list_of_experimentfiles.txt");
	if (!current_saved_experiments.good()) {
		std::cerr << "ERROR: File could not be opened" << std::endl;
		std::cin.get();
		exit(1);
	}
	std::string filename;
	while (getline(current_saved_experiments, filename))
	{
		if (filename != experiment_title) available_experiments.push_back(filename);
	}
	current_saved_experiments.close();
	// disclude deleted experiment
	std::ofstream new_saved_experiments("list_of_experimentfiles.txt");
	for (size_t i{ 0 }; i < available_experiments.size(); i++) {
		if (available_experiments[i] != experiment_title) new_saved_experiments << available_experiments[i] << std::endl;
	}
	new_saved_experiments.close();
	std::cout << "Deleted experiment '" << experiment_title << "' successfully" << std::endl;
}
void experiment::update_experiment(experiment& input_experiment)
{
	// Create a new experiment to add the new data to the current data
	experiment new_experiment;
	new_experiment.set_title(experiment_title);
	experiment current_experiment{ *this };
	int new_number_of_rows{ number_of_rows + input_experiment.number_of_rows };
	std::vector<std::string> variable_names{ input_experiment.get_list_of_variable_names() };
	for (int i{ 0 }; i < number_of_variables; i++) {


		if (variable_types[i] == "double") {
			// Get current data
			std::string variable_name{ current_experiment[i].get_variable_name() };
			std::vector<double> variable_data{ string_to_double_vector(current_experiment[i].get_string_data()) };
			std::vector<double> variable_error_data;
			std::vector<std::string> variable_datestamp_data;

			// Get input data
			for (int k{ 0 }; k < input_experiment.number_of_rows; k++) {
				variable_data.push_back(string_to_double_vector(input_experiment[i].get_string_data())[k]);
			}
			// look for associated variables (error or datestamp)
			bool found_error_data{ false };
			bool found_datestamp_data{ false };
			for (int j{ 0 }; j < number_of_variables; j++) {
				if (variable_names[j].find(variable_names[i]) != std::string::npos) {

					// work out whether associated variable is an error or datestamp
					if (variable_names[j].find("Error") != std::string::npos) {
						// find out location of error and get the data
						std::vector<std::string>::iterator it = std::find(variable_names.begin(), variable_names.end(), variable_names[j]);
						int error_index = std::distance(variable_names.begin(), it);
						// find out how many error variables came before it
						int count_errors_before{ 0 };
						for (int k{ 0 }; k < error_index; k++) {
							if (variable_names[k].find("Error") != std::string::npos) count_errors_before++;
						}
						// Add both current and new errors
						std::vector<double> current_variable_error_data{ string_to_double_vector(current_experiment[j].get_string_data()) };
						for (size_t k{ 0 }; k < current_variable_error_data.size(); k++) variable_error_data.push_back(current_variable_error_data[k]);
						std::vector<double> new_variable_error_data{ string_to_double_vector(input_experiment[j].get_string_data()) };
						for (size_t k{ 0 }; k < new_variable_error_data.size(); k++) variable_error_data.push_back(new_variable_error_data[k]);
						found_error_data = true;
					} else if (variable_names[j].find("Datestamps") != std::string::npos) {
						// find out location of datestamp and get the data
						std::vector<std::string>::iterator it = std::find(variable_names.begin(), variable_names.end(), variable_names[j]);
						int datestamp_index = std::distance(variable_names.begin(), it);
						// find out how many datestamp variables came before it
						int count_datestamps_before{ 0 };
						for (int k{ 0 }; k < datestamp_index; k++) {
							if (variable_names[k].find("Datestamps") != std::string::npos) count_datestamps_before++;
						}
						// Add both current and new datestamps
						std::vector<std::string> current_variable_datestamp_data{ current_experiment[j].get_string_data() };
						for (size_t k{ 0 }; k < current_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(current_variable_datestamp_data[k]);
						std::vector<std::string> new_variable_datestamp_data{ input_experiment[j].get_string_data() };
						for (size_t k{ 0 }; k < new_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(new_variable_datestamp_data[k]);
						found_datestamp_data = true;
					}
				}
			}
			// If can't find error data, set errors to 0
			if (!found_error_data) {
				for (int k{ 0 }; k < new_number_of_rows; k++) variable_error_data.push_back(0);
			}
			// If can't find datestamp data, use the experiment datestamps
			if (!found_datestamp_data) {
				std::vector<std::string> current_variable_datestamp_data{ current_experiment[0].get_string_data() };
				for (size_t k{ 0 }; k < current_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(current_variable_datestamp_data[k]);
				std::vector<std::string> new_variable_datestamp_data{ input_experiment[0].get_string_data() };
				for (size_t k{ 0 }; k < new_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(new_variable_datestamp_data[k]);
			}
			// Create variable and add to experiment
			double_data temp_variable{ variable_name, variable_data, variable_error_data, variable_datestamp_data };
			new_experiment.add_variable(temp_variable);
		} else if (variable_types[i] == "nominal") {
			// Current data
			std::vector<std::string> variable_data{ current_experiment[i].get_string_data() };
			std::string variable_name{ input_experiment[i].get_variable_name() };
			std::vector<std::string> variable_datestamp_data;

			// Get input data
			for (int k{ 0 }; k < input_experiment.number_of_rows; k++) {
				variable_data.push_back(input_experiment[i].get_string_data()[k]);
			}

			// look for associated variables (datestamp)
			bool found_error_data{ false };
			bool found_datestamp_data{ false };
			for (int j{ 0 }; j < number_of_variables; j++) {
				if (variable_names[j].find(variable_names[i]) != std::string::npos) {

					if (variable_names[j].find("Datestamps") != std::string::npos) {
						// find out location of datestamp and get the data
						std::vector<std::string>::iterator it = std::find(variable_names.begin(), variable_names.end(), variable_names[j]);
						int datestamp_index = std::distance(variable_names.begin(), it);
						// find out how many datestamp variables came before it
						int count_datestamps_before{ 0 };
						for (int k{ 0 }; k < datestamp_index; k++) {
							if (variable_names[k].find("Datestamps") != std::string::npos) count_datestamps_before++;
						}
						// Add both current and new datestamps
						std::vector<std::string> current_variable_datestamp_data{ current_experiment[j].get_string_data() };
						for (size_t k{ 0 }; k < current_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(current_variable_datestamp_data[k]);
						std::vector<std::string> new_variable_datestamp_data{ input_experiment[j].get_string_data() };
						for (size_t k{ 0 }; k < new_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(new_variable_datestamp_data[k]);
						found_datestamp_data = true;
					}
				}
			}
			// If can't find datestamp data, use the experiment datestamps
			if (!found_datestamp_data) {
				std::vector<std::string> current_variable_datestamp_data{ current_experiment[0].get_string_data() };
				for (size_t k{ 0 }; k < current_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(current_variable_datestamp_data[k]);
				std::vector<std::string> new_variable_datestamp_data{ input_experiment[0].get_string_data() };
				for (size_t k{ 0 }; k < new_variable_datestamp_data.size(); k++) variable_datestamp_data.push_back(new_variable_datestamp_data[k]);
			}
			// Create variable and add to experiment
			nominal_data temp_variable{ variable_name, variable_data, variable_datestamp_data };
			new_experiment.add_variable(temp_variable);
		}
	}
	// Copy new experiment
	number_of_rows = new_experiment.get_number_of_rows();
	number_of_variables = new_experiment.get_number_of_variables();
	variable_types = new_experiment.get_variable_types();
	experiment_datestamps = new_experiment.get_datestamps();
	experiment_data.clear();
	experiment_data = new_experiment.get_experiment_data();
}
