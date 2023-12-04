/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Header.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/13 09:46:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/04 10:41:05 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//!!WORK IN PLACE NOT IMPLEMENTED!!

#include "header.hpp"

#include <string>
#include <map>


Header(const std::string& header) : _key(), _value(), _valueVector(), _valueMap(), _valueAmount(0), _definesValuePairs(false) {
	
	std::size_t columPos = header.find(":");
	if (columPos != std::string::npos)
	{
		_key	= header.substr(0,columPos);
		_value	= header.substr(columPos + 1);

		//header keys are case insensitive
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		_value = removeWhitespaces(value);
	}

	//parse value to map
	std::istringstream	valueStream(value);
	std::string			valueLine;
	char				headerDelim = "'"; //todo configurable

	while(std::getline(valueStream, valueLine, headerDelim))
	{
		size_t equalPos = valueLine.find('=');
		if (equalPos != std::string::npos)
		{
			_definesKeyValuePairs = true;
			
			std::string name	=	valueLine.substr(0, equalPos);
			std::string value	=	valueLine.substr(equalPos + 1);
			name				=	removeWhitespaces(name);
			value				=	removeWhitespaces(value);
			_valueMap.insert(std::make_pair(name, value));
		}
		else
		{
			_definesKeyValuePairs = false;
			_valueVector.push_back(valueLine);
		}
			
		_valueAmount += 1;
	}
}


Header(const Header& origin) {
	*this = origin;
}

const Header& operator=(const Header& rhs) {
	if (this != &rhs)
	{
		_key						= rhs._key;
		_value						= rhs._value;
		_valueMap.clear()
		_valueMap					= rhs._valueMap;
		_ValueVector.clear()
		_valueVector				= rhs._valueVector;
		_valueAmount				= rhs._valueAmont;
		_definesKeyValuePairs		= rhs._definesValuePairs;
	}
	return *this;
}

~Header(void) {
	_valueMap.clear();
	_valueVector.clear();
}



// ========== Getters ===========
std::string							getName(void) const					{	return _key;				}
std::string							getValue(void) const				{	return _value;				}
std::map<std::string, std::string>	getValueVector(void) const			{	return _valueVector;		}
std::map<std::string, std::string>	getValueMap(void) const				{	return _valueMap;			}
int									getValueAmount(void) const			{	return _valueAmount;		}
bool								valuesArePairs(void) const			{	return _definesValuePairs;	}

bool hasHeader(std::string name) const {
	if (valuesArePairs())
	{
		for (const auto& pair : valueMap) {
			if (pair.first == name) 
				return true;
		}
		return false;
	}
	// else
	// {
	// 	for (const auto& header : valueMap) {

	// }
}



// ========== Setters ===========
void								setKey(std::string key) {
	_key	=	key;
}
void								addValueToKey(std::string key, std::string value) {
	
}
void								setValue(std::string key, std::string name, std::string value) {
	
}
void								mapValues(void) {
	
}

// ========== Other ===========

void	printHeaders(void)	{
	for (const auto& pair : _valueMap)
	 	std::cout << "key: " << pair.first << " |  value: " << pair.second << std::endl;
}
