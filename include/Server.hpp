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

		// ============= con-/destructors ================
		Server();
		Server( Server const & src );
		Server &operator=( Server const & rhs ) = delete;
		~Server();

		// ============= methods ================
		int	assign_name();
		int	set_to_listen(int backlog);
		int	initServer(struct ServerSettings const & settings, int epollFd);

		// ============= getters ================
		int			get_FD() const;
		std::string	get_serverName() const;
		std::string	get_rootFolder() const;
		std::string	get_index() const;
		std::list<struct LocationSettings>	get_locations() const;
		// std::list<ErrorPages>	get_errorPages() const;
	
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

std::list<Server> initServers(std::list<struct ServerSettings> settings, int epollFd);

#endif /* **************************************************** Server_H */