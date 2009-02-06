// last modified <2004/02/08 20:30:05 2004 JST>
// あ
#include <memory>
#include <errno.h>
#include <SDL_thread.h>
#include "localeio.hpp"
#include "network.hpp"

using namespace edw;
using namespace edw::sdl;
using namespace std;
using namespace zu3ga;
using namespace zu3ga::local;
using namespace zu3ga::network;
using namespace boost;

namespace {
  const int FPS = 30;
  const int sample = 2;
  const int ready_time = FPS*2*sample;
  const int result_time = FPS*4*sample;

  class Mutex;
  typedef boost::shared_ptr<Mutex> sp_Mutex;
  class Mutex
  {
  private:
    Mutex()
      : mut_(SDL_CreateMutex())
    {
      if (!mut_)
	throw runtime_error("Can't create mutex.");
    }
  public:
    static sp_Mutex Create()
    {
      return sp_Mutex( new Mutex );
    }
    ~Mutex()
    {
      SDL_DestroyMutex(mut_);
    }
    void Lock() const
    {
      if ( SDL_LockMutex(mut_) == -1 )
	throw runtime_error("Can't lock mutex.");
    }
    void Unlock() const
    {
      if ( SDL_UnlockMutex(mut_) == -1 )
	throw runtime_error("Can't unlock mutex.");
    }
  private:
    SDL_mutex* mut_;
  };

  class Locker
  {
  public:
    Locker( const sp_Mutex& mut )
      : mut_(mut)
    {
      mut_->Lock();
    }
    ~Locker()
    {
      mut_->Unlock();
    }
  private:
    sp_Mutex mut_;
  };

  template <class T> class AtmicComp
  {
  public:
    explicit AtmicComp( const T& data )
      : data_(data), mut_( Mutex::Create() ) {}
    AtmicComp() {}
    bool operator != ( const T& data )
    {
      Locker l(mut_);
      return data_ != data;
    }
    bool operator == ( const T& data )
    {
      Locker l(mut_);
      return data_ == data;
    }
    const AtmicComp<T>& operator = ( const T& data )
    {
      Locker l(mut_);
      data_ = data;
      return *this;
    }
    T Get()
    {
      Locker l(mut_);
      return data_;
    }
  private:
    T data_;
    sp_Mutex mut_;
  };

  struct ConID // Connection & ID
  {
    sp_Connection con;
    int id;
    ConID( const sp_Connection& con, int id )
      : con(con), id(id) {}
    ConID( const sp_Connection& con )
      : con(con), id(0) {}
    ConID() : id(0) {}
  };

  inline char NumToChar(int i)
  {
    switch (i) {
    case 0:
      return '0';
      break;
    case 1:
      return '1';
      break;
    case 2:
      return '2';
      break;
    case 3:
      return '3';
      break;
    default:
      break;
    }
    return 0;
  }

  //______________________________________________________________________
  //----------------------------------------------------------------------

  unsigned char
  StrToEve( const string& temp )
  {
    unsigned char eve = 0;
    switch (temp[0]) {
    case 'P':
      eve |= ClientEvent::Press;
      break;
    case 'R':
      break;
    default:
      throw SyntaxError(temp);
      break;
    }

    switch (temp[1]) {
    case 'B':
      switch (temp[4]) {
      case '1':
	eve |= ClientEvent::Button1;
	break;
      case '2':
	eve |= ClientEvent::Button2;
	break;
      case '3':
	eve |= ClientEvent::Button3;
	break;
      default:
	throw SyntaxError(temp);
	break;
      }
      break;
    case 'D':
      eve |= ClientEvent::Down;
      break;
    case 'L':
      eve |= ClientEvent::Left;
      break;
    case 'R':
      eve |= ClientEvent::Right;
      break;
    case 'U':
      eve |= ClientEvent::Up;
      break;
    default:
      throw SyntaxError(temp);
      break;
    }
    return eve;
  }
}

//======================================================================
//======================================================================


class local::Server
{
public:
  explicit Server( const lobby::Info& info, const vector<sp_Connection>& descs );
  ~Server();
  SendDataPair GetDataPair();
  GameStatus::t GetStatus();
  void PushEvent( unsigned char event );
  bool ConnectionClosed() { return cclose_.Get(); }

private:
  static int Runner(void* arg)
  {
    Server* this_ = reinterpret_cast<Server*>(arg);
    this_->MainLoop();
    return 0;
  }
  void Stop();
  void Start();
  void MainLoop();
  void InitObjs();

  void SendNext();
  void SendReady();
  void SendExit();
  void SendStart();
  void SendResult();

  enum Stat {
    StRun,
    StStopRequested,
    StStop,
  };

  lobby::Info info_;
  int myindex_;
  sp_Mutex om_;
  sp_Objects objs_;
  SDL_Thread* thr_;
  AtmicComp<Stat> stat_;
  AtmicComp<GameStatus::t> gstat_;
  vector<ConID> cid_;
  AtmicComp<bool> cclose_;
  const SendObjAry* ary_;
};

SendDataPair
Server::GetDataPair()
{
  Locker l(om_);
  return SendDataPair(*ary_,objs_->GetSendParam(cid_[myindex_].id));
}

void
Server::Stop()
{
  if ( stat_ != StRun )
    return;
  stat_ = StStopRequested;
  SDL_WaitThread(thr_,0);
}

void
Server::Start()
{
  if ( stat_ == StRun )
    return;
  stat_ = StRun;
  if ((thr_ = SDL_CreateThread( &Server::Runner, this )) == 0)
    throw runtime_error("Can't start server.");
}

GameStatus::t
Server::GetStatus()
{
  return gstat_.Get();
}

void
Server::PushEvent( unsigned char eve )
{
  Locker l(om_);
  objs_->ChangePlayerState(eve,cid_[myindex_].id);
}

void
Server::InitObjs()
{
  objs_ = sp_Objects( new Objects(info_.stage) );
  Locker l(om_);
  for ( unsigned int i = 0; i < cid_.size(); i++ ) {
    cid_[i].id = objs_->AddPlayer(info_.initdata[i]);
  }
  objs_->UpdateAll();
  objs_->HitCheck();
}

Server::Server( const lobby::Info& info, const vector<sp_Connection>& descs )
  : info_(info)
  , myindex_(info.self)
  , om_( Mutex::Create() )
  , stat_(StStop)
  , gstat_(GameStatus::Ready)
  , cclose_(false)
{
  for ( unsigned int i = 0; i < descs.size(); i++ ) {
    cid_.push_back(ConID(descs[i]));
  }
  InitObjs();
  Start();
}

Server::~Server()
{
  Stop();
}

void
Server::SendNext()
{
  const SendObjAry* ary;
  if ( (ary = objs_->GetSendData()) == 0 )
    return;
  ary_ = ary;
  ONetworkData data;
  data << *ary;
  for ( unsigned int i = 0; i < cid_.size(); i++ ) {
    if ( !cid_[i].con )
      continue;
    ONetworkData param;
    param << objs_->GetSendParam(cid_[i].id);
    string temp("data ");
    temp += IntToStr(data.Get().size() + param.Get().size()) + "\n";
    cid_[i].con->Write(temp+data.Get()+param.Get());
  }
}

void
Server::SendExit()
{
  for ( Uint32 i = 0; i < cid_.size(); i++ ) {
    if ( !cid_[i].con )
      continue;
    cid_[i].con->Write("good job!\n");
  }
}

void
Server::SendReady()
{
  for ( unsigned int i = 0; i < cid_.size(); i++ ) {
    if ( !cid_[i].con )
      continue;
    cid_[i].con->Write("ready\n");
  }
}

void
Server::SendStart()
{
  for ( unsigned int i = 0; i < cid_.size(); i++ ) {
    if ( !cid_[i].con )
      continue;
    cid_[i].con->Write("start\n");
  }
  Locker l(om_);
  objs_->LetsStart();
}

void
Server::MainLoop()
{
  SDLTimer timer;
  timer.SetFPS(FPS*2);
  int statecount = 0, count = 0;
  SendReady();
  while (stat_ == StRun) {
    timer.Start();

    // 次へ行けと指令出す
    if ( ++count %= 2 ) {
      Locker l(om_);
      objs_->UpdateAll();
      objs_->HitCheck();
      SendNext();
    }

    if ( gstat_ == GameStatus::Result ) { // Result
      // すべて破棄
      for ( unsigned int i = 0; i < cid_.size(); i++ ) {
	if ( !cid_[i].con ) continue;
	try {
	  cid_[i].con->Read();
	} catch (...) {
	  cid_[i].con.reset();
	  cclose_ = true;
	}
      }
      statecount++;
      if ( statecount > result_time ) {
	// しばらくしたら開始
	// objs_を解放
	gstat_ = GameStatus::Ready;
	SendReady();
	statecount = 0;
	objs_.reset();
      }
    } // Result
    else if ( gstat_ == GameStatus::Final ) { // Final
      if ( statecount++ > FPS ) {
	// クローズして終了
	SendExit();
	SDLTimer timer;
	timer.SetFPS(30);
	while (stat_ == StRun) {
	  timer.Start();
	  bool finish = true;
	  for ( Uint32 i = 0; i < cid_.size(); i++ ) {
	    if ( !cid_[i].con ) {
	      continue;
	    }
	    finish = false;
	    string data = cid_[i].con->Read();
	    if ( data.find("bye\n") == 0 ) {
	      cid_[i].con->Close();
	      cid_[i].con.reset();
	    }
	  }
	  if ( finish )
	    break;
	  timer.Wait();
	}
	goto endfunc;
      }
    } // Final
    else { // Game & Ready
      if ( gstat_ == GameStatus::Ready ) {
	if ( !objs_ )
	  InitObjs();

	if ( statecount == ready_time ) {
	  gstat_ = GameStatus::Game;
	  SendStart();
	  statecount = 0;
	} else {
	  statecount++;
	}
      } else if ( objs_->IsFinished() != Team::None ) {
	gstat_ = GameStatus::Final;
      }

      for ( unsigned int i = 0; i < cid_.size(); i++ ) {
	if ( !cid_[i].con )
	  continue;
	string temp;
	try {
	  temp= cid_[i].con->Read();
	} catch (...) {
	  cid_[i].con.reset();
	  cclose_ = true;
	}
	if ( temp.empty() )
	  continue;

	string::size_type endp = temp.find("\n");
	if ( endp == temp.npos ) {
	  cid_[i].con->PutBack(temp);
	  continue;
	}

	unsigned char eve = StrToEve(temp);

	temp.erase(0,endp+1);

	if (!temp.empty())
	  cid_[i].con->PutBack(temp);

	Locker l(om_);
	objs_->ChangePlayerState(eve,cid_[i].id);
      } // for

    } // Game & Ready

    if ( errno == EPIPE ) {
      cclose_ = true;
      break;
    }

    if ( cclose_ == true )
      break;

    timer.Wait();
  } // while (thread continue?)

 endfunc:
  stat_ = StStop;
}

//======================================================================
//======================================================================
void
EventIO::Update()
{
  if ( serv_->ConnectionClosed() ) {
    throw Zu3gaError("ネットワークが切断されました。");
  }
}

GameStatus::t
EventIO::GetStatus()
{
  return serv_->GetStatus();
}

void
EventIO::PushEvent( unsigned char event )
{
  serv_->PushEvent( event );
}

sp_EventIO
EventIO::Create( const lobby::Info& info, const vector<sp_Connection>& descs )
{
  return sp_EventIO( new EventIO(info,descs) );
}

EventIO::~EventIO() {}

EventIO::EventIO( const lobby::Info& info, const vector<sp_Connection>& descs )
  : serv_( new Server(info,descs) )
{}

SendDataPair
EventIO::GetData()
{
  return serv_->GetDataPair();
}
