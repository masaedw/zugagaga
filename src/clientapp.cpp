// last modified <2004/03/02 22:46:24 2004 JST>
#include <iostream>
#include <string>
#include <stdexcept>
#include "clientapp.hpp"
#include "networkio.hpp"
#include "localio.hpp"
#include "event.hpp"

using namespace std;
using namespace zu3ga;
using namespace zu3ga::network;

namespace {
  struct EventInit
  {
    EventInit()
    {
      EventManager::Init();
    }

    ~EventInit()
    {
      EventManager::Quit();
    }
  };
}

ClientApp::ClientApp()
{
  SDL_WM_SetCaption("ZugagagA", "ZugagagA");
  SDLNet_Init();
}

ClientApp::~ClientApp()
{
  SDLNet_Quit();
}

void
ClientApp::ResetPointers()
{
  lobbyio_.reset();
  dmngr_.reset();
}

bool
ClientApp::Run()
{
  EventInit eventinit__;
  while ( Init() ) {
    try {
      if ( Lobby() ) {
	eio_ = lobbyio_->GetEventIO();
	dmngr_ = lobbyio_->GetDrawingManager();
	Mainloop();
      }
    }
    catch (const Zu3gaError& e) {
      errmsg_ = e.JaMsg();
    }
    catch (const exception& e) {
      errmsg_ = e.what();
    }
    catch (...) {
      errmsg_ = "よー分からんエラー";
    }
    lobbyio_.reset();
    eio_.reset();
    dmngr_.reset();
  }

  return true;
}

namespace {
  sp_Connection InitNetwork()
  {
    string host, port;
    cout << "host?(default:localhost): " << flush;
    getline( cin, host );
    if ( host.empty() ) {
      host = "localhost";
    }
    cout << "connection port?(default:9999): " << flush;
    getline( cin, port );
    if ( port.empty() ) {
      port = "9999";
    }
    return sp_Connection( new Connection( host, StrToInt(port) ) );
  }

  string AskName()
  {
    string temp;
    do {
      cout << "your name?: " << endl;
      getline( cin, temp );
    } while ( temp.empty() );
    return temp;
  }

  namespace InitSelect {
    enum t {
      Connect,
      NewGame,
      Exit,
    };
  }
}

bool
ClientApp::Init()
{
  //return InitText();
  ResetPointers();
  return InitGraphic();
}

bool
ClientApp::InitText()
{
  bool retvalue = true;

  string input;
  cout << "server(and client)? client only? exit? (S/n/e): " << flush;
  getline( cin, input );

  if ( !input.size() ) {
    input = "server";
  }

  switch ( input[0] ) {
  case 'e': case 'E':
    retvalue = false;
    break;
  case 'n': case 'N':
    {
      sp_Connection co = InitNetwork();
      string name = AskName();
      lobbyio_ = network::LobbyIO::Create( co, name );
    }
    break;
  default:
    {
      string port;
      cout << "listen port?(default:9999): " << flush;
      getline( cin, port );
      if ( port.empty() ) {
	port = "9999";
      }
      string name = AskName();
      lobbyio_ = local::LobbyIO::Create(StrToInt(port), name);
    }
    break;
  }

  cout << "init end" << endl;
  return retvalue;
}
