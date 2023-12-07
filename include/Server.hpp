/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Server.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: cariencaljouw <cariencaljouw@student.co      +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 11:11:31 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/03 17:32:19 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_HPP
# define SERVER_HPP

#include "defines.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Server
{

	public:

		// ============= con-/destructors ================
		Server();
		Server( Server const & src );
		Server &operator=( Server const & rhs ) = delete;
		~Server();

		// ============= methods ================
		int		assign_name();
		int		set_to_listen(int backlog);
		int		initServer(struct ServerSettings *settings, int epollFd, double timeout, int maxNrOfRequests);
		int		checkClientId(std::string id, struct connection* conn);
		void	addClientId(std::string newCookieValue);
		void	addSubDomain(struct ServerSettings *settings);

		// ============= getters ================
		uint16_t							get_port(void) const;
		int									get_FD() const;
		struct sockaddr_in					get_serverAddr() const;
		double								get_timeout() const;
		int									get_maxNrOfRequests() const;
		std::map<std::string, int>			get_knownClientIds() const;
		std::list<struct ServerSettings *>	get_settings() const;
		struct connection*					get_connection() const;
		
		struct ServerSettings*		get_hostSettings(std::string host) const;
		struct LocationSettings*	get_locationSettings(std::string host, std::string location) const;
		
		std::string					get_serverName(std::string host) const;
		std::map<int, std::string>*	get_errorPages(std::string host) const;
		size_t						get_maxBodySize(std::string host) const;

		std::string					get_index(std::string host, std::string location) const;
		bool						get_dirListing(std::string host, std::string location) const;
		std::map<int, std::string>	get_redirect(std::string host, std::string location) const;
		std::string					get_rootFolder(std::string host, std::string location) const;
		size_t						get_maxBodySize(std::string host, std::string location) const;
		std::string					get_uploadDir(std::string host, std::string location) const;
		
		// ============= setters ================
		void	set_connection(struct connection *conn);

		// ============= exception ================
		class ServerException : public std::exception {
			public:
				ServerException(const std::string& message) {
					_message = message + std::string(strerror(errno));
				}
				const char*	what() const noexcept override		{	return _message.c_str();	}

			private:
				std::string						_message;
		};
	
	private:
		std::string							get_rootFolder(std::string host) const;
		std::string							get_uploadDir(std::string host) const;

		int 								_fd;
		struct sockaddr_in					_serverAddr;
		std::map<std::string, int>			_knownClientIds;
		double								_timeout;
		int									_maxNrOfRequests;
		std::list<struct ServerSettings *>	_settings;
		struct connection					*_conn;
} ;

#endif /* **************************************************** Server_H */