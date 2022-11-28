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

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  std::string target_str;
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
    ofstream myfile(argv[2]);
    myfile << greeter->RenderImage(argv[2]) << endl;
    myfile.close();
    return 0;
  }

  // RayTracer *tracer = new RayTracer();
  // int line;
  int waitListSize = 3;

  String jsonStr = reeter->RayTracerGet();
  /**
   * Json parser and import into tracker
   */
  RayTracer* tracer = new RayTracer();
  Worker* worker = new Worker(greeter, waitListSize, tracer);
  while (worker->run())
    ;

  delete tracer;
  delete worker return 0;
}
