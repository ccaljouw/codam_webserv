#include<string>
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

