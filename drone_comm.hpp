#include <sstream>
#include <vector>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

//namespace para las funciones que comunican con el drone

namespace dcom{

	curlpp::Easy request;
	std::stringstream bin_data;
	std::string tmp;
	std::vector<char> bin_frame;
	std::string url = "localhost:8080";

	void setUrl(char *u){
		url.assign(u);
	};

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
			std::cout << e.what() << std::endl;
		}
		catch ( curlpp::RuntimeError & e ) {
			std::cout << e.what() << std::endl;
		}
	};
	
	void tell(std::string instruc){
		try {
			request.setOpt(new curlpp::options::Url(url+"/"+instruc));
			request.perform();
			return;
		}
		catch ( curlpp::LogicError & e ) {
			std::cout << e.what() << std::endl;
		}
		catch ( curlpp::RuntimeError & e ) {
			std::cout << e.what() << std::endl;
		}
	};
	
	
}


