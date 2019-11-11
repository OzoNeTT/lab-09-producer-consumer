#ifndef UNTITLED1_QUEUE_T_HPP
#define UNTITLED1_QUEUE_T_HPP

#include <queue>
#include <mutex>
#include <iostream>


class Queue{
public:
    void push(std::string& str){
        std::lock_guard<std::mutex> lk{mtx};
        queue_.push(str);
    }
    std::string front(){
        std::lock_guard<std::mutex> lk{mtx};
        return queue_.front();
    }
    void pop(){
        std::lock_guard<std::mutex> lk{mtx};
        queue_.pop();
    }
    bool empty(){
        std::lock_guard<std::mutex> lk{mtx};
        return queue_.empty() ? 1 : 0;
    }
private:
    std::mutex mtx;
    std::queue<std::string> queue_;
};

Queue queues_;
Queue queues_pict;

#endif //UNTITLED1_QUEUE_T_HPP
