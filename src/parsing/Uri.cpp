/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Uri.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 12:17:27 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/10 09:57:04 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Uri.hpp"
#include "HttpRequest.hpp"

#include <regex>
#include <exception>
#include <sstream>
// #include <unistd.h>
#include <cstring>

//todo: move this to config class
std::vector<std::string> acceptedExtensions = {
	".txt",
	".css",
	".html",
	".json",
	".jgeg",
	".jpg",
	".png",
	".gif",
	".bmp",
	".ico",
	".pdf",
	".csv",
};
	
	
Uri::Uri() : _scheme(), _authority(), _path(), _query(), _queryMap(), _fragment(), _userinfo(), _host(), _port()  {}


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
		throw HttpRequest::parsingException(400, "bad request");
	}

	splitAuthority();
	//scheme and host are case insensitive and as such are normalized here
	std::transform(_scheme.begin(), _scheme.end(), _scheme.begin(), ::tolower);
	std::transform(_host.begin(), _host.end(), _host.begin(), ::tolower);

	mapQueries();
}

Uri::Uri(const Uri& origin) { 
	*this = origin;
}

const Uri& Uri::operator=(const Uri& rhs) {
	if (this != &rhs) {
		_scheme			= rhs._scheme;
		_authority		= rhs._authority;
		_path			= rhs._path;
		_query			= rhs._query;
		_queryMap.clear();
		_queryMap		= rhs._queryMap;
		_fragment		= rhs._fragment;
		_userinfo		= rhs._userinfo;
		_host			= rhs._host;
		_port			= rhs._port;
	}
	return *this;
}

Uri::~Uri() {
	_queryMap.clear();
}



//==============================

void	Uri::splitAuthority() {

	//check for optional userinfo	
	size_t atPos	= _authority.find("@");
	if (atPos != std::string::npos) {
		_userinfo	= _authority.substr(0, atPos);
		_host		= _authority.substr(atPos + 1);
	} 
	else
		_host = _authority;

	//check for optional port
	size_t columPos	= _host.find(":");
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

std::string	Uri::getScheme() const								{	return _scheme;		}
std::string	Uri::getAuthority() const							{	return _authority;	}
std::string	Uri::getPath() const								{	return _path; 		}
std::string	Uri::getQuery() const								{	return _query;		}
std::string	Uri::getFragment() const							{	return _fragment;	}
std::string	Uri::getUserInfo() const							{	return _userinfo;	}
std::string	Uri::getHost() const								{	return _host;		}
int			Uri::getPort() const								{	return _port;		}
std::map<std::string, std::string> Uri::getQueryMap(void) const	{	return _queryMap;	}



std::string	Uri::getExecutable(void) const {
	std::string temp = _path;
	if (temp[0] == '/')
		temp = temp.substr(1);
	size_t slash = temp.find("/");
	if (slash != std::string::npos) {
		return temp.substr(0, slash);
	}
	return ""; //todo error
}


std::string	Uri::getPathInfo(void) const {
	std::string temp = _path;
	if (temp[0] == '/')
		temp = temp.substr(1);
	size_t slash = temp.find("/");
	if (slash != std::string::npos) {
		return  temp.substr(slash + 1);
	}
	return "";	//todo error
}


//=========== setters =================
void Uri::mapQueries() {
	
	std::istringstream qurieStream(_query);
	
	std::string token;
	while(getline(qurieStream, token, '&')) {
		size_t equalPosition = token.find('=');
		if (equalPosition != std::string::npos) {
			std::string key = token.substr(0, equalPosition);
			std::string value = token.substr(equalPosition + 1); //todo: URL-decode key and value is needed
			_queryMap[key] = value;
		}
		else {
			_queryMap[token] = "";
		}
	}
}

bool	Uri::isValidExtension()
{
	size_t periodPos = _path.rfind('.');
	if (periodPos != std::string::npos) {
		std::string actualExtention = _path.substr(periodPos);
		for (std::string& checker : acceptedExtensions)
			if (checker == actualExtention) {
				std::cout << "matched extention: " << checker << "\n" << std::endl; 	//todo remove
				return true;
			}
	}
	std::cout << "no matching extention for this path " << _path << "\n" << std::endl; 	//todo remove
	return false;
}
