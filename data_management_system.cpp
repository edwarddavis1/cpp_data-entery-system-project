/* 
* Edward Davis 9937240
* Main script for the data management system, handling the running of the program and inputs
* Program gives the user many different quick commands to explore available experiments
* User can add to existing experiments or create new ones by either inputting data directly into the command line or through a csv file
* Any edited experiments are contained for the duration that the program runs for and saved each experiment when quit
* Last Modified: 01/05/2020
* Compiled using Visual Studio 2019
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

int main()
{
	// vector to contain any experiment edited during the program
	std::vector<experiment> edited_experiments;
	bool program_running{ true };
	bool program_shutting_down{ false };

	std::cout << "*** Welcome to the Edward Davis Data Management System ***\n" << std::endl;

	while (program_running) {
		std::cout << "List of quick commands\n"
			<< "  print ID:           returns a table showing the data from the selected experiment\n"
			<< "  summarise ID:       returns a statistical summary of the experiment data by variable\n"
			<< "  delete ID:          deletes the selected experiment - this cannot be undone!\n"
			<< "  saved experiments:  shows a list of available experiments with their ID number\n"
			<< "  select ID:          selects an experiment to input data\n"
			<< "  show edits:         displays experiments that have recently been edited\n"
			<< "  new:                creates a new experiment to input data\n"
			<< "  [filename.csv]      creates a new experiment from a csv file\n"
			<< "  x:                  exits the program and saves edited experiments"
			<< std::endl;
		// Search for previous saved experiments
		std::ifstream saved_experiments("list_of_experimentfiles.txt");
		if (!saved_experiments.good()) {
			std::cerr << "ERROR: File could not be opened" << std::endl;
			noexcept("list_of_experimentfiles.txt could not be found");
		}
		// Output list of available experiments with a file ID
		std::string filename;
		int file_ID{ 0 };
		std::vector<std::string> list_of_selections;
		std::vector<std::string> list_of_available_experiments;
		std::cout << "\nList of Available Experiments: " << std::endl;
		while (getline(saved_experiments, filename))
		{
			file_ID++;
			list_of_selections.push_back(std::to_string(file_ID));
			list_of_available_experiments.push_back(filename);
			std::cout << "\t(" << file_ID << ") " << filename << std::endl;
		}
		saved_experiments.close();
		std::cout << std::endl;
		// For each input quick command recognised, check file ID is valid and perform required action
		experiment exp_for_input;
		bool getting_input{ true };
		while (getting_input)
		{
			std::string input;
			getline(std::cin, input);
			if (is_trigger_used(input, "print")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "print") };
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					experiment selected_experiment(list_of_available_experiments[stoi(selected_experiment_number) - 1] + ".txt");
					std::cout << selected_experiment << std::endl;

				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (is_trigger_used(input, "summarise")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "summarise") };
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					experiment selected_experiment(list_of_available_experiments[stoi(selected_experiment_number) - 1] + ".txt");
					selected_experiment.summary();

				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (is_trigger_used(input, "delete")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "delete") };
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };
					// Delete experiment then update and output the new available experiments list
					experiment selected_experiment(list_of_available_experiments[stoi(selected_experiment_number) - 1] + ".txt");
					selected_experiment.delete_experiment();
					std::ifstream saved_experiments("list_of_experimentfiles.txt");
					if (!saved_experiments.good()) {
						std::cerr << "ERROR: File could not be opened" << std::endl;
						noexcept("list_of_experimentfiles.txt could not be found");
					} else {
						file_ID = 0;
						list_of_selections.clear();
						list_of_available_experiments.clear();
						while (getline(saved_experiments, filename)) {
							file_ID++;
							list_of_selections.push_back(std::to_string(file_ID));
							list_of_available_experiments.push_back(filename);
							std::cout << "\t(" << file_ID << ") " << filename << std::endl;
						}
						saved_experiments.close();
						std::cout << std::endl;
					}
				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (input == "saved experiments") {
				// Search for previous saved experiments and output found files with file ID
				std::ifstream saved_experiments("list_of_experimentfiles.txt");
				if (!saved_experiments.good()) {
					std::cerr << "ERROR: File could not be opened" << std::endl;
					noexcept("list_of_experimentfiles.txt could not be found");
				} else {
					file_ID = 0;
					list_of_selections.clear();
					list_of_available_experiments.clear();
					while (getline(saved_experiments, filename))
					{
						file_ID++;
						list_of_selections.push_back(std::to_string(file_ID));
						list_of_available_experiments.push_back(filename);
						std::cout << "\t(" << file_ID << ") " << filename << std::endl;
					}
					saved_experiments.close();
					std::cout << std::endl;
				}
			} else if (is_trigger_used(input, "select")) {
				// Select a file based on ID and create a corresponding experiment object for input
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "select") };
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					experiment selected_experiment(list_of_available_experiments[selected_experiment_index] + ".txt");
					exp_for_input = selected_experiment;
					getting_input = false;

				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (input == "show edits") {
				if (edited_experiments.size() == 0) std::cout << "No recently edited experiments" << std::endl;
				// Output all experiments in the edited_experiments vector using a lambda function
				for_each(edited_experiments.begin(), edited_experiments.end(), [](experiment exp)
				{
					std::cout << exp << std::endl;
				});
			} else if (input == "help") {
				std::cout << "List of quick commands\n"
					<< "  print ID:           returns a table showing the data from the selected experiment\n"
					<< "  summarise ID:       returns a statistical summary of the experiment data by variable\n"
					<< "  delete ID:          deletes the selected experiment - this cannot be undone!\n"
					<< "  saved experiments:  shows a list of available experiments with their ID number\n"
					<< "  select ID:          selects an experiment to input data\n"
					<< "  show edits:         displays experiments that have recently been edited\n"
					<< "  [filename.csv]      creates a new experiment from a csv file\n"
					<< "  new:                creates a new experiment to input data\n"
					<< "  x:                  exits the program and saves edited experiments"
					<< std::endl;
			} else if (input.find(".csv") != std::string::npos) {
				// If a *.csv is input, create an object from that file
				experiment exp_csv{ input };
				std::cout << "File found, please name your experiment" << std::endl;
				bool title_accepted{ false };
				while (!title_accepted) {
					std::string csv_exp_title;
					getline(std::cin, csv_exp_title);
					if (csv_exp_title.find("|") != std::string::npos) {
						std::cout << "ERROR: Please do not use '|' in your title" << std::endl;
					} else if (remove_trailing_spaces(csv_exp_title).empty()) {
						std::cout << "ERROR: Please enter a title" << std::endl;
					} else {
						title_accepted = true;
						exp_csv.set_title(csv_exp_title);
					}
				}
				// User can either save the *.csv as a new experiment or add to the experiment
				std::cout << exp_csv << std::endl;
				std::cout << "\nWould you like to [save] or [select] this file?" << std::endl;
				bool csv_choice_accepted{ false };
				bool save_csv{ false };
				bool select_csv{ false };
				while (!csv_choice_accepted) {
					std::string csv_choice;
					getline(std::cin, csv_choice);
					if (csv_choice == "save") {
						save_csv = true;
						csv_choice_accepted = true;
					} else if (csv_choice == "select") {
						select_csv = true;
						csv_choice_accepted = true;
					} else {
						std::cout << "ERROR: Please type either [save] or [select]" << std::endl;
					}
				}
				if (save_csv) {
					exp_csv.save_experiment();
					edited_experiments.push_back(exp_csv);
				}
				else if (select_csv) {
					exp_for_input = exp_csv;
					getting_input = false;
				}
			} else if (input == "new") {
				// Create a new experiment through manual input
				experiment new_exp;
				bool title_accepted{ false };
				// Name the experiment
				while (!title_accepted) {
					std::cout << "Experiment title: ";
					std::string exp_title;
					getline(std::cin, exp_title);
					if (exp_title.find("|") != std::string::npos) {
						std::cout << "ERROR: Please do not use '|' in your title" << std::endl;
					} else if (remove_trailing_spaces(exp_title).empty()) {
						std::cout << "ERROR: Please enter a title" << std::endl;
					} else {
						title_accepted = true;
						new_exp.set_title(exp_title);
					}
				}
				// Create variables
				std::vector<std::string> new_variable_names;
				bool adding_variables{ true };
				int number_of_new_variables{ 0 };
				std::cout << "\nEnter the names of your variables (type 'x' when done):" << std::endl;
				while (adding_variables) {
					// Name the variable
					std::string variable_name;
					bool variable_name_accepted{ false };
					while (!variable_name_accepted) {
						std::cout << "Variable name " << number_of_new_variables + 1 << " : ";
						getline(std::cin, variable_name);
						if (variable_name.find("|") != std::string::npos) {
							std::cout << "ERROR: Please do not use '|' in your name" << std::endl;
						} else if (remove_trailing_spaces(variable_name).empty()) {
							std::cout << "ERROR: Please enter a variable name" << std::endl;
						} else if (variable_name == "x") {
							adding_variables = false;
							break;
						} else {
							variable_name_accepted = true;
							new_variable_names.push_back(variable_name);
							number_of_new_variables++;
						}
					}

				}
				// Enter the date
				std::string datestamp;
				bool datestamp_accepted{ false };
				while (!datestamp_accepted) {
					std::cout << "\nEnter the date (dd/mm/yyyy): ";
					getline(std::cin, datestamp);
					if (!is_data_point_a_datestamp(datestamp)) std::cout << "ERROR: Please enter a date in the format dd/mm/yyyy" << std::endl;
					else datestamp_accepted = true;
				}
				// Enter variable data row by row
				std::cout << "\nEnter the data row by row" << std::endl;
				int row_number{ 1 };
				std::vector<std::vector<std::string>> string_new_variable_data(new_variable_names.size());
				bool getting_data{ true };
				while (getting_data) {
					for (size_t i{ 0 }; i < new_variable_names.size(); i++) {
						bool data_point_accepted{ false };
						while (!data_point_accepted) {
							std::cout << new_variable_names[i] << "[" << row_number << "]: ";
							std::string string_data_point;
							getline(std::cin, string_data_point);
							if (string_data_point.find("|") != std::string::npos) {
								std::cout << "ERROR: Please do not use '|' in data point" << std::endl;
							} else if (remove_trailing_spaces(string_data_point).empty()) {
								std::cout << "ERROR: Please enter a data point" << std::endl;
							} else {
								data_point_accepted = true;
								string_new_variable_data[i].push_back(string_data_point);
							}
						}
					}
					std::cout << "Enter another row? [y/n]" << std::endl;
					bool new_row_choice_accepted{ false };
					while (!new_row_choice_accepted) {
						std::string new_row_choice;
						getline(std::cin, new_row_choice);
						if (new_row_choice == "y") {
							new_row_choice_accepted = true;
						} else if (new_row_choice == "n") {
							new_row_choice_accepted = true;
							getting_data = false;
						} else std::cout << "ERROR: Please enter 'y' or 'n'" << std::endl;
					}
					row_number++;
				}
				// Check if data is double or nominal for variable object creation
				std::vector<std::string> variable_data_types;
				size_t number_of_double_variables{ string_new_variable_data.size() };
				for (size_t i{ 0 }; i < new_variable_names.size(); i++) {
					bool found_string{ false };
					for (size_t j{ 0 }; j < string_new_variable_data[0].size(); j++) {
						if (!is_data_point_a_number(string_new_variable_data[i][j])) {
							found_string = true;
							break;
						}
					}
					std::string type{ "double" };
					if (found_string) {
						type = "nominal";
						number_of_double_variables--;
					}
					variable_data_types.push_back(type);
				}
				// Convert any numierical data from string to double
				std::vector<std::vector<double>> double_new_variable_data(number_of_double_variables);
				int index{ 0 };
				for (size_t i{ 0 }; i < variable_data_types.size(); i++) {
					if (variable_data_types[i] == "double") {
						for (size_t j{ 0 }; j < string_new_variable_data[i].size(); j++) {
							double_new_variable_data[index].push_back(stod(string_new_variable_data[i][j]));
						}
						index++;
					}
				}
				// State variables with errors
				std::cout << "\nType the name of any variables with predetermined errors, separated by commas\nType [skip] to skip." << std::endl;
				bool accepted_variable_choices{ false };
				std::vector<std::string> variables_with_predetermined_errors;
				while (!accepted_variable_choices) {
					std::string input_list_of_variables;
					getline(std::cin, input_list_of_variables);
					if (remove_trailing_spaces(input_list_of_variables).empty()) {
						std::cout << "Please enter an input" << std::endl;
						continue;
					} else if (remove_trailing_spaces(input_list_of_variables) == "skip") {
						accepted_variable_choices = true;
						break;
					}
					// Count the number of selected variables
					int number_of_choices{ 1 };
					for (size_t i{ 0 }; i < input_list_of_variables.length(); i++) {
						if (input_list_of_variables[i] == ',') number_of_choices++;
					}
					// Check, then get the selected variables
					bool any_bad_choices{ false };
					std::vector<std::string> temp_variables_with_predetermined_errors;
					for (int i{ 0 }; i < number_of_choices; i++) {
						std::string choice{ choose_split(input_list_of_variables, ',', i) };
						bool variable_okay{ false };
						// Check variable exists
						for (size_t j{ 0 }; j < new_variable_names.size(); j++) {
							if (choice == new_variable_names[j]) {
								// Check variable is a double
								if (variable_data_types[j] == "double") variable_okay = true;
							}
						}

						if (variable_okay)	temp_variables_with_predetermined_errors.push_back(choice);
						else {
							std::cout << choice << " was not one of the variable names you entered, please try again" << std::endl;
							any_bad_choices = true;
							break;
						}
					}
					if (!any_bad_choices) {
						variables_with_predetermined_errors = temp_variables_with_predetermined_errors;
						accepted_variable_choices = true;
					}
				}
				// Enter predetermined errors
				std::vector<std::vector<double>> predetermined_errors(variables_with_predetermined_errors.size());
				if (variables_with_predetermined_errors.size() != 0) {
					std::cout << "Enter the predetermined error for each variable" << std::endl;
					for (size_t j{ 0 }; j < string_new_variable_data[0].size(); j++) {
						for (size_t i{ 0 }; i < variables_with_predetermined_errors.size(); i++) {
							bool error_accepted{ false };
							while (!error_accepted) {
								// Get the entered value
								int variable_index{ 0 };
								for (size_t k{ 0 }; k < new_variable_names.size(); k++) {
									if (variables_with_predetermined_errors[i] == new_variable_names[k]) variable_index = k;
								}
								std::cout << variables_with_predetermined_errors[i] << "[" << j + 1 << "] (value = " << string_new_variable_data[variable_index][j] << "): ";
								std::string error;
								getline(std::cin, error);
								// Check error is numerical
								if (is_data_point_a_number(error)) {
									predetermined_errors[i].push_back(stod(error));
									error_accepted = true;
								} else std::cout << "ERROR: Please enter a number" << std::endl;
							}
						}
					}
				}
				// Give user the option to use automatic errors for the non-predetermined errors
				bool use_auto_errors{ false };
				if (static_cast<int>(variables_with_predetermined_errors.size()) != number_of_double_variables) {
					std::cout << "\nWould you like to use automatic errors for the variables without predetermined errors? [y/n]" << std::endl;
					bool accepted_choice{ false };
					std::string auto_error_choice;
					while (!accepted_choice) {
						getline(std::cin, auto_error_choice);
						if (auto_error_choice == "y") {
							accepted_choice = true;
							use_auto_errors = true;
						} else if (auto_error_choice == "n") {
							accepted_choice = true;
							use_auto_errors = false;
						} else std::cerr << "ERROR: Please enter 'y' to use automatic errors or 'n' to include no error variables without predetermined errors" << std::endl;
					}
				}
				// Create variable objects and add to experiment
				int double_index{ 0 };
				for (size_t i{ 0 }; i < variable_data_types.size(); i++) {
					std::string type{ variable_data_types[i] };
					if (type == "double") {
						// Check if it has errors
						std::vector<double> variable_errors;
						bool has_errors{ false };
						for (size_t j{ 0 }; j < variables_with_predetermined_errors.size(); j++) {
							if (new_variable_names[i] == variables_with_predetermined_errors[j]) {
								has_errors = true;
								variable_errors = predetermined_errors[j];
							}
						}
						// If it has no errors, set all to zero
						if (!has_errors) {
							for (size_t j{ 0 }; j < string_new_variable_data[i].size(); j++) {
								variable_errors.push_back(0);
							}
						}
						// Create variable and add to experiment
						double_data temp_variable{ new_variable_names[i], double_new_variable_data[double_index], variable_errors, datestamps_from_single_date(datestamp, double_new_variable_data[double_index].size())};
						try {
							new_exp.add_variable(temp_variable, false, false, use_auto_errors);
						} catch (int error_flag) {
							if (error_flag == bad_variable_size_flag) {
								std::cerr << temp_variable.get_variable_name() << " is not the same length as variables in " << new_exp.get_title() << std::endl;
								exit(error_flag);
							}
						}
						double_index++;
					} else if (type == "nominal") {
						// Create variable and add to experiment
						nominal_data temp_variable{new_variable_names[i], string_new_variable_data[i], datestamps_from_single_date(datestamp, string_new_variable_data[i].size()) };
						try {
							new_exp.add_variable(temp_variable);
						}
						catch (int error_flag) {
							if (error_flag == bad_variable_size_flag) {
								std::cerr << temp_variable.get_variable_name() << " is not the same length as variables in " << new_exp.get_title() << std::endl;
								exit(error_flag);
							}
						}
					}
				}

				// Show experiment
				std::cout << "\nHere is your experiment\n" << std::endl;
				std::cout << new_exp << std::endl;
				new_exp.summary();
				new_exp.save_experiment();

				// Add to container
				edited_experiments.push_back(new_exp);
				
			} else if (input == "x") {
				getting_input = false;
				program_running = false;
				program_shutting_down = true;
				break;
			} else {
				// If command not recognised, ask for another input
				bool selection_valid{ false };
				for (size_t i{ 0 }; i < list_of_selections.size(); i++)
				{
					if (input == list_of_selections[i]) selection_valid = true;
				}
				if (selection_valid) std::cout << "Please use one of the quick commands with the data (type [help] to see list of commands)" << std::endl;
				else std::cout << "I'm sorry, '" << input << "' is not a valid input" << std::endl;
			}
		}
		if (program_shutting_down) break;

		// Input data for selected experiment
		std::cout << exp_for_input.get_title() << " so far..." << std::endl;
		std::cout << exp_for_input << std::endl;
		std::cout << "Inputting data for " << exp_for_input.get_title() << "..." << std::endl;
		std::vector<std::string> variable_names_for_input{ exp_for_input.get_list_of_variable_names() };
		std::vector<std::string> variable_types_for_input{ exp_for_input.get_variable_types() };
		int input_row{ exp_for_input.get_number_of_rows() + 1 };
		bool inputting_data{ true };
		int number_of_new_rows{ 0 };
		// Find out how many of each variable are in the select experiment
		int num_doubles{ 0 };
		int num_nominals{ 0 };
		int num_errors{ 0 };
		int num_datestamps{ 0 };
		for (size_t i{ 0 }; i < variable_types_for_input.size(); i++) {
			if (variable_types_for_input[i] == "double") num_doubles++;
			else if (variable_types_for_input[i] == "nominal") num_nominals++;
			else if (variable_types_for_input[i] == "error") num_errors++;
			else if (variable_types_for_input[i] == "datestamp") num_datestamps++;
		}
		// Containers for input data: vectors of variable data
		std::vector<std::vector<double>> double_input_data(num_doubles);
		std::vector < std::vector<double>> error_input_data(num_errors);
		std::vector < std::vector<std::string>> nominal_input_data(num_nominals);
		std::vector < std::vector<std::string>> variable_datestamp_input_data(num_datestamps);
		std::vector<std::string> experiment_datestamp_input_data;
		while (inputting_data)
		{
			int double_count{ 0 };
			int error_count{ 0 };
			int nominal_count{ 0 };
			int datestamp_count{ 0 };
			// Input the data for each column row by row
			for (int input_col{ 0 }; input_col < exp_for_input.get_number_of_variables(); input_col++) {
				// Check inputs depending on variable type
				bool input_accepted{ false };
				while (!input_accepted) {
					std::cout << variable_names_for_input[input_col] << "[" << input_row << "]: ";
					std::string input_data_point;
					getline(std::cin, input_data_point);
					std::cout << std::endl;
					if (variable_types_for_input[input_col] == "double") {
						if (!is_data_point_a_number(input_data_point)) {
							std::cerr << "ERROR: Variable only accepts numerical data" << std::endl;
						} else {
							// Add double to data
							input_accepted = true;
							double_input_data[double_count].push_back(stod(input_data_point));
							double_count++;
						}
					} else if (variable_types_for_input[input_col] == "error") {
						if (!is_data_point_a_number(input_data_point)) {
							std::cerr << "ERROR: Variable only accepts numerical data" << std::endl;
						} else {
							// Add double to data
							input_accepted = true;
							error_input_data[error_count].push_back(stod(input_data_point));
							error_count++;
						}
					} else if (variable_types_for_input[input_col] == "datestamp") {
						if (!is_data_point_a_datestamp(input_data_point)) {
							std::cerr << "ERROR: Variable only accepts dates with format dd/mm/yyyy" << std::endl;
						} else {
							// Add datestamp to data
							input_accepted = true;
							// Update experiment datestamps if appropriate
							if (variable_names_for_input[input_col] == "Datestamps") {
								experiment_datestamp_input_data.push_back(input_data_point);
							}
							variable_datestamp_input_data[datestamp_count].push_back(input_data_point);
							datestamp_count++;
						}
					} else if (variable_types_for_input[input_col] == "nominal") {
						input_accepted = true;
						nominal_input_data[nominal_count].push_back(input_data_point);
						nominal_count++;
					}
				}

			}
			number_of_new_rows++;
			std::cout << "Enter another row? [y/n]" << std::endl;
			bool accepted_choice{ false };
			std::string row_choice;
			while (!accepted_choice) {
				getline(std::cin, row_choice);
				if (row_choice == "y") {
					accepted_choice = true;
					input_row++;
				} else if (row_choice == "n") {
					accepted_choice = true;
					inputting_data = false;
				} else std::cerr << "ERROR: Please enter 'y' to enter a new row or 'n' to stop data input" << std::endl;
			}
		}
		std::cout << "Adding data to " << exp_for_input.get_title() << "..." << std::endl;
		// Create a temporary experiment with new variable data
		int double_index{ 0 };
		int nominal_index{ 0 };
		experiment temp_experiment;
		temp_experiment.set_title(exp_for_input.get_title());
		for (size_t i{ 0 }; i < variable_types_for_input.size(); i++) {
			std::string type{ variable_types_for_input[i] };
			// Make double variables
			if (type == "double") {
				// Initialise variable member data
				std::string variable_name{ variable_names_for_input[i] };
				std::vector<double> variable_data;
				std::vector<double> variable_error_data;
				std::vector<std::string> variable_datestamp_data;
				bool found_error_data{ false };
				bool found_datestamp_data{ false };
				// get the variable data
				variable_data = double_input_data[double_index];
				// look for associated variables (error or datestamp)
				for (size_t j{ 0 }; j < variable_names_for_input.size(); j++) {
					if (variable_names_for_input[j].find(variable_name) != std::string::npos) {

						// work out whether associated variable is an error or datestamp
						if (variable_names_for_input[j].find("Error") != std::string::npos) {
							// find out location of error and get the data
							std::vector<std::string>::iterator it = std::find(variable_names_for_input.begin(), variable_names_for_input.end(), variable_names_for_input[j]);
							int error_index = std::distance(variable_names_for_input.begin(), it);
							// find out how many error variables came before it
							int count_errors_before{ 0 };
							for (int k{ 0 }; k < error_index; k++) {
								if (variable_names_for_input[k].find("Error") != std::string::npos) count_errors_before++;
							}
							variable_error_data = error_input_data[count_errors_before];
							found_error_data = true;
						} else if (variable_names_for_input[j].find("Datestamps") != std::string::npos) {
							// find out location of datestamp and get the data
							std::vector<std::string>::iterator it = std::find(variable_names_for_input.begin(), variable_names_for_input.end(), variable_names_for_input[j]);
							int datestamp_index = std::distance(variable_names_for_input.begin(), it);
							// find out how many datestamp variables came before it
							int count_datestamps_before{ 0 };
							for (int k{ 0 }; k < datestamp_index; k++) {
								if (variable_names_for_input[k].find("Datestamps") != std::string::npos) count_datestamps_before++;
							}
							variable_datestamp_data = variable_datestamp_input_data[count_datestamps_before];
							found_datestamp_data = true;
						}
					}
				}
				// If can't find error data, set errors to 0
				bool force_include_error{ false };
				std::vector<double> zeroes_list;
				for (int k{ 0 }; k < number_of_new_rows; k++) zeroes_list.push_back(0);
				if (!found_error_data) {
					variable_error_data = zeroes_list;
				}
				if (found_error_data && variable_error_data == zeroes_list) force_include_error = true;
				// If can't find datestamp data, use the experiment datestamps
				bool force_include_datestamp{ false };
				if (!found_datestamp_data) {
					variable_datestamp_data = experiment_datestamp_input_data;
				}
				if (found_datestamp_data && variable_datestamp_data == experiment_datestamp_input_data) {
					force_include_datestamp = true;
				}
				// Create variable and add to experiment
				double_data temp_variable{ variable_name, variable_data, variable_error_data, variable_datestamp_data };
				try {
					temp_experiment.add_variable(temp_variable, force_include_datestamp = force_include_datestamp, force_include_error = force_include_error);
				} catch (int error_flag) {
					if (error_flag == bad_variable_size_flag) {
						std::cerr << temp_variable.get_variable_name() << " is not the same length as variables in " << temp_experiment.get_title() << std::endl;
						exit(error_flag);
					}
				}
				double_index++;
			} else if (type == "nominal") {
				// Make nominal variables
				// Initialise variable member data
				std::string variable_name{ variable_names_for_input[i] };
				std::vector<std::string> variable_data;
				std::vector<std::string> variable_datestamp_data;
				bool found_datestamp_data{ false };
				// get the variable data
				variable_data = nominal_input_data[nominal_index];
				// look for associated variables (only datestamps)
				for (size_t j{ 0 }; j < variable_names_for_input.size(); j++) {
					if (variable_names_for_input[j].find(variable_name) != std::string::npos && variable_names_for_input[j] != variable_name) {
						// find out location of datestamp and get the data
						std::vector<std::string>::iterator it = std::find(variable_names_for_input.begin(), variable_names_for_input.end(), variable_names_for_input[j]);
						int datestamp_index = std::distance(variable_names_for_input.begin(), it);
						// find out how many datestamp variables came before it
						int count_datestamps_before{ 0 };
						for (int k{ 0 }; k < datestamp_index; k++) {
							if (variable_names_for_input[k].find("Datestamps") != std::string::npos) count_datestamps_before++;
						}
						variable_datestamp_data = variable_datestamp_input_data[count_datestamps_before];
						found_datestamp_data = true;
					}
				}
				// If can't find datestamp data, use the experiment datestamps
				bool force_include_datestamp{ false };
				if (!found_datestamp_data) {
					variable_datestamp_data = experiment_datestamp_input_data;
				}
				if (found_datestamp_data && variable_datestamp_data == experiment_datestamp_input_data) {
					force_include_datestamp = true;
				}
				// Create variable and add to experiment
				nominal_data temp_variable{ variable_name, variable_data, variable_datestamp_data };
				try {
					temp_experiment.add_variable(temp_variable, force_include_datestamp = force_include_datestamp);
				} catch (int error_flag) {
					if (error_flag == bad_variable_size_flag) {
						std::cerr << temp_variable.get_variable_name() << " is not the same length as variables in " << temp_experiment.get_title() << std::endl;
						exit(error_flag);
					}
				}
				nominal_index++;
			}
		}
		// Summarise the input data
		std::cout << "Data input stopped." << std::endl << "Here is the data that was added:" << std::endl
			<< temp_experiment << std::endl;
		std::cout << "Previous Experiment Summary:" << std::endl;
		exp_for_input.summary();
		std::cout << "Input Summary:" << std::endl;
		temp_experiment.summary();

		// Add inputs to select experiment
		bool add_new_data_choice_accepted{ false };
		bool add_new_data;
		std::cout << "\n\nWould you like to add this new data? [y/n]" << std::endl;
		while (!add_new_data_choice_accepted) {
			std::string add_new_data_choice;
			getline(std::cin, add_new_data_choice);
			if (add_new_data_choice == "y") {
				add_new_data_choice_accepted = true;
				add_new_data = true;
			} else if (add_new_data_choice == "n") {
				add_new_data_choice_accepted = true;
				add_new_data = false;
			} else std::cerr << "ERROR: Please enter 'y' to add the new data or 'n' to abort" << std::endl;
		}
		if (add_new_data) {
			std::cout << "Adding new input to " << exp_for_input.get_title() << std::endl;
			exp_for_input.update_experiment(temp_experiment);
			std::cout << exp_for_input << std::endl;
			// Add experiment to container
			edited_experiments.push_back(exp_for_input);
		} else {
			std::cout << "New input aborted" << std::endl;
		}
	}

	// Save all experiments in container
	std::cout << "Exiting...saving experiment changes" << std::endl;
	for (size_t i{ 0 }; i < edited_experiments.size(); i++) {
		edited_experiments[i].save_experiment();
	}

	return 0;
}
