// header comments

#ifndef EXPERIMENT_CLASS_H
#define EXPERIMENT_CLASS_H

class experiment {
	friend std::ostream& operator<<(std::ostream& os, const experiment& mat);
private:
	std::vector<std::shared_ptr<measurement>> experiment_data;
	int number_of_variables{ 0 };
	int number_of_rows{ 0 };
	std::string experiment_title{ "My Experiment" };
	std::vector<std::string> experiment_datestamps;
	std::vector<std::string> variable_types;
public:
	experiment() = default;
	experiment(nominal_data variable, std::string title_of_experiment);
	experiment(double_data variable, std::string title_of_experiment);
	experiment(std::vector<std::shared_ptr<measurement>> exp_data, int exp_number_of_variables, int exp_number_of_rows, std::string exp_title, std::vector<std::string> exp_datestamps, std::vector<std::string> exp_variable_types);
	// Constructor for data from saved .txt files of specific format
	experiment(std::string experiment_filename);
	~experiment() {}
	experiment& operator=(experiment& exp);
	measurement& operator[](int index);
	std::vector<std::shared_ptr<measurement>> get_experiment_data() const;
	void set_title(std::string title);
	std::string get_title() const;
	int get_number_of_variables() const;
	int get_number_of_rows() const;
	std::vector<std::string> get_list_of_variable_names() const;
	std::vector<std::string> get_datestamps() const;
	std::vector<std::string> get_variable_types() const;
	void add_datestamp(std::string datestamp);
	void summary();
	void save_experiment() const;
	void delete_experiment();
	void update_experiment(experiment& exp);

	template <class c_type> void add_variable(c_type variable, bool force_include_datestamp = false, bool force_include_error = false)
	{
		// Add a variable to an existing experiment object
		if (variable.get_length() != number_of_rows && number_of_rows != 0) {
			std::cerr << "ERROR: Attempted to add a varaible whose length does not match the length of the experiment" << std::endl;
			return;
		}
		//std::cout << "Adding " << variable.get_variable_name() << " to '" << experiment_title << "'" << std::endl;

		bool add_datestamps{ false };
		if (number_of_variables == 0) add_datestamps = true;
		number_of_variables++;
		if (number_of_rows == 0) number_of_rows = variable.get_length();
		if (add_datestamps) {
			experiment_data.emplace_back(new nominal_data("Datestamps", variable.get_datestamps(), variable.get_datestamps()));
			variable_types.push_back("datestamp");
			experiment_datestamps = variable.get_datestamps();
			number_of_variables++;
			force_include_datestamp = false;
		}
		experiment_data.emplace_back(new c_type(variable.get_variable_name(), variable.get_data(), variable.get_datestamps()));
		variable_types.push_back(variable.get_measurement_type());

		// If the error variable is non-zero add it as an error variable
		std::vector<double> variable_error_data{ variable.get_error_data() };
		bool non_zero_error{ false };
		for (size_t i{ 0 }; i < variable_error_data.size(); i++) {
			if (variable_error_data[i] != 0) non_zero_error = true;
		}
		if (non_zero_error || force_include_error) {
			//std::cout << "Added error variable" << std::endl;
			experiment_data.emplace_back(new double_data(variable.get_variable_name() + " Error", variable.get_error_data(), variable.get_datestamps()));
			variable_types.push_back("error");
			number_of_variables++;
		}

		// If a variable has different datestamps to the experiment, add those as separate variables
		// Force create new variable if force_include_date is true (used when inputting data)
		std::vector<std::string> variable_datestamps{ variable.get_datestamps() };
		bool add_extra_date_variable{ false };
		for (size_t i{ 0 }; i < variable_datestamps.size(); i++) {
			if (variable_datestamps[i] != experiment_datestamps[i]) add_extra_date_variable = true;
		}
		if (add_extra_date_variable || force_include_datestamp) {
			//std::cout << "Added extra date variable" << std::endl;
			experiment_data.emplace_back(new nominal_data(variable.get_variable_name() + " Datestamps", variable.get_datestamps(), variable.get_datestamps()));
			variable_types.push_back("datestamp");
			number_of_variables++;
		}
	}
};

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
