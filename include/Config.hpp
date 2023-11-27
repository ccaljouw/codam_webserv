/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Config.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/09 14:02:40 by bfranco       #+#    #+#                 */
/*   Updated: 2023/11/29 10:08:30 by carlo         ########   odam.nl         */
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

#define LETTERS "abcdefghijklmnopqrstuvwxyz"
#define NUMBERS "0123456789"
#define WHITESPACE " \t\n\v\f\r"

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
	std::string								_index;
	bool									_dirListing;
	std::string								_locationRoot;
};

struct ServerSettings
{
	std::string								_serverName;
	std::string								_rootFolder;
	std::string								_uploadDir;
	std::string								_index; //default
	uint16_t								_port;
	size_t									_maxBodySize;
	std::list<struct LocationSettings *>	_locations;
	std::map<int, std::string>*				_errorPages;
	// double									_timeout;
	// int										_maxNrOfRequests;
};

class Config
{
	public:
		class NoSuchFileException : public std::exception
		{
			virtual const char *	what() const throw() {
				return ("Invalid config file or wrong permissions");
			}
		};

		class InvalidParameterException : public std::exception
		{
			std::string	_msg;
			public:
				InvalidParameterException(int lineNr)
				{
					_msg = std::string("Line ") + std::to_string(lineNr) + std::string(": Invalid parameter");
				}
				virtual const char *	what() const throw() {
					return (_msg.c_str());
				}
		};

		class MissingParameterException : public std::exception
		{
			virtual const char *	what() const throw() {
				return ("Missing mandatory parameter");
			}
		};
	
		Config(int argc, char **argv);
		~Config();
		Config( const Config& src );
		Config&	operator=( Config const & rhs );
		
		std::list<struct ServerSettings*>	getServers() const;
		std::map<int, std::string>*			getErrorPages() const;
		bool								getError() const;
		void								printServers() const;
	
	private:
		std::string							_filename;
		std::list<struct ServerSettings*>	_servers;
		bool								_error;
		unsigned int						_lineNr;
		// double									_timeout; // to add in the future
		// int										_maxNrOfRequests; // to add in the future

		void								_readConfigFile();
		int									_handleBlockEnd(configBlock *currentBlock, void *currentBlockPtr);
		int									_handleBlockContent(std::string line, configBlock currentBlock, void *currentBlockPtr);
		void								_checkMandatoryParameters(const struct ServerSettings *server);
};

int		parseServer(std::string line, struct ServerSettings *server);
int		parseLocation(std::string line, struct LocationSettings *location);
int		parseErrorPage(std::string line, std::map<int, std::string> *errorPages);
void	*initServerBlock();
void	*initLocationBlock(std::string line);

#endif
