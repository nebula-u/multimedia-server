#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "common.h"

class MessageQueue
{
public:
    MessageQueue(size_t maxQueueSize);
    ~MessageQueue() = default;
    RETURN_CODE EnQueue(std::string message);
    std::string DeQueue();
private:
    std::queue<std::string> messageQueue_;  // 消息队列
    std::mutex MessageQueueMtx_;            // 保证对消息队列操作的安全性
    std::condition_variable notEmpty_;      // 保证取消息在消息队列为空的时候阻塞
    std::condition_variable notFull_;       // 保证存消息在消息队列为满的时候阻塞
    size_t MAX_QUEUE_SIZE;
    size_t messageQueuesize_;
};

#endif // MESSAGEQUEUE_H

