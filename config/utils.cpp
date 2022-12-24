#include "config.hpp"

void	validate_extension(const char *path, char *ext)
{
	int	size;

	size = strlen(path) - 4;
    //.. Comparing the last 4 characters to see if its matches the extension
	if (strcmp(ext, path + size))
    {
		std::cout << "Usage: webserv *.conf" << std::endl;
        exit(1);
    }
}

std::vector<std::string> split(std::string line)
{
    std::istringstream ss(line);
 
    std::string word; // for storing each word
 
    // Traverse through all words
    // while loop till we get
    // strings to store in string word
    while (ss >> word)
    {
        // print the read word
        std::cout << word << "\n";
    }
}

void    validate_file_content(std::ifstream & configFile)
{
    //.. Chechking that the file is not empty
    if (configFile.peek() == std::ifstream::traits_type::eof())
    {
        std::cout << "Config file is empty." << std::endl;
        exit(1);
    }
}

std::string getLine(std::string &line) {
    //.. Removing whitespaces
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    //.. Checking if line contains a comment
    if (line.empty() || line[0] == '#')
        return std::string();
    return (line);
}

std::vector<std::string>   read_config_file(std::string & path)
{
    validate_extension(path.c_str(), ".conf");
    std::ifstream config;
    std::string line;
    std::vector<std::string> configFile;

    //.. Openning the the config file
    config.open(path);
    //.. Checking if its open
    if (config.is_open())
    {
        validate_file_content(config);
        while (getline(config, line))
        {
            configFile.push_back(getLine(line));
        }
    }
    return (configFile);
}

void    check_brackets(std::string filename)
{
    std::string line;
    std::ifstream file;
    int bracketsLevel;

    bracketsLevel = 0;
    //.. Openning the config file
    file.open(filename);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            int openBrackets = std::count(line.begin(), line.end(), '{');
            int closedBrackets = std::count(line.begin(), line.end(), '}');
            bracketsLevel = closedBrackets - openBrackets;
        }
    }
    //.. Checking that the currly brackets have been closed in a correct way.
    if (bracketsLevel != 0)
    {
        std::cout << "Missing {} please check the config file." << std::endl;
        exit(1);
    }
}

