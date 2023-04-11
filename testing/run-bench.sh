#!/usr/bin/bash
#
# Copyright 2023 Develop Group Participants. All right reserver.
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
set -e

SCRIPT_ROOT="$(cd "$(dirname "$0")"; pwd -P)"
CI_ROOT=${SCRIPT_ROOT}

if [ ! -d "build/benchmark" ];then
  echo "can not find benchmark to be run!"
  exit
fi

cd build
ctest -L benchmark

if [ x$1 != x"-v" -a x$1 != x"--verbose" ];then
  exit 0
fi
cd benchmark
echo `pwd`
while read benchmark; do
   echo ${benchmark}; 
   ./${benchmark}; done << EOF
$(ls bm_*)
EOF


