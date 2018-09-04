#ifndef  __H_BITCOIN_KEYS__
#define __H_BITCOIN_KEYS__


#ifdef __cplusplus
extern "C" {
#endif

	extern void bitcoinPrivate2Address(const char privIn[64], char * privStr, char addrStr[35], int mainNet);

#ifdef __cplusplus
}
#endif

#endif
