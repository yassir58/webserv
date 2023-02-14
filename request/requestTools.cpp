/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requestTools.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ma3ert <yait-iaz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/31 15:57:54 by Ma3ert            #+#    #+#             */
/*   Updated: 2023/02/13 10:17:56 by Ma3ert           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

int isDir(const char* fileName)
{
    struct stat path;

    stat(fileName, &path);

    return S_ISDIR(path.st_mode);
}

int	Request::getCRLF(std::string &newLine, char *delim)
{
	if (start == fileString.length())
	{
		newLine = "";
		return (1);
	}
	pos = fileString.find(delim, start);
	newLine = fileString.substr(start, pos - start);
	if (pos == std::string::npos)
	{
		pos = fileString.length();
		start = pos;
	}
	else
		start = pos + strlen(delim);
	return (newLine.empty());
}

void	Request::printResult(void)
{
	std::cout << "method: " << startLine.method << std::endl;
	std::cout << "tartget: " << startLine.requestTarget << std::endl;
	std::cout << "http version: " << startLine.httpVersion << std::endl;
	std::cout << "\n===============this headerField===============\n" << std::endl;
	headerFieldList::iterator end = headerFields.end();
	headerFieldList::iterator begin = headerFields.begin();
	for (; begin != end; ++begin)
	{
		std::cout << "key: " << begin->key << std::endl;
		std::cout << "value: " << begin->value << std::endl;
	}
	std::cout << "\n=============this body field===============\n";
	// std::vector<std::string>::iterator vend = body.end();
	// std::vector<std::string>::iterator vbegin = body.begin();
	// for (; vbegin != vend; ++vbegin)
	// {
	// 	std::cout << *vbegin;
	// }
	std::cout << getBody();
}

std::string Request::adjustPath(std::string const &prefix, std::string const &sufix)
{
	std::string toReturn;
	if ((*sufix.begin()) == '/' && (*(prefix.end() - 1)) == '/')
	{
		toReturn = sufix.substr(1, std::string::npos);
		toReturn = prefix + toReturn;
		return (toReturn);
	}
	else if ((*sufix.begin() != '/') && (*(prefix.end() - 1) != '/'))
	{
		toReturn = prefix + "/" + sufix;
		return (toReturn);
	}
	toReturn = prefix + sufix;
	return (toReturn);
}