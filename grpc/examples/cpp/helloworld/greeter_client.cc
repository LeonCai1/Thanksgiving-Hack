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

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

// #include "GreeterClient.hpp"
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
// class GreeterClient {
//  public:
//   GreeterClient(std::shared_ptr<Channel> channel)
//       : stub_(Greeter::NewStub(channel)) {}

//   // Assembles the client's payload, sends it and presents the response back
//   // from the server.
//   std::string SayHello(const std::string& user) {
//     // Data we are sending to the server.
//     HelloRequest request;
//     request.set_name(user);

//     // Container for the data we expect from the server.
//     HelloReply reply;

//     // Context for the client. It could be used to convey extra information
//     to
//     // the server and/or tweak certain RPC behaviors.
//     ClientContext context;

//     // The actual RPC.
//     Status status = stub_->SayHello(&context, request, &reply);

//     // Act upon its status.
//     if (status.ok()) {
//       return reply.message();
//     } else {
//       std::cout << status.error_code() << ": " << status.error_message()
//                 << std::endl;
//       return "RPC failed";
//     }
//   }
//   string RenderImage(string file) {
//     RenderImageRequest request;
//     RenderImageResponse reply;
//     ClientContext context;

//     // The actual RPC.
//     Status status = stub_->RenderFile(&context, request, &reply);

//     // Act upon its status.
//     if (status.ok()) {
//       return reply.pack();
//     } else {
//       std::cout << status.error_code() << ": " << status.error_message()
//                 << std::endl;
//       return "Failed";
//     }
//   }
//   int RequestTask() {
//     TaskRequest request;
//     TaskResponse reply;
//     ClientContext context;

//     // The actual RPC.
//     Status status = stub_->RequestTask(&context, request, &reply);

//     // Act upon its status.
//     if (status.ok()) {
//       return reply.index();
//     } else {
//       std::cout << status.error_code() << ": " << status.error_message()
//                 << std::endl;
//       return -1;
//     }
//   }
//   void SendResult(int line, string result) {
//     SendResultRequest request;
//     SendResultResponse reply;
//     ClientContext context;
//     request.set_index(line);
//     request.set_pack(result);
//     // The actual RPC.
//     Status status = stub_->SendResult(&context, request, &reply);

//     // Act upon its status.
//     if (status.ok()) {

//     } else {
//       std::cout << status.error_code() << ": " << status.error_message()
//                 << std::endl;
//     }
//   }
//  private:
//   std::unique_ptr<Greeter::Stub> stub_;
// };

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
  // std::string arg_str("--target");
  // if (argc > 1) {
  //   std::string arg_val = argv[1];
  //   size_t start_pos = arg_val.find(arg_str);
  //   if (start_pos != std::string::npos) {
  //     start_pos += arg_str.size();
  //     if (arg_val[start_pos] == '=') {
  //       target_str = arg_val.substr(start_pos + 1);
  //     } else {
  //       std::cout << "The only correct argument syntax is --target="
  //                 << std::endl;
  //       return 0;
  //     }
  //   } else {
  //     std::cout << "The only acceptable argument is --target=" << std::endl;
  //     return 0;
  //   }
  // } else {
  //   target_str = "localhost:50051";
  // }
  target_str = "localhost:50051";
  GreeterClient* greeter = new GreeterClient(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

  std::string user("Trying to connect to server");
  std::string reply = greeter->SayHello(user);
  std::cout << reply << std::endl;

  /**
   * client input standard:
   *    for worker client [address[], worker]
   *    for command client [jsonfile input] later, rn it is empty
   */
  if (argc > 2 && strcmp(argv[2], "start") == 0) {  // worker client
    reply = greeter->SayHello(argv[2]);             // test the name of worker
    std::cout << "Greeter received: " << reply << "\n";
    ofstream myfile(argv[1]);
    myfile << greeter->RenderImage(argv[1]) << endl;
    myfile.close();
    return 0;
  }

  // RayTracer *tracer = new RayTracer();
  // int line;
  int waitListSize = 3;
  Worker* worker = new Worker(greeter, waitListSize);
  while (worker->run())
    ;

  return 0;
}
