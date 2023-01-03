#include "config.hpp"
#include "utils.hpp"

void	validate_extension(const char *path, const char *ext)
{
	int	size;

	size = strlen(path) - 5;
    //.. Comparing the last 5 characters to see if its matches the extension
	if (strcmp(ext, path + size))
        throw parseError("Usage: ./webserv [configFile]");

}

bool    validate_host(std::string ipAddress)
{
    int index;
    int size;
    stringContainer ip;

    if (std::count(ipAddress.begin(), ipAddress.end(), '.') != 3)
        return (false);
    index = 0;
    ip = splitSeparator(ipAddress, '.');
    size = ip.size();
    while (index < size)
    {
        if (!is_number(ip.at(index)) || atoi(ip.at(index).c_str()) > 255)
            return (false);
        index++;
    }
    return (true);
}

void    printContainer(stringContainer table)
{
    int i;

    i = 0;

    std::cout << "------------------------------" << std::endl;
    while (i < table.size())
    {
        std::cout << table.at(i) << std::endl;
        i++;
    }
}

stringContainer split(std::string line)
{
    stringContainer result;
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
            result.push_back(line.substr(i, j));
            i += j;
            j = 0;
        }
        i++;
    }
    return (result);
}

stringContainer splitSeparator(std::string line, char c)
{
    stringContainer result;
    int i = 0;
    int j = 0;

    while (i < line.length())
    {
        while (line[i] && line[i] == c)
            i++;
        if (line[i] && line[i] != c)
        {
            while (line[i + j] && line[i + j] != c)
                j++;
            result.push_back(line.substr(i, j));
            i += j;
            j = 0;
        }
        i++;
    }
    return (result);
}

void    validate_file_content(std::ifstream & configFile)
{
    //.. Chechking that the file is not empty
    if (configFile.peek() == std::ifstream::traits_type::eof())
        throw parseError("Config Error: config file is empty");

}

std::string getLine(std::string &line) {
    //.. Removing whitespaces
    // line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    //.. Checking if line contains a comment
    if (line.find('#') != std::string::npos)
        line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty() || line[0] == '#')
        return std::string();
    return (line);
}

stringContainer   read_config_file(std::string & path)
{
    std::string ext;
    std::ifstream config;
    std::string line;
    std::string parsedLine;
    stringContainer configFile;

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
            parsedLine = getLine(line);
            if (!parsedLine.empty())
                configFile.push_back(parsedLine);
        }
    }
    return (configFile);
}

void    check_brackets(stringContainer configContent)
{
    std::string line;
    int bracketsLevel;
    int i;

    i = 0;
    bracketsLevel = 0;
    while (i < configContent.size())
    {
        line = configContent[i];
        bracketsLevel += std::count(line.begin(), line.end(), '{');
        bracketsLevel -= std::count(line.begin(), line.end(), '}');
        i++;
    }
    //.. Checking that the currly brackets have been closed in a correct way.
    if (bracketsLevel != 0)
        throw parseError("Syntax Error: Missing a closing bracket");

}

bool checkValidDirectives(std::string line, int context)
{
    if (context == MAIN)
    {
        if (strcmp(line.c_str(), "pid"))
            return (false);
    }
    else if (context == HTTP)
    {
        if (!checkDirectiveKey(line, httpContext))
            return (false);
    }
    else if (context == SERVER)
    {
        if (!checkDirectiveKey(line, serverContext))
            return (false);
    }
    else if (context == LOCATION)
    {
        if (!checkDirectiveKey(line, locationContext))
            return (false);
    }
    return (true);
}

void    validateDirective(stringContainer & line, int context)
{
    std::string directiveEnd;

    directiveEnd = line[line.size() - 1];
    if (line.size() < 2)
        throw parseError("Syntax Error: Directive must be listed as key : value pattern");
    if (strcmp(directiveEnd.c_str(), ";") && directiveEnd[directiveEnd.length() - 1] != ';')
        throw parseError("Syntax Error: Directive must end with ;");
    if (!checkValidDirectives(line[0], context))
        throw parseError("Config Error: Invalid directive name: " + line[0]);
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
           throw parseError("Syntax Error: Could not find file: " + path);
    }
    else if (mode == DIR_MODE)
    {
        if (stat(path.c_str(), &sb) != 0)
           throw parseError("Syntax Error: Could not find directory: " + path);
    }
}

void parse_error_pages(stringContainer page, Pages * errorPages)
{
    if (atoi(page[1].c_str()) == 404)
    {
        if (errorPages->path_not_found.length() == 0)
            errorPages->path_not_found = page[2];
        else
            throw parseError("Error Pages: status code page 404 is duplicated");
    }
    else if (atoi(page[1].c_str()) == 403)
    {
        if (errorPages->path_forbidden.length() == 0)
            errorPages->path_forbidden = page[2];
        else
            throw parseError("Error Pages: status code page 403 is duplicated");

    }
    else if (atoi(page[1].c_str()) == 500)
    {
        if (errorPages->path_internal_error.length() == 0)
            errorPages->path_internal_error = page[2];
        else
            throw parseError("Error Pages: status code page 500 is duplicated");
    }
    else
        throw parseError("Error Pages: invalid status code");
}

int getClosingIndex(stringContainer fileContent, int position)
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
            return (position - 1);
    }
    return (0); // Just for fun.
}


bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}