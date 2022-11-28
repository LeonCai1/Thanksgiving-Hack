#ifndef WORKER_H
#define WORKER_H
#include <algorithm>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>

#include "GreeterClient.hpp"
#include "RayTracing/RayTracer.hpp"

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::RenderImageRequest;
using helloworld::RenderImageResponse;
using helloworld::SendResultRequest;
using helloworld::SendResultResponse;
using helloworld::TaskRequest;
using helloworld::TaskResponse;
using namespace std;

class Worker {
  mutex m;
  GreeterClient* rpc;
  int buffSize;
  RayTracer* tracer;
  deque<int> waitList;
  bool finished = false;

 public:
  Worker(GreeterClient* rpc, int buffSize, RayTracer* rayTracer) {
    this->rpc = rpc;
    this->buffSize = buffSize;
    tracer = rayTracer;
  }
  bool run() {
    if (waitList.size() < buffSize) {
      auto fun = async(Worker::getQueue, this, buffSize - waitList.size());
    }
    while (waitList.empty()) {
      if (finished) {
        return false;
      }
    }
    m.lock();
    int line = waitList.front();
    waitList.pop_front();
    m.unlock();
    std::stringstream ss;
    ss << tracer->render(line) << "\n";
    std::string str = ss.str();
    rpc->SendResult(line, str);
    cout << "complete: " << line << endl;
    return true;
  }

 private:
  bool getLine() {
    int line = rpc->RequestTask();
    if (line < 0) {
      return false;
    }
    m.lock();
    waitList.push_back(line);
    m.unlock();
    return true;
  }
  static void getQueue(Worker* worker, int num) {
    for (int i = 0; i < num && (worker->finished = !worker->getLine()); i++)
      ;
  }
};

#endif