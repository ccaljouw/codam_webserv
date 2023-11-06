/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 23:45:10 by cariencaljo       #+#    #+#             */
/*   Updated: 2023/11/06 15:07:53 by ccaljouw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <eventloop.hpp>
#include <webServ.hpp>
#include <unistd.h>

// TODO: check errors, check duplicates
// TODO: check what to do if backlog is full?
void	newConnection(int epollFd, int fd) 
{
	std::cout << "new connection request" << std::endl;
	register_client(epollFd,  fd);
}

// TODO: check errors
void readData(connection *conn) 
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	std::cout << "read data" << std::endl;
    if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) {
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
		std::cout << "bytes read: " << bytesRead << std::endl;
		if (bytesRead < BUFFER_SIZE && !conn->request.empty())
			conn->state = HANDLING;
    }
   	else if (conn->request.empty()) {
		std::cout << "nothing to read" << std::endl;
		conn->state = CLOSING;
    }
	else
		conn->state = READING;
}

// TODO: a lot :)
void handleRequest(int epollFd, connection *conn) 
{
    // Process the request data
	std::cout << "handle request" << std::endl;
	HttpRequest request(conn->request);
	HttpResponse response(request);
	// if (to CGI)
		cgiHandler(request.getUri(), response, epollFd);
		conn->state = IN_CGI;
	// else
	// {
	// 	conn->response = response.serializeResponse();
	// 	conn->request.clear();
	// 	std::cout << "Response ready" << std::endl;
	// 	conn->state = WRITING; // change to response ready status?
	// }
}

void readCGI(connection *conn)
{
	char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	// read CGI pipe and return string;
	// conn->response = string;
	
	if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) 
	{
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
		std::cout << "bytes read: " << bytesRead << std::endl;
	}
	conn->request.clear();
	std::cout << "Response ready" << std::endl;
	conn->state = WRITING; 
}

// TODO: check errors
void writeData(connection *conn) 
{
	size_t	len;
	
	conn->state = WRITING;
	std::cout << "write data" << std::endl;
	len = std::min(conn->response.length(), static_cast<size_t>(BUFFER_SIZE));
    len = send(conn->fd, conn->response.c_str(), conn->response.length(), 0);
	if (len < conn->response.length())
	{
		conn->response = conn->response.substr(len, conn->response.npos);
		conn->state = WRITING;
	}
	else
	{
		conn->response.clear();
		std::cout << "Response sent" << std::endl;
		conn->state = CONNECTED;
	}
}

// TODO: handle errors
void	closeConnection(int epollFd, connection *conn)
{
	std::cout << "close connection" << std::endl;
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, nullptr);
    close(conn->fd);
	delete conn;
    std::cout << "Connection closed" << std::endl;
}

void	handleError(connection *conn)
{
	conn->state = CLOSING;
}