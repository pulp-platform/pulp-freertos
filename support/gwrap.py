#!/usr/bin/env python3
#
# Copyright (C) 2020 ETH Zurich
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

# Description: Instantiate gvsoc runner
# Authors: Robert Balas (balasr@iis.ee.ethz.ch)

import sys
import os
sys.path.append(os.path.abspath('install/python'))
sys.path.append(os.path.abspath('runner/python'))
sys.path.append(os.path.abspath('pulp-configs/python'))
sys.path.append(os.path.abspath('gvsoc/models'))
import vp_runner

from plp_runner import *

runner = Runner()
retval = runner.run()

exit(retval)
