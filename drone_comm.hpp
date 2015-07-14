/** @file	drone_comm.cpp
*	@brief	Libreria de funciones para la comunicacion con el drone. Aqui todo es bastante autodescriptivo.
*	@author	Juan Pablo Alvarez
*	@author	Renato Aguilar
*/

#include <sstream>
#include <vector>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

namespace dcom{

	curlpp::Easy request;
	std::stringstream bin_data;
	std::string tmp;
	std::vector<char> bin_frame;
	std::string url = "localhost:8080";

	void setUrl(char *u){
		url.assign(u);
	};

	/**
	 * @brief Retorna un puntero a vector de caracteres, el cual contiene el equivalente binary de una imagen png.
	*/
	std::vector<char> *getFrame(){
		try {
			bin_data.str( std::string() );
			bin_data.clear();	
			request.setOpt(new curlpp::options::Url(url));
			request.setOpt(new curlpp::options::WriteStream(&bin_data));
			request.perform();
			tmp = bin_data.str();
			bin_frame.assign(tmp.c_str(), tmp.c_str()+tmp.size());
			return &bin_frame;
		}
		catch ( curlpp::LogicError & e ) {
			std::cout << "logic error:" << std::endl << e.what() << std::endl;
		}
		catch ( curlpp::RuntimeError & e ) {
			std::cout << "runtime error:" << std::endl << e.what() << std::endl;
		}
	};
	/**
	 * @brief Envia un HTTP GET al server en node.js, indicando una instruccion en la url.
	*/	
	void tell(std::string instruc){
		try {
			request.setOpt(new curlpp::options::Url(url+"/"+instruc));
			request.perform();
			//std::cout << instruc << std::endl;
			return;
		}
		catch ( curlpp::LogicError & e ) {
			std::cout << "logic error:" << std::endl << e.what() << std::endl;
		}
		catch ( curlpp::RuntimeError & e ) {
			std::cout << "runtime error:" << std::endl << e.what() << std::endl;
		}
	};
	
	
}


