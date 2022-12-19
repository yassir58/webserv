#include "config.hpp"

void	validate_extension(char *path, char *ext)
{
	int	size;

	size = strlen(path) - 4;
	if (strcmp(ext, path + size))
    {
		std::cout << "Usage: webserv *.conf" << std::endl;
        exit(1);
    }
}

void    validate_file_content(std::ifstream &file)
{
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        std::cout << "Config file is empty." << std::endl;
        exit(1);
    }
}

bool    read_config_file(std::string & line)
{
    //? Validation
    //TODO: remove whitespaces
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    //TODO: check if the line starts with a comment
    if (line.empty() || line[0] == '#')
        return (false);
    //TODO: check that the line ends with ; if does not end with { }
    //TODO: overide the line old content with new content
}

void    check_brackets(std::ifstream &file, char *filename)
{
    // TODO: open file
    // TODO: read the file line by line and calculate bracketsLevel
    // TODO: return true if bracketsLevel is 0 return false if otherwise
    std::string line;
    int bracketsLevel;

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
    if (bracketsLevel != 0)
    {
        std::cout << "Missing {} please check the config file." << std::endl;
        exit(1);
    }
}

int main(int argc, char **argv)
{
    char *line;

    std::ifstream configFile;
    // Do all the neccessary checks to validate the file name and the file content.
    // Check the path
    std::vector<std::string> lines;
    validate_extension(argv[1], ".conf");
    // Check that the file is not empty.
    configFile.open(argv[1]);
    if (configFile.is_open())
    {
        validate_file_content(configFile);
        //TODO: Read all the file into an array of string without the lines containing the comments
        //TODO: Check that all the curly brackets are closed
        //TODO: Create a parse directive function which should adaptes to every directive in the config file
        //TODO: Create a small config file which should tell the parser how every directive will be parsed.
        //TODO: Should create that will convert ip address and port number to integers
        //TODO: Create a pathParse function that will validate file path and check if it exists
        
    }
    // Check the syntax
}