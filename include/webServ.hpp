/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webServ.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 16:57:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/12/08 13:30:03 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERV_H
# define WEBSERV_H

//includes
#include "defines.hpp"
#include "Config.hpp"
#include "Header.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "CGI.hpp"
#include <sys/epoll.h>
#include <cstdint>


//main.cpp
std::list<Server> initServers(const Config& config, int epollFd);

// connectionHandling.cpp
void	newConnection(int epollFd, int serverFd, Server *server);
void	readData(connection *conn);
void	readCGI(int epollFd, connection *conn);
void	writeData(connection *conn);
void	closeConnection(int epollFd, connection *conn);
void	closeCGIpipe(int epollFd, connection *conn);

// requestHandling.cpp
void		handleRequest(int epollFd, connection *conn);
std::string replaceCookiePng(std::string location, std::string cookieValue);

// CGIHandling.cpp
int		cgiHandler(const HttpRequest& req, connection *conn, int epollFd); // todo : const ref request
void	execChild(const HttpRequest& req, CGI &cgi, int oldFd);
int		writeBody(const HttpRequest& req, int *fd);
char	*getProgramPath(const Uri& uri, char *program);

// registerEvents.cpp
int		register_server(int epollFd, int fd, Server *server);
int		register_client(int epollFd, int severFd, Server *Server);
int		register_CGI(int epollFd, int cgiFd, connection *conn);

//define in Httpresponse.cpp
extern	std::map<int, std::string> HttpResponseCodes;

extern	int g_shutdown_flag;

// utils.cpp
int				checkTimeout(connection *conn);
void			setResponse(connection *conn, HttpResponse resp);
void			setErrorResponse(connection *conn, int error);
std::string		checkAndSetCookie(connection* conn, HttpRequest& request);
std::string		removeWhitespaces(std::string str);
void 			handleSignal(int signal);

//errorPageGen
std::string		generateErrorPage(int e);

//cookieGen
std::string		checkAndSetCookie(connection* conn, HttpRequest& request);

#endif
