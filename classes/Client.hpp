#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>

class Client
{

	public:

		Client();
		~Client();

		void		append_requestData(const char *str, long unsigned int size);
		void		set_response(std::string str);
		
		void		clear_request();
		void		clear_response();

		std::string	get_request();
		std::string	get_response();

	private:

		Client(Client const & src);
		Client &	operator=( Client const & rhs );

		std::string	_request;
		std::string	_response;
};

std::ostream &		operator<<( std::ostream & o, Client const & i );

#endif /* ********************************************************** CLIENT_H */