//-finstrument-functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX 256 
static char path[PATH_MAX];

__attribute__((constructor no_instrument_function))
static void executable_path_init() {
	char    buf[PATH_MAX];

	memset(buf, 0, sizeof(buf));
	memset(path, 0, sizeof(path));

#ifdef _SOLARIS_TRACE
	getcwd(buf, PATH_MAX);
	sprintf(path, "%s/%s", buf, getexecname());
#elif _LINUX_TRACE
	readlink("/proc/self/exe", path, PATH_MAX);
#else
	    #error "The OS has not been supported!"
#endif
}

__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *this_fn, void *call_site) {
	char buf[PATH_MAX];
	char cmd[PATH_MAX];

	memset(buf, 0, sizeof(buf));
	memset(cmd, 0, sizeof(cmd));

	sprintf(cmd, "addr2line %p -e %s -f | head -1", this_fn, path);

	FILE *ptr = NULL;
	memset(buf, 0, sizeof(buf));

	if ((ptr = popen(cmd, "r")) != NULL) {
		fgets(buf, PATH_MAX, ptr);
		printf("enter func => %p:%s", this_fn, buf);
	}

	(void) pclose(ptr);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *this_fn, void *call_site) {
	char buf[PATH_MAX];
	char cmd[PATH_MAX];

	memset(buf, 0, sizeof(buf));
	memset(cmd, 0, sizeof(cmd));

	sprintf(cmd, "addr2line %p -e %s -f|head -1", this_fn, path);

	FILE *ptr = NULL;
	memset(buf, 0, sizeof(buf));

	if ((ptr = popen(cmd, "r")) != NULL) {
		fgets(buf, PATH_MAX, ptr);
		printf("exit func <= %p:%s", this_fn, buf);
	}

	(void) pclose(ptr);
}

int func4()
{
	printf("%s %d\n", __FUNCTION__, __LINE__);
	return (0);
}

int func3()
{
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	func4();
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	return (0);
}

int func2()
{
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	func3();
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	func4();
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	return (0);
}

int func1()
{
	int a;
	int b;
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	func2();
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	return (0);
}

int main(int argc, char *argv[])
{
	printf("%s %d\n", __FUNCTION__, __LINE__);	
	func1();
	printf("%s %d\n", __FUNCTION__, __LINE__);
	return (0);
}
