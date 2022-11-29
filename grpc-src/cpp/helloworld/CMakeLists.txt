# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ src example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building src.

cmake_minimum_required(VERSION 3.5.1)

project(Src C CXX)

include(../cmake/common.cmake)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Proto file
get_filename_component(src_proto "../../protos/src.proto" ABSOLUTE)
get_filename_component(src_proto_path "${src_proto}" PATH)

# Generated sources
set(src_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/src.pb.cc")
set(src_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/src.pb.h")
set(src_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/src.grpc.pb.cc")
set(src_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/src.grpc.pb.h")
add_custom_command(
      OUTPUT "${src_proto_srcs}" "${src_proto_hdrs}" "${src_grpc_srcs}" "${src_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${src_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${src_proto}"
      DEPENDS "${src_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# src_grpc_proto
add_library(src_grpc_proto
  ${src_grpc_srcs}
  ${src_grpc_hdrs}
  ${src_proto_srcs}
  ${src_proto_hdrs})
target_link_libraries(src_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets greeter_[async_](client|server)
foreach(_target
  client server)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    src_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
