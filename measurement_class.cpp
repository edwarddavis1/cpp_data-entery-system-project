/*
* Measurement Class
* Abstract base class: measurement
* Derived classes: nominal_data, double_data (variable objects)
* Each variable class contains member functions for summary statistical measures
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
int nominal_data::number_of_unique_values() const
{
	std::vector<std::string> unique_values{ get_unique(data) };
	int number_of_unique{ static_cast<int>(unique_values.size()) };
	return number_of_unique;
}
std::string nominal_data::mode_average() const {
	std::string mode_avg;
	// list of highest occurring points
	std::vector<std::string> mode_averages{ get_mode_averages(data) };
	// if more than one mode average found output no clear mode
	if (mode_averages.size() == 1) mode_avg = mode_averages[0];
	else mode_avg = "none clear";
	return mode_avg;
}
void nominal_data::summary() const { std::cout << "nominal data summary" << std::endl; }
std::vector<std::string> nominal_data::summary_measures()
{
	std::vector<std::string> list_of_summary_measures;
	list_of_summary_measures.push_back(std::to_string(this->count()));
	list_of_summary_measures.push_back((this->mode_average()));
	list_of_summary_measures.push_back(std::to_string((this->number_of_unique_values())));
	list_of_summary_measures.push_back("n/a");
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

std::vector<std::string> nominal_data::get_string_data() const { return data; }
std::vector<std::string> nominal_data::get_data() const { return data; }

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
int double_data::number_of_unique_values() const
{
	std::vector<double> unique_values{ get_unique(data) };
	int number_of_unique{ static_cast<int>(unique_values.size()) };
	return number_of_unique;
}
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
double double_data::standard_error() const
{
	double standard_deviation{ this->standard_deviation() };
	double standard_error{ standard_deviation / sqrt(static_cast<double>(data.size()) - 1) };
	return standard_error;
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
std::string double_data::mode_average() const {
	std::string mode_avg;
	// list of highest occurring points
	std::vector<double> mode_averages{ get_mode_averages(data) };
	// if more than one mode average found output no clear mode
	if (mode_averages.size() == 1) mode_avg = std::to_string(mode_averages[0]);
	else mode_avg = "none clear";
	return mode_avg;
}
void double_data::summary() const
{
	int length_of_heading{ static_cast<int>(variable_name.length()) + 14 };
	for (int i{ 0 }; i < length_of_heading; i++) std::cout << "-";
	std::cout << "\n   " << variable_name << " Summary   " << std::endl;
	for (int i{ 0 }; i < length_of_heading; i++) std::cout << "-";
	std::cout << "\nCount: " << this->count() << std::endl
		<< "Mean: " << this->mean() << std::endl
		<< "Mode: " << this->mode_average() << std::endl
		<< "Unique: " << this->number_of_unique_values() << std::endl
		<< "Std: " << this->standard_deviation() << std::endl
		<< "Error: " << this->standard_error() << std::endl
		<< "Min: " << this->min() << std::endl
		<< "25%: " << this->get_percentile_value(25) << std::endl
		<< "50%: " << this->get_percentile_value(50) << std::endl
		<< "75%: " << this->get_percentile_value(75) << std::endl
		<< "Max: " << this->max() << std::endl;
	for (int i{ 0 }; i < length_of_heading; i++) std::cout << "-";
	std::cout << std::endl;
}
std::vector<std::string> double_data::summary_measures()
{
	std::vector<std::string> list_of_summary_measures;
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->count())));
	list_of_summary_measures.push_back(remove_zero_trail(this->mode_average()));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string((this->number_of_unique_values()))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->mean())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->standard_deviation())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->standard_error())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->min())));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->get_percentile_value(25))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->get_percentile_value(50))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->get_percentile_value(75))));
	list_of_summary_measures.push_back(remove_zero_trail(std::to_string(this->max())));
	return list_of_summary_measures;
}
