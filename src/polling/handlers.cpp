/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaljouw <ccaljouw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 23:45:10 by cariencaljo       #+#    #+#             */
/*   Updated: 2023/11/13 12:08:01 by ccaljouw         ###   ########.fr       */
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
		std::cerr << "\033[31;1mError\n" << e.what() << "\n\033[0m";
	}	
}

void readData(connection *conn) 
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

	if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) // what recv flag to use??
	{
		std::cout << YELLOW << buffer << RESET << std::endl; //for testing
		std::time(&conn->time_last_request);
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
	}
	switch(bytesRead)
	{
		case -1:
			std::cerr << "error reading" << std::endl; // for testing
			setErrorResponse(conn, 500); // segfault?
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

// TODO: switch tree with utils helpers, add cookieId to other methods
void handleRequest(int epollFd, connection *conn) 
{
	try {
		// Process the request data
		HttpRequest request(conn->request, conn->server);
		
		std::string cookieValue = checkAndSetCookie(conn, request);
	
			// Handle parsing error
		if (request.getRequestStatus() != 200) {
			setErrorResponse(conn, request.getRequestStatus());
		} 
	
		
		// handle CGI
		else if (request.uri.getExecutable() == "cgi-bin") { //todo:make configurable
			//case error in cgi handler
			if (cgiHandler(request.getUri(), conn, epollFd, request.getEnvArray()) == 1 ) 
				setErrorResponse(conn, 500);	
			else
			{
				// conn->state = CLOSING;
				conn->request.clear();
				conn->state = IN_CGI;
			}
		
		} else {
			std::string extension = request.uri.getExtension();
			std::string contentType = request.uri.getMime(extension);
			
			// todo:check allowed methods for contentType
			
			//handle GET
			if (request.getMethod() == "GET")
			{
				if (!contentType.empty())
				{
					std::string fullPath = "data/" + contentType + request.uri.getPath();
					std::ifstream f(fullPath);
					if (f.good())
						request.uri.setPath(fullPath);
					
					else
						throw HttpRequest::parsingException(404, "Path not found");
		
					HttpResponse response(request);
					response.setBody(request.uri.getPath());
					response.addHeader("Content-type", contentType);

					response.setHeader("Set-Cookie", cookieValue);
					setResponse(conn, response);
				} 
					
				else
					throw HttpRequest::parsingException(501, "Extension not supported");
			}
		
		
			//handle POST		
			else if (request.getMethod() == "POST")
			{
				if (!contentType.empty())
				{
					std::cout << "add code for cgi POST" << std::endl;
					throw HttpRequest::parsingException(405, "POST METHOD not supported yet"); // todo: remove line
				}
				else
					throw HttpRequest::parsingException(501, "Extension not supported"); 
			}

			// handle DELETE
			else if (request.getMethod() == "DELETE")
			{
				if (!contentType.empty())
				{
					std::cout << "add code for cgi DELETE" << std::endl;
					throw HttpRequest::parsingException(405, "DELETE METHOD not supported yet"); // todo: remove line
				}
				else
					throw HttpRequest::parsingException(501, "Extension not supported");
			}
			
			// handle unsupported methods
			else
				throw HttpRequest::parsingException(405, "METHOD not supported");
		}
	} 
	catch (const HttpRequest::parsingException& exception)
	{
		std::cout << "Error: " << exception.what() << std::endl;
		setErrorResponse(conn, exception.getErrorCode());
	}
}

void readCGI(int epollFd, connection *conn)
{
	char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	std::cout << "read data CGI" << std::endl;
    if ((bytesRead = read(conn->cgiFd, buffer, sizeof(buffer))) > 0) {
		conn->response.append(buffer, static_cast<long unsigned int>(bytesRead));
    }
	if (bytesRead < BUFFER_SIZE)
	{
		std::cout << BLUE << buffer << RESET << std::endl;
		close(conn->cgiFd);
		epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->cgiFd, nullptr);
		conn->state = WRITING;
	}
	if (conn->response.empty() || bytesRead == -1)
	{
		setErrorResponse(conn, 500);
		std::cerr << "\033[31;1mError\nproblem reading CGI\033[0m" << std::endl;
	}
}

void writeData(connection *conn) 
{
	int			len;
	
	conn->state = WRITING;
	std::cout << PURPLE << conn->response << RESET << std::endl;
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
		if (conn->nr_of_requests == conn->server->get_maxNrOfRequests())
		{
			std::cout << CYAN << "\033[31;1mMax requests on open socket\033[0m" << RESET << std::endl;
			conn->state = CLOSING;
		}
		else
			conn->state = READING;
	}
}

void	closeConnection(int epollFd, connection *conn)
{
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, nullptr);
    close(conn->fd);
    std::cout << CYAN << "Connection on fd " << conn->fd << " closed" << RESET << std::endl;
	delete conn;
}

