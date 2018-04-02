#ifndef  CONFIG_H
#define  CONFIG_H
#if 0
#define  DEBUG_OUT(fmt,...)     \
	printf("file(%s)-line<%d>-function(%s)\n"#fmt"\n",__FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#endif

#define  DEBUG_OUT

#endif
