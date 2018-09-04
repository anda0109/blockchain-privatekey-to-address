/*
**  (c)maester.xyz
**  EOSIO keys and address CLang lib
**  No dependence except clib. Can be used on linux, windows, Android or iOS
*/

#ifndef __EOSKEYS__
#define __EOSKEYS__

#define u8 unsigned char

#define BN256_STR_LEN 64
#define BN256_STR_LEN 64

#ifdef __cplusplus
extern "C" { 
#endif

void eosPrivate2Address(const char privIn[64], char * privStr, char *addrStr);

#ifdef __cplusplus
}
#endif

#endif
