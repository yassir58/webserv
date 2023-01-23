#include "config.hpp"
#include "utils.hpp"

void	validateExtension(const char *path, const char *ext)
{
	int size;

	size = strlen(path) - 5;
    //.. Comparing the last 5 characters to see if its matches the extension
	if (strcmp(ext, path + size))
        throw parseError("Usage: ./webserv [configFile]");

}

bool    validateHost(std::string ipAddress)
{
    unsigned int index;
    unsigned int size;
    stringContainer ip;

    if (std::count(ipAddress.begin(), ipAddress.end(), '.') != 3)
        return (false);
    index = 0;
    ip = splitSeparator(ipAddress, '.');
    size = ip.size();
    while (index < size)
    {
        if (!isNumber(ip.at(index)) || atoi(ip.at(index).c_str()) > 255)
            return (false);
        index++;
    }
    return (true);
}

void    printContainer(stringContainer table)
{
    unsigned int i;

    i = 0;

    std::cout << "===============================" << std::endl;
    while (i < table.size())
    {
        printf("%s", table.at(i).c_str());
        // std::cout << table.at(i) << std::endl;
        i++;
    }
}

stringContainer split(std::string line)
{
    stringContainer result;
    unsigned int i = 0;
    unsigned int j = 0;

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
    unsigned int i = 0;
    unsigned int j = 0;

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

void    checkFileEmpty(std::ifstream & configFile)
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
    validateExtension(path.c_str(), ext.c_str());
    config.open(path);
    //.. Checking if its open
    if (config.is_open())
    {
        checkFileEmpty(config);
        while (getline(config, line))
        {
            parsedLine = getLine(line);
            if (!parsedLine.empty() && !checkSpaces(parsedLine))
                configFile.push_back(parsedLine);
        }
    }
    return (configFile);
}

void    checkBrackets(stringContainer configContent)
{
    std::string line;
    unsigned int bracketsLevel;
    unsigned int i;

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
        if (!keyExistsInTable(line, httpContext))
            return (false);
    }
    else if (context == SERVER)
    {
        if (!keyExistsInTable(line, serverContext))
            return (false);
    }
    else if (context == LOCATION)
    {
        if (!keyExistsInTable(line, locationContext))
            return (false);
    }
    return (true);
}

void    validateDirective(stringContainer & line, int context)
{
    std::string directiveEnd;

    directiveEnd = line[line.size() - 1];
    if (line.size() < 2 && (line[0] == "server" || line[0] == "http" || line[0] == "location"))
        throw parseError("Syntax Error: newline detected after context name: " + line[0]);
    if (line.size() < 2)
        throw parseError("Syntax Error: Directive must be listed as key : value pattern");
    if (strcmp(directiveEnd.c_str(), ";") && directiveEnd[directiveEnd.length() - 1] != ';')
        throw parseError("Syntax Error: Directive must end with ;");
    if (!checkValidDirectives(line[0], context))
        throw parseError("Config Error: Invalid directive name: " + line[0]);
}

bool keyExistsInTable(std::string key,const char **table)
{
    unsigned int i;

    i = 0;
    while (table[i])
    {
        if (strcmp(key.c_str(), table[i]) == 0)
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

int getClosingIndex(stringContainer fileContent, unsigned int position)
{
    unsigned int i;
    unsigned int bracketsLevel;
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
    return (0);
}

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

stringContainer   stripSemiColon(stringContainer str)
{
    if ((str[str.size() - 1][str[str.size() - 1].length() - 1]) == ';' && str[str.size() - 1].length() > 1)
    {
        str[str.size() - 1].pop_back();
        if ((str[str.size() - 1][str[str.size() - 1].length() - 1]) == ';')
            throw parseError("Syntax Error: Duplicated semi-colons");
    }
    else
        str.pop_back();
    return (str);
}

bool checkSpaces(std::string string)
{
    unsigned int i;

    i = 0;
    while (i < string.length())
    {
        if (!isspace(string[i]))
            return (false);
        i++;
    }
    return (true);
}

std::string int2assci(int nb)
{
	std::stringstream res;
	std::string str;
	res << nb;
	str = res.str();
	return (str);
}

std::string toUpperCase(std::string str)
{
    int i;

    i = 0;
    while (str[i])
    {
        str[i] = toupper(str[i]); 
        i++;
    }
    return (str);
}

void    freeTable(char **table)
{
    int i;

    i = 0;
    while (table[i])
        free(table[i++]);
    free(table);
}

std::string convertBody(stringContainer body)
{
    unsigned int i;
    std::string res;

    i = 0;
    while (i < body.size())
    {
        res.append(body[i]);
        i++;
    }
    return (res);
}

void    print_table(char **table)
{
    int i;
    
    i = 0;
    while (table[i])
    {
        std::cout << table[i] << std::endl;
        i++;
    }
}

std::string readContent(std::string path)
{
    std::string content = "File:";
    std::string line;

    std::ifstream file(path);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            content.append(line).append("\n");
        }
        content.pop_back();
        file.close();
    }
    return (content);
}