/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handlers.cpp                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/11 23:10:14 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include <arpa/inet.h>


// TODO: check errors, check duplicates
void	newConnection(int epollFd, int serverFd, Server *server) 
{
	int					fd;
	// struct sockaddr_in 	address;
	// socklen_t			len = sizeof(address);
	// struct timeval tv;

	// tv.tv_usec  = 20000;
	std::cout << "new connection request" << std::endl; // for testing
	try {
		// if ((fd = accept(serverFd, reinterpret_cast<struct sockaddr*>(&address), &len)) == -1)
		if ((fd = accept(serverFd, nullptr, nullptr)) == -1)
			throw std::runtime_error("client accept: " + std::string(strerror(errno)));
		// if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval)) != 0)
		// 	throw std::runtime_error("client setsocket: " + std::string(strerror(errno)));
		if (register_client(epollFd,  fd, server))
			throw std::runtime_error("register client: " + std::string(strerror(errno)));
	}
	catch (std::runtime_error& e) {
		std::cerr << "\033[31;1mError\n" << e.what() << "\n\033[0m";
	}
	// std::cout << "adress is: " << address.sin_addr.s_addr << std::endl;
	// std::cout << "IP is: " << inet_ntoa(address.sin_addr) << std::endl;
	// std::cout << "port is: " << address.sin_port << std::endl;
	
}

void readData(connection *conn) 
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
	
	if ((bytesRead = recv(conn->fd, buffer, sizeof(buffer), 0)) > 0) // what recv flag to use??
	{
		std::cout << "\n\033[0;33m" << buffer << "\033[0m\n" << std::endl; //for testing
		conn->request.append(buffer, static_cast<long unsigned int>(bytesRead));
	}
	switch(bytesRead)
	{
		case -1:
			std::cerr << "error reading" << std::endl; // for testing
			setErrorResponse(conn, 500);
			break;
		case 0:
			std::cerr << "nothing to read" << std::endl; //for testing
			checkTimeout(conn);
			break;
		case BUFFER_SIZE:
			conn->state = READING;
			break;
		default:
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
				// conn->state = CLOSING;
				conn->state = IN_CGI;
		
		} else {
			std::string extension = request.uri.getExtension();
			std::string contentType = request.uri.getMime(extension);
			
			//handle GET
			if (request.getMethod() == "GET")
			{
				if (!contentType.empty())
				{
					std::filesystem::path fullPath = "data/" + contentType + request.uri.getPath();
					if (std::filesystem::is_regular_file(fullPath))
						request.uri.setPath(fullPath.generic_string());
					
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
		std::cout << "\033[34;1m" << buffer << "\033[0m\n" << std::endl;
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


// TODO: check errors
void writeData(connection *conn) 
{
	int			len;
	
	conn->state = WRITING;
	std::cout << "\033[32;1m" << conn->response << "\033[0m\n" << std::endl;
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
		std::cout << "Response sent" << std::endl; //for testing
		conn->response.clear();
		conn->state = READING;
	}
}

// TODO: handle errors??
void	closeConnection(int epollFd, connection *conn)
{
	std::cout << "close connection" << std::endl;
	epoll_ctl(epollFd, EPOLL_CTL_DEL, conn->fd, nullptr);
    close(conn->fd);
	delete conn;
    std::cout << "Connection closed" << std::endl;
}

