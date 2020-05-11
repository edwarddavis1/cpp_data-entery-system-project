/*
* Experiment Class
* An experiment class contains multiple variable objects like columns in a table
* An experiment also has associated datestamps and holds the types of variable in the data (nominal, double, error or datestamp)
* Last modified: 01/05/2020
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

experiment::experiment(nominal_data variable, std::string title_of_experiment)
{
	// Constructor from existing nominal variable
	number_of_rows = variable.get_length();
	number_of_variables = 2;
	experiment_title = title_of_experiment;
	// Add datestamp
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
	// Add datestamp
	experiment_data.emplace_back(new nominal_data("Datestamps", variable.get_datestamps(), variable.get_datestamps()));
	variable_types.push_back("datestamp");
	experiment_datestamps = variable.get_datestamps();
	experiment_data.emplace_back(new double_data(variable.get_variable_name(), variable.get_data(), variable.get_datestamps()));
	variable_types.push_back("double");
	// Add error variable if non-zero
	std::vector<double> variable_error_data{ variable.get_error_data() };
	bool non_zero_error{ is_non_zero_error(variable_error_data) };
	if (non_zero_error) {
		experiment_data.emplace_back(new double_data(variable.get_variable_name() + " Error", variable.get_error_data(), variable.get_datestamps()));
		variable_types.push_back("error");
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
		std::cerr << "ERROR: File unavailable or not supported; please use a (specified format) .txt or a (general) .csv file" << std::endl;
		noexcept("File could not be opened");
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
	std::vector<std::string> measures{ "Count", "Mode", "Unique", "Mean", "Std", "Error", "Min", "25%", "50%", "75%", "Max" };
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
void experiment::save_to_csv() const
{
	// save file
	std::ofstream outfile(experiment_title + ".csv");
	outfile << this->get_title() << std::endl;
	// enter the variable names as headings
	for (size_t i{ 0 }; i < experiment_data.size() - 1; i++) {
		outfile << experiment_data[i]->get_variable_name() << ",";
	}
	// last variable name
	outfile << experiment_data[experiment_data.size() - 1]->get_variable_name() << std::endl;
	// enter the data
	for (int j{ 0 }; j < number_of_rows; j++) {
		for (size_t i{ 0 }; i < experiment_data.size() - 1; i++) {
			std::string data_point{ experiment_data[i]->get_string_data()[j] };
			outfile << data_point << ",";
		}
		// last data point of each row
		std::string last_point_of_row{ experiment_data[experiment_data.size() - 1]->get_string_data()[j] };
		outfile << last_point_of_row << std::endl;
	}
	outfile.close();
	std::cout << "saved experiment: " << experiment_title << " as " << experiment_title << ".csv" << std::endl;
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
