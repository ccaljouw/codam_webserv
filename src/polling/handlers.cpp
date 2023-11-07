/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handlers.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/07 22:21:40 by carlo         ########   odam.nl         */
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
void readData(int epollFd, connection *conn) 
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
    if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) {
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
		std::cout << "\n\033[0;33m" << buffer << "\033[0m\n" << std::endl;
    }
   	else if (conn->request.empty()) //add timeout?
	{
		std::cout << "nothing to read" << std::endl;
		conn->state = CLOSING;
    }
	if (bytesRead < BUFFER_SIZE && !conn->request.empty())
	{
		conn->state = HANDLING;
		modifyEvent(epollFd, EPOLLOUT, conn);
	}
}

// TODO: a lot :)
void handleRequest(int epollFd, connection *conn) 
{
	(void)epollFd;

	try {
		// Process the request data
		HttpRequest request(conn->request);
	
		//case parsing error:
		if (request.getRequestStatus() != 200) {
			conn->response = HttpResponse(request).serializeResponse();
			conn->request.clear();
			conn->state = WRITING;
		} 
		else if (request.uri.getExecutable() == "cgi-bin") { //todo:make configurable
			// call CGI handler
			//case error in cgi handler
			if (cgiHandler(request.getUri(), conn, epollFd, request.getEnvArray()) == 1 ) 
			{
				HttpResponse respons(request);
				respons.setStatusCode(500);
				conn->response = respons.serializeResponse();
				conn->request.clear();
				conn->state = WRITING; // change to response ready status?		
			} 
			else {
				conn->state = IN_CGI;
			}
		}	
		else if (request.getMethod() == "GET") {
				HttpResponse response(request);
				response.setBody("." + request.uri.getPath()); //todo ugly solution maybe better parsing?
				conn->response = response.serializeResponse();
				conn->request.clear();
				conn->state = WRITING;
		}
		else { //anyrhing but CGI and GET for now
			HttpResponse response(request);
			conn->response = response.serializeResponse();
			conn->request.clear();
			conn->state = WRITING;
		}
	} catch (const HttpRequest::parsingException& exception) {
		std::cout << "Error: " << exception.what() << std::endl;
		HttpResponse response;
		response.setStatusCode(404);
		conn->response = response.serializeResponse();
		conn->request.clear();
		conn->state = WRITING;
	}
}

void readCGI(int epollFd, connection *conn)
{
	char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	std::cout << "read data CGI" << std::endl;
    if ((bytesRead = read(conn->cgiFd, buffer, sizeof(buffer))) > 0) {
		std::cout << "bytes read: " << bytesRead << " :\n" << buffer << std::endl;
		conn->response.append(buffer, static_cast<long unsigned int>(bytesRead));
    }
	else if (conn->response.empty())
	{
		std::cout << "nothing to read" << std::endl;
		epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->cgiFd, nullptr);
		close(conn->cgiFd);
    }
	if (bytesRead < BUFFER_SIZE && !conn->response.empty())
	{
		std::cout << "close pipe" << std::endl;
		epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->cgiFd, nullptr);
		close(conn->cgiFd);
		conn->state = WRITING;
	}
}

// TODO: check errors
void writeData(int epollFd, connection *conn) 
{
	size_t	len;
	
	conn->state = WRITING;
	std::cout << "\n\033[34;1m" << conn->response << "\033[0m\n" << std::endl;
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
		modifyEvent(epollFd, EPOLLIN, conn);
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
	std::cout << "errorHandler" << std::endl;
	conn->state = CLOSING;
}