#include "map.c"

/*
>> INITALIZING MAP ...
>> ADDING ENTRIES       PASSED
>> ***---DUPLICATE KEY        FAILED ****
>> GETTING VALID KEY    PASSED
>> GETTING INVALID KEY  PASSED
>> ***CHECKING SIZE (4)    FAILED ****
>> REMOVING KEY         PASSED
>> ***GETTING INVALID KEY  FAILED ****
>> GETTING VALID KEY    PASSED
>> ***REMOVING INVALID KEY FAILED ****
>> REMOVING KEY         PASSED
>> REMOVING KEY         PASSED
>> SERIALIZING          PASSED
*/
int main(int argc,char *argv[])
{
	map_t cool_map;
	printf("INITIALIZING MAP ...\r\n");
	map_init(&cool_map);
	printf("ADDING ENTRIES\t");
	map_put(&cool_map,"bom","bom");
	map_put(&cool_map,"bam","bam");
	printf("PASSED\r\nDUPLICATE KEY\t");
	printf("%d\r\n",map_put(&cool_map,"bom","bom"));
	printf("%d\r\n",map_size(&cool_map));
	printf("%d\r\n",map_remove(&cool_map,"fkkka"));
	printf("%s\r\n",map_get(&cool_map,"fuck"));

}