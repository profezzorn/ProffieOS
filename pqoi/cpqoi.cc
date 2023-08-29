#include "pqoi.h"

#include <string>
#include <map>
#include <utility>
#include <cassert>

#include "pamstream.h"
#include "pqoiml.h"


int main(int argc, char **argv) {
  Pqoiml pqoiml;
  std::string filename(argv[1]);
  std::string scaling_commands;
  if (argc > 2) {
    pqoiml.set_default_scaling_commands(argv[2]);
  }

  if (endswith(filename, ".pqoiml")) {
    pqoiml.parseFile(filename);
  } else {
    assert(pqoiml.parse("label start"));
    assert(pqoiml.parse("file "+filename));
    if (getfps(filename).first != 0) assert(pqoiml.parse("goto start"));
  }

  // pqoiml.dump(stderr);

  Generator generator;
  pqoiml.Generate(&generator);
  generator.Output(stdout);
}

