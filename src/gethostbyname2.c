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

#include "common.h"

static int verbose;

void printgethostbyname2(char *host, int family);
void printgethostbyname2(char *host, int family) {
  struct hostent *hosts = gethostbyname2(host, family);
  if (hosts == NULL) {
    error(EXIT_FAILURE, 0, "gethostbyname2(\"%s\") - %s\n", host,
          hstrerror(h_errno));
  }
  printf("hostname: %s\n", hosts->h_name);
  for (int i = 0;; ++i) {
    char *alias = hosts->h_aliases[i];
    if (alias == NULL) {
      break;
    }
    printf("alias: %s\n", alias);
  }
  char familystr[NI_MAXHOST];
  fill_family(hosts->h_addrtype, familystr, sizeof(familystr));
  // printf("addrtype: %s\n", familystr);
  for (int i = 0;; ++i) {
    char *addr = hosts->h_addr_list[i];
    if (addr == NULL) {
      break;
    }
    char straddr[NI_MAXHOST];
    if (inet_ntop(family, addr, straddr, sizeof(straddr)) == NULL) {
      error(EXIT_FAILURE, errno, "inet_ntop(%d, \"%s\")", family, addr);
    }
    // char* straddr2 = inet_ntop( *( struct in_addr*)( addr ));
    printf("address: %s\n", straddr);
  }
}

int usage(void);
int usage(void) {
  printf("%s: get ip address of dns name\n", program_name);
  puts("also see 'man gethostbyname2'");
  printf("Usage: %s [-v] [-f family] [host]\n", program_name);
  puts("-v: enable verbose mode");
  puts("-f family: lookup a specific address family.");
  puts("           Constant name or int value.");
  puts("example");
  puts("");
  printf("$%s -f AF_INET6 mail.google.com\n", program_name);
  puts("hostname: googlemail.l.google.com");
  puts("address: 2a00:1450:400f:807::2005");
  puts("");
  exit(EXIT_OTHER_ERROR);
}

static char *default_family = "AF_INET";

int main(int argc, char **argv) {
  program_name = argv[0];
  int opt;
  char *family_s = default_family;
  while ((opt = getopt(argc, argv, "vf:")) != -1) {
    switch (opt) {
    case 'v':
      verbose = 1;
      break;
    case 'f':
      family_s = strdup2(optarg);
      break;
    default:
      usage();
    }
  }
  if (optind >= argc) {
    usage();
  }
  char *host = argv[optind];
  printgethostbyname2(host, lookup_family(family_s));
  if (family_s != default_family) {
    free(family_s);
  }
  return 0;
}
