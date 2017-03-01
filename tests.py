#!/usr/bin/env python
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
print check("target/bin/getaddrinfo -h", 2, ["target/bin/getaddrinfo: network address and service translation"])
# non-existent
print check("target/bin/getaddrinfo localhost", 0, ["AF_INET\tSOCK_STREAM\tIPPROTO_TCP\t127.0.0.1"]);
print check("target/bin/gethostbyname localhost", 0, ["hostname: localhost", "address: 127.0.0.1"]);
print check("target/bin/gethostbyname2 localhost", 0, ["hostname: localhost", "address: 127.0.0.1"]);
# multi-level alias
print "OK"
