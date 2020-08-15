## A data entry program project using object-oriented programming in C++

### Overview 
User can navigate the program through a series of quick commands, which allow the user to explore, edit or create datasets. For this project I was inspired by Pandas dataframes from python and tried to reproduce something similar using OOP in C++. Similar to pandas, datasets can be viewed as tables (in this case ASCII tables) and simple summary statistical measures can be quickly viewed. However, this system can hold multiple datasets either from previous user input or from  excel spreadsheets (with a particular layout). Another crucial aspect of the program is that it helps experimental scientists make sure that their experimental environments are similar for each day of recording. When a scientist adds more data to an existing dataset on a different day, the program will display summary statistical variables from the existing dataset as well as those from the new session. This allows the scientist to decide to not add the new data if the summary statistics is significantly different to those of previous days.

### Class Structure
The program has a measurement abstract base class, with derived classes: double_data, and nominal_data; these classes handle numerical and categorical data respectively. Additionally, there is an experiment class which holds a polymorphic vector of measurements as well as metadata about the dataset it holds.  

### Available Quick Commands
- **help:**                displays available quick commands
- **print ID:**            returns a table showing the data rom the selected experiment
- **summarise ID:**        returns a statistical summary of the experiment data by variable
- **delete ID:**           deletes the selected experiment - this cannot be undone!
- **saved experiments:**   shows a list of available experiments with their ID number
- **select ID:**           selects an experiment to input data
- **to csv ID:**           outputs the experiment as a .csv file
- **show edits:**          displays experiments that have recently been edited
- **new:**                 creates a new experiment to input data
- **[filename.csv]:**       creates a new experiment from a csv file
- **x:**                   exits the program and saved edited experiments
