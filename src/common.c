#include "common.h"

char* program_name;

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

void verbosep(int verbose, char* format, ...) {
  if (verbose) {
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
  }
}

void *alloc(size_t size) {
  void* m = malloc(size);
  if (!m) {
    error(EXIT_OTHER_ERROR, errno, "failed to allocate memory");
  }
  return m;
}

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

