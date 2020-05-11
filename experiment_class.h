/*
* Experiment Class Header
* An experiment class contains multiple variable objects like columns in a table
* An experiment also has associated datestamps and holds the types of variable in the data (nominal, double, error or datestamp)
* Last modified: 01/05/2020
*/
#ifndef EXPERIMENT_CLASS_H
#define EXPERIMENT_CLASS_H

const int bad_variable_size_flag{ -3 };

class experiment {
	friend std::string format_variables(const experiment& exp);
	friend std::ostream& operator<<(std::ostream& os, const experiment& exp);
private:
	std::vector<std::shared_ptr<measurement>> experiment_data;
	int number_of_variables{ 0 };
	int number_of_rows{ 0 };
	std::string experiment_title{ "My Experiment" };
	std::vector<std::string> experiment_datestamps;
	std::vector<std::string> variable_types;
public:
	experiment() = default;
	// Constructors from single variables
	experiment(nominal_data variable, std::string title_of_experiment);
	experiment(double_data variable, std::string title_of_experiment);
	// Constructor using experiment member data
	experiment(std::vector<std::shared_ptr<measurement>> exp_data, int exp_number_of_variables, int exp_number_of_rows, std::string exp_title, std::vector<std::string> exp_datestamps, std::vector<std::string> exp_variable_types);
	// Constructor from data in a *.csv or a previously saved *.txt file
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
	void summary();
	void save_experiment() const;
	void save_to_csv() const;
	void delete_experiment();
	// Used to add data to an existing experiment
	void update_experiment(experiment& exp);
	template <class c_type> void add_variable(c_type variable, bool force_include_datestamp = false, bool force_include_error = false, bool add_standard_error = false)
	{
		// Add a variable to an existing experiment object
		if (variable.get_length() != number_of_rows && number_of_rows != 0) {
			std::cerr << "ERROR: Attempted to add a varaible whose length does not match the length of the experiment" << std::endl;
			throw bad_variable_size_flag;
		}
		// Add date stamps if the experiment has no prior variables
		bool add_datestamps{ false };
		if (number_of_variables == 0) add_datestamps = true;
		number_of_variables++;
		if (number_of_rows == 0) number_of_rows = variable.get_length();
		if (add_datestamps) {
			experiment_data.emplace_back(new nominal_data("Datestamps", variable.get_datestamps(), variable.get_datestamps()));
			variable_types.push_back("datestamp");
			experiment_datestamps = variable.get_datestamps();
			number_of_variables++;
			// If date stamp already added, cancel the forced date stamp
			force_include_datestamp = false;
		}
		// Add the variable
		experiment_data.emplace_back(new c_type(variable.get_variable_name(), variable.get_data(), variable.get_datestamps()));
		variable_types.push_back(variable.get_measurement_type());

		// If the error variable is non-zero (or it is forced) add it as an error variable
		std::vector<double> variable_error_data{ variable.get_error_data() };
		bool non_zero_error{ false };
		for (size_t i{ 0 }; i < variable_error_data.size(); i++) {
			if (variable_error_data[i] != 0) non_zero_error = true;
		}
		if (non_zero_error || force_include_error) {
			experiment_data.emplace_back(new double_data(variable.get_variable_name() + " Error", variable.get_error_data(), variable.get_datestamps()));
			variable_types.push_back("error");
			number_of_variables++;
		} else if (variable.get_measurement_type() == "double" && add_standard_error) {
			// If error not given and the variable is numerical, then use standard error
			std::vector<double> standard_error_data;
			double standard_error{ variable.standard_error() };
			for (size_t i{ 0 }; i < variable_error_data.size(); i++) {
				standard_error_data.push_back(standard_error);
			}
			experiment_data.emplace_back(new double_data(variable.get_variable_name() + " Error", standard_error_data, variable.get_datestamps()));
			variable_types.push_back("error");
			number_of_variables++;
		}

		// If a variable has different datestamps to the experiment, add those as separate variables
		std::vector<std::string> variable_datestamps{ variable.get_datestamps() };
		bool add_extra_date_variable{ false };
		for (size_t i{ 0 }; i < variable_datestamps.size(); i++) {
			if (variable_datestamps[i] != experiment_datestamps[i]) add_extra_date_variable = true;
		}
		if (add_extra_date_variable || force_include_datestamp) {
			experiment_data.emplace_back(new nominal_data(variable.get_variable_name() + " Datestamps", variable.get_datestamps(), variable.get_datestamps()));
			variable_types.push_back("datestamp");
			number_of_variables++;
		}
	}
};


#endif
