/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <unistd.h>

#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif
// #define IMAGE_LINE 400
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::Greeter;
using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::ImageRequest;
using helloworld::ImageResponse;
using helloworld::RenderImageRequest;
using helloworld::RenderImageResponse;
using helloworld::SendResultRequest;
using helloworld::SendResultResponse;
using helloworld::TaskRequest;
using helloworld::TaskResponse;
using namespace std;
class RenderTaskManager {
  string file;
  int totalLine = 400;
  vector<string> result;
  set<int> remaining;
  set<int>::iterator current;

 public:
  RenderTaskManager(string file) {
    this->file = file;
    for (int i = 0; i < 400; i++) {
      result.push_back("");
      remaining.insert(i);
    }
    current = remaining.begin();
  }
  int nextLine() {
    if (remaining.empty()) {
      return -1;
    }
    if (current == remaining.end()) {
      current = remaining.begin();
    }
    int res = *current;
    current++;
    return res;
  }
  bool writeLine(int index, string line) {
    result[index] = line;
    if (remaining.count(index)) {
      remaining.erase(remaining.find(index));
    }
    return true;
  }
  bool done() { return remaining.empty(); }

  string fullContent() {
    string res = "";
    for (int i = result.size() - 1; i >= 0; i--) {
      res = res + result[i];
    }
    return res;
  }
};

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  //   RenderTaskManager* taskManager = NULL;
  queue<RenderTaskManager*> waitingLine;

  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Connection works");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
  Status RenderFile(ServerContext* context, const RenderImageRequest* request,
                    RenderImageResponse* reply) override {
    // no event then cancelled
    this.waitingLine.push(new RenderTaskManager("1")
    
    while (!this->waitingLine.front()->done()) {
      sleep(1);
    }
    stringstream myfile;
    myfile << "P3\n" << 600 << " " << 400 << "\n255\n";
    myfile << this->waitingLine.front()->fullContent();
    reply->set_pack(myfile.str());
    this->waitingLine.pop();

    return Status::OK;
  }

  Status RequestTask(ServerContext* context, const TaskRequest* request,
                     TaskResponse* reply) override {
    if (this->waitingLine.size() == 0) {  // no event then cancelled
      return Status::CANCELLED;
    }
    reply->set_index(this->waitingLine.front()->nextLine());
    return Status::OK;
  }
  Status SendResult(ServerContext* context, const SendResultRequest* request,
                    SendResultResponse* reply) override {
    if (this->waitingLine.size() == 0) {
      return Status::CANCELLED;
    }
    this->waitingLine.front()->writeLine(request->index(), request->pack());
    return Status::OK;
  }

  /**
   * Recived Request from user or command client.
   */
  Status CommandImageRequest(ServerContext* context,
                             const ImageRequest* request,
                             ImageResponse* reply) override {
    // change somewhere to a ip or wut
    // reply->set_message("received render request from somewhere");

    waitingLine.push(new RenderTaskManager("1"));
    // reply->set_message("accept render request from somewhere");
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
