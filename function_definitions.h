// header commetns

#ifndef FUNCTION_DEFINITIONS_H
#define FUNCTION_DEFINITIONS_H

std::string remove_zero_trail(std::string string_number);
std::string remove_trailing_spaces(std::string string_data);
bool is_data_point_a_number(std::string data_point);
bool is_data_point_a_datestamp(std::string data_point);
std::vector<std::string> datestamps_from_single_date(std::string date, size_t length);
std::vector<double> string_to_double_vector(std::vector<std::string> string_vector);
std::ostream& operator<<(std::ostream& os, const experiment& exp);
void print_file(std::string filename);
bool is_trigger_used(std::string input, std::string trigger_word);
std::string get_selected_experiment_ID(std::string input, std::string trigger);
bool check_selection_is_valid(std::string selected_experiment_number, std::vector<std::string> list_of_selections);
std::string choose_split(std::string string_to_split, char delimeter, int pos);
experiment get_experiment_from_save_file(std::string experiment_filename);
experiment get_experiment_from_csv(std::string experiment_filename);

#endif
