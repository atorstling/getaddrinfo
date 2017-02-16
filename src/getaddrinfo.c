#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <regex.h>
#include <assert.h>
#include <unistd.h>
#include <libgen.h>
#include <stdarg.h>

static int EXIT_OTHER_ERROR=2;
static char* program_name;
static int verbose_flag_set=0;

void error(int exit_code, int errnum, char* format, ...)__attribute__((noreturn));
void error(int exit_code, int errnum, char* format, ...) {
  fflush(stdout);
  fputs(program_name, stderr);
  fputs(": ", stderr);
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  if (errnum != 0) {
    fputs(": ", stderr);
    fputs(strerror(errnum), stderr);
  }
  fputs("\n", stderr);
  exit(exit_code);
}

void verbose(char* format, ...);
void verbose(char* format, ...) {
  if (verbose_flag_set) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
  }
}

void *alloc(size_t size);
void *alloc(size_t size) {
  void* m = malloc(size);
  if (!m) {
    error(EXIT_OTHER_ERROR, errno, "failed to allocate memory");
  }
  return m;
}

char* strdup2(const char* str);
char* strdup2(const char* str) {
  if (str == NULL) {
    return NULL;
  }
  char* duped = strdup(str);
  if (duped == NULL) {
    error(EXIT_OTHER_ERROR, errno, "Unable to duplicate string '%s'", str);
  } 
  return duped;
}

//static int BUF_SIZE=500;

int lookup(char* host);
int lookup(char* host)
{
  /* Obtain address(es) matching host/port */
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */

  struct addrinfo *result;
  int s = getaddrinfo(host, "80", &hints, &result);
  if (s != 0) {
		error(EXIT_FAILURE, s, "getaddrinfo: %s\n", gai_strerror(s));
  }
	return 0;
}

int usage(void);
int usage(void) {
  char* msg;
  asprintf(&msg, "Usage: %s [-v] command", program_name);
  error(EXIT_OTHER_ERROR, 0, msg);
}

int main(int argc, char** argv)
{
  program_name = argv[0];
  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
     switch (opt) {
     case 'v':
         verbose_flag_set = 1;
         break;
     default: 
         usage();
     }
  }
  if (optind >= argc) {
      usage();
  }
  char* host = argv[optind]; 
  return lookup(host);
}
