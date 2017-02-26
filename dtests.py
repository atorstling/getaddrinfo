#!/usr/bin/env python
# Docker tests
import subprocess;
import os;
from subprocess import Popen;

def check(args, expected_code, expected_texts):
  cmd = "./vrun " + args
  p = Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE);
  code = p.wait();
  out, err = p.communicate();
  all = out + err
  if code != expected_code:
    raise Exception("command '%s' exited with status %s, expected %s. Output:\n%s\n" % ( cmd, code, expected_code, all))
  for e in expected_texts:
    if not e in all: 
      raise Exception("expected \n===\n%s\n===\nin command output: \n===\n%s\n===\n" % (e, all))
  return (cmd, out, err) 

# no arguments
#print check("target/getaddrinfo -h", 2, ["target/getaddrinfo: network address and service translation"])
print check("target/getaddrinfo bobo", 0, ["AF_INET\tSOCK_STREAM\tIPPROTO_TCP\t172.16.238.10", "AF_INET6\tSOCK_STREAM\tIPPROTO_TCP\t2001:3984:3989::10"]);
print check("target/getaddrinfo bobob", 0, ["AF_INET\tSOCK_STREAM\tIPPROTO_TCP\t172.16.239.10", "AF_INET6\tSOCK_STREAM\tIPPROTO_TCP\t2001:3984:3990::10"]);
# multi-level alias
print "OK"
