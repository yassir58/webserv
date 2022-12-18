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


int main(int argc, char **argv)
{
    char *line;

    std::ifstream configFile;
    // Do all the neccessary checks to validate the file name and the file content.
    // Check the path
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