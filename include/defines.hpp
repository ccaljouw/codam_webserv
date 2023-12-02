#pragma once
#ifndef DEFINES_H
# define DEFINES_H

#include <string>
#include <cstring>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <ctime>
#include <iostream>
#include <exception>

// HTTP
#define LINE_END "\r\n"
#define WHITE_SPACE " \t\n\v\f\r"
#define HTTP_PROTOCOL "HTTP/1.1"

// settings
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024
#define TIMEOUT	5
#define CGI_TIMEOUT 3
#define	MAX_NR_REQUESTS 30

// config
#define LETTERS "abcdefghijklmnopqrstuvwxyz" //todo: change to standard c++ function?
#define NUMBERS "0123456789"				 //todo: change to standard c++ function?

//colors
#define RESET	"\033[0m"
#define RED		"\033[31;1m"
#define GREEN	"\033[32;1m"
#define YELLOW	"\033[33;1m"
#define BLUE	"\033[34;1m"
#define PURPLE	"\033[35;1m"
#define CYAN	"\033[36;1m"


enum	states {
	LISTENING,
	READING,
	HANDLING,
	IN_CGI,
	WRITING,
	CLOSING
};

class Server;
struct connection
{
	int							fd;
	int							cgiFd;
	pid_t						cgiPID; 
	states						state;
	std::string					request;
	std::string					response;
	Server						*server;
	std::time_t					time_last_request;
	int							nr_of_requests;
	bool						close_after_response;
};

#endif

