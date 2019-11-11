#include "Consumer.hpp"
#include "Producer.hpp"
#include "Globals.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <queue>

using boost::asio::ip::tcp;
using namespace boost::asio::ssl;
using namespace boost::beast::http;
namespace po = boost::program_options;
using namespace std;

//std::string url;
//std::string output;
//unsigned depth;
//unsigned network_threads;
//unsigned parser_threads;

void parse_cmdl(int argc,char* argv[]){

    po::options_description desc{"Options"};
    desc.add_options()
            ("url", po::value<std::string>())
            ("output", po::value<std::string>())
            ("depth",po::value<unsigned>())
            ("network_threads",po::value<unsigned>())
            ("parser_threads",po::value<unsigned>());

    po::variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);


    if(vm.count("url"))
        url = vm["url"].as<std::string>();
    if(vm.count("output"))
        output = vm["output"].as<std::string>();
    if(vm.count("depth"))
        depth = vm["depth"].as<unsigned>();
    if(vm.count("network_threads"))
        network_threads = vm["network_threads"].as<unsigned>();
    if(vm.count("parser_threads"))
        parser_threads = vm["parser_threads"].as<unsigned>();
    std::cout<<url<<std::endl;
}

int main(int argc, char* argv[]){
    std::queue<std::string> q;
    parse_cmdl(argc,argv);

    Producer p(network_threads);
    Consumer k(parser_threads);
    p.producing(url,depth,k);
    return 0;
}
