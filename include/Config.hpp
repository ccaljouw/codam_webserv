/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 14:02:40 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/10 20:41:15 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

// #pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <list>
#include <map>
#include <iostream>
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
	// std::list<ErrorPages>		_errorPages;
};

class Config
{
	class NoSuchFileException : public std::exception
	{
		virtual const char *	what() const throw() {
			return ("No such file exists");
		}
	};

	class InvalidParameterException : public std::exception
	{
		virtual const char *	what() const throw() {
			return ("Invalid parameter in config file");
		}
	};
	
	public:
		Config();
		Config(std::string configFile);
		~Config();
		Config( const Config& src );
		Config &		operator=( Config const & rhs );
		
		std::list<struct ServerSettings>	getServers() const;

		void								setFile(std::string filename);
		
	
	private:
		std::string							_filename;
		std::list<struct ServerSettings>	_servers;

		void								_readServerSettings();
		void								_parseConfigFile();
};

#endif