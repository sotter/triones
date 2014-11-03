/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "util.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef HAVE_GETTIMEOFDAY
#define HAVE_GETTIMEOFDAY
#endif

#if defined(HAVE_CLOCK_GETTIME)
#include <time.h>
#elif defined(HAVE_GETTIMEOFDAY)
#include <sys/time.h>
#endif // defined(HAVE_CLOCK_GETTIME)
namespace triones
{

int64_t Util::current_time_ticks(int64_t ticks_per_sec)
{
	int64_t result;

#if defined(HAVE_CLOCK_GETTIME)
	struct timespec now;
	int ret = clock_gettime(CLOCK_REALTIME, &now);
	assert(ret == 0);
	to_ticks(result, now, ticks_per_sec);
#elif defined(HAVE_GETTIMEOFDAY)
	struct timeval now;
	int ret = gettimeofday(&now, NULL);
	assert(ret == 0);
	to_ticks(result, now, ticks_per_sec);
#else
	result = time(NULL);
#endif // defined(HAVE_CLOCK_GETTIME)
	return result;
}

} // namespace triones
