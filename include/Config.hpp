/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 14:02:40 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/27 11:56:27 by bfranco       ########   odam.nl         */
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

enum configBlock
{
	NONE,
	SERVER,
	LOCATION,
	ERROR_PAGE
};

struct LocationSettings
{
	std::string								_locationId;
	std::set<std::string>					_allowedMethods;
	std::map<int, std::string>				_redirect;
	std::string								_index; //default
	bool									_autoindex;
};

struct ServerSettings
{
	std::string								_serverName;
	std::string								_rootFolder;
	uint16_t								_port;
	std::string								_index; //default
	std::list<struct LocationSettings *>	_locations;
	std::map<int, std::string>*				_errorPages;
	double									_timeout;
	int										_maxNrOfRequests;
	size_t									_maxBodySize;
	std::string								_uploadDir;
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
		Config&	operator=( Config const & rhs );
		
		std::list<struct ServerSettings*>	getServers() const;
		bool								getError() const;
	
	private:
		std::string							_filename;
		std::list<struct ServerSettings*>	_servers;
		bool								_error;
		unsigned int						_lineNr;

		void								_readConfigFile();
		int									_handleBlockEnd(configBlock *currentBlock, void *currentBlockPtr);
		int									_handleBlockContent(std::string line, configBlock currentBlock, void *currentBlockPtr);
};

int		parseServer(std::string line, struct ServerSettings *server);
int		parseLocation(std::string line, struct LocationSettings *location);
int		parseErrorPage(std::string line, std::map<int, std::string> *errorPages);
void	*initServerBlock();
void	*initLocationBlock(std::string line);

#endif
