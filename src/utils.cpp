#include<string>
#include<sstream>
#include<ctime>
#include "webServ.hpp"

std::string getTimeStamp() {
	//get current time
    std::time_t now = std::time(nullptr);
	
	//store current time in struct as gmt time
	std::tm* timeInfoGmt = std::gmtime(&now);
    
	// Define the format for the output string
    char buffer[40];
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfoGmt);
    return buffer;
}

void	setResponse(connection *conn, HttpResponse resp)
{
	conn->response = resp.serializeResponse();
	conn->request.clear();
	conn->state = WRITING;
}

// to do: make timeout check
void	checkTimeout(connection *conn)
{
	if (conn->request.empty()) // and timeout
		conn->state = CLOSING; 
	else
		conn->state = HANDLING;
}

void	setErrorResponse(connection *conn, int error)
{
	HttpResponse response;
	response.setStatusCode(error);
	setResponse(conn, response);
}


// format for future cookie: setHeader("Set-Cookie", "name=webserv42, id=000, trigger=000"); 
//todo: more cookies than one	
std::string		checkAndSetCookie(connection* conn, HttpRequest& request) {

	std::string	cookieValue	=	request.getHeaderValue("cookie");

	//trim leading whitespace	
	size_t		first		=	cookieValue.find_first_not_of(" ");
	cookieValue 			=	(first == std::string::npos) ? "" :  cookieValue.substr(first);

	if (!cookieValue.empty())
	{
		for (auto& pair : conn->server->get_knownClientIds())
		{
			if (pair.first == cookieValue)
			{
				pair.second += 1;
				std::cout << "known user_id ("<< cookieValue << "). This ID has visited us " << pair.second << " times!" << std::endl;
				return cookieValue;
			}
		}
	}

	//set cookie for empty and unknow client using current time-hash
	std::hash<std::time_t>		timeHash;
	std::time_t now			=	std::time(nullptr);
	size_t hashValue		=	timeHash(now);

	std::string newCookieValue = std::to_string(hashValue);
	conn->server->addClientId(newCookieValue);
	return newCookieValue;
}
