// last modified <2004/02/14 18:56:24 2004 JST>
#include <iostream>
#include <memory>
#include "clientapp.hpp"
#include "verbose.hpp"
#include "utility.hpp"

using namespace std;
using namespace zu3ga;

int main(int argc, char** argv)
{
  string imagesdir = "./images/";
  int verboselevel = 0;
  for ( int i = 1; i < argc; i++ ) {
    if ( argv[i][0] == '-' ) {
      switch (argv[i][1]) {
      case 'v':
	if ( ++i < argc )
	  verboselevel= StrToInt(argv[i]);
	break;
      case 'd':
	if ( ++i < argc )
	  imagesdir = argv[i];
	break;
      case '-':
	if ( argv[i] == string("--images-dir") ) {
	  if ( ++i  < argc ) {
	    imagesdir = argv[i];
	    if ( imagesdir[imagesdir.size()-1] != '/' ) {
	      imagesdir += '/';
	    }
	  }
	} else if ( argv[i] == string("--verbose") ) {
	  if ( ++i < argc )
	    verboselevel = StrToInt(argv[i]);
	}
	break;
      default:
	break;
      }
    }
  }

  auto_ptr<ClientApp> clientapp;
  graphic::SetImagesDir(imagesdir);
  verbose::setVerboseLevel(verboselevel);
  try {
    auto_ptr<ClientApp> hoge( new ClientApp );
    clientapp = hoge;
    clientapp->Run();
  }
  catch (exception& e) {
    cerr << e.what() << endl;
    return 1;
  }

  return 0;
}
