#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "bch.h"
#include "base32_bch.h"

char* encode(char prefix[], char tp[], uint8_t hh[]);
uint8_t* toUint5Array(uint8_t data[], int data_len);
uint8_t* prefixToUint5Array(uint8_t* prefix, int len);

struct btcaddr
{
	uint8_t version;
	uint8_t pubkey_hash[20];
	uint8_t checksum[4];
};

//12c6DSiU4Rq3P4ZxziKxzrL5LmMBrzjrJX
//bitcoincash:qqgekzvw96vq5g57zwdfa5q6g609rrn0ycp33uc325

void btc_to_bch(char addrStr[35])
{
	int datalen = 21;
	uint8_t data[25];
	base58_decode_check(addrStr, data, datalen);
	struct btcaddr addr;
	memcpy(&addr, data, 25);

	char prefix[20] = {0};
	char type[20] = {0};
	switch (addr.version)
	{
	case 0x00:
		strncpy(prefix, "bitcoincash", strlen("bitcoincash"));
		strncpy(type, "P2PKH", strlen("P2PKH"));
		break;
	case 0x05:
		strncpy(prefix, "bitcoincash", strlen("bitcoincash"));
		strncpy(type, "P2SH", strlen("P2SH"));
		break;
	case 0x6F:
		strncpy(prefix, "bchtest", strlen("bchtest"));
		strncpy(type, "P2PKH", strlen("P2PKH"));
		break;
	default:
		break;
	}

	char* bchaddr = encode(prefix, type, addr.pubkey_hash);
	printf("%s:%s\n", prefix, bchaddr);
	free(bchaddr);
}

int getTypeBits(char tp[])
{
	if (strcmp(tp, "P2PKH")==0)
		return 0;
	if (strcmp(tp, "P2SH")==0)
		return 8;
	return -1;
}

int getHashSizeBits(int hash_len)
{
	switch (hash_len* 8) 
	{
	case 160:
		return 0;
	case 192:
		return 1;
	case 224:
		return 2;
	case 256:
		return 3;
	case 320:
		return 4;
	case 384:
		return 5;
	case 448:
		return 6;
	case 512:
		return 7;
	default:
		return -1;
	}
}

/**
* Computes a checksum from the given input data as specified for the CashAddr
* format: https://github.com/Bitcoin-UAHF/spec/blob/master/cashaddr.md.
*
* @private
* @param {Uint8Array} data Array of 5-bit integers over which the checksum is to be computed.
* @returns {BigInteger}
*/
int64_t polymod(uint8_t* data, int data_len) {
	int64_t GENERATOR[] = { 0x98f2bc8e61, 0x79b76d99e2, 0xf33e5fb3c4, 0xae2eabe2a8, 0x1e4f43e470 };
	int64_t checksum = 1;
	for (int i = 0; i < data_len; ++i) {
		uint8_t value = data[i];
		int64_t topBits = checksum>>35;
		checksum = ((checksum & (0x07ffffffff))<<5)^value;
		//checksum = checksum.and(0x07ffffffff).shiftLeft(5).xor(value);
		for (int j = 0; j < 5; ++j) {
			if (((topBits>>j)&1) == 1)
			{
				checksum = checksum^(GENERATOR[j]);
			}
		}
	}
	return checksum ^ (1);
}

/**
* Returns an array representation of the given checksum to be encoded
* within the address' payload.
*
* @private
* @param {BigInteger} checksum Computed checksum.
* @returns {Uint8Array}
*/
uint8_t* checksumToUint5Array(uint64_t checksum) {
	printf("checksum:%I64d\n", checksum);
	uint8_t* result = malloc(8);
	for (int i = 0; i < 8; ++i) {
		result[7 - i] = checksum & (31);
		checksum = checksum>> (5);
	}
	return result;
}


char* encode(char prefix[], char tp[], uint8_t hh[20]) 
{
	    
		//prefix
		int prefixDataLen = strlen(prefix) + 1;
		uint8_t* prefixData = malloc(strlen(prefix)+1);
		memset(prefixData, 0, strlen(prefix) + 1);
		memcpy(prefixData, prefixToUint5Array(prefix, strlen(prefix)), strlen(prefix));

		//°æ±¾×Ö½Ú 
		uint8_t payloadDataUint8[25] = { 0 };
		uint8_t versionByte = getTypeBits(tp) + getHashSizeBits(20);
		int index = 0;
		memcpy(payloadDataUint8 + index, &versionByte, sizeof(versionByte));
		index += sizeof(versionByte);
		 
		memcpy(payloadDataUint8 + index, hh, 20);
		index += 20;

		uint8_t* payloadData = toUint5Array(payloadDataUint8, index);
		int payloadDataLen = 34;
		for (int i = 0; i < 34; ++i)
		{
			printf("%d\n", payloadData[i]);
		}

		//
		int checksumDataLen = prefixDataLen + payloadDataLen + 8;
		uint8_t* checksumData = malloc(checksumDataLen);
		memset(checksumData, 0, prefixDataLen + payloadDataLen + 8);
		memcpy(checksumData, prefixData, prefixDataLen);
		memcpy(checksumData + prefixDataLen, payloadData, payloadDataLen);
		
		//var checksumData = concat(concat(prefixData, payloadData), new Uint8Array(8));
		int payloadLen = payloadDataLen + 8;
		uint8_t* payload = malloc(payloadLen);
		memset(payload, 0, payloadLen);
		memcpy(payload, payloadData, payloadDataLen);
		memcpy(payload + payloadDataLen, checksumToUint5Array(polymod(checksumData, checksumDataLen)), 8);

		for (int i = 0; i < 42; ++i)
		{
			printf("%d\n", payload[i]);
		}

		char* bchaddr = base32_encode(payload, payloadLen);
		printf(bchaddr);
		return bchaddr;
}

uint8_t* convertBits(uint8_t data[], int data_len, int from, int to, bool strictMode) {
	int length = data_len * from / to;
	if((data_len * from) % to != 0)
		length = strictMode? length : length+1;
	int mask = (1 << to) - 1;
	uint8_t* result = malloc(length);
	int index = 0;
	int accumulator = 0;
	int bits = 0;
	for (int i = 0; i < data_len; ++i) 
	{
		uint8_t value = data[i];
		//validate(0 <= value && (value >> from) == = 0, 'Invalid value: ' + value + '.');
		accumulator = (accumulator << from) | value;
		bits += from;
		while (bits >= to) {
			bits -= to;
			result[index] = (accumulator >> bits) & mask;
			++index;
		}
	}
	if (!strictMode) {
		if (bits > 0) {
			result[index] = (accumulator << (to - bits)) & mask;
			++index;
		}
	}
	else {
		//validate(bits < from && ((accumulator << (to - bits)) & mask) == = 0,'Input cannot be converted to ' + to + ' bits without padding, but strict mode was used.'	);
	}
	return result;
};

/**
* Converts an array of 8-bit integers into an array of 5-bit integers,
* right-padding with zeroes if necessary.
*
* @private
* @param {Uint8Array} data
* @returns {Uint8Array}
*/
uint8_t* toUint5Array(uint8_t data[], int data_len) {
	return convertBits(data, data_len, 8, 5, false);
}


uint8_t* prefixToUint5Array(uint8_t* prefix, int len) {
	uint8_t* result = (uint8_t*)malloc(len);
	for (int i = 0; i < len; ++i) 
	{
		result[i] = prefix[i] & 31;
	}
	return result;
}

/*
function toCashAddress(address) {
	var decoded = decodeAddress(address)
	return encodeAsCashaddr(decoded)
}


function decodeAddress(address) {
	try {
		return decodeBase58Address(address)
	}
	catch (error) {
	}
	try {
		return decodeCashAddress(address)
	}
	catch (error) {
	}
	throw new InvalidAddressError()
}


function encodeAsCashaddr(decoded) {
	var prefix = decoded.network == = Network.Mainnet ? 'bitcoincash' : 'bchtest'
		var type = decoded.type == = Type.P2PKH ? 'P2PKH' : 'P2SH'
		var hash = Uint8Array.from(decoded.hash)
		return cashaddr.encode(prefix, type, hash)
}


function encode(prefix, type, hash) {
	validate(typeof prefix == = 'string' && isValidPrefix(prefix), 'Invalid prefix: ' + prefix + '.');
	validate(typeof type == = 'string', 'Invalid type: ' + type + '.');
	validate(hash instanceof Uint8Array, 'Invalid hash: ' + hash + '.');
	var prefixData = concat(prefixToUint5Array(prefix), new Uint8Array(1));
	var versionByte = getTypeBits(type) + getHashSizeBits(hash);
	var payloadData = toUint5Array(concat(Uint8Array.of(versionByte), hash));//34 bytes
	var checksumData = concat(concat(prefixData, payloadData), new Uint8Array(8));
	var payload = concat(payloadData, checksumToUint5Array(polymod(checksumData)));
	return prefix + ':' + base32.encode(payload);
}
*/
