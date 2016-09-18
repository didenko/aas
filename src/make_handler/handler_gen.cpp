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
#include <algorithm>
#include <thread>
#include <fstream>

#include "pugixml.hpp"
#include "NLTemplate.h"

const auto handler_template = std::string{R"template(
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

#include "MessageHeader.h"{% block message_includes %}
#include "{{ name }}.h"{% endblock %}

{{ namespaces_open }}
inline static ::aeron::fragment_handler_t make_handler() {
  return [](
      ::aeron::AtomicBuffer &buffer,
      ::aeron::util::index_t offset,
      ::aeron::util::index_t length,
      ::aeron::Header &header
  ) -> void {
    MessageHeader hdr;
    hdr.wrap(
        (char *) buffer.buffer(),
        (const uint64_t) offset,
        0,
        (const uint64_t) buffer.capacity()
    );

    switch (hdr.templateId()) {
      {% block message_cases %}
      case {{ name }}::sbeTemplateId(): {
        {{ name }} body;
        body.wrapForDecode(
          (char *) buffer.buffer(),
          offset + hdr.encodedLength(),
          hdr.blockLength(),
          hdr.version(),
          (const uint64_t) buffer.capacity()
        );
        intake<{{ name }}>(hdr, body);
        break;
      }
      {% endblock %}
    }
  };
};

{{ namespaces_close }}

)template"};

std::size_t replace(std::string &in, const std::string &from, const std::string &to) {
  if (in.size() == 0) return 0;
  std::string::size_type cursor = 0;
  std::string::size_type new_cursor = 0;
  std::size_t count = 0;
  while (true) {
    new_cursor = in.find(from, cursor);
    if (new_cursor == std::string::npos) break;
    in.replace(new_cursor, from.size(), to);
    ++count;
    if ((cursor = new_cursor + to.size()) >= in.size()) break;
  }
  return count;
}

void failedUsage() {
  std::cerr
      << "Invalid number of arguments. The following arguments are expected:" << std::endl << std::endl
      << "1. Schema filename." << std::endl
      << "2. Output handler code filename." << std::endl << std::endl;
  exit(1);
}

void failedOutput(const char *filename) {
  std::cerr
      << "Failed to open the "
      << filename
      << " file for writing."
      << std::endl;
  exit(1);
}

void failureSchemaLoad(const char *descr) {
  std::cerr
      << "XML schema load failure: "
      << descr
      << std::endl;
  exit(1);
}

int main(int argc, char *argv[]) {

  if (argc != 3) failedUsage();

  std::ofstream output(argv[2]);
  if (!output) failedOutput(argv[2]);

  pugi::xml_document doc;
  auto result = doc.load_file(argv[1]);
  if (!result) failureSchemaLoad(result.description());

  auto nspaces = std::string{doc.child("sbe:messageSchema").attribute("package").value()};
  auto nspaces_num = nspaces.size() == 0 ? 0 : replace(nspaces, ".", " {\nnamespace ") + 1;
  if (nspaces_num > 0) nspaces = "namespace " + nspaces + " {\n";

  auto messages = doc.child("sbe:messageSchema").children("sbe:message");

  try {
    NL::Template::LoaderMemory tpl_loader;
    tpl_loader.add("handler_template", handler_template);
    auto handler = NL::Template::Template(tpl_loader);
    handler.load("handler_template");

    size_t num_msg_classes = (std::size_t) std::distance(messages.begin(), messages.end());
    handler.block("message_includes").repeat(num_msg_classes);
    handler.block("message_cases").repeat(num_msg_classes);

    handler.set("namespaces_open", nspaces);

    std::size_t i{0};
    for (const auto &msg: messages) {
      auto name = msg.attribute("name").value();
      handler
          .block("message_includes")[i]
          .set("name", name);
      handler
          .block("message_cases")[i]
          .set("name", name);
      ++i;
    };

    handler.set("namespaces_close", std::string(nspaces_num, '}'));

    handler.render(output);
  }
  catch (const char *exc) {
    std::cerr
        << "Template exception: "
        << exc
        << std::endl;
    exit(1);
  }
}
