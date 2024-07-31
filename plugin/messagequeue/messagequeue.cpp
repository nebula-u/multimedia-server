#include "messagequeue.h"

MessageQueue::MessageQueue(size_t maxQueueSize)
{
    this->MAX_QUEUE_SIZE = maxQueueSize;
    this->messageQueuesize_ = 0;
}

RETURN_CODE MessageQueue::EnQueue(std::string message)
{
    std::unique_lock<std::mutex> lock(this->MessageQueueMtx_);
    // 如果消息队列已满，则等待10秒，如果依然满，则返回失败
    if(!this->notFull_.wait_for(lock, std::chrono::seconds(2), [&]()->bool {return this->messageQueue_.size() < this->MAX_QUEUE_SIZE;}))
    {
        std::cerr << "消息队列已满，消息提交失败: " << message << std::endl;
        return RETURN_CODE::MESSAGE_ENQUEUE_ERROR;
    }
    this->messageQueue_.emplace(message);
    messageQueuesize_++;
    notEmpty_.notify_all();
    return RETURN_CODE::NO_ERROR;
}

std::string MessageQueue::DeQueue()
{
    std::string message = "";
    std::unique_lock<std::mutex> lock(this->MessageQueueMtx_);
    notEmpty_.wait(lock, [&]() -> bool {return this->messageQueue_.size()>0;});
    message = messageQueue_.front();
    messageQueue_.pop();
    this->messageQueuesize_--;
    if(this->messageQueue_.size()>0) notEmpty_.notify_all();
    notFull_.notify_all();
    return message;
}
