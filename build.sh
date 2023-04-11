#!/usr/bin/env bash
# 
# Copyright 2023 Develop Group Participants. All rights reserved.
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

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd -P)"
YSCRIPT_HOME=${SCRIPT_DIR}
BUILD_DIR=${YSCRIPT_HOME}/out

function usage() {
  cat << EOF
  Usage: $0 [options]
  Options:
      -c|--clean       Clean Project
      -f|--force       Force clean
      -h|--help        Print This Message
EOF
  exit 0;
}

function cleanup() {
  echo "-- clean project"
  if [ -d "${BUILD_DIR}" ];then
    rm -rf ${BUILD_DIR}
  fi
  exit 0
}

force_flags=false
build_target=target_all
function options_parse() {
  while test $# -gt 0; do
      case "$1" in
        -h|--help)
          usage
          ;;
        -c|--clean)
          cleanup
          ;;
        -f|--force)
          force_flags="-f"
          ;;
        *)
        build_target=$1
      esac
      shift
  done
}

echo $0 $@
options_parse $@

cd ${YSCRIPT_HOME}
if [ -d ${BUILD_DIR} -a x"$force_flags" = x"-f" ];then
  echo "** force clean ${BUILD_DIR}"
  rm -rf ${BUILD_DIR}
fi

if [ ! -d ${BUILD_DIR} ];then
  mkdir -p ${BUILD_DIR}
fi

cd ${BUILD_DIR}
cmake ${YSCRIPT_HOME}
make
