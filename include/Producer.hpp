#ifndef UNTITLED1_PRODUCER_HPP
#define UNTITLED1_PRODUCER_HPP

#include "ThreadPool.hpp"
#include "Queue.hpp"
#include "root_certificates.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include <queue>
#include <mutex>
#include <gumbo.h>
#include <future>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using tcp = boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
namespace http = boost::beast::http;

class Producer{
public:
    Producer(unsigned poolsCount):pools(poolsCount){}
    void producing(std::string url,unsigned depth,Consumer& k);
    std::string downloader_url(std::string host, std::string target);
    std::string parse_url_to_host(std::string url);
    std::string parse_url_to_target(std::string url);
    void do_something(GumboNode* node);
    void producing2();
private:
    std::map <std::string,std::string> urlss_;
    std::vector<std::future<std::string>> fut;
    ThreadPool pools;
};
void Producer::producing2() {
    unsigned counts = fut.size();
    for(unsigned i = 0; i < counts; ++i){
        GumboOutput* out = gumbo_parse(fut[i].get().c_str());
        do_something(out->root);
        gumbo_destroy_output(&kGumboDefaultOptions, out);
    }
    std::cout<<"finish"<<std::endl;
}


void Producer::producing(std::string url_, unsigned depth_,Consumer& k){
//    unsigned count = 0;
    GumboOutput* out = gumbo_parse(downloader_url(parse_url_to_host(url_),parse_url_to_target(url_)).c_str());
    do_something(out->root);
    gumbo_destroy_output(&kGumboDefaultOptions, out);
    k.consuming();
    while(depth_ > 0){
        depth_--;
        producing2();
    }
}
std::string Producer::downloader_url(std::string host_, std::string target_){
    try {
        auto const host = host_.c_str();
        auto const port = "443";
        auto const target = target_.c_str();
        int version = 11;

        boost::asio::io_context ioc;

        ssl::context ctx{ssl::context::sslv23_client};

        load_root_certificates(ctx);

        tcp::resolver resolver{ioc};
        ssl::stream<tcp::socket> stream{ioc, ctx};

        if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
            boost::system::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::system::system_error{ec};
        }

        auto const results = resolver.resolve(host, port);

        boost::asio::connect(stream.next_layer(), results.begin(), results.end());

        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(stream, req);

        boost::beast::flat_buffer buffer;

        http::response<http::string_body> res;

        http::read(stream, buffer, res);

        boost::system::error_code ec;
        //stream.shutdown(ec);

        if (ec == boost::asio::error::eof) {
            ec.assign(0, ec.category());
        }
        if (ec)
            throw boost::system::system_error{ec};
//        unsigned count;

        queues_.push(res.body());

        return res.body();

    }catch (std::exception const& e){
        // std::cerr << e.what() << std::endl;
    }
    return "";
}
std::string Producer::parse_url_to_host(std::string url_){
    if (url_.find("https://") == 0)
        url_ = url_.substr(8);
    std::string result_host ;
    for (unsigned i = 0; i < url_.size(); ++i) {
        if ((url_[i] == '/') || (url_[i] == '?')) break;
        result_host+=url_[i];
    }
    return result_host;
}
std::string Producer::parse_url_to_target(std::string url_){
    if (url_.find("https:") == 0)
        url_ = url_.substr(8);
    std::string result_target;
    unsigned pos = 0;
    //while (url_[pos] != '/') { ++pos; }
    for (; pos < url_.size(); ++pos) {
        if ((url_[pos] == '/') || (url_[pos] == '?')) break;
    }
    for (unsigned i = pos; i < url_.size(); ++i) {
        result_target += url_[i];
    }

    return result_target;
}
void Producer::do_something(GumboNode* node){
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href = nullptr;
    if (node->v.element.tag == GUMBO_TAG_A &&(href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        std::string curr_str = href->value;
        if (curr_str.find("https:") == 0) {
            unsigned count = urlss_.size();
            urlss_.insert(std::pair<std::string,std::string>(curr_str,"res"));
            if(urlss_.size() > count)
                fut.push_back(pools.enqueue(&Producer::downloader_url,this,parse_url_to_host(curr_str),parse_url_to_target(curr_str)));
        }
    }
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        do_something(static_cast<GumboNode*>(children->data[i]));
    }
}
#endif //UNTITLED1_PRODUCER_HPP
