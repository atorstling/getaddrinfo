#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
//#define MAX( a, b ) ( ( a > b) ? a : b )

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




int printaddrinfo(char* host);
int printaddrinfo(char* host)
{
  printf("query: %s\n", host);
  /* Obtain address(es) matching host/port */
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* AF_INET or AF_INET6 */
  hints.ai_socktype = 0; /* SOCK_STREAM or SOCK_DGRAM */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_flags = AI_CANONNAME;

  struct addrinfo *result;
  int s = getaddrinfo(host, "80", &hints, &result);
  if (s != 0) {
		error(EXIT_FAILURE, s, "getaddrinfo: %s\n", gai_strerror(s));
  }

	//python:
	//getaddrinfo(...)
  //  getaddrinfo(host, port [, family, socktype, proto, flags])
  //      -> list of (family, socktype, proto, canonname, sockaddr)
  if(result!=NULL) {
    //Canon name is returned in first item in list
    printf("canonical name: %s\n", result->ai_canonname);
  }

	for(struct addrinfo *rp=result; rp!=NULL; rp=rp->ai_next) {
    /*
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV]; 
    int r = getnameinfo(rp->ai_addr, rp->ai_addrlen, hbuf, 
            sizeof(hbuf), sbuf, sizeof(sbuf), 
            NI_NUMERICHOST | NI_NUMERICSERV);
    if (r != 0) {
      error(EXIT_FAILURE, r, "getnameinfo: %s\n", gai_strerror(r));
    }
    */
    /*
    if (rp->ai_addr->sa_family == AF_INET) {
      struct sockaddr_in *inaddr_ptr = (struct sockaddr_in *)rp->ai_addr;
      asprintf(&ip, "%s", inet_ntoa(inaddr_ptr->sin_addr)); 
    } else {
      ip = rp->ai_addr->sa_data; 
    }*/
    char ip[NI_MAXHOST];
    int r = getnameinfo(rp->ai_addr, rp->ai_addrlen, ip, 
            sizeof(ip), 0, 0, NI_NUMERICHOST);
    if (r != 0) {
      error(EXIT_FAILURE, r, "getnameinfo: %s\n", gai_strerror(r));
    }
    //inet_ntop(rp->ai_addr->sa_family, rp->ai_addr, ip, rp->ai_addrlen);
		printf("%d, %d, %d, %d, %d, %s\n", 
				rp->ai_flags,
				rp->ai_family, 
				rp->ai_socktype,
				rp->ai_protocol,
				rp->ai_addr->sa_family,
				ip
        );
  /* if (ip != rp->ai_addr->sa_data) {
     free(ip);
    }
*/ 
	}
	freeaddrinfo(result);
	return 0;
}

int lookup(char* host);
int lookup(char* host)
{
  return printaddrinfo(host);
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
