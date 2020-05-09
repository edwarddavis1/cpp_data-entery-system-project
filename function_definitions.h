/*
* General Function Definitions Header
* Contains declares general use functions.
* Last Modified: 01/05/2020
*/
#ifndef FUNCTION_DEFINITIONS_H
#define FUNCTION_DEFINITIONS_H

// Checks an error vector for any non zero values
bool is_non_zero_error(std::vector<double> variable_error_data);
// Removes zeros from the end of a string number
std::string remove_zero_trail(std::string string_number);
std::string remove_trailing_spaces(std::string string_data);
bool is_data_point_a_number(std::string data_point);
bool is_data_point_a_datestamp(std::string data_point);
std::vector<std::string> datestamps_from_single_date(std::string date, size_t length);
// Converts a vector string of number to a vector double
std::vector<double> string_to_double_vector(std::vector<std::string> string_vector);
// Format ASCII data tables from experiment object
std::string format_variables(const experiment& exp);
std::ostream& operator<<(std::ostream& os, const experiment& exp);
// Checks for a quick command trigger word in an input line
bool is_trigger_used(std::string input, std::string trigger_word);
// Extract an experiment file ID substring from input
std::string get_selected_experiment_ID(std::string input, std::string trigger);
// Check that a selected file ID is associated with an existing file
bool check_selection_is_valid(std::string selected_experiment_number, std::vector<std::string> list_of_selections);
// Extract a substring of a given position from a delimiter separated list
std::string choose_split(std::string string_to_split, char delimeter, int pos);
// Creates an experiment object from a previously saved .txt file
experiment get_experiment_from_save_file(std::string experiment_filename);
// Creates an experiment object from a general .csv file
experiment get_experiment_from_csv(std::string experiment_filename);
template <class c_type> std::vector<c_type> get_mode_averages(std::vector<c_type> data)
{
	// Count occurances of points
	std::vector<int> counts;
	for (size_t i{ 0 }; i < data.size(); i++) {
		counts.push_back(std::count(data.begin(), data.end(), data[i]));
	}
	// Get max count
	int max_count = *max_element(counts.begin(), counts.end());
	std::vector<c_type> most_occuring_points;
	for (size_t i{ 0 }; i < counts.size(); i++) {
		if (counts[i] == max_count) most_occuring_points.push_back(data[i]);
	}
	// Remove duplicates in most occuring data points
	std::vector<c_type> mode_points;
	for (size_t i{ 0 }; i < most_occuring_points.size(); i++) {
		if (i == 0) {
			mode_points.push_back(most_occuring_points[i]);
		} else {
			// if unique add to mode list
			if (std::find(mode_points.begin(), mode_points.end(), most_occuring_points[i]) == mode_points.end()) {
				mode_points.push_back(most_occuring_points[i]);
			}
		}
	}
	return mode_points;
}
// Returns a list of unique value from a vector
template <class c_type> std::vector<c_type> get_unique(std::vector<c_type> data)
{
	// Get the unique values of a vector
	std::vector<c_type> unique_values{ data };
	sort(unique_values.begin(), unique_values.end());
	unique_values.erase(unique(unique_values.begin(), unique_values.end()), unique_values.end());
	return unique_values;
}

#endif
