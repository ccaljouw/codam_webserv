/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: carlo <carlo@student.codam.nl>               +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/09/29 11:00:35 by carlo         #+#    #+#                 */
/*   Updated: 2023/11/01 08:39:01 by carlo         ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"

#include<iostream>


int	main(int argc, char **argv) {
	
	(void)argv;

	if (argc != 2) {
		std::cout << RED << "please pass one argument"	<< RESET << std::endl;
		return 1;
		}

	std::cout << GREEN << "\n Config file:  "	<< argv[1] << RESET << std::endl;
	
	return (0);
}
