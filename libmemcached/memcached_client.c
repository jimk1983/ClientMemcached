
#include <libmemcached/memcached.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
 
#define TOTAL_NUM	3
 
int main(int argc, char **argv){
 
	//配置
	memcached_st *memc = NULL;
	memcached_return rc = 0;
	memcached_server_st *servers;		//服务器列表
	
	memc = memcached_create(NULL);
	
	servers = memcached_server_list_append(NULL, (char *)"182.61.36.87",11211, &rc);
	//servers = memcached_server_list_append(servers, (char *)"127.0.0.1",11212, &rc);
	//servers = memcached_server_list_append(servers, (char *)"127.0.0.1",11213, &rc);
	
	rc = memcached_server_push(memc, servers);
	
	memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA);
	
	//写入
	char key[16];
	size_t key_len = 0;
	int i =0;
	for(i = 0; i < TOTAL_NUM; i++){
		key_len = snprintf(key, 16, "key%d", i);
		//key-value
		rc = memcached_set(memc, key, key_len, key, key_len, (time_t)1800, (uint32_t)0);		
	
		if(rc == MEMCACHED_SUCCESS){
			static int no;
			if((no++ % 1000) == 0) printf("key:%s value:%s\n", key, key);			
		}else{
			printf("set key(%s) failed, reason:%s\n", key, memcached_strerror(memc, rc));	
		}
	}
	
	//读取
	char *result_str = NULL;
	size_t result_str_len = 0;
	uint32_t flags;
	
	for(i = 0; i < TOTAL_NUM; i++){
		key_len = snprintf(key, 16, "key%d", i);
		result_str = memcached_get(memc, key, key_len, &result_str_len, &flags, &rc);
		
		if(rc == MEMCACHED_SUCCESS){
			static int num;
			if((num++ % 1000) == 0) printf("key:%s value:%s\n", key, result_str);		
		}else{
			fprintf(stderr, "get key %s failed.\n", key);
		}
	}
	
	memcached_free(memc);
	
	return 0;
}
