/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandling.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/12/03 15:20:23 by cariencaljo   ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"

// TODO:	check allowed methods for contentType
void	handleRequest(int epollFd, connection *conn) {
	std::cout << "handleRequest" << "\tfd = " << conn->fd << std::endl; //for testing
	try {
		// Process the request data
		HttpRequest request(conn->request, conn->server);
		// Handle parsing error
		if (request.getRequestStatus() != 200) 
			setErrorResponse(conn, request.getRequestStatus());
		//check and set cookie
		std::string cookieValue = checkAndSetCookie(conn, request);
		//get extension and type
		std::string extension = request.uri.getExtension();
		std::string contentType = request.uri.getMime(extension);

		// handle CGI
		if (request.uri.getExecutable() == "cgi-bin") {
			
			size_t	maxContentLength		= conn->server->get_maxBodySize(request.getHeaderValue("host")); //todo: replace host
			size_t	actualContentLength		= request.getBody().size();
			size_t	headerContentLength		= 0;
			if (request.isHeader("content-length")) {
				std::cout << "header content len: " << request.getHeaderValue("content-length") << "\n";
				std::cout << "actual content len: " << actualContentLength << "\n";
				headerContentLength = std::stoi(request.getHeaderValue("content-length"));
			}
			if (headerContentLength > maxContentLength || headerContentLength != actualContentLength) 
			 	throw HttpRequest::parsingException(400, "content-length to big or wrong");

			//case error in cgi handler
			if (cgiHandler(request, conn, epollFd) == 1 ) 
				setErrorResponse(conn, 500);	
			else {
				conn->request.clear();
				conn->state = IN_CGI;
			}
		}
		
		//handle GET
		else if (request.getMethod() == "GET") {
			if (!contentType.empty()) {
				std::string fullPath = "data/" + contentType + request.uri.getPath();
				std::ifstream f(fullPath);

				if (f.good())
					request.uri.setPath(fullPath);
				else
					throw HttpRequest::parsingException(404, "Path not found");

				HttpResponse response(request);
				std::string path = replace_cookiePng(request, cookieValue);
				response.setBody(path, request.uri.getIsBinary());
				response.addHeader("Content-type", contentType);
				response.setHeader("Set-Cookie", cookieValue);
				setResponse(conn, response);
			} 
			else
				throw HttpRequest::parsingException(501, "Extension not supported");
		}

		//handle POST
		//todo decide on handling
		else if (request.getMethod() == "POST") {
			if (request.uri.getExecutable() != "cgi-bin") {
				HttpResponse response(request);
				response.setBody("data/text/html/upload.html", false); //todo:make config
				setResponse(conn, response);
			}
			// throw HttpRequest::parsingException(403, "POST METHOD forbidden outside of CGI"); //todo remove
		}

		// handle DELETE
		else if (request.getMethod() == "DELETE") {
			if (request.uri.getExecutable() != "cgi-bin") {
				throw HttpRequest::parsingException(403, "DELETE METHOD forbidden outside of CGI"); //todo remove
			}
		}

		// handle unsupported methods
		else
			throw HttpRequest::parsingException(405, "METHOD or Extension not supported");


	} catch (const HttpRequest::parsingException& exception) {
		std::cout << RED << "Error: " + std::to_string(exception.getErrorCode()) << " " << exception.what() << RESET << std::endl;
		setErrorResponse(conn, exception.getErrorCode());
	}
	std::cout << "end of handleRequest" << std::endl; //testing
}

std::string replace_cookiePng(HttpRequest request, std::string cookieValue) {
	std::string path(request.uri.getPath());
	long unsigned int pos = path.find("cookie.png");
	if (pos < path.size()) {
		std::string trigger = cookieValue.substr(cookieValue.rfind("=") + 1) ;
		path = path.substr(0, pos) + trigger + ".png";
	}
	return path;
}	

