/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   requestHandling.cpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: bfranco <bfranco@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/11/03 23:45:10 by cariencaljo   #+#    #+#                 */
/*   Updated: 2023/12/04 13:49:40 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include <fstream>

// TODO:	check allowed methods for contentType
void	handleRequest(int epollFd, connection *conn) {
	
	std::cout << "handleRequest" << "\tfd = " << conn->fd << std::endl; //for testing
	
	try {
		// Process the request data
		HttpRequest request(conn->request, conn->server);

		// Handle parsing error
		if (request.getRequestStatus() != 200) 
			setErrorResponse(conn, request.getRequestStatus());
	
		//set variables
		std::string extension		= request.uri.getExtension();
		std::string contentType		= request.uri.getMime(extension);
		std::string host			= request.uri.getHost();
		std::string location		= request.uri.getPath();
		std::string root 			= conn->server->get_rootFolder(host, location);
		std::string index			= conn->server->get_index(host, location);
		bool dirListing				= conn->server->get_dirListing(host, location);
		
		//**testprint**
		std::cout << BLUE << "variables set in requesthandling:\nextension: " << extension << "\ncontentType: " << contentType << "\nhost: " << host << "\nlocation: " 
			<< location << "\nindex: " << index << "\nroot: " << root << "\ndirListing: " << RESET << std::endl;

		//check and set cookie
		std::string cookieValue = checkAndSetCookie(conn, request);
		
		//handle redirect
		std::map<int, std::string> redirect_location_header = conn->server->get_redirect(host, location);
		if (!redirect_location_header.empty()) {
			
			//**testprint**
			std::cout << BLUE << "\ntarget is redirected to: " << redirect_location_header.begin()->second << RESET << std::endl;

			request.headers->addHeader("location", redirect_location_header.begin()->second);
			HttpResponse response(request);
			response.setStatusCode(redirect_location_header.begin()->first);
			setResponse(conn, response);
		return;
		}
		
		//handle default for directories
		if (request.uri.isDir()) {

			//**testprint**
			std::cout << BLUE << "\ntarget is a directory and its dirlisting = " << dirListing << RESET << std::endl;
					
			if (dirListing == true) {
				HttpResponse response(request);
				request.uri.setPath("/cgi-bin/index.py");
				request.addEnvironVar("QUERY_STRING", location);
			
				if (cgiHandler(request, conn, epollFd) == 1 ) 
					setErrorResponse(conn, 500);	
				else {
					conn->request.clear();
					conn->state = IN_CGI;
				}
			}
	
			else if (!index.empty()) {
				std::string bodyPath = root + "/text/html/" + index;
				
				//**testprint**
				std::cout << BLUE << "index found: " << index << " | setting body to :" << bodyPath <<  RESET << std::endl;
				
				HttpResponse response(request);
				response.reSetBody(bodyPath, false);
				response.headers->addHeader("Content-type", contentType);
				response.headers->setHeader("Set-Cookie", cookieValue);
				setResponse(conn, response);
			}
			else
				throw HttpRequest::parsingException(404, "Path not found");
		}
					
		// handle CGI
		else if (request.uri.getExecutable() == "cgi-bin") {

			//**testprint**
			std::cout << "\nin CGI\n" << std::endl;
			
			// check contentn -length
			size_t	maxContentLength		= conn->server->get_maxBodySize(request.headers->getHeaderValue("host"));
			size_t	actualContentLength		= request.getBody().size();
			size_t	headerContentLength		= 0;
			if (request.headers->isHeader("content-length")) {
				headerContentLength = std::stoi(request.headers->getHeaderValue("content-length"));
				
				//**test print
				std::cout << "header content len: " << headerContentLength << "\n";
				std::cout << "actual content len: " << actualContentLength << "\n";
				std::cout << "max content len: " 	<< maxContentLength << "\n";
			}
			if (headerContentLength > maxContentLength || headerContentLength != actualContentLength) 
			 	throw HttpRequest::parsingException(400, "content-length to big or wrong"); //todo check error thrown in this case

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
			
			//**testprint**
			std::cout << "\nin GET handler\n" << std::endl;

			if (!contentType.empty()) {
				//check if target exists
				std::string fullPath = root + "/" + contentType + location;
				std::cout << "full path: " << fullPath << std::endl;
				std::ifstream f(fullPath);
				
				if (f.good())
					request.uri.setPath(fullPath);
				else
					throw HttpRequest::parsingException(404, "Path not found");
		
				HttpResponse response(request);
				response.reSetBody(request.uri.getPath(), request.uri.getIsBinary());
				response.headers->addHeader("Content-type", contentType);
				response.headers->setHeader("Set-Cookie", cookieValue);
				setResponse(conn, response);
			} 
			else
				throw HttpRequest::parsingException(404, "Page not found in request handler");
		}

		//handle POST
		//todo decide on handling
		else if (request.getMethod() == "POST") {
			
			//**testprint**
			std::cout << "\nin POST handler\n" << std::endl;
			
			HttpResponse response(request);
			response.reSetBody("data/text/html/upload.html", false); //todo:make config
			setResponse(conn, response);
		}

		// handle DELETE
		else if (request.getMethod() == "DELETE") {

			//**testprint**
			std::cout << "\nin DELETE handler\n" << std::endl;
			
			HttpResponse response(request);
			response.reSetBody("data/text/html/418.html", false); //todo:make config
			setResponse(conn, response);
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
