/*
 * commhash.cpp
 *
 *  Created on: 2014年12月16日
 *      Author: water
 */

#include "commhash.h"
#include <ctype.h>

namespace triones
{
CommHash::CommHash() {
	// TODO Auto-generated constructor stub

}

CommHash::~CommHash() {
	// TODO Auto-generated destructor stub
}

}

namespace triones
{

uint32_t CommHash::_hash_seed = 5381;


/* Thomas Wang's 32 bit Mix Function */
unsigned int CommHash::int_hash(unsigned int key)
{
	key += ~(key << 15);
	key ^= (key >> 10);
	key += (key << 3);
	key ^= (key >> 6);
	key += ~(key << 11);
	key ^= (key >> 16);
	return key;
}

///* Identity hash function for integer keys */
unsigned int CommHash::id_hash(unsigned int key)
{
	return key;
}

/* MurmurHash2, by Austin Appleby
 * Note - This code makes a few assumptions about how your machine behaves -
 * 1. We can read a 4-byte value from any address without crashing
 * 2. sizeof(int) == 4
 *
 * And it has a few limitations -
 *
 * 1. It will not work incrementally.
 * 2. It will not produce the same results on little-endian and big-endian
 *    machines.
 */

unsigned int CommHash::str_hash(const void *key, int len)
{
	/* 'm' and 'r' are mixing constants generated offline.
	 They're not really 'magic', they just happen to work well.  */
	uint32_t seed = _hash_seed;
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	/* Initialize the hash to a 'random' value */
	uint32_t h = seed ^ len;

	/* Mix 4 bytes at a time into the hash */
	const unsigned char *data = (const unsigned char *) key;

	while (len >= 4)
	{
		uint32_t k = *(uint32_t*) data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	/* Handle the last few bytes of the input array  */
	switch (len)
	{
	case 3:
	h ^= data[2] << 16;
	case 2:
	h ^= data[1] << 8;
	case 1:
	h ^= data[0];
	h *= m;
	};

	/* Do a few final mixes of the hash to ensure the last few
	 * bytes are well-incorporated. */
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return (unsigned int) h;
}

/* And a case insensitive hash function (based on djb hash) */
unsigned int CommHash::istr_hash(const unsigned char *buf, int len)
{
	unsigned int hash = (unsigned int) _hash_seed;

	while (len--)
		hash = ((hash << 5) + hash) + (tolower(*buf++)); /* hash * 33 + c */
	return hash;
}

}
