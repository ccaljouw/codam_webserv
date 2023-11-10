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

class Server
{

	public:

		Server() = delete;
		Server(struct ServerSettings const & settings, int epollFd);
		Server( Server const & src ) = delete;
		Server &operator=( Server const & rhs ) = delete;
		~Server();

		
		void	assign_name();
		void	set_to_listen(int backlog);

		int		get_FD() const;
		
	private:

		
		int 								_fd;
		struct sockaddr_in					_serverAddr; //?
		std::string							_serverName;
		std::string							_rootFolder;
		uint16_t							_port;
		// in_addr_t							_host;
		std::string							_index;
		std::list<struct LocationSettings>	_locations;
		// std::list<ErrorPages>		_errorPages;

} ;

std::list<Server *> initServers(std::list<struct ServerSettings> settings, int epollFd);

#endif /* **************************************************** Server_H */