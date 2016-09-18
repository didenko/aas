/*
Copyright 2016 Vlad Didenko

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <Aeron.h>
#include <sbe/sbe.h>

#include "tests_messages/MessageHeader.h"
#include "tests_messages/Request.h"
#include "tests_messages/Directory.h"

namespace tests {
namespace messages {

template<class MessageBody>
void intake(MessageHeader &hdr, MessageBody &body);

template<>
void intake(MessageHeader &hdr, Request &req) {
  std::cout
      << "Received a request for "
      << req.getSymbolAsString()
      << std::endl;
};

template<>
void intake(MessageHeader &hdr, Directory &dir) {
  ASSERT_EQ(std::string{"NSDQ"}, dir.venue()) << "Received a wrong venue value";
  std::cout << "Received venue: " << dir.venue() << std::endl;

  auto &prod = dir.products();
  ASSERT_EQ(2, prod.count()) << "Expected two products";
  std::cout << "Number of products: " << prod.count() << std::endl;

  ASSERT_TRUE(prod.hasNext()) << "No products found";
  prod.next();
  ASSERT_EQ(1, prod.prod_id()) << "Wrong product id";
  ASSERT_EQ(std::string{"AAPL"}, prod.symbol()) << "First product symbol is wrong";
  std::cout << "Product: " << prod.symbol() << std::endl;

  ASSERT_TRUE(prod.hasNext()) << "Second product not found";
  prod.next();
  ASSERT_EQ(2, prod.prod_id()) << "Wrong product id";
  ASSERT_EQ(std::string{"GOOG"}, prod.symbol()) << "Second product symbol is wrong";
  std::cout << "Product: " << prod.symbol() << std::endl;

  ASSERT_FALSE(prod.hasNext()) << "Too many products found";
  std::cout << "Products done" << std::endl;
};

}
}
