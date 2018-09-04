/** \file ripemd160.h
  *
  * \brief Describes functions exported by ripemd160.c.
  *
  * To calculate a RIPEMD-160 hash, call ripemd160Begin(), then call
  * ripemd160WriteByte() for each byte of the message, then call
  * ripemd160Finish(). The hash will be in HashState#h, but it can also be
  * extracted and placed into to a byte array using writeHashToByteArray().
  *
  * This file is licensed as described by the file LICENCE.
  */

#ifndef RIPEMD160_H_INCLUDED
#define RIPEMD160_H_INCLUDED

#include "common.h"
#include "hash.h"

extern void ripemd160Begin(HashState *hs);
extern void ripemd160WriteByte(HashState *hs, uint8_t byte);
extern void ripemd160Finish(HashState *hs);

//一次调用
extern void ripemd160(const uint8_t *msg, uint32_t msg_len, uint8_t *hash);

#endif // #ifndef RIPEMD160_H_INCLUDED
