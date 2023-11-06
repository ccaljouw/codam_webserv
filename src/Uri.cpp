/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Uri.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 12:17:27 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 11:07:56 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Uri.h"

#include <regex>
#include <exception>


Uri::Uri() : _scheme(), _authority(), _path(), _query(), _fragment(), _userinfo(), _host(), _port()  {}


// regex teken directly from RFC 2396 : ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))? added R for ignoring escape characterss
Uri::Uri(const std::string& uri) {
	std::regex uriRegex(R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)");
	
	std::smatch matches;
	
	if(std::regex_match(uri, matches, uriRegex)) {
		_scheme 	= matches[2].str();
		_authority 	= matches[4].str();
		_path 		= matches[5].str();
		_query 		= matches[7].str();
		_fragment 	= matches[9].str();
	} else {
		//infvalid uri format
		throw std::invalid_argument("invalid URI format");
	}

	splitAuthority();
	//scheme and host are case insensitive and as such are normalized here
	std::transform(_scheme.begin(), _scheme.end(), _scheme.begin(), ::tolower);
	std::transform(_host.begin(), _host.end(), _host.begin(), ::tolower);
}

Uri::Uri(const Uri& origin) { 
	*this = origin;
}

const Uri& Uri::operator=(const Uri& rhs) {
	if (this != &rhs) {
		this->_scheme		= rhs._scheme;
		this->_authority	= rhs._authority;
		this->_path			= rhs._path;
		this->_query		= rhs._query;
		this->_fragment		= rhs._fragment;
		this->_userinfo		= rhs._userinfo;
		this->_host			= rhs._host;
		this->_port			= rhs._port;
	}
	return *this;
}


//==============================

void	Uri::splitAuthority() {

	//check for optional userinfo	
	size_t atPos = _authority.find("@");
	if (atPos != std::string::npos) {
		_userinfo 	= _authority.substr(0, atPos);
		_host 		= _authority.substr(atPos + 1);
	} 
	else
		_host = _authority;

	//check for optional port
	size_t columPos = _host.find(":");
	if (columPos != std::string::npos) {
		_port = stoi(_host.substr(columPos + 1));
		_host = _host.substr(0, columPos);
	}
}

std::string	Uri::serializeUri() {
	
	std::string serializedUri;

	if (!_scheme.empty())
		serializedUri += _scheme + ":";

	if (!_authority.empty())
		serializedUri += "//" + _authority;

	if (!_path.empty()) {
		if (_path[0] != '/')
			serializedUri += "/";
		serializedUri += _path;
	}
	
	if (!_query.empty())
		serializedUri += "?" + _query;
	
	if (!_fragment.empty())
		serializedUri += "#" + _fragment;
		
	return serializedUri;
}

//=========== getters =================

std::string	Uri::getScheme() const		{	return _scheme;		}
std::string	Uri::getAuthority() const	{	return _authority;	}
std::string	Uri::getPath() const		{	return _path; 		}
std::string	Uri::getQuery() const		{	return _query;		}
std::string	Uri::getFragment() const	{	return _fragment;	}
std::string	Uri::getUserInfo() const	{	return _userinfo;	}
std::string	Uri::getHost() const		{	return _host;		}
int			Uri::getPort() const		{	return _port;		}
