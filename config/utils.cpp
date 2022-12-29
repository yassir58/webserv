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

// bool checkValidDirectives(std::string line, int context)
// {
//     if (context == MAIN)
//     {
//         if (strcmp(line.c_str(), "pid"))
//             return (false);
//     }
//     else if (context == HTTP)
//     {
//         if (!checkDirectiveKey(line, httpContext))
//             return (false);
//     }
//     else if (context == SERVER)
//     {
//         if (!checkDirectiveKey(line, serverContext))
//             return (false);
//     }
//     else if (context == LOCATION)
//     {
//         if (!checkDirectiveKey(line, locationContext))
//             return (false);
//     }
//     return (true);
// }

void    validateDirective(std::vector<std::string> line, int context)
{
    //! In the function in need to check that am checking the last character of the last directive line array.
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
    // if (!checkValidDirectives(line[0], context))
    // {
    //     std::cout << "Config Error: Invalid Directive name" << std::endl;
    //     exit(1);
    // }
}

// bool checkDirectiveKey(std::string directiveName,const char **directivesTable)
// {
//     unsigned int i;

//     i = 0;
//     while (directivesTable[i])
//     {
//         if (strcmp(directiveName.c_str(), directivesTable[i]) == 0)
//             return (true);
//         i++;
//     }  
//     return (false);
// }



void checkPath(std::string path, int mode)
{
    std::ifstream checkFile;
    struct stat sb;

    checkFile.open(path);
    if (mode == CREATE_MODE)
    {
        if (checkFile.is_open())
            return;
        std::ofstream output(path);
    }
    else if (mode == CHECK_MODE)
    {
        if (!checkFile.is_open())
        {
            std::cout << "Syntax error: " << path << "could not be found" << std::endl;
            exit(1);
        }
    }
    else if (mode == DIR_MODE)
    {
        if (stat(path.c_str(), &sb) != 0)
        {
           std::cout << "Syntax error: " << path << "could not be found" << std::endl;
           exit(1); 
        }
    }
}

void    parse_error_pages(std::vector<std::string> page, Http & http)
{
    if (atoi(page[1].c_str()) == 400)
        http.getErrorPages().path_not_found = page[2];
    else if (atoi(page[1].c_str()) == 403)
        http.getErrorPages().path_forbidden = page[2];
    else if (atoi(page[1].c_str()) == 500)
        http.getErrorPages().path_internal_error = page[2];
    else
    {
        std::cout << "Invalid status code page" << std::endl;
        exit(1);
    }
}

int getClosingIndex(std::vector<std::string> fileContent, int position)
{
    int i;
    int bracketsLevel;
    std::string line;

    i = fileContent.size();
    bracketsLevel = 1;
    while (position < i)
    {
        line = fileContent[position];
        bracketsLevel += std::count(line.begin(), line.end(), '{');
        bracketsLevel -= std::count(line.begin(), line.end(), '}');
        position += 1;
        if (bracketsLevel == 0)
            return (position);
    }
    return (-1);
}

Pages    Http::getErrorPages()
{
    return (this->pages);
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}