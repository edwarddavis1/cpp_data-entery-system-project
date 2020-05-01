// Edward Davis 9937240
// Last modified: 
// 
// Compiled using Visual Studio 2017

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
			<< "  new:                creates a new experiment to input data\n"
			<< "  [filename.csv]      creates a new experiment from a csv file\n"
			<< "  x:                  exits the program and saves edited experiments"
			<< std::endl;
		// Search for previous saved experiments
		std::ifstream saved_experiments("list_of_experimentfiles.txt");

		if (!saved_experiments.good()) {
			std::cerr << "ERROR: File could not be opened" << std::endl;
			std::cin.get();
			return(1);
		}
		std::string filename;
		int file_number{ 0 };
		std::vector<std::string> list_of_selections;
		std::vector<std::string> list_of_available_experiments;
		std::cout << "\nList of Available Experiments: " << std::endl;
		while (getline(saved_experiments, filename))
		{
			file_number++;
			list_of_selections.push_back(std::to_string(file_number));
			list_of_available_experiments.push_back(filename);
			std::cout << "\t(" << file_number << ") " << filename << std::endl;
		}
		saved_experiments.close();
		std::cout << std::endl;

		// experiment object for the selected experiment object
		experiment exp_for_input;

		// initial inputs - for quick access to saved data
		bool getting_input{ true };
		while (getting_input)
		{
			std::string input;
			getline(std::cin, input);
			if (is_trigger_used(input, "print")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "print") };
				// check selection is valid
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					experiment selected_experiment(list_of_available_experiments[stoi(selected_experiment_number) - 1] + ".txt");
					std::cout << selected_experiment << std::endl;

				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (is_trigger_used(input, "summarise")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "summarise") };
				// check selection is valid
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					experiment selected_experiment(list_of_available_experiments[stoi(selected_experiment_number) - 1] + ".txt");
					selected_experiment.summary();

				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (is_trigger_used(input, "delete")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "delete") };
				// check selection is valid
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					// delete experiment
					experiment selected_experiment(list_of_available_experiments[stoi(selected_experiment_number) - 1] + ".txt");
					selected_experiment.delete_experiment();

					// update the available experiments list
					std::ifstream saved_experiments("list_of_experimentfiles.txt");

					if (!saved_experiments.good()) {
						std::cerr << "ERROR: File could not be opened" << std::endl;
					} else {
						file_number = 0;
						list_of_selections.clear();
						list_of_available_experiments.clear();
						while (getline(saved_experiments, filename)) {
							file_number++;
							list_of_selections.push_back(std::to_string(file_number));
							list_of_available_experiments.push_back(filename);
							std::cout << "\t(" << file_number << ") " << filename << std::endl;
						}
						saved_experiments.close();
						std::cout << std::endl;
					}
				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (input == "saved experiments") {
				// Search for previous saved experiments
				std::ifstream saved_experiments("list_of_experimentfiles.txt");

				if (!saved_experiments.good()) {
					std::cerr << "ERROR: File could not be opened" << std::endl;
				} else {
					file_number = 0;
					list_of_selections.clear();
					list_of_available_experiments.clear();
					while (getline(saved_experiments, filename))
					{
						file_number++;
						list_of_selections.push_back(std::to_string(file_number));
						list_of_available_experiments.push_back(filename);
						std::cout << "\t(" << file_number << ") " << filename << std::endl;
					}
					saved_experiments.close();
					std::cout << std::endl;
				}
			} else if (is_trigger_used(input, "select")) {
				std::string selected_experiment_number{ get_selected_experiment_ID(input, "select") };
				// check selection is valid
				bool selection_valid{ check_selection_is_valid(selected_experiment_number, list_of_selections) };
				if (selection_valid) {
					int selected_experiment_number_int{ stoi(selected_experiment_number) };
					int selected_experiment_index{ selected_experiment_number_int - 1 };

					experiment selected_experiment(list_of_available_experiments[selected_experiment_index] + ".txt");
					exp_for_input = selected_experiment;
					getting_input = false;

				} else std::cout << "I'm sorry, '" << selected_experiment_number << "' is not a valid selection" << std::endl;
			} else if (input == "help") {
				std::cout << "List of quick commands\n"
					<< "  print ID:           returns a table showing the data from the selected experiment\n"
					<< "  summarise ID:       returns a statistical summary of the experiment data by variable\n"
					<< "  delete ID:          deletes the selected experiment - this cannot be undone!\n"
					<< "  saved experiments:  shows a list of available experiments with their ID number\n"
					<< "  select ID:          selects an experiment to input data\n"
					<< "  [filename.csv]      creates a new experiment from a csv file\n"
					<< "  new:                creates a new experiment to input data\n"
					<< "  x:                  exits the program and saves edited experiments"
					<< std::endl;
			} else if (input.find(".csv") != std::string::npos) {
				// Create new experiment from csv file
				experiment exp_csv{ input };
				std::cout << "File found, please name your experiment" << std::endl;
				bool title_accepted{ false };
				while (!title_accepted) {
					std::string csv_exp_title;
					getline(std::cin, csv_exp_title);
					if (csv_exp_title.find("|") != std::string::npos) {
						std::cout << "ERROR: Please do not use '|' in your title" << std::endl;
					} else {
						title_accepted = true;
						exp_csv.set_title(csv_exp_title);
					}
				}
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
				if (save_csv) exp_csv.save_experiment();
				else if (select_csv) {
					exp_for_input = exp_csv;
					getting_input = false;
				}
			} else if (input == "x") {
				getting_input = false;
				program_running = false;
				program_shutting_down = true;
				break;
			} else {
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

		// input data for selected experiment
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
				temp_experiment.add_variable(temp_variable, force_include_datestamp = force_include_datestamp, force_include_error = force_include_error);
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
				temp_experiment.add_variable(temp_variable, force_include_datestamp = force_include_datestamp);
				nominal_index++;
			}
		}
		// Summarise the input data
		std::cout << "Data input stopped." << std::endl << "Here is the data that was added:" << std::endl
			<< temp_experiment << std::endl << "And a summary of your input: " << std::endl;
		temp_experiment.summary();

		// Add inputs to select experiment
		std::cout << "Adding new input to " << exp_for_input.get_title() << std::endl;
		exp_for_input.update_experiment(temp_experiment);
		std::cout << exp_for_input << std::endl;

		// Add experiment to container
		edited_experiments.push_back(exp_for_input);

	}

	// Save all experiments in container
	std::cout << "Exiting...saving experiment changes" << std::endl;
	for (size_t i{ 0 }; i < edited_experiments.size(); i++) {
		edited_experiments[i].save_experiment();
	}

	return 0;
}
