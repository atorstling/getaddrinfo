#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int EXIT_OTHER_ERROR = 2;

// global variable for argv[0]
extern char *program_name;

struct family {
  int value;
  char *name;
};

static struct family families[] = {
    {AF_UNSPEC, "AF_UNSPEC"}, {AF_INET, "AF_INET"}, {AF_INET6, "AF_INET6"},
};

void fill_family(int ai_family, char *buf, size_t buflen);
int lookup_family(char *family_s);
void error(int exit_code, int errnum, char *format, ...)
    __attribute__((noreturn));
void verbosep(int verbose, char *format, ...);
void *alloc(size_t size);
int toi(const char *str);
char *strdup2(const char *str);
