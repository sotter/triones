/*
 * commhash.h
 *
 *  Created on: 2014年12月16日
 *      Author: water
 */

#ifndef COMMHASH_H_
#define COMMHASH_H_
#include <stdint.h>

namespace triones
{
class CommHash {
public:
	CommHash();
	virtual ~CommHash();

public:
	static unsigned int int_hash(unsigned int key);
	static unsigned int id_hash(unsigned int key);
	static unsigned int str_hash(const void *key, int len);
	static unsigned int istr_hash(const unsigned char *buf, int len);
	static uint32_t _hash_seed;
};


}
#endif /* COMMHASH_H_ */
