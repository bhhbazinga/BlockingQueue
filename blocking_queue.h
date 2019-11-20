
#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <mutex>

template <typename T>
class BlockingQueue {
 public:
  BlockingQueue() : head_(new Node), tail_(head_), size_(0) {}
  ~BlockingQueue() {
    T value;
    while (TryDequeue(value))
      ;
    delete head_;
  }

  void Enqueue(const T& value) {
    std::lock_guard<std::mutex> lk(tail_mutex_);
    tail_->value = value;
    InternalEnqueue();
  }

  void Enqueue(T&& value) {
    std::lock_guard<std::mutex> lk(tail_mutex_);
    tail_->value = std::move(value);
    InternalEnqueue();
    return tail_->value;
  }

  bool TryDequeue(T& value) {
    std::unique_lock<std::mutex> lk(head_mutex_);
    if (head_ == get_tail()) {
      return false;
    }

    Node* old_head = head_;
    value = std::move(old_head->value);
    head_ = old_head->next;
    lk.unlock();

    size_.fetch_sub(1, std::memory_order_release);
    delete old_head;
    return true;
  }

  T Dequeue() {
    std::unique_lock<std::mutex> lk(head_mutex_);
    cond_var_.wait(lk, [this] { return head_ != get_tail(); });

    Node* old_head = head_;
    T value = std::move(old_head->value);
    head_ = old_head->next;
    lk.unlock();

    size_.fetch_sub(1, std::memory_order_release);
    delete old_head;
    return value;
  }

  bool empty() const {
    std::unique_lock<std::mutex> lk(head_mutex_);
    return head_ == get_tail();
  }

  size_t size() const { return size_.load(std::memory_order_acquire); }

 private:
  struct Node;

  void InternalEnqueue() {
    Node* new_tail = new Node;
    tail_->next = new_tail;
    tail_ = new_tail;
    size_.fetch_add(1, std::memory_order_release);
    cond_var_.notify_one();
  }

  Node* get_tail() const {
    std::lock_guard<std::mutex> lk(tail_mutex_);
    return tail_;
  }

  struct Node {
    T value;
    Node* next;

    Node() {}
    Node(const T& v) : value(v), next(nullptr) {}
    Node(T&& v) : value(std::move(v)), next(nullptr) {}
  };

  mutable std::mutex head_mutex_;
  mutable std::mutex tail_mutex_;
  std::condition_variable cond_var_;
  Node* head_;
  Node* tail_;
  std::atomic<size_t> size_;
};

#endif