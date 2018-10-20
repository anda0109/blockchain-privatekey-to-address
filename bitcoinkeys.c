#include "bitcoinkeys.h"
#include "bignum256.h"
#include "sha256.h"
#include "ripemd160.h"
#include "base58.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>

/*
Version = 1 byte of 0 (zero); on the test network, this is 1 byte of 111
Key hash = Version concatenated with RIPEMD-160(SHA-256(public key))
Checksum = 1st 4 bytes of SHA-256(SHA-256(Key hash))
Bitcoin Address = Base58Encode(Key hash concatenated with Checksum)
https://en.bitcoin.it/wiki/Protocol_documentation#Addresses
*/
void bitcoinPrivate2Address(const char privIn[64], char * privStr, char addrStr[35], int mainNet)
{
	uint8_t priv[32];
	uint8_t x[32];
	uint8_t y[32];
	uint8_t pub33[33];//保存公钥
	uint8_t h256[32+1];//sha256后的hash
	uint8_t hripemd[20+12];//ripemd后的hash
	uint8_t keyhash[21+1];
	uint8_t hdouble256[32+1];
	uint8_t checksum[4];
	uint8_t addressbin[25];

	//1、从私钥得到公钥public key
	bigFromHexString(privIn, priv);
	ecPubkey(priv, x, y);

	if (y[0] % 2)
		pub33[32] = 0x03;
	else
		pub33[32] = 0x02;
	memcpy(pub33, x, 32);

	//2、h256 = sha256(public key)
	HashState hs;
	sha256Begin(&hs);
	for (int i = 0; i < 33; ++i)
		sha256WriteByte(&hs, pub33[32-i]);
	sha256Finish(&hs);
	writeHashToByteArray(h256, &hs, true);

	//3、keyhash = version + ripemd160(h256)
	ripemd160Begin(&hs);
	for (int i = 0; i<32; i++)
		ripemd160WriteByte(&hs, h256[i]);
	ripemd160Finish(&hs);
	writeHashToByteArray(hripemd, &hs, true);
	if(mainNet == 1)
		keyhash[0] = 0;//litecoin:pubkeyhash(0x30),scripthash:(0x32)
	else
		keyhash[0] = 111;
	for (int i = 0; i < 20; ++i)
		keyhash[i + 1] = hripemd[i];

	//4、checksum = first 4bytes of SHA256(SHA256(keyhash))
	sha256Begin(&hs);
	for (int i = 0; i < 21; ++i)
		sha256WriteByte(&hs, keyhash[i]);
	sha256FinishDouble(&hs);
	writeHashToByteArray(hdouble256, &hs, true);
	for (int i = 0; i < 4; ++i)
		checksum[i] = hdouble256[i];

	//5、Address = Base58Encode(keyhash + checksum)
	memcpy(addressbin, keyhash, 21);
	memcpy(addressbin + 21, checksum, 4);

	int addrlen = 35;
	b58enc(addrStr, &addressbin, addressbin, 25);
}
