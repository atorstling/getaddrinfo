#include "common.h"
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int verbose;

void printgethostbyname(char *host);
void printgethostbyname(char *host) {
  struct hostent *hosts = gethostbyname(host);
  if (hosts == NULL) {
    error(EXIT_FAILURE, 0, "gethostbyname(\"%s\") - %s", host,
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
  for (int i = 0;; ++i) {
    char *addr = hosts->h_addr_list[i];
    if (addr == NULL) {
      break;
    }
    char straddr[NI_MAXHOST];
    inet_ntop(hosts->h_addrtype, addr, straddr, sizeof(straddr));
    printf("address: %s\n", straddr);
  }
}

int usage(void);
int usage(void) {
  printf("%s: get ip address of dns name\n", program_name);
  puts("also see 'man gethostbyname'");
  printf("Usage: %s [-v] [host]\n", program_name);
  puts("-v: enable verbose mode");
  puts("example");
  puts("");
  printf("$%s mail.google.com\n", program_name);
  puts("hostname: googlemail.l.google.com");
  puts("address: 216.58.201.165");
  puts("");
  exit(EXIT_OTHER_ERROR);
}

int main(int argc, char **argv) {
  program_name = argv[0];
  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
    case 'v':
      verbose = 1;
      break;
    default:
      usage();
    }
  }
  if (optind >= argc) {
    usage();
  }
  char *host = argv[optind];
  printgethostbyname(host);
  return 0;
}
