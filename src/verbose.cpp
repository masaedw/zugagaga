// last modified <2004/01/29 14:19:58 2004 JST>
#include "verbose.hpp"

namespace {
  unsigned int verboselevel;
}

unsigned int
zu3ga::verbose::getVerboseLevel()
{
  return verboselevel;
}

unsigned int
zu3ga::verbose::setVerboseLevel( unsigned int a )
{
  unsigned int temp = verboselevel;
  verboselevel = a;
  return temp;
}
