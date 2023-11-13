/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 14:02:40 by bfranco           #+#    #+#             */
/*   Updated: 2023/11/13 16:47:27 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <list>
#include <map>
#include <iostream>
#include <unistd.h>
#include <set>

struct LocationSettings
{
	std::string					_locationId;
	std::set<std::string>		_allowedMethods;
	std::string					_index; //default
	bool						_autoindex;
	std::string					_redirect;
	std::string					_uploadDir;
};

struct ServerSettings
{
	std::string							_serverName;
	std::string							_rootFolder;
	std::string							_host;
	uint16_t							_port;
	std::string							_index; //default
	std::list<struct LocationSettings>	_locations;
	// std::list<ErrorPages>		_errorPages;
	double								_timeout;
	int									_maxNrOfRequests;
	size_t								_maxBodySize;
};

// struct LocationSettings
// {
// 	std::string					_locationName;
// 	std::string					_rootFolder;
// 	std::set<std::string>		_allowedMethods;
// 	std::string					_index;
// 	bool						_autoindex;
// };

// struct ServerSettings
// {
// 	std::string							_serverName;
// 	std::string							_rootFolder;
// 	uint16_t							_port;
// 	// in_addr_t							_host;
// 	std::string							_index;
// 	std::list<struct LocationSettings>	_locations;
// 	// std::list<ErrorPages>		_errorPages;
// 	double								_timeout;
// 	int									_maxNrOfRequests;
// };

class Config
{
	class NoSuchFileException : public std::exception
	{
		virtual const char *	what() const throw() {
			return ("Invalid config file or wrong permissions");
		}
	};

	class InvalidParameterException : public std::exception
	{
		virtual const char *	what() const throw() {
			return ("Invalid parameter in config file");
		}
	};
	
	public:
		Config(int argc, char **argv);
		~Config();
		Config( const Config& src );
		Config &		operator=( Config const & rhs );
		
		std::list<struct ServerSettings>	getServers() const;
		bool								getError() const;
	
	private:
		std::string							_filename;
		std::list<struct ServerSettings>	_servers;
		bool								_error;

		void						_readServerSettings();
		int							_parseConfigFile();
		// void						_parseServer(std::string line);    // add later if needed
		// void						_parseLocation(std::string line);  // add later if needed
		// void						_parseErrorPage(std::string line); // add later if needed
};

#endif