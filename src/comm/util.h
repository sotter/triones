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

#ifndef __TRIONES_UTIL_H__
#define __TRIONES_UTIL_H__

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

namespace triones
{

/**
 * Utility methods
 *
 * This class contains basic utility methods for converting time formats,
 * and other common platform-dependent concurrency operations.
 * It should not be included in API headers for other concurrency library
 * headers, since it will, by definition, pull in all sorts of horrid
 * platform dependent stuff.  Rather it should be inluded directly in
 * concurrency library implementation source.
 */
class Util
{

	static const int64_t NS_PER_S = 1000000000LL;
	static const int64_t US_PER_S = 1000000LL;
	static const int64_t MS_PER_S = 1000LL;

	static const int64_t NS_PER_MS = NS_PER_S / MS_PER_S;
	static const int64_t NS_PER_US = NS_PER_S / US_PER_S;
	static const int64_t US_PER_MS = US_PER_S / MS_PER_S;

public:

	/**
	 * Converts millisecond timestamp into a timespec struct
	 *
	 * @param struct timespec& result
	 * @param time or duration in milliseconds
	 */
	static void to_timespec(struct timespec& result, int64_t value, int64_t n = 0)
	{
		result.tv_sec = value / MS_PER_S + n; // ms to s
		result.tv_nsec = (value % MS_PER_S) * NS_PER_MS; // ms to ns
	}

	static void to_timeval(struct timeval& result, int64_t value)
	{
		result.tv_sec = value / MS_PER_S; // ms to s
		result.tv_usec = (value % MS_PER_S) * US_PER_MS; // ms to us
	}

	static const void to_ticks(int64_t& result, int64_t secs, int64_t old_ticks,
			int64_t old_ticks_per_sec, int64_t new_ticks_per_sec)
	{
		result = secs * new_ticks_per_sec;
		result += old_ticks * new_ticks_per_sec / old_ticks_per_sec;

		int64_t old_per_new = old_ticks_per_sec / new_ticks_per_sec;
		if (old_per_new && ((old_ticks % old_per_new) >= (old_per_new / 2)))
		{
			++result;
		}
	}

	/**
	 * Converts struct timespec to arbitrary-sized ticks since epoch
	 */
	static const void to_ticks(int64_t& result, const struct timespec& value, int64_t ticks_per_sec)
	{
		return to_ticks(result, value.tv_sec, value.tv_nsec, NS_PER_S, ticks_per_sec);
	}

	/**
	 * Converts struct timeval to arbitrary-sized ticks since epoch
	 */
	static const void to_ticks(int64_t& result, const struct timeval& value, int64_t ticks_per_sec)
	{
		return to_ticks(result, value.tv_sec, value.tv_usec, US_PER_S, ticks_per_sec);
	}

	/**
	 * Converts struct timespec to milliseconds
	 */
	static const void to_mseconds(int64_t& result, const struct timespec& value)
	{
		return to_ticks(result, value, MS_PER_S);
	}

	/**
	 * Converts struct timeval to milliseconds
	 */
	static const void to_mseconds(int64_t& result, const struct timeval& value)
	{
		return to_ticks(result, value, MS_PER_S);
	}

	/**
	 * Converts struct timespec to microseconds
	 */
	static const void to_useconds(int64_t& result, const struct timespec& value)
	{
		return to_ticks(result, value, US_PER_S);
	}

	/**
	 * Converts struct timeval to microseconds
	 */
	static const void to_useconds(int64_t& result, const struct timeval& value)
	{
		return to_ticks(result, value, US_PER_S);
	}

	/**
	 * Get current time as a number of arbitrary-size ticks from epoch
	 */
	static const int64_t current_time_ticks(int64_t ticks_per_sec);

	/**
	 * Get current time as milliseconds from epoch
	 */
	static const int64_t current_time()
	{
		return current_time_ticks(MS_PER_S);
	}

	/**
	 * Get current time as micros from epoch
	 */
	static const int64_t current_time_useconds()
	{
		return current_time_ticks(US_PER_S);
	}
};

} // namespace triones

#endif // #ifndef __TRIONES_UTIL_H__
