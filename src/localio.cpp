// last modified <2004/08/19 03:13:17 2004 JST>
// サーバー側処理
// ロビー提供/イベント管理提供
// 構文解析 networkio.cpp も
#include <vector>
#include <list>
#include "localio.hpp"
#include "localeio.hpp"

using namespace std;
using namespace zu3ga;
using namespace zu3ga::local;
using namespace zu3ga::network;

//======================================================================
//======================================================================
namespace {
  TCPsocket SockInit(int port)
  {
    IPaddress ip;
    ip.host = INADDR_ANY;
    ip.port = htons(port);
    return SDLNet_TCP_Open(&ip);
  }
}

LobbyIO::LobbyIO( int port, const string& name )
  : port_( port )
  , sock_( SockInit(port_) )
  , stat_( lobby::Stat::Continue )
  , bye_( false )
{
  info_.initdata[0].name = name;
  info_.initdata[0].team = Team::I;
  info_.self = 0;
  info_.stage = StageType::Forest;
  if (sock_ == 0) {
    throw Error("サーバーを開始できません");
  }
}

LobbyIO::~LobbyIO()
{}

namespace {
  bool Requested(TCPsocket sock) {
    SDLNet_SocketSet ss = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(ss,sock);
    bool ret = SDLNet_CheckSockets(ss,1) == 1;
    SDLNet_FreeSocketSet(ss);
    return ret;
  }
}

void
LobbyIO::Update()
{
  cpool_.Update();
  if ( sock_ ) {
    if ( Requested(sock_) ) {
      for ( int i = 0; i < 3; i++ ) {
	if ( !con_[i] ) {
	  con_[i] = sp_Connection(new Connection(SDLNet_TCP_Accept(sock_)));
	  if ( i == 2 ) {
	    SDLNet_TCP_Close(sock_);
            sock_ = 0;
          }
	  break;
	}
      }
    }
  }

  // 受信フェーズ
  for ( int i = 0; i < 3 && con_[i]; i++ ) {
    if ( !con_[i]->IsOpened() ) {
      if ( info_.initdata[i-1].team != Team::None )
	Bye(i+1);
      continue;
    }
    string msg = con_[i]->Read();
    if ( msg.empty() )
      continue;

    string::size_type endp = msg.find("\n");
    if ( endp == msg.npos )
      continue;

    bool add = true;

    switch (msg[0]) {
    case '\'':
      if ( msg[endp-1] == '\'' ) {
	add = AddUser(i+1, Unescape(msg.substr(1,endp-2)));
      }
      break;

    case 'b':
      if ( msg.find("bye\n") == 0 ) {
	Bye(i+1);
      }
      break;

    case 'c':// cancel
      if ( msg.find("cancel\n") == 0 ) {
	Cancel(i+1);
      }
      break;

    case 'm':// msg 'MSG'
      if ( msg.find("msg") == 0 ) {
	SendMsg(i+1,GetString(msg,3));
      }
      break;

    case 'n':// nextteam, nexttype
      if ( msg.find("nextteam\n") == 0 ) {
	NextTeam(i+1);
      } else if ( msg.find("nexttype\n") == 0 ) {
	NextChara(i+1);
      }
      break;

    case 'o':// ok
      if ( msg.find("ok\n") == 0 ) {
	Ready(i+1);
      }
      break;

    case 'p':// prevteam, prevtype
      if ( msg.find("prevteam\n") == 0 ) {
	PrevTeam(i+1);
      } else if ( msg.find("prevtype\n") == 0 ) {
	PrevChara(i+1);
      }
      break;

    default:
      break;
    }

    msg.erase(0,endp+1);
    if ( !msg.empty() && add )
      con_[i]->PutBack(msg);
  } // for

  //送信フェーズ
  Flush();

  if ( bye_ )
    stat_ = lobby::Stat::Quit;
}

sp_LobbyIO
LobbyIO::Create( int port, const string& name )
{
  return sp_LobbyIO( new LobbyIO(port,name) );
}

sp_EventIO
LobbyIO::GetEventIO()
{
  vector<sp_Connection> descs;
  descs.push_back(sp_Connection());
  for (int i = 0; i < 3; i++ ) {
    if ( info_.initdata[i+1].team == Team::None )
      break;
    descs.push_back(con_[i]);
  }
  Flush();
  return EventIO::Create( info_, descs );
}

sp_DrawingManager
LobbyIO::GetDrawingManager()
{
  return sp_DrawingManager(new DrawingManager(info_.GetDMInitData()));
}

const lobby::Info*
LobbyIO::GetData()
{
  return &info_;
}

void
LobbyIO::NextChara()
{
  NextChara(0);
}

void
LobbyIO::PrevChara()
{
  PrevChara(0);
}

void
LobbyIO::NextTeam()
{
  NextTeam(0);
}

void
LobbyIO::PrevTeam()
{
  PrevTeam(0);
}

void
LobbyIO::Ready()
{
  Ready(0);
}

void
LobbyIO::Cancel()
{
  Cancel(0);
}

void
LobbyIO::Bye()
{
  Bye(0);
  sendbuf_ += "exit\n";
  bye_ = true;
}

void
LobbyIO::NextStage()
{
  if ( info_.stage == StageType::Forest ) {
    info_.stage = StageType::Future;
    sendbuf_ += "stage 1\n";
  } else {
    info_.stage = StageType::Forest;
    sendbuf_ += "stage 0\n";
  }
}

void
LobbyIO::PrevStage()
{
  if ( info_.stage == StageType::Forest ) {
    info_.stage = StageType::Future;
    sendbuf_ += "stage 1\n";
  } else {
    info_.stage = StageType::Forest;
    sendbuf_ += "stage 0\n";
  }
}

void
LobbyIO::SendMsg( const std::string& msg )
{
  sendbuf_ += string("msg '") + Escape(info_.initdata[0].name) + "' '" + Escape(msg) + "'\n";
  msgbuf_ += info_.initdata[0].name + ": " + msg + "\n";
}

std::string
LobbyIO::GetMsg()
{
  string temp;
  temp.swap(msgbuf_);
  return temp;
}

void
LobbyIO::NextChara(int i)
{
  PlayerType::t type(PlayerType::Planex);
  string num;
  switch (info_.initdata[i].type) {
  case PlayerType::Planex:
    type = PlayerType::Yamaha;
    num = "' 1\n";
    break;
  case PlayerType::Yamaha:
    type = PlayerType::NTT;
    num = "' 2\n";
    break;
  case PlayerType::NTT:
    type = PlayerType::Planex;
    num = "' 0\n";
    break;
  }
  info_.initdata[i].type = type;
  sendbuf_ += string("type '") + Escape(info_.initdata[i].name) + num;
}

void
LobbyIO::PrevChara(int i)
{
  PlayerType::t type(PlayerType::Planex);
  string num;
  switch (info_.initdata[i].type) {
  case PlayerType::Planex:
    type = PlayerType::NTT;
    num = "' 2\n";
    break;
  case PlayerType::Yamaha:
    type = PlayerType::Planex;
    num = "' 0\n";
    break;
  case PlayerType::NTT:
    type = PlayerType::Yamaha;
    num = "' 1\n";
    break;
  }
  info_.initdata[i].type = type;
  sendbuf_ += string("type '") + Escape(info_.initdata[i].name) + num;
}

void
LobbyIO::NextTeam(int i)
{
  Team::t team(Team::I);
  string num;
  switch ( info_.initdata[i].team ) {
  case Team::I:
    team = Team::Ro;
    num = "' 1\n";
    break;
  case Team::Ro:
    team = Team::Ha;
    num = "' 2\n";
    break;
  case Team::Ha:
    team = Team::Ni;
    num = "' 3\n";
    break;
  case Team::Ni:
    team = Team::I;
    num = "' 0\n";
    break;
  default:
    throw runtime_error("NextTeam: invalid team.");
    break;
  }
  info_.initdata[i].team = team;
  sendbuf_ += string("team '") + Escape(info_.initdata[i].name) + num;
}

void
LobbyIO::PrevTeam(int i)
{
  Team::t team = Team::t(0);
  string num;
  switch ( info_.initdata[i].team ) {
  case Team::I:
    team = Team::Ni;
    num = "' 3\n";
    break;
  case Team::Ro:
    team = Team::I;
    num = "' 0\n";
    break;
  case Team::Ha:
    team = Team::Ro;
    num = "' 1\n";
    break;
  case Team::Ni:
    team = Team::Ha;
    num = "' 2\n";
    break;
  default:
    break;
  }
  info_.initdata[i].team = team;
  sendbuf_ += string("team '") + Escape(info_.initdata[i].name) + num;
}

void
LobbyIO::Ready(int i)
{
  info_.initdata[i].ok = true;
  sendbuf_ += string("ok '") + Escape(info_.initdata[i].name) + "'\n";
}

void
LobbyIO::Cancel(int i)
{
  info_.initdata[i].ok = false;
  sendbuf_ += string("cancel '") + Escape(info_.initdata[i].name) + "'\n";
}

bool
LobbyIO::IsHost()
{
  return true;
}

void
LobbyIO::Bye(int i)
{
  info_.initdata[i].team = Team::None;
  sendbuf_ += string("bye '") + Escape(info_.initdata[i].name) + "'\n";
}

void
LobbyIO::Start()
{
  sendbuf_ += "good luck!\n";
  stat_ = lobby::Stat::Start;
}

lobby::Stat::t
LobbyIO::GetStatus()
{
  return stat_;
}

void
LobbyIO::SendMsg(int i, const string& msg)
{
  sendbuf_ += string("msg '") + Escape(info_.initdata[i].name) + "' '" + Escape(msg) + "'\n";
  msgbuf_ += info_.initdata[i].name + ": " + msg + "\n";
}

bool
LobbyIO::AddUser( int i, const string& name )
{
  // 同名プレイヤーがいたら拒否
  for ( unsigned int c = 0; c < 4; c++ ) {
    if ( info_.initdata[c].team == Team::None )
      break;
    if ( info_.initdata[c].name != name )
      continue;
    con_[i-1]->Write("invalidname\n");
    cpool_.Add(con_[i-1]);
    con_[i-1].reset();
    return false;
  }
  info_.initdata[i].name = name;
  info_.initdata[i].type = PlayerType::Planex;
  info_.initdata[i].team = Team::I;
  info_.initdata[i].ok = false;
  string usermsg("user '");
  usermsg += Escape(name) + "' type 0 team 0\n";
  for ( int j = 0; j < i; j++ ) {
    con_[j]->Write(usermsg);
  }

  string initmsg("info\n");
  for ( int j = 0; j <= i; j++ ) {
    initmsg += "name '";
    initmsg += Escape(info_.initdata[j].name);
    initmsg += "' type ";
    switch (info_.initdata[j].type) {
    case PlayerType::Planex:
      initmsg += "0";
      break;
    case PlayerType::Yamaha:
      initmsg += "1";
      break;
    case PlayerType::NTT:
      initmsg += "2";
      break;
    }
    initmsg += " team ";
    switch (info_.initdata[j].team) {
    case Team::I:
      initmsg += "0";
      break;
    case Team::Ro:
      initmsg += "1";
      break;
    case Team::Ha:
      initmsg += "2";
      break;
    case Team::Ni:
      initmsg += "3";
      break;
    default:
      throw logic_error("AddUser: invalid team.");
      break;
    }
    initmsg += "\n";
  }
  initmsg += "end\n";
  initmsg += "stage ";
  switch (info_.stage) {
  case StageType::Forest:
    initmsg += "0";
    break;
  case StageType::Future:
    initmsg += "1";
    break;
  }
  for (int j = 0; j < 4; j++ ) {
    if ( info_.initdata[j].team == Team::None ) {
      break;
    }
    if ( info_.initdata[j].ok == true ) {
      initmsg += "\nok '" + Escape(info_.initdata[j].name) + "'";
    }
  }
  initmsg += "\n";
  con_[i-1]->Write(initmsg);
  return true;
}

void
LobbyIO::Flush()
{
  // バッファ送信
  if ( !sendbuf_.empty() ) {
    for ( int i = 0; i < 3; i++ ) {
      if ( !con_[i] )
	break;
      con_[i]->Write(sendbuf_);
    }
    sendbuf_.erase();
  }

  // 終了したクライアントがあるか
  sp_Connection tcon[3];
  lobby::PlayerData tpd[3];
  int ti = 0;
  for ( int i = 0; i < 3; i++ ) {
    if ( info_.initdata[i+1].team == Team::None && con_[i] ) {
      if ( !sock_ ) {
	sock_ = SockInit(port_);
      }
      if ( con_[i]->IsOpened() )
	con_[i]->Write("exit\n");
      cpool_.Add(con_[i]);
      con_[i].reset();
    } else {
      tcon[ti] = con_[i];
      tpd[ti] = info_.initdata[i+1];
      ti++;
    }
  }
  for ( int i = 0; i < ti; i++ ) {
    con_[i] = tcon[i];
    info_.initdata[i+1] = tpd[i];
  }
}
