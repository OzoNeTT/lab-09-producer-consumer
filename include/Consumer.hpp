#ifndef UNTITLED1_CONSUMER_HPP
#define UNTITLED1_CONSUMER_HPP

#include "ThreadPool.hpp"
#include "Queue.hpp"
#include "Globals.hpp"
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <gumbo.h>


class Consumer{
public:
    Consumer(unsigned poolsCount):pools(poolsCount){}
    void consuming();
    void wrating();
    void parse_url(GumboNode* node);
private:
    ThreadPool pools;
    ThreadPool pools_w{1};
};

void Consumer::parse_url(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* src = nullptr;
    if (node->v.element.tag == GUMBO_TAG_IMG &&
        (src = gumbo_get_attribute(&node->v.element.attributes, "src"))) {
        std::string str = src->value;
        queues_pict.push(str);
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        parse_url(static_cast<GumboNode*> (children->data[i]));
    }
}
void Consumer::wrating(){
    std::ofstream fout;
    fout.open(output);
    while(true){
        if(queues_pict.empty()){
            sleep(4);
            if(queues_pict.empty())
                break;
            else
                continue;
        }
        fout<<queues_pict.front()<<std::endl;
        queues_pict.pop();
    }
    fout.close();
}

void Consumer::consuming() {
    pools_w.enqueue(&Consumer::wrating,this);
    while(true){
        if(queues_.empty()) {
            sleep(4);
            if(queues_.empty())
                break;
            else
                continue;
        }
        pools.enqueue(&Consumer::parse_url,this, gumbo_parse(queues_.front().c_str())->root);
        queues_.pop();
    }
}

#endif //UNTITLED1_CONSUMER_HPP
