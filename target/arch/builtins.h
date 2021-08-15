/*
 * Copyright 2021 ETH Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#ifdef __pulp__
/* Position of the most significant bit of x */
#define __FF1(x) __builtin_pulp_ff1((x))
#define __FL1(x) __builtin_pulp_fl1((x))
#else
#error FF1 is an unimplemented builtin
#define __FF1(x) 
#define __FL1(x) (31 - __builtin_clz((x)))
#endif

#endif /* __BUILTINS_H__ */
