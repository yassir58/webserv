#include "config.hpp"

void	validate_extension(const char *path, const char *ext)
{
	int	size;

	size = strlen(path) - 5;
    //.. Comparing the last 5 characters to see if its matches the extension
	if (strcmp(ext, path + size))
    {
		std::cout << "Usage: webserv *.conf" << std::endl;
        exit(1);
    }
}

std::vector<std::string> split(std::string line)
{
    std::vector<std::string> splitted;
    int i = 0;
    int j = 0;

    while (i < line.length())
    {
        while (line[i] && std::isspace(line[i]))
            i++;
        if (line[i] && !std::isspace(line[i]))
        {
            while (line[i + j] && !std::isspace(line[i + j]))
                j++;
            splitted.push_back(line.substr(i, j));
            i += j;
            j = 0;
        }
        i++;
    }
    return (splitted);
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
    std::string ext;
    std::ifstream config;
    std::string line;
    std::vector<std::string> configFile;

    ext = ".conf";
    //.. Openning the the config file
    validate_extension(path.c_str(), ext.c_str());
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
            bracketsLevel += std::count(line.begin(), line.end(), '{');
            bracketsLevel -= std::count(line.begin(), line.end(), '}');
        }
    }
    //.. Checking that the currly brackets have been closed in a correct way.
    if (bracketsLevel != 0)
    {
        std::cout << "Missing {} please check the config file." << std::endl;
        exit(1);
    }
}

void    checkDirective(std::vector<std::string> line, int context)
{
    if (line.size() < 2)
    {
        std::cout << "Syntax Error: Directive must be listed as key : value pattern" << std::endl;
        exit(1);
    }
    if (strcmp(line[line.size() - 1].c_str(), ";"))
    {
        std::cout << "Syntax Error: Directive must end with ;" << std::endl;
        exit(1);
    }
    if (!checkValidDirectives(line[0], context))
    {
        std::cout << "Config Error: Invalid Directive name" << std::endl;
        exit(1);
    }
}

bool checkValidDirectives(std::string line, int context)
{
    if (context == MAIN_CONTEXT)
    {
        if (strcmp(line.c_str(), "pid"))
            return (false);
    }
    else if (context == HTTP_CONTEXT)
    {
        if (!checkDirectiveKey(line, httpContext))
            return (false);
    }
    else if (context == SERVER_CONTEXT)
    {
        if (!checkDirectiveKey(line, serverContext))
            return (false);
    }
    else if (context == LOCATION_CONTEXT)
    {
        if (!checkDirectiveKey(line, locationContext))
            return (false);
    }
    return (true);
}

bool checkDirectiveKey(std::string directiveName,const char **directivesTable)
{
    unsigned int i;

    i = 0;
    while (directivesTable[i])
    {
        if (strcmp(directiveName.c_str(), directivesTable[i]) == 0)
            return (true);
        i++;
    }  
    return (false);
}

void createFile(std::string path, int mode)
{
    std::ifstream checkFile;
    

    checkFile.open(path);
    if (mode == CREATE_MODE)
    {
        if (checkFile.is_open())
            return;
        std::ofstream output(path);
    }
    else 
    {
        if (!checkFile.is_open())
        {
            std::cout << "Syntax error: " << path << "could not be found" << std::endl;
            exit(1);
        }
    }
}