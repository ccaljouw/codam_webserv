# include <Client.hpp>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client()
{
}

Client::Client( const Client & src )
{
	(void)src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Client &				Client::operator=( Client const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	(void)rhs;
	return *this;
}

std::ostream &			operator<<( std::ostream & o, Client const & i )
{
	//o << "Value = " << i.getValue();
	(void)i;
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Client::append_requestData(const char *str, long unsigned int size)
{
	// std::cout << "in append: " << str << std::endl;
	_request.append(str, size);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string	Client::get_request()
{
	return this->_request;
}

std::string	Client::get_response()
{
	return this->_response;
}

void	Client::set_response(std::string str)
{
	this->_response = str;
}

void	Client::clear_request()
{
	this->_request.clear();
}

void	Client::clear_response()
{
	this->_response.clear();
}

/* ************************************************************************** */