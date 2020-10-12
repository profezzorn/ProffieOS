#ifndef THREAD_HELPER_H
#define THREAD_HELPER_H

#include <pthread.h>
#include <stdio.h>

#include "timing.h"

#define NUM_THREADS 36
// #define NUM_THREADS 1

class Mutex {
public:
  Mutex() {
    pthread_mutex_init(&mutex_, NULL);
  }
  ~Mutex() {
    pthread_mutex_destroy(&mutex_);
  }
  void Lock() {
    pthread_mutex_lock(&mutex_);
  }
  void Unlock() {
    pthread_mutex_unlock(&mutex_);
  }
private:
  pthread_mutex_t mutex_;
};

class ThreaderBase {
public:
  virtual void Worker() = 0;
  static void *WorkerWrapper(void *arg) {
    ((ThreaderBase *)arg)->Worker();
    return nullptr;
  }

  void Run() {
#if NUM_THREADS > 1
    for (int i = 0; i < NUM_THREADS; i++) {
      pthread_create(threads_ + i,
		     NULL,
		     &ThreaderBase::WorkerWrapper,
		     this);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
      pthread_join(threads_[i], NULL);
    }
#else
    Worker();
#endif
  }

protected:
  Mutex mutex_;

private:
  pthread_t threads_[NUM_THREADS];
};

class Threader : public ThreaderBase {
public:
  virtual void DoTask(int task) = 0;

  virtual void Worker() {
    while (true) {
      mutex_.Lock();
      int task = current_task_;
      current_task_++;
      if (task >= num_tasks_) {
	mutex_.Unlock();
	return;
      }
      double time_so_far = Time() - start_time_;
      double tasks_per_second = time_so_far / (task + 1);
      int total_time = tasks_per_second * num_tasks_;
      fprintf(stderr, "\r %d / %d   %02dh%02dm%02ds / %02dh%02dm%02ds ",
	      task, num_tasks_,
	      ((int)time_so_far) / 3600,
	      (((int)time_so_far) / 60) % 60,
	      ((int)time_so_far) % 60,
	      total_time / 3600,
	      (total_time / 60) % 60,
	      total_time % 60);
      mutex_.Unlock();
      DoTask(task);
    }
  }

  void Run(int num_tasks) {
    num_tasks_ = num_tasks;
    current_task_ = 0;
    start_time_ = Time();
    ThreaderBase::Run();
    fprintf(stderr,"\n");
  }

private:
  int current_task_;
  int num_tasks_;
  struct timeval start_time_;
};

#endif
