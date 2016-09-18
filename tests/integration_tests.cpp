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

#include <iostream>
#include <iomanip>
#include <cstdlib>

#include <gtest/gtest.h>

#include <Aeron.h>

#include "integration_tests_intakes.h"
#include "tests_messages/MakeHandler.h"

namespace tests {
namespace integrated {

namespace config {
static const std::string CHANNEL = "aeron:udp?endpoint=localhost:50123";
static const std::int32_t STREAM_ID = 10;
static const int FRAGMENTS_LIMIT = 10;
static const int64_t LINGER_MS = 250;
static const std::chrono::duration<long, std::milli> IDLE_SLEEP_MS(1);
}

typedef std::array<std::uint8_t, 256> buffer_t;

struct AeronIntegration: public ::testing::Test {
  ::aeron::Context context;
  std::shared_ptr<::aeron::Aeron> aeron;

protected:
  virtual void SetUp() {
    context.errorHandler(
        [](const std::exception &e) {
          std::cerr
              << "context.errorHandler caught an exception:" << std::endl
              << std::endl << e.what() << std::endl;
          std::cerr.flush();
          std::exit(1);
        }
    );

    try {
      aeron = ::aeron::Aeron::connect(context);
      EXPECT_NE(nullptr, aeron);
    }
    catch (const ::aeron::IOException &e) {
      FAIL() << e.what() << std::endl
             << "It is likely that the Aeron MediaDriver is not running." << std::endl;
    };
  }

};

TEST_F(AeronIntegration, passMessages) {
  std::atomic_bool testing{true};
  std::shared_ptr<::aeron::Subscription> subscription;
  std::shared_ptr<::aeron::Publication> publication;

  std::cout << std::endl;

  try {
    std::int64_t id_sub = aeron->addSubscription(config::CHANNEL, config::STREAM_ID);
    subscription = aeron->findSubscription(id_sub);
    while (!subscription) {
      std::this_thread::yield();
      subscription = aeron->findSubscription(id_sub);
    }

    std::int64_t id_pub = aeron->addPublication(config::CHANNEL, config::STREAM_ID);
    publication = aeron->findPublication(id_pub);
    while (!publication) {
      std::this_thread::yield();
      publication = aeron->findPublication(id_pub);
    }
  }
  catch (std::exception &e) {
    FAIL() << e.what() << " : " << std::endl;
  }

  auto handler = messages::make_handler();

  auto listener = std::thread(
      [this, &testing, &subscription, handler]() {
        ::aeron::SleepingIdleStrategy idler(config::IDLE_SLEEP_MS);
        while (testing) {
          const int fragmentsRead = subscription->poll(handler, config::FRAGMENTS_LIMIT);
          if (0 < fragmentsRead) {
          }
          idler.idle(fragmentsRead);
        };
      }
  );

  typedef std::array<std::uint8_t, 256> buffer_t;
  AERON_DECL_ALIGNED(buffer_t buffer, 8);

  ::aeron::concurrent::AtomicBuffer abuff(&buffer[0], buffer.size());

  using namespace messages;
  MessageHeader hdr;
  Directory dir;

  hdr.wrap((char *) &buffer[0], 0, 0, buffer.size())
      .blockLength(Directory::sbeBlockLength())
      .templateId(Directory::sbeTemplateId())
      .schemaId(Directory::sbeSchemaId())
      .version(Directory::sbeSchemaVersion());

  dir.wrapForEncode((char *) &buffer[0], hdr.encodedLength(), buffer.size());

  dir.putVenue("NSDQ");
  messages::Directory::Products &prod = dir.productsCount(2);
  prod.next().prod_id(1).putSymbol("AAPL");
  prod.next().prod_id(2).putSymbol("GOOG");

  auto result = publication->offer(abuff, 0, (aeron::index_t) (hdr.encodedLength() + dir.encodedLength()));

  ::aeron::SleepingIdleStrategy idler(config::IDLE_SLEEP_MS);

  auto cutoff = std::chrono::steady_clock::now() + std::chrono::milliseconds(config::LINGER_MS);
  while (result < 0 and cutoff > std::chrono::steady_clock::now()) {
    idler.idle(1);
    result = publication->offer(abuff, 0, (aeron::index_t) (hdr.encodedLength() + dir.encodedLength()));
  }

  if (result < 0) {
    if (::aeron::NOT_CONNECTED == result) {
      std::cout << "Not connected yet" << std::endl;
    }
    else if (::aeron::BACK_PRESSURED == result) {
      std::cout << "Back-pressured" << std::endl;
    }
    else {
      std::cout << "Unknown offer response" << result << std::endl;
    }
  }
  else {
    std::cout << "Successfully offered" << std::endl;
  }

  std::cout << "Lingering for " << config::LINGER_MS << " milliseconds." << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(config::LINGER_MS));

  testing.store(false);
  listener.join();
}
}
}
