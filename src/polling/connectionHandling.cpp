/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   connectionHandling.cpp                             :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/28 15:15:47 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include <arpa/inet.h>
#include <fstream>

void	newConnection(int epollFd, int serverFd, Server *server) 
{
	int					fd;

	std::cout << "new connection request" << std::endl; // for testing
	try {
		if ((fd = accept(serverFd, nullptr, nullptr)) == -1)
			throw std::runtime_error("client accept: " + std::string(strerror(errno)));
		if (register_client(epollFd,  fd, server))
			throw std::runtime_error("register client: " + std::string(strerror(errno)));
	}
	catch (std::runtime_error& e) {
		std::cerr << RED << "Error\n" << e.what() << RESET << std::endl;
	}	
}

void readData(connection *conn) 
{
    char 	buffer[BUFFER_SIZE];
    ssize_t bytesRead;

	if (conn->nr_of_requests == conn->server->get_maxNrOfRequests()) {
		std::cout << CYAN << "Too many requests on open socket, closing connection" << RESET << std::endl;
		setErrorResponse(conn, 429);
		return;
	}
	if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) {
		// std::cout << YELLOW << buffer << RESET << std::endl; //for testing
		std::time(&conn->time_last_request);
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
	}
	switch(bytesRead)
	{
		case -1:
			setErrorResponse(conn, 500);
			break;
		case 0:
			checkTimeout(conn);
			break;
		case BUFFER_SIZE:
			conn->state = READING;
			break;
		default:
			conn->nr_of_requests += 1;
			conn->state = HANDLING;
	}
}

void readCGI(int epollFd, connection *conn)
{
	char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	// std::cout << "read data CGI" << "\tfd = " << conn->cgiFd << std::endl; //for testing
    if ((bytesRead = read(conn->cgiFd, buffer, BUFFER_SIZE)) > 0) {
		// std::cout << BLUE << "Appending" << RESET << std::endl; // for testing
		conn->response.append(buffer, static_cast<long unsigned int>(bytesRead));
    }
	if (bytesRead < BUFFER_SIZE)
	{
		// std::cout << BLUE << buffer << RESET << std::endl;
		close(conn->cgiFd);
		epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->cgiFd, nullptr);
		conn->state = WRITING;
	}
	if (conn->response.empty() || bytesRead == -1)
	{
		std::cerr << RED << "Error\nproblem reading CGI" << RESET << std::endl;
		setErrorResponse(conn, 500);
	}
	// std::cout << BLUE << "OUT OF READ cgi" << RESET << std::endl; //for testing
}

void writeData(connection *conn) 
{
	int			len;
	
	conn->state = WRITING;
	// std::cout << PURPLE << conn->response << RESET << std::endl; //for testing
	len = std::min(static_cast<int>(conn->response.length()), BUFFER_SIZE);
    len = send(conn->fd, conn->response.c_str(), conn->response.length(), 0);
	if (len == -1)
	{
		std::cout << "error sending" << std::endl; // for testing
		conn->state = CLOSING;
	}
	else if (len < static_cast<int>(conn->response.length()))
	{
		std::cout << "not finished writing yet" << std::endl; // for testing
		conn->response = conn->response.substr(len, conn->response.npos);
		conn->state = WRITING;
	}
	else
	{
		std::cout << "Response sent, nr of requsts: " << conn->nr_of_requests << std::endl; //for testing
		conn->response.clear();
		if (conn->close_after_response)
			conn->state = CLOSING;
		else
			conn->state = READING;
	}
}

void	closeConnection(int epollFd, connection *conn)
{
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, nullptr);
	if (conn->cgiFd) {
		std::cout << CYAN << "Connection on CGIfd " << conn->cgiFd << " closed" << RESET << std::endl;
		close (conn->cgiFd);
		conn->cgiFd = 0;
	}
	else {
		std::cout << CYAN << "Connection on fd " << conn->fd << " closed" << RESET << std::endl;
		close(conn->fd);
		delete conn;
	}
}

