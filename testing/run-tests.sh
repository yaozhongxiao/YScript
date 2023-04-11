#!/bin/bash
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

# set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd -P)"
YSCRIPT_HOME=${SCRIPT_DIR}/..
TESTS_HOME=${SCRIPT_DIR}

function usage() {
  cat << EOF
  Usage: $0 [options]
  Options:
      --cli  bin        cli binary options: [ysrun]
      -h|--help         Print This Message
      --abort           Abort Testing While Case Failure
      --logging         Show Testing Logs
EOF
  exit 0;
}

cli_bin=ysrun
abort_fail=false
show_logging=false
function options_parse() {
  while test $# -gt 0; do
      case "$1" in
        -h|--help)
          usage
          ;;
        --abort)
          abort_fail=true
          ;;
        --logging)
          show_logging=true
          ;;
        --cli)
          shift
          cli_bin=$1
          ;;
        *)
        cli_bin=$1
      esac
      shift
  done
}

echo $0 $@
options_parse $@

cd ${TESTS_HOME}
EXEC_CLI=${YSCRIPT_HOME}/out/tools/cli/${cli_bin}
if [ ! -f ${EXEC_CLI} ];then
  echo "${cli_bin} missed, please build ${cli_bin} before hands!"
  exit -1
fi

test_logs=()
test_suit=samples
# test_cases=(`find samples -name "*.ys"`)
test_cases=(
            samples/assignment/global.ys
            samples/class/inherited_method.ys
            samples/constructor/call_init_explicitly.ys
            samples/method/print_bound_method.ys
            samples/for/closure_in_body.ys
            samples/function/print.ys
            samples/inheritance/constructor.ys
            samples/super/bound_method.ys
            samples/this/closure.ys
          )
# fail_cases=
total_cases=${#test_cases[@]}
suc_cases=0
for((i=0; i<${total_cases}; i++))
do
  echo "[$i] : start run ${test_cases[$i]}"
  ${EXEC_CLI} ${test_cases[$i]}
  if [ $? -eq 0 ];then
    ((suc_cases++))
    echo "[$i]: ${test_cases[$i]} passed!"
    test_logs+=("[$i]: ${EXEC_CLI} ${test_cases[$i]} passed!")
  else
    echo "x[$i]: ${test_cases[$i]} failed!"
    test_logs+=("x[$i]: ${EXEC_CLI} ${test_cases[$i]} failed!")
    if [ x"${abort_fail}" = x"true" ];then
      exit -1
    fi
  fi
done

echo ""
echo "${test_suit}[${suc_cases}/${total_cases}]: ${suc_cases} success out of ${total_cases} cases!"
echo "=================================================="
function show_logs() {
  for log in "${test_logs[@]}"
  do
    echo ${log}
  done
}

if [ x"${show_logging}" = x"true" ];then
  show_logs
fi
if [ ${suc_cases} -ne ${total_cases} ];then
  if [ x"${show_logs}" = x"true" ];then
    show_logs
  fi
  exit -1
fi
