/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webServ.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: ccaljouw <ccaljouw@student.42.fr>            +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 16:57:19 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/24 14:59:33 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERV_H
# define WEBSERV_H

//macros
#define LINE_END "\r\n"
#define WHITE_SPACE " \t\n\v\f\r"
#define HTTP_PROTOCOL "HTTP/1.1"
#define HOST "webserv42"

//colors
#define RESET	"\033[0m"
#define RED		"\033[31;1m"
#define GREEN	"\033[32;1m"
#define YELLOW	"\033[33;1m"
#define BLUE	"\033[34;1m"
#define PURPLE	"\033[35;1m"
#define CYAN	"\033[36;1m"

//includes
#include "eventloop.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "CGI_Handler.hpp"
#include <algorithm>
#include <unistd.h>
#include <filesystem>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <sstream>
#include <cstring>


//defined in Uri.cpp
extern	std::map<std::string, std::string> acceptedExtensions;
extern	std::vector<std::string> binaryExtensions;

//define in Httpresponse.cpp
extern	std::map<int, std::string> HttpResponseCodes;

extern	int g_shutdown_flag;

// utils.cpp
void			checkTimeout(connection *conn);
void			setResponse(connection *conn, HttpResponse resp);
void			setErrorResponse(connection *conn, int error);
std::string		checkAndSetCookie(connection* conn, HttpRequest& request);
std::string		removeWhitespaces(std::string str);
void 			handleSignal(int signal);

//errorPageGen
std::string		generateErrorPage(int e);

//cookieGen
std::string		checkAndSetCookie(connection* conn, HttpRequest& request);

// requestHandling
void			handleRequest(int epollFd, connection *conn);

#endif
