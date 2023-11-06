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

class Server
{

	public:

		Server(uint16_t port, int epollFd);
		~Server();

		Server &		operator=( Server const & rhs );
		
		void	assign_name();
		void	set_to_listen(int backlog);

		int		get_FD();
		
	private:

		Server();
		Server( Server const & src );
		
		int 				_fd;
		int 				_port;
		struct sockaddr_in	_serverAddr;

} ;

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* **************************************************** Server_H */