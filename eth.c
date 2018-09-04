#include "eth.h"
#include "keccak.h"

#include <stdio.h>

//https://ethereum.stackexchange.com/questions/3514/how-to-call-a-contract-method-using-the-eth-call-json-rpc-api
//method_code: transfer(address, uint256)
void get_method_id(IN const char* method_code, OUT char method_id[11])
{
	int len = strlen(method_code);
	uint8_t md[32];
	keccak(method_code, len, md);
	for (int i = 0; i < 4; ++i)
	{
		snprintf(method_id+i*2, 3, "%02x", md[i]);
	}	
}

