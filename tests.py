#!/usr/bin/env python
import subprocess;
import os;
from subprocess import Popen;

def check(args, expected_code, expected_texts):
  cmd = "./vrun.sh " + args
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
print check("", 2, ["target/getaddrinfo: Usage: target/getaddrinfo [-v] command"])
# non-existent
print check("miss", 1, ["no match"]);
# multi-level alias
print check("ll", 0, ["'ll' is an alias for 'ls' in shell '/bin/bash': 'ls -alF'",
                      "'ls' is an alias for 'ls' in shell '/bin/bash': 'ls --color=auto'",
                      "'ls' found in PATH as '/bin/ls'",
                      "'/bin/ls' is an executable"]);
print "OK"
