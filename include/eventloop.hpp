/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eventloop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 18:13:17 by cariencaljo       #+#    #+#             */
/*   Updated: 2023/11/06 14:22:15 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EVENTLOOP_HPP
# define EVENTLOOP_HPP

# include <iostream>
# include <string>
# include <unistd.h>

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/epoll.h>

# include <Server.hpp>

# define MAX_EVENTS 10
# define BUFFER_SIZE 5

enum	states {
	LISTENING,
	CONNECTED,
	READING,
	HANDLING,
	CGI,
	WRITING,
	CLOSING
};

struct connection
{
	int			fd;
	states		state;
	std::string	request;
	std::string	response;
};

// handlers.cpp
void	newConnection(int epollFd, int fd);
void	readData(connection *conn);
void	handleRequest(int epollFd, connection *conn);
void 	readCGI(connection *conn);
void	handleError(connection *conn);
void	writeData(connection *conn);
void	closeConnection(int epollFd, connection *conn);

// registerEvents.cpp
void	register_server(int epollFd, int fd);
void	register_client(int epollFd, int serverFd);
void	register_CGI(int epollFd, int serverFd);

#endif