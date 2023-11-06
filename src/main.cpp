/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/09/29 11:00:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/06 11:01:39 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

#include "Uri.h"
#include "HttpRequest.h"

#include <iostream>

void testRequest(void);
void testUri(void);
void testHttpResponse();



int	main(int argc, char **argv) {
	
	(void)argv;
	(void)argc;

	// if (argc != 2) {
	// 	std::cout << RED << "please pass one argument"	<< RESET << std::endl;
	// 	return 1;
	// 	}

	// std::cout << GREEN << "\n Config file:  "	<< argv[1] << RESET << std::endl;

	testRequest();
    std::cout << std::endl;
	testUri();
    std::cout << std::endl;
	testHttpResponse();

	return (0);
}
