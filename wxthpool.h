#pragma once

#include "wxerr.h"

#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <exception>
namespace ws
{
  class WXthpool
  {
  private:
    using Task = std::function<void()>;
    std::vector<std::thread> pool;
    std::queue<Task> tasks;
    std::atomic<bool> run;
    std::mutex th_mutex;
    std::mutex err_mutex;
    std::exception_ptr err_ptr;
    std::condition_variable cond;
  public:
    WXthpool(std::size_t size): run(true) { add_thread(size); }
    ~WXthpool()
    {
      run = false;
      cond.notify_all();
      for (auto &th : pool) 
      {
        if(th.joinable())  th.join(); 
      }
    }
    template <typename Func, typename... Args>
    auto add_task(Func&& f, Args&&... args) 
    -> std::future<typename std::result_of<Func(Args...)>::type>;
    void add_thread(std::size_t num)
    {
      for(std::size_t i = 0;i < num;i++)
      {
        pool.emplace_back(
          [this]  
          {
            while(this->run)
            {
              Task task;
              {
                std::unique_lock<std::mutex> lock(this->th_mutex);
                this->cond.wait(lock, [this]{ return !this->run || !this->tasks.empty(); });
                if(!this->run && this->tasks.empty())  return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
              }
              task();
            }
          }
        );
      }
    }
  };
    template <typename Func, typename... Args>
    auto WXthpool::add_task(Func&& f, Args&&... args) 
    -> std::future<typename std::result_of<Func(Args...)>::type>
    {
      if(!run)
        throw WXerr(WS_ERROR_LOCATION, __func__, "add task on stopped WXthpool");
      using ret_type = typename std::result_of<Func(Args...)>::type;
      auto task = std::make_shared<std::packaged_task<ret_type()>>
        (std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
      std::future<ret_type> ret = task->get_future();
      {
        std::lock_guard<std::mutex> lock(th_mutex);
        tasks.emplace( [task] {(*task)();} );
      }
      cond.notify_one();
      return ret;
    }

}
