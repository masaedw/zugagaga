// last modified <2004/03/02 21:56:03 2004 JST>
#ifndef ZUGAGAGA_LOCALIO_HPP
#define ZUGAGAGA_LOCALIO_HPP

#include <list>
#include "gameio.hpp"
#include "objects.hpp"
#include "network.hpp"

namespace zu3ga {

  namespace local {
    //時間差で close するためのもの
    class CPool
    {
      struct Inner
      {
	network::sp_Connection con;
	unsigned int cnt;
	Inner( const network::sp_Connection& c, unsigned int i )
	  : con(c), cnt(i) {}
      };
    public:
      void Update()
      {
	if ( !clist_.empty() ) {
	  if ( --(clist_.front().cnt) == 0 ) {
	    clist_.pop_front();
	  }
	}
	if ( count_ < 10 ) count_++;
      }

      void Add( const network::sp_Connection& c )
      {
	clist_.push_back(Inner(c,count_));
	count_ = 0;
      }
    private:
      unsigned int count_;
      std::list<Inner> clist_;
    };

    class LobbyIO : public LobbyIOBase
    {
    public:
      void Update();
      const lobby::Info* GetData();
      sp_EventIO GetEventIO();
      sp_DrawingManager GetDrawingManager();

      static sp_LobbyIO Create(int port, const std::string& name);
      ~LobbyIO();

      void NextChara();
      void PrevChara();
      void NextStage();
      void PrevStage();
      void NextTeam();
      void PrevTeam();
      void Ready();
      void Cancel();
      bool IsHost();
      void Bye();
      void Start();
      lobby::Stat::t GetStatus();
      void SendMsg(const std::string&);
      std::string GetMsg();
      std::string GetError() { return error_; }

    private:
      void NextChara(int);
      void PrevChara(int);
      void NextTeam(int);
      void PrevTeam(int);
      void Ready(int);
      void Cancel(int);
      void Bye(int);
      void SendMsg(int,const std::string&);
      bool AddUser(int,const std::string& name);
      void Flush();

      LobbyIO(int port, const std::string& name);
      int port_;
      network::sp_Connection con_[3];
      TCPsocket sock_;
      lobby::Info info_;
      lobby::Stat::t stat_;
      std::string sendbuf_;
      std::string msgbuf_;
      std::string error_;
      bool bye_;
      CPool cpool_;
    };
  }
}

#endif // ZUGAGAGA_LOCALIO_HPP
