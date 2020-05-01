// Header comments

#ifndef MEASUREMENT_CLASS_H
#define MEASUREMENT_CLASS_H

class measurement {
public:
	virtual ~measurement() {}
	virtual void summary() const = 0;
	virtual std::vector<std::string> summary_measures() const = 0;
	virtual void print_data() = 0;
	virtual int count() const = 0;
	virtual std::string get_variable_name() const = 0;
	virtual std::vector<std::string> get_string_data() const = 0;
	virtual std::string get_measurement_type() const = 0;
	virtual std::vector<std::string> get_datestamps() const = 0;
	virtual std::vector<double> get_error_data() const = 0;
};
class labelled_data : public measurement {
public:
	~labelled_data() {}
	virtual std::string mode() = 0;
};
class nominal_data : public labelled_data {
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
	std::vector<std::string> summary_measures() const;
	std::string get_measurement_type() const;
	std::string get_variable_name();
	int count() const;
	std::string mode();
	std::vector<std::string> get_string_data() const;
	std::vector<std::string> get_data() const;
	void add_data(std::vector<std::string> data_to_add);
	void add_datestamps(std::vector<std::string> datestamps_to_add);
};
class double_data : public measurement {
private:
	std::vector<double> data;
	std::vector<double> error_data;
	std::string variable_name{ "My Variable" };
	std::vector<std::string> datestamps;
public:
	double_data() = default;
	double_data(const std::string name_of_variable, const std::vector<double> variable_data, std::vector<std::string> variable_datestamps);
	double_data(const std::string name_of_variable, const std::vector<double> variable_data, const double systematic_error, std::vector<std::string> variable_datestamps);
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
	double mean() const;
	double standard_deviation() const;
	double min() const;
	double max() const;
	double get_percentile_value(double percentile) const;
	void summary() const;
	std::vector<std::string> summary_measures() const;
	void add_data(std::vector<double> data_to_add);
	void add_datestamps(std::vector<std::string> datestamps_to_add);
	void add_errors(std::vector<double> errors_to_add);
};

#endif
