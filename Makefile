# Copyright 2021 ETH Zurich
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
# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

TESTS = tests/hello_world_pmsis tests/uart tests/queue tests/semaphore \
	tests/streambufferisr tests/timer \
	nortos/hello_world nortos/memscan nortos/setjmp
#	tests/spi \ # atm messed up due to DPI
#	tests/cluster/cluster_fork_gvsim # does not work with gvsoc
TEST_LOGS = test.log

CTAGS = ctags

check: test

## Run tests. Use separate build trees.
.PHONY: test
test:
	@[ -e $(TEST_LOGS) ] && cp $(TEST_LOGS) old_$(TEST_LOGS); \
	rm -f $(TEST_LOGS); \
	err=0; \
	for test in $(TESTS); do \
	  printf "%-50s" "Running '$$test' ... "; \
	  source env/pulp.sh; \
	  mkdir -p $$test/build; \
	  pushd . > /dev/null; cd $$test/build; \
	  make -f ../Makefile all run-gvsoc >> $(TEST_LOGS); \
	  if [ $$? -eq 0 ]; then \
	    printf "PASS\n"; \
	  else \
	    err=$$?; \
	  fi; \
	  popd > /dev/null; \
	done; \
	exit $$err;

.PHONY: TAGS
TAGS:
	$(CTAGS) -R -e --exclude=support/* --exclude=template/* --exclude=*sim/* .
