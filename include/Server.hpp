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
		int	assign_name();
		int	set_to_listen(int backlog);
		int	initServer(struct ServerSettings const & settings, int epollFd);
		void	addClientId(std::string newCookieValue);

		// ============= getters ================
		int			get_FD() const;
		std::string	get_serverName() const;
		std::string	get_rootFolder() const;
		std::string	get_index() const;
		std::list<struct LocationSettings>	get_locations() const;
		std::map<std::string, int>	get_knownClientIds() const;
		double	get_timeout() const;
		int		get_maxNrOfRequests() const;
		// std::list<ErrorPages>	get_errorPages() const;
		

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
		struct sockaddr_in					_serverAddr; //?
		std::string							_serverName;
		std::string							_rootFolder;
		uint16_t							_port;
		// in_addr_t							_host;
		std::string							_index;
		std::list<struct LocationSettings>	_locations;
		std::map<std::string, int>			_knownClientIds;
		double								_timeout;
		int									_maxNrOfRequests;

		// std::list<ErrorPages>		_errorPages;

} ;

std::list<Server> initServers(std::list<struct ServerSettings> settings, int epollFd);

#endif /* **************************************************** Server_H */