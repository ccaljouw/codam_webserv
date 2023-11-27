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

#ifndef SERVER_HPP
# define SERVER_HPP

#include <eventloop.hpp>
#include "Config.hpp"
#include <string.h>

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
		int		checkClientId(std::string id);
		void	addClientId(std::string newCookieValue);
		void	addSubDomain(struct ServerSettings *settings);

		// ============= getters ================
		uint16_t	get_port(void) const;
		int			get_FD() const;
		std::string	get_serverName(std::string host) const;
		std::string	get_rootFolder(std::string host) const;
		std::string	get_index(std::string host) const;
		const struct LocationSettings*	get_locationSettings(std::string host, std::string location) const;
		std::map<std::string, int>	get_knownClientIds() const;
		double	get_timeout() const;
		int		get_maxNrOfRequests() const;
		size_t	get_maxBodySize(std::string host) const;
		// std::list<ErrorPages>	get_errorPages(std::string host) const;
		struct connection *get_connection();
		
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


		int 								_fd;
		struct sockaddr_in					_serverAddr;
		std::map<std::string, int>			_knownClientIds;
		double								_timeout;
		int									_maxNrOfRequests;
		std::list<struct ServerSettings *>	_settings;
		struct connection					*_conn;
} ;

std::list<Server> initServers(const Config& config, int epollFd);

#endif /* **************************************************** Server_H */