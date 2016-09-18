#!/bin/bash

#    Copyright 2016 Vlad Didenko
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

my_dir=$(dirname $0)
build_dir=${my_dir}/build
install_dir=${my_dir}/bin

rm -rf "${build_dir}"
rm -rf "${install_dir}"
