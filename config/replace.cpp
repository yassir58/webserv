#include <iostream>
#include <string>
#include <fstream>

std::string    replace_line(std::string line, std::string needle, std::string added)
{
    int i;
    int lineLength;
    int pos;

    i = 0;
    std::cout << needle << std::endl;
    lineLength = line.length();
    pos = line.find(needle);
    while (pos != -1 && i < lineLength && needle.compare(""))
    {
        line.erase(pos, needle.length());
        line.insert(pos, added);
        pos = line.find(needle, i);
        i = pos + needle.length();
    }
    return line;
}

int main(int argc, char **argv)
{
    std::string line;
    std::string fileName;
    std::string str1;
    std::string str2;

    if (argc > 3)
    {
        str1.assign(argv[2]);
        str2.assign(argv[3]);
        fileName.assign(argv[1]);
        std::ifstream originalFile;
        std::ofstream replaceFile;

        originalFile.open(fileName);
        if (originalFile.is_open())
        {
            replaceFile.open(fileName.append(".replace"));
            while (getline(originalFile, line))
            {
                line = replace_line(line, str1, str2);
                replaceFile << line << std::endl;
            }
            originalFile.close();
            replaceFile.close();
        }
        else
            std::cout << "Error could not open the file." << std::endl;
    }
    return (0);
}
