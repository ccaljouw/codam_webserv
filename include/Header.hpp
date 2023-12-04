/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Header.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/13 09:46:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/04 10:52:34 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

//!!WORK IN PLACE NOT IMPLEMENTED!!

#pragma once
#ifndef HEADER_H
# define HEADER_H

#include "webServ.hpp"
#include <string>
#include <map>

class Header {

public:

	Header(const std::string& headerBlock);
	Header(const Header& origin);
	const Header& operator=(const Header& rhs);
	~Header(void);

// ========== Getters ===========
	std::map<std::string, std::string>	getHeaders(void) const;
	std::string							getHeaderValue(std::string) const;
	std::vector<char*>					getHeaderVector(void) const;
	bool								isHeader(std::string key) const;

// ========== Setters ===========
	void								addHeader(const std::string& key, const std::string& value);
	void								setHeader(const std::string& key, const std::string& value);
	void								fillStandardHeaders(void);


//========== Other ===============
	void								printHeaders(void);

private:
	std::string							_headerString;
	std::map<std::string, std::string>	_headerMap;
};

#endif
