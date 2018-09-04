#include "etherkeys.h"
#include "keccak.h"
#include "ecdsa.h"
#include "bignum256.h"
#include <stdio.h>


void etherPrivate2Address(const char privStr[BN256_STR_LEN], char addrStr[ETH_ADD_STR_LEN])
{
	int i;
	uint8_t temp;
	uint8_t priv[32];
	uint8_t x[32];
	uint8_t y[32];
	uint8_t pub64[64];
	uint8_t outBuff[32];

	bigFromHexString(privStr, priv);
	ecPubkey(priv, x, y);

	memcpy(pub64, y, 32);
	memcpy(pub64 + 32, x, 32);

	for (i = 0; i<32; i++)
	{
		temp = pub64[i];
		pub64[i] = pub64[63 - i];
		pub64[63 - i] = temp;
	}

	//keccak256(outBuff, 32, pub64, 64);
	keccak(pub64, 64, outBuff);

#ifdef __DEBUG
	printf("PUB1: %s\n", pubStr128);
	printf("PUB:  ");
	for (i = 0; i<64; i++)
		printf("%02x", pub64[63 - i]);

	printf("\n\nKeccak: \n");
	for (i = 0; i<32; i++)
		printf("%02x", outBuff[i]);

	printf("\n\n");
#endif    


	snprintf(addrStr, 3, "0x");
	for (i = 0; i<20; i++)
		snprintf(addrStr + 2 + i * 2, 3, "%02x", outBuff[12 + i]);
}