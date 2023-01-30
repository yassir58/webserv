#include "config.hpp"
#include "utils.hpp"

bool    validateExtension(std::string path, std::string ext)
{
    if (!strcmp(path.c_str() + path.find(".", 2) + 1, ext.c_str()))
        return (1);
    return (0);
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

bool validateCGIExtension(std::string key)
{
    if (keyExistsInTable(key, cgiExtensions))
        return (true);
    return (false);
}

bool validateIndexExtension(std::string filename)
{
    int i;

    i = 0;
    while (indexExtensions[i])
    {
        if (validateExtension(filename, indexExtensions[i]))
            return (true);
        i++;
    }
    return (false);
}

void    printContainer(stringContainer table)
{
    unsigned int i;

    i = 0;

    while (i < table.size())
    {
        printf("%s\n", table.at(i).c_str());
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
    std::ifstream config;
    std::string line;
    std::string parsedLine;
    stringContainer configFile;

    //.. Openning the the config file
    if (!validateExtension(path, "conf"))
        throw parseError("Usage: ./webserv [configFile]"); 
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
    std::string content = "";
    std::string line;

    std::ifstream file(path);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            content.append(line).append("\n");
        }
        if (content.length() > 0 && content[content.length() - 1] == '\n')
            content.pop_back();
        file.close();
    }
    return (content);
}

mapContainer createMiMe()
{
    mapContainer MiMeList;

    MiMeList["html"] = "text/html";
    MiMeList["htm"] = "text/html";
    MiMeList["shtml"] = "text/html";
    MiMeList["css"] = "text/css";
    MiMeList["xml"] = "text/xml";
    MiMeList["gif"] = "image/gif";
    MiMeList["jpeg"] = "image/jpeg";
    MiMeList["jpg"] = "image/jpeg";
    MiMeList["js"] = "application/javascript";


    MiMeList["txt"] = "text/plain";
    MiMeList["avif"] = "image/avif";
    MiMeList["png"] = "image/png";
    MiMeList["svg"] = "image/svg+xml";
    MiMeList["svgz"] = "image/svg+xml";
    MiMeList["tif"] = "image/tiff";
    MiMeList["tiff"] = "image/tiff";
    MiMeList["wbmp"] = "image/vnd.wap.wbmp";
    MiMeList["webp"] = "image/webp";
    MiMeList["ico"] = "image/x-icon";
    MiMeList["jng"] = "image/x-jng";
    MiMeList["bmp"] = "image/x-ms-bmp";


    MiMeList["woff"] = "font/woff";
    MiMeList["woff2"] = "font/woff2";
    MiMeList["jar"] = "application/java-archive";
    MiMeList["war"] = "application/java-archive";
    MiMeList["ear"] = "application/java-archive";
    MiMeList["json"] = "application/json";
    MiMeList["doc"] = "application/msword";
    MiMeList["pdf"] = "application/pdf";
    MiMeList["rtf"] = "application/rtf";
    MiMeList["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    MiMeList["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    MiMeList["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    MiMeList["wmlc"] = "application/vnd.wap.wmlc";
    MiMeList["wasm"] = "application/wasm";
    MiMeList["7z"] = "application/x-7z-compressed";
    MiMeList["pl"] = "application/x-perl";
    MiMeList["pm"] = "application/x-perl";

    MiMeList["rar"] = "application/x-rar-compressed";
    MiMeList["swf"] = "application/x-shockwave-flash";
    MiMeList["sit"] = "application/x-stuffit";
    MiMeList["tcl"] = "application/x-tcl";
    MiMeList["tk"] = "application/x-tcl";
    MiMeList["der"] = "application/x-x509-ca-cert";
    MiMeList["pem"] = "application/x-x509-ca-cert";
    MiMeList["crt"] = "application/x-x509-ca-cert";
    MiMeList["xpi"] = "application/x-xpinstall";
    MiMeList["xhtml"] = "application/xhtml+xml";
    MiMeList["xspf"] = "application/xspf+xml";
    MiMeList["zip"] = "application/zip";
    MiMeList["bin"] = "application/octet-stream";
    MiMeList["dll"] = "application/octet-stream";
    MiMeList["exe"] = "application/octet-stream";
    MiMeList["deb"] = "application/octet-stream";
    MiMeList["dmg"] = "application/octet-stream";
    MiMeList["iso"] = "application/octet-stream";
    MiMeList["img"] = "application/octet-stream";
    MiMeList["msi"] = "application/octet-stream";
    MiMeList["msp"] = "application/octet-stream";
    MiMeList["msm"] = "application/octet-stream";
    MiMeList["mid"] = "audio/midi";
    MiMeList["midi"] = "audio/midi";
    MiMeList["kar"] = "audio/midi";
    MiMeList["mp3"] = "audio/mpeg";
    MiMeList["ogg"] = "audio/ogg";
    MiMeList["m4a"] = "audio/x-m4a";
    MiMeList["ra"] = "audio/x-realaudio";
    MiMeList["3gpp"] = "video/3gpp";
    MiMeList["3gp"] = "video/3gpp";
    MiMeList["ts"] = "video/mp2t";
    MiMeList["mp4"] = "video/mp4";
    MiMeList["mpeg"] = "video/mpeg";
    MiMeList["mpg"] = "video/mpeg";
    MiMeList["mov"] = "video/quicktime";
    MiMeList["webm"] = "video/webm";
    MiMeList["flv"] = "video/x-flv";
    MiMeList["m4v"] = "video/x-m4v";
    MiMeList["mng"] = "video/x-mng";
    MiMeList["asx"] = "video/x-ms-asf";
    MiMeList["asf"] = "video/x-ms-asf";
    MiMeList["wmv"] = "video/x-ms-wmv";
    MiMeList["avi"] = "video/x-msvideo";

    return MiMeList;
}

bool checkVectorDuplicate(stringContainer container, std::string key)
{
    stringContainer::iterator begin = container.begin();
    stringContainer::iterator end = container.end();
    stringContainer::iterator it = std::find(begin, end, key);

    if (it == end)
        return (false);
    return (true);
}