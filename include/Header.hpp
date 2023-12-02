/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Header.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/13 09:46:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/02 22:02:10 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */


//!!WORK IN PLACE NOT IMPLEMENTED!!

#pragma once
#ifndef HEADER_H
# define HEADER_H

#include "defines.hpp"

class Header {

public:

	Header(const std::string& header);
	Header(const Header& origin);
	const Header& operator=(const Header& rhs);
	~Header(void);

// ========== Getters ===========
	std::string							getName(void) const;
	std::string							getValue(void) const;
	std::map<std::string, std::string>	getValueVector(void);
	std::map<std::string, std::string>	getValueMap(void) const;
	int									getValueAmount(void) const;
	bool								valuesArePairs(void) const;
	bool								hasHeader(std::string name) const;

// ========== Setters ===========
	void	setKey(std::string key);
	void	addValueToKey(std::string key, std::string value);
	void	setValue(std::string key, std::string name, std::string value);
	void	mapValues(void);


//========== Other ===============
	void	printHeaders(void);
	
private:

	std::string							_name;
	std::string							_value;
	std::vector<std::string>			_valueVector;
	std::map<std::string, std::string>	_valueMap;
	int									_valueAmount;
	bool								_valuesArePairs;
	bool								_hasHeader;
};

