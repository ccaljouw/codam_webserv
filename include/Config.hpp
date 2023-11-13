/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 14:02:40 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/13 14:01:40 by bfranco       ########   odam.nl         */
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
	std::string					_locationName;
	std::string					_rootFolder;
	std::set<std::string>		_allowedMethods;
	std::string					_index;
	bool						_autoindex;
};

struct ServerSettings
{
	std::string							_serverName;
	std::string							_rootFolder;
	uint16_t							_port;
	// in_addr_t							_host;
	std::string							_index;
	std::list<struct LocationSettings>	_locations;
	std::map<std::string, std::string>	_errorPages;
	double								_timeout;
	int									_maxNrOfRequests;
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