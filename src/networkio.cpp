// last modified <2004/02/08 04:29:47 2004 JST>
// クライアント側処理
// 構文解析 うげげ tokenizer 作れよ localio.cpp も！
#include <stdexcept>
#include "objects.hpp"
#include "networkio.hpp"
#include "verbose.hpp"

using namespace std;
using namespace edw;
using namespace zu3ga;
using namespace zu3ga::network;

namespace {
  class EventIO : public EventIOBase
  {
  public:
    void Update();
    SendDataPair GetData() { return pair_; }
    void PushEvent(unsigned char event);
    GameStatus::t GetStatus() { return stat_; }

    static sp_EventIO Create( const sp_Connection& );
    ~EventIO();
  private:
    EventIO( const sp_Connection& );

    sp_Connection connection_;
    lobby::Info info_;
    GameStatus::t stat_;
    SendDataPair pair_;
  };
}

sp_EventIO
EventIO::Create( const sp_Connection& connection )
{
  return sp_EventIO( new EventIO(connection) );
}

EventIO::EventIO( const sp_Connection& connection )
  : connection_(connection)
  , stat_(GameStatus::Ready)
{}

EventIO::~EventIO()
{}

void
EventIO::PushEvent( unsigned char event )
{
  if ( stat_ != GameStatus::Game && stat_ != GameStatus::Ready )
    return;
  connection_->Write(EveToString(event)+"\n");
}

void
EventIO::Update()
{
  string temp = connection_->Read();
  if ( temp.empty() ) {
    return;
  }
  string::size_type endp = temp.find("\n");
  if ( endp == temp.npos ) {
    return;
  }

  switch (temp[0]) {
  case 'd':
    if ( temp.find("data") == 0 ) {
      try {
	string::size_type size = StrToInt(temp.substr(5,endp-5));
	size +=endp+1;
	if ( temp.size() >= size ) {
	  INetworkData data(temp.substr(endp+1));
	  data >> pair_.first;
	  data >> pair_.second;
	  temp.erase(0,size);
	}
      } catch (exception& e) {
	throw SyntaxError(temp);
      }
    } else
      throw SyntaxError(temp);
    break;
  case 'g':
    if ( temp.find("good job!\n") == 0 ) {
      connection_->Write("bye\n");
      stat_ = GameStatus::Final;
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;
  case 'r':
    if ( temp.find("result\n") == 0 ) {
      stat_ = GameStatus::Result;
    } else if ( temp.find("ready\n") == 0 ) {
      stat_ = GameStatus::Ready;
    } else
      throw SyntaxError(temp);
    temp.erase(0,endp+1);
    break;
  case 's':
    if ( temp.find("start\n") == 0 ) {
      stat_ = GameStatus::Game;
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;
  default:
    throw SyntaxError(temp);
    break;
  }

  if ( !temp.empty() ) {
    connection_->PutBack(temp);
  }
}

//======================================================================
// ロビー クライアント版
//======================================================================
LobbyIO::LobbyIO( const sp_Connection& connection, const string& name)
  : connection_( connection )
  , stat_( lobby::Stat::Continue )
  , name_( name )
  , info_ready_( false )
{
  connection_->Write( string("'")+name_+"'\n");
}

LobbyIO::~LobbyIO()
{
}

sp_LobbyIO
LobbyIO::Create( const sp_Connection& c, const string& n )
{
  return sp_LobbyIO( new LobbyIO(c,n) );
}

namespace {
  struct NameSize
  {
    string name;
    string::size_type size;
  };

  NameSize
  GetNameSize( const string& str, string::size_type pos = 0 )
  {
    NameSize temp;
    string::size_type begin = FindQuote(str,pos);
    string::size_type end = FindQuote(str,begin+1);
    if ( begin == str.npos || end == str.npos ) {
      throw SyntaxError(str);
    }
    temp.size = end-begin-1;
    temp.name = Unescape(str.substr(begin+1, temp.size));
    return temp;
  }

  lobby::PlayerData GetInitData( const string& msg, string::size_type pos )
  {
    lobby::PlayerData temp;
    temp.name = GetName(msg,pos);
    pos = msg.find("type",pos);
    try {
      temp.type = PlayerType::t(StrToInt(msg.substr(pos+5,1)));
      temp.team = Team::t(StrToInt(msg.substr(pos+12,1)));
    }
    catch (exception& e) {
      cerr << e.what() << endl;
      throw SyntaxError(msg);
    }
    return temp;
  }

  struct MsgData
  {
    string name,msg;
    MsgData( const string& name, const string& msg )
      : name(name), msg(msg) {}
  };

  MsgData GetMsgData(const string& str, string::size_type pos = 0 )
  {
    string::size_type nbegin	= FindQuote(str,pos);
    string::size_type nend	= FindQuote(str,nbegin+1);
    string::size_type mbegin	= FindQuote(str,nend+1);
    string::size_type mend	= FindQuote(str,mbegin+1);
    if ( nbegin == str.npos || nend == str.npos ||
	 mbegin == str.npos || nend == str.npos ) {
      throw SyntaxError(str);
    }
    return MsgData( str.substr(nbegin+1, nend-nbegin-1), str.substr(mbegin+1, mend-mbegin-1) );
  }

  lobby::PlayerData* Find(lobby::Info& info, const string& name)
  {
    for ( int i = 0; i < 4; i++ ) {
      if ( info.initdata[i].team == Team::None )
	break;
      if ( info.initdata[i].name == name )
	return &info.initdata[i];
    }
    return 0;
  }

  void Tamp(lobby::Info& info)
  {
    lobby::PlayerData temp[4];
    int pos = 0, self = 0;

    for ( int i = 0; i < 4; i++ ) {
      if (info.initdata[i].team != Team::None) {
	if ( i == info.self )
	  self = pos;
	temp[pos++] = info.initdata[i];
      }
    }
    info.self = self;
    for ( int i = 0; i < 4; i++ ) {
      info.initdata[i] = temp[i];
    }
  }

}

void
LobbyIO::Update() // まったくオブジェクト指向的でない！
{
  if ( !connection_->IsOpened() ) {
    stat_ = lobby::Stat::Error;
    error_ = "ネットワークが切断しました";
    return;
  }
    
  string temp = connection_->Read();
  if ( temp.empty() )
    return;

  string::size_type endp = temp.find("\n");
  if ( endp == temp.npos ) {
    connection_->PutBack(temp);
    return;
  }

  //cout << "<-- " << temp << endl;
  switch (temp[0]) {
  case 'b':
    if ( temp.find("bye") == 0 ) {
      if ( lobby::PlayerData* data = Find(info_,GetName(temp, 3)) ) {
	data->team = Team::None;
	Tamp(info_);
      } else
	throw SyntaxError(temp);
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 'c':
    if ( temp.find("cancel") == 0 ) {
      if ( lobby::PlayerData* data = Find(info_,GetName(temp,6)) ) {
	data->ok = false;
      } else
	throw SyntaxError(temp);
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 'e':
    if ( temp.find("exit") == 0 ) {
      stat_ = lobby::Stat::Quit;
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 'g':
    if ( temp.find("good luck!") == 0 ) {
      stat_ = lobby::Stat::Start;
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 'i':
    if ( temp.find("info") == 0 ) {
      string::size_type epos = temp.find("end\n");
      if ( epos == temp.npos )
	break;
      string::size_type pos = 4;
      for ( int i = 0; i < 4; i++ ) {
	pos = temp.find("name",pos);
	if ( pos == temp.npos )
	  break;
	info_.initdata[i] = GetInitData(temp,pos);
	pos++;
	info_.self = i;
      }
      info_ready_ = true;
      temp.erase(0,epos+4);
    } else if ( temp.find("invalidname") == 0 ) {
      verbose::puts(2,"invalidname");
      temp.erase(0,endp+1);
      stat_ = lobby::Stat::Error;
      error_ = "名前が不正です";
    } else
      throw SyntaxError(temp);
    break;

  case 'm':
    if ( temp.find("msg") == 0 ) {
      MsgData d = GetMsgData(temp,4);
      msgcache_ += d.name + ": " + d.msg + "\n";
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 'o':
    if ( temp.find("ok") == 0 ) {
      if ( lobby::PlayerData* data = Find(info_,GetName(temp,3)) ) {
	data->ok = true;
      } else
	throw SyntaxError(temp);
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 's':
    if ( temp.find("stage") == 0 ) {
      try {
	info_.stage = StageType::t(StrToInt(temp.substr(6,1)));
      } catch (exception& e) {
	cerr << e.what() << endl;
	throw SyntaxError(temp);
      }
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  case 't':
    if ( temp.find("team") == 0 ) {
      NameSize ns = GetNameSize(temp,5);
      if ( lobby::PlayerData* data = Find(info_,ns.name) ) {
	try {
	  data->team = Team::t(StrToInt(temp.substr(6+ns.size+2,1)));
	} catch (exception& e) {
	  cerr << e.what() << endl;
	  throw SyntaxError(temp);
	}
      } else
	throw SyntaxError(temp);
    } else if ( temp.find("type") == 0 ) {
      NameSize ns = GetNameSize(temp,5);
      if ( lobby::PlayerData* data = Find(info_,ns.name) ) {
	try {
	  data->type = PlayerType::t(StrToInt(temp.substr(6+ns.size+2,1)));
	} catch (exception& e) {
	  cerr << e.what() << endl;
	  throw SyntaxError(temp);
	}
      } else
	throw SyntaxError(temp);
    } else
      throw SyntaxError(temp);
    temp.erase(0,endp+1);
    break;

  case 'u':
    if ( temp.find("user") == 0 ) {
      int i = info_.self+1;
      for (; i < 4; i++ ) {
	if ( info_.initdata[i].team == Team::None )
	  break;
      }
      info_.initdata[i] = GetInitData(temp,5);
      temp.erase(0,endp+1);
    } else
      throw SyntaxError(temp);
    break;

  default:
    break;
  }
  connection_->PutBack(temp);
}

const lobby::Info*
LobbyIO::GetData()
{
  if ( info_ready_ )
    return &info_;
  else
    return 0;
}

sp_EventIO
LobbyIO::GetEventIO()
{
  return EventIO::Create( connection_ );
}

sp_DrawingManager
LobbyIO::GetDrawingManager()
{
  return sp_DrawingManager(new DrawingManager(info_.GetDMInitData()));
}

void
LobbyIO::NextChara()
{
  connection_->Write("nexttype\n");
}

void
LobbyIO::PrevChara()
{
  connection_->Write("prevtype\n");
}

void
LobbyIO::NextStage()
{
  throw logic_error("Can't change stage over network.");
}

void
LobbyIO::PrevStage()
{
  throw logic_error("Can't change stage over network.");
}

void
LobbyIO::NextTeam()
{
  connection_->Write("nextteam\n");
}

void
LobbyIO::PrevTeam()
{
  connection_->Write("prevteam\n");
}

void
LobbyIO::Ready()
{
  connection_->Write("ok\n");
}

void
LobbyIO::Cancel()
{
  connection_->Write("cancel\n");
}

bool
LobbyIO::IsHost()
{
  return false;
}

void
LobbyIO::Bye()
{
  connection_->Write("bye\n");
  stat_ = lobby::Stat::Quit;
}

void
LobbyIO::Start()
{
  throw logic_error("Can't start game over network.");
}

lobby::Stat::t
LobbyIO::GetStatus()
{
  return stat_;
}

void
LobbyIO::SendMsg( const std::string& amsg )
{
  string temp, msg = amsg;
  temp = "msg '";
  for (string::size_type i = 0; i < msg.size(); i++) {
    if ( msg[i] == '\'' ) {
      msg.insert(i, "\\");
      i++;
    }
  }
  connection_->Write(temp+msg+"'\n");
}

std::string
LobbyIO::GetMsg()
{
  string temp;
  temp.swap(msgcache_);
  return temp;
}
