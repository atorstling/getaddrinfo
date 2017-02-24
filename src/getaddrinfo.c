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

int toi(const char* str);
int toi(const char* str)
{
  char* end;
  int res = (int) strtol(str, &end, 10);
  if (end == str)
  {
    error(EXIT_OTHER_ERROR, errno, "could not convert string '%s' to integer", str);
  }
  return res;
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

struct family {
  int value;
  char *name;
};

static struct family families[] = {
  {AF_UNSPEC, "AF_UNSPEC"},
  {AF_INET, "AF_INET"},
  {AF_INET6, "AF_INET6"},
};

void fill_family(int ai_family, char* buf, size_t buflen);
void fill_family(int ai_family, char* buf, size_t buflen)
{
  for(unsigned int i=0; i<(sizeof(families)/sizeof(families[0]));++i) {
    struct family f=families[i];
    if (f.value == ai_family) {
      strcpy(buf, f.name);
      return;
    }
  }
  snprintf(buf, buflen, "%d", ai_family);  
}

int lookup_family(char* family_s);
int lookup_family(char* family_s)
{
  if (family_s == NULL)
  {
    return 0;
  }
  for(unsigned int i=0; i<(sizeof(families)/sizeof(families[0]));++i) {
    struct family f=families[i];
    if (strcmp(f.name, family_s) == 0) {
      return f.value;
    }
  }
  return toi(family_s);
}

struct socktype {
  int value;
  char* name;
};

static struct socktype socktypes[] = {
  { SOCK_STREAM, "SOCK_STREAM" },
  { SOCK_DGRAM, "SOCK_DGRAM" },
  { SOCK_SEQPACKET, "SOCK_SEQPACKET" },
  { SOCK_RAW, "SOCK_RAW" },
  { SOCK_RDM, "SOCK_RDM" },
  { SOCK_PACKET, "SOCK_PACKET" },
};

void fill_socktype(int ai_socktype, char* buf, size_t buflen);
void fill_socktype(int ai_socktype, char* buf, size_t buflen)
{
  for(unsigned int i=0; i<(sizeof(socktypes)/sizeof(socktypes[0]));++i) {
    struct socktype s=socktypes[i];
    if (s.value == ai_socktype) {
      strcpy(buf, s.name);
      return;
    }
  }
  snprintf(buf, buflen, "%d", ai_socktype);  
}

int lookup_socktype(char* socktype_s);
int lookup_socktype(char* socktype_s)
{
  if (socktype_s == NULL)
  {
    return 0;
  }
  for(unsigned int i=0; i<(sizeof(socktypes)/sizeof(socktypes[0]));++i) {
    struct socktype s=socktypes[i];
    if (strcmp(s.name, socktype_s) == 0) {
      return s.value;
    }
  }
  return toi(socktype_s);
}


struct protocol {
  int value;
  char* name;
};

static struct protocol protocols[] = {
  { IPPROTO_IP, "IPPROTO_IP" },
  { IPPROTO_IPV6, "IPPROTO_IPV6" },
  { IPPROTO_ICMP, "IPPROTO_ICMP" },
  { IPPROTO_RAW, "IPPROTO_RAW" },
  { IPPROTO_TCP, "IPPROTO_TCP" },
  { IPPROTO_UDP, "IPPROTO_UDP" },
};

void fill_protocol(int ai_protocol, char* buf, size_t buflen);
void fill_protocol(int ai_protocol, char* buf, size_t buflen)
{
  for(unsigned int i=0; i<(sizeof(protocols)/sizeof(protocols[0]));++i) {
    struct protocol p=protocols[i];
    if (p.value == ai_protocol) {
      strcpy(buf, p.name);
      return;
    }
  }
  snprintf(buf, buflen, "%d", ai_protocol);  
}

int lookup_protocol(char* protocol_s);
int lookup_protocol(char* protocol_s)
{
  if (protocol_s == NULL)
  {
    return 0;
  }
  for(unsigned int i=0; i<(sizeof(protocols)/sizeof(protocols[0]));++i) {
    struct protocol p=protocols[i];
    if (strcmp(p.name, protocol_s) == 0) {
      return p.value;
    }
  }
  return toi(protocol_s);
}

struct flag {
  int value;
  char* name;
};

static struct flag flags[] = {
  { AI_CANONNAME, "AI_CANONNAME" },
  { AI_NUMERICSERV, "AI_NUMERICSERV" },
  { AI_NUMERICHOST, "AI_NUMERICHOST" },
  { AI_PASSIVE, "AI_PASSIVE" },
  { AI_ADDRCONFIG, "AI_ADDRCONFIG" },
  { AI_V4MAPPED, "AI_V4MAPPED" },
  { AI_IDN, "AI_IDN" },
  { AI_CANONIDN, "AI_CANONIDN" },
  { AI_IDN_ALLOW_UNASSIGNED, "AI_IDN_ALLOW_UNASSIGNED" },
  { AI_IDN_USE_STD3_ASCII_RULES, "AI_IDN_USE_STD3_ASCII_RULES" },
};

int lookup_flag(char* flag_s);
int lookup_flag(char* flag_s)
{
  for(unsigned int i=0; i<(sizeof(flags)/sizeof(flags[0]));++i) {
    struct flag f=flags[i];
    if (strcmp(f.name, flag_s) == 0) {
      return f.value;
    }
  }
  return toi(flag_s);
}

int lookup_flags(char** flags_s);
int lookup_flags(char** flags_s)
{
  if (flags_s == NULL)
  {
    return 0;
  }
  int result=0;
  for(unsigned int i=0;; ++i)
  {
    char* current = flags_s[i];
    if (current == NULL) {
      break;
    }
    result |= lookup_flag(current);
  }
  return result;
}

void printaddrinfo(char* host, char* service, char* family_s, char* socktype_s, char* protocol_s, char** flags_s);
void printaddrinfo(char* host, char* service, char* family_s, char* socktype_s, char* protocol_s, char** flags_s)
{
  /* Obtain address(es) matching host/port */
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  // AF_UNSPEC, AF_INET AF_INET6 etc
  hints.ai_family = lookup_family(family_s); 
  // SOCK_STREAM etc
  hints.ai_socktype = lookup_socktype(socktype_s); 
  // IPPROTO_ICMP etc 
  hints.ai_protocol = lookup_protocol(protocol_s);
  // AI_CANONNAME etc
	hints.ai_flags = lookup_flags(flags_s);

  verbose("host: %s\nservice: %s\nai_family: %d\nai_socktype: %d\n"
         "ai_protocol: %d\nai_flags: %d\n",
         host, service, hints.ai_family, hints.ai_socktype, 
         hints.ai_protocol, hints.ai_flags);

  struct addrinfo *result;
  int s = getaddrinfo(host, service, &hints, &result);
  if (s != 0) {
		error(EXIT_FAILURE, s, "getaddrinfo: %s\n", gai_strerror(s));
  }

  verbose("#family\tsocktype\tprotocol\tip\tcanonname\n");

	for(struct addrinfo *rp=result; rp!=NULL; rp=rp->ai_next) {
    char ip[NI_MAXHOST];
    int r = getnameinfo(rp->ai_addr, rp->ai_addrlen, ip, 
            sizeof(ip), 0, 0, NI_NUMERICHOST);
    if (r != 0) {
      error(EXIT_FAILURE, r, "getnameinfo: %s\n", gai_strerror(r));
    }
    char *canonname="";
    if(rp->ai_canonname)
    {
      canonname=rp->ai_canonname;
    }
    char family[NI_MAXHOST];
    fill_family(rp->ai_family, family, sizeof(family));
    char socktype[NI_MAXHOST];
    fill_socktype(rp->ai_socktype, socktype, sizeof(socktype));
    char protocol[NI_MAXHOST];
    fill_protocol(rp->ai_protocol, protocol, sizeof(protocol));
    //inet_ntop(rp->ai_addr->sa_family, rp->ai_addr, ip, rp->ai_addrlen);
		printf("%s\t%s\t%s\t%s\t%s\n", 
				family, 
				socktype,
				protocol,
				ip,
        canonname
        );
	}
	freeaddrinfo(result);
}

void printgethostbyname(char *host);
void printgethostbyname(char *host)
{
  struct hostent* hosts = gethostbyname(host);
  if(hosts == NULL) {
		error(EXIT_FAILURE, 0, "gethostbyname(\"%s\"): %s", host, hstrerror(h_errno));
  }
  printf("hostname: %s\n", hosts->h_name);
  printf("aliases:\n");
  for(int i=0;;++i) {
    char* alias = hosts->h_aliases[i];
    if (alias==NULL) {
      break;
    }
    printf("%s\n", alias);
  }
  char familystr[NI_MAXHOST];
  fill_family(hosts->h_addrtype, familystr, sizeof(familystr));
  //printf("addrtype: %s\n", familystr);
  printf("addresses:\n");
  for(int i=0;;++i) {
    char* addr = hosts->h_addr_list[i];
    if (addr==NULL) {
      break;
    }
    char straddr[NI_MAXHOST];
    inet_ntop(hosts->h_addrtype, addr, straddr, sizeof(straddr));
    //char* straddr2 = inet_ntop( *( struct in_addr*)( addr )); 
    printf("%s\n", straddr);
  }
}

void printgethostbyname2(char *host, int family);
void printgethostbyname2(char *host, int family)
{
  struct hostent* hosts = gethostbyname2(host, family);
  if(hosts == NULL) {
		error(EXIT_FAILURE, h_errno, "gethostbyname2(\"%s\"): %s\n", host, hstrerror(h_errno));
  }
  printf("hostname: %s\n", hosts->h_name);
  printf("aliases:\n");
  for(int i=0;;++i) {
    char* alias = hosts->h_aliases[i];
    if (alias==NULL) {
      break;
    }
    printf("%s\n", alias);
  }
  char familystr[NI_MAXHOST];
  fill_family(hosts->h_addrtype, familystr, sizeof(familystr));
  //printf("addrtype: %s\n", familystr);
  printf("addresses:\n");
  for(int i=0;;++i) {
    char* addr = hosts->h_addr_list[i];
    if (addr==NULL) {
      break;
    }
    char straddr[NI_MAXHOST];
    inet_ntop(family, addr, straddr, sizeof(straddr));
    //char* straddr2 = inet_ntop( *( struct in_addr*)( addr )); 
    printf("%s\n", straddr);
  }
}

int lookup(char* host);
int lookup(char* host)
{
//  printf("query: %s\n", host);
//  printf("- gethostbyname (only AF_INET) -\n");
//  printgethostbyname(host);
//  printf("- gethostbyname2 AF_INET -\n");
//  printgethostbyname2(host, AF_INET);
//  printf("- gethostbyname2 AF_INET6 -\n");
//  printgethostbyname2(host, AF_INET6);
//  printf("- getaddrinfo -\n");
// void printaddrinfo(char* host, char* service, char* family_s, char* socktype_s, char* protocol_s, char** flags_s);
  printaddrinfo(host, "0", "AF_INET", "SOCK_STREAM", "0", NULL);
  return 0;
}

int usage(void);
int usage(void) {
  printf("%s: network address and service translation\n", program_name);
  puts("also see 'man addrinfo'");
  printf("Usage: %s [-v] [-e service] [-f family] [-s socktype] [-p protocol]"
          " [-l flag] [host]\n", program_name);
  puts("-v: enable verbose mode");
  puts("-e service: lookup for specific service.");
  puts("            Service name or int value.");
  puts("            examples: 80 (port 80) or https (port 443)");
  puts("-f family: lookup a specific address family.");
  puts("           Constant name or int value.");
  puts("           examples: AF_INET (IPv4) or AF_INET6 (IPv6), 0");
  puts("-s socktype: lookup for specific socket type."); 
  puts("             Constant name or int value.");
  puts("             examples: SOCK_STREAM, SOCK_DGRAM, 2");
  puts("-p protocol: lookup for specific protocol.");
  puts("             Constant name or int value.");
  puts("             examples: IPPROTO_IP, IPPROTO_ICMP, 0");
  puts("-l flag: set flags. See 'man getaddrinfo' for details");
  puts("             Constant name or int value.");
  puts("             examples: AI_CANONNAME, AI_PASSIVE, 4");
  puts("");
  puts("examples");
  puts("");
  puts("To find out which address to listen to (AI_PASSIVE) ");
  puts("ssh traffic (port 22) on IPv4 (AF_INET) TCP (SOCK_STREAM) do the following:");
  puts("");
  printf("$%s -e 22 -f AF_INET -s SOCK_STREAM -l AI_PASSIVE\n", program_name);
  puts("AF_INET SOCK_STREAM IPPROTO_TCP 0.0.0.0");
  puts("");
  puts("A gethostbyname equivalent:");
  puts("");
  printf("$%s -f AF_INET -s SOCK_STREAM -l AI_CANONNAME mail.google.com\n", program_name);
  puts("AF_INET SOCK_STREAM IPPROTO_TCP 172.217.22.165  googlemail.l.google.com");
  puts("");
  exit(EXIT_OTHER_ERROR);
}

#define MAXFLAGS 16

int main(int argc, char** argv)
{
  program_name = argv[0];
  int opt;
  char *service_s=NULL;
  char *family_s=NULL;
  char *socktype_s=NULL;
  char *protocol_s=NULL;
  char *flags_s[MAXFLAGS];
  int flag_counter=0;
  while ((opt = getopt(argc, argv, "vhe:f:s:p:l:")) != -1) {
     switch (opt) {
     case 'v':
         verbose_flag_set = 1;
         break;
     case 'h':
         usage();
         break;
     case 'e':
         service_s=strdup2(optarg);
         break;
     case 'f':
         family_s=strdup2(optarg);
         break;
     case 's':
         socktype_s=strdup2(optarg);
         break;
     case 'p':
         protocol_s=strdup2(optarg);
         break;
     case 'l':
         flags_s[flag_counter++]=strdup2(optarg);
         break;
     default: 
         usage();
     }
  }
  flags_s[flag_counter]=NULL;
  char* host=NULL;
  if (optind < argc) {
      host = argv[optind]; 
  }
  printaddrinfo(host, service_s, family_s, socktype_s, protocol_s, flags_s);
  free(service_s);
  free(family_s);
  free(socktype_s);
  free(protocol_s);
  for(int i=0;;++i)
  {
    char* f = flags_s[i];
    if(f == NULL)
    {
      break;
    }
    free(f);
  }
  return 0;
}
