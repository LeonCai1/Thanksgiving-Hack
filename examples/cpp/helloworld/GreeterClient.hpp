#ifndef GREETER_CLIENT_H
#define GREETER_CLIENT_H

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "RayTracing/RayTracer.hpp"
#include "Worker.hpp"

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
using helloworld::ImageRequest;
using helloworld::ImageResponse;
using helloworld::RenderImageRequest;
using helloworld::RenderImageResponse;
using helloworld::SendResultRequest;
using helloworld::SendResultResponse;
using helloworld::TaskRequest;
using helloworld::TaskResponse;
using namespace std;
class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }
  string RenderImage(string file) {
    RenderImageRequest request;
    RenderImageResponse reply;
    ClientContext context;

    // The actual RPC.
    Status status = stub_->RenderFile(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.pack();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "Failed";
    }
  }
  int RequestTask() {
    TaskRequest request;
    TaskResponse reply;
    ClientContext context;

    // The actual RPC.
    Status status = stub_->RequestTask(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.index();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return -1;
    }
  }
  void SendResult(int line, string result) {
    SendResultRequest request;
    SendResultResponse reply;
    ClientContext context;
    request.set_index(line);
    request.set_pack(result);
    // The actual RPC.
    Status status = stub_->SendResult(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    }
  }

  void CommandImageRequest() {
    ImageRequest request;
    ImageResponse reply;
    ClientContext context;
    // The actual RPC.
    Status status = stub_->CommandImageRequest(&context, request, &reply);
    //
    //
    if (status.ok()) {
      std::cout << "The work add already!";
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

#endif