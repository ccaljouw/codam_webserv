/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandling.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/11/27 22:52:47 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include <fstream>

// TODO:	check allowed methods for contentType
void	handleRequest(int epollFd, connection *conn) {
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

		//handle default for directories
		if (request.uri.isDir()) {
			// std::cout << BLUE << "Is Directory" << RESET << std::endl;
			std::string index = request.getDefault();
			if (!index.empty()) {
				std::string bodyPath = "data/text/html" + request.uri.getPath() + index;		//todo add root
				HttpResponse response(request);
				response.setBody(bodyPath, false);
				response.addHeader("Content-type", contentType);
				response.setHeader("Set-Cookie", cookieValue);
				setResponse(conn, response);
			}
			else
				throw HttpRequest::parsingException(404, "Path not found");
		}
					
			// handle CGI
		else if (request.uri.getExecutable() == "cgi-bin") {
			
			size_t	maxContentLength		= conn->server->get_maxBodySize(request.getHeaderValue("host"));
			size_t	actualContentLength		= request.getBody().size();
			size_t	headerContentLength		= 0;

			if (request.isHeader("content-length"))
				headerContentLength = std::stoi(request.getHeaderValue("content-length"));
			if (headerContentLength > maxContentLength || headerContentLength != actualContentLength) 
			 	throw HttpRequest::parsingException(400, "content-length to big or wrong");

			//case error in cgi handler
			if (cgiHandler(request, conn, epollFd) == 1 ) 
				setErrorResponse(conn, 500);	
			else {
				// conn->state = CLOSING;
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
				response.setBody(request.uri.getPath(), request.uri.getIsBinary());
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
			// throw HttpRequest::parsingException(403, "POST METHOD forbidden outside of CGI"); //todo remove
			}
		}

		// handle DELETE
		else if (request.getMethod() == "DELETE") {
			if (request.uri.getExecutable() != "cgi-bin") {
				HttpResponse response(request);
				response.setBody("data/text/html/418.html", false); //todo:make config
				setResponse(conn, response);
				
		//		throw HttpRequest::parsingException(403, "DELETE METHOD forbidden outside of CGI"); //todo remove
			}
		}

		// handle unsupported methods
		else
			throw HttpRequest::parsingException(405, "METHOD or Extension not supported");


	} catch (const HttpRequest::parsingException& exception) {
		std::cout << RED << "Error: " + std::to_string(exception.getErrorCode()) << " " << exception.what() << RESET << std::endl;
		setErrorResponse(conn, exception.getErrorCode());
	}
}
