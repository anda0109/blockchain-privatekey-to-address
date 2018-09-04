#ifndef __H_ETH_H__
#define __H_ETH_H__

#define IN
#define OUT
#define INOU

#ifdef __cplusplus
extern "C" {
#endif

	extern void get_method_id(IN const char* method_code, OUT char method_id[11]);

#ifdef __cplusplus
}
#endif

#endif // !__H_ETH_H__