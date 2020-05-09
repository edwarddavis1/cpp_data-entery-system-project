/*
* Measurement Class Header
* Abstract base class: measurement
* Derived classes: nominal_data, double_data (variable objects)
* Each variable class contains member functions for summary statistical measures
* Last modified: 01/05/2020
*/
#ifndef MEASUREMENT_CLASS_H
#define MEASUREMENT_CLASS_H

class measurement {
public:
	virtual ~measurement() {}
	virtual void summary() const = 0;
	virtual std::vector<std::string> summary_measures() = 0; 
	virtual void print_data() = 0;		
	virtual int count() const = 0;							
	virtual std::string get_variable_name() const = 0;
	virtual std::vector<std::string> get_string_data() const = 0;
	virtual std::string get_measurement_type() const = 0;
	virtual std::vector<std::string> get_datestamps() const = 0;
	virtual std::vector<double> get_error_data() const = 0;
	virtual std::string mode_average() const = 0;						  
	virtual int number_of_unique_values() const = 0;
};
class nominal_data : public measurement {
private:
	std::vector<std::string> data;
	std::string variable_name;
	std::vector<std::string> datestamps;
public:
	nominal_data() = default;
	nominal_data(const std::string name_of_variable, const std::vector<std::string> variable_data, const std::vector<std::string> variable_datestamps);
	~nominal_data() {}
	void print_data();
	size_t get_length() const;
	std::string get_variable_name() const;
	std::vector<std::string> get_datestamps() const;
	std::vector<double> get_error_data() const;
	std::string& operator[](size_t index);
	void summary() const;
	std::vector<std::string> summary_measures();
	std::string get_measurement_type() const;
	std::string get_variable_name();
	int count() const;
	int number_of_unique_values() const;
	std::string mode_average() const;
	std::vector<std::string> get_string_data() const;
	std::vector<std::string> get_data() const;
};
class double_data : public measurement {
private:
	std::vector<double> data;
	std::vector<double> error_data;
	std::string variable_name{ "My Variable" };
	std::vector<std::string> datestamps;
public:
	double_data() = default;
	// Constructor for variable without errors
	double_data(const std::string name_of_variable, const std::vector<double> variable_data, std::vector<std::string> variable_datestamps);
	// Constructor for variable with errors
	double_data(const std::string name_of_variable, const std::vector<double> variable_data, const std::vector<double> measurement_error, std::vector<std::string> variable_datestamps);
	~double_data() {}
	void print_data();
	size_t get_length() const;
	std::vector<std::string> get_datestamps() const;
	std::vector<double> get_error_data() const;
	std::string get_variable_name() const;
	std::string get_measurement_type() const;
	double& operator[](size_t index);
	std::vector<std::string> get_string_data() const;
	std::vector<double> get_data() const;
	int count() const;
	int number_of_unique_values() const;
	std::string mode_average() const;
	double mean() const;
	double standard_deviation() const;
	double standard_error() const;
	double min() const;
	double max() const;
	double get_percentile_value(double percentile) const;
	void summary() const;
	std::vector<std::string> summary_measures();
};

#endif
