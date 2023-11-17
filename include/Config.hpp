/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 14:02:40 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/17 11:48:57 by bfranco       ########   odam.nl         */
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
#include <cstdint>

struct LocationSettings
{
	std::string					_locationId;
	std::set<std::string>		_allowedMethods;
	std::string					_redirect;
	std::string					_index; //default
	bool						_autoindex;
	std::string					_uploadDir;
};

struct ServerSettings
{
	std::string							_serverName;
	std::string							_rootFolder;
	uint16_t							_port;
	std::string							_index; //default
	std::list<struct LocationSettings>	_locations;
	std::map<std::string, std::string>	_errorPages;
	double								_timeout;
	int									_maxNrOfRequests;
	size_t								_maxBodySize;
};

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
			return ("Invalid parameter");
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

		void						_readConfigFile();
		void						_parseConfigFile(const std::string& line);
		// void						_parseServer(std::string line);    // add later if needed
		// void						_parseLocation(std::string line);  // add later if needed
		// void						_parseErrorPage(std::string line); // add later if needed
};

#endif