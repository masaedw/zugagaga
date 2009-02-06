// last modified <2004/02/08 04:24:33 2004 JST>
#ifndef ZUGAGAGA_NETWORKIO_HPP
#define ZUGAGAGA_NETWORKIO_HPP

#include <string>
#include "gameio.hpp"
#include "network.hpp"

namespace zu3ga {
  namespace network {
    class LobbyIO : public LobbyIOBase
    {
    public:
      void Update();
      const lobby::Info* GetData();
      sp_EventIO GetEventIO();
      sp_DrawingManager GetDrawingManager();

      static sp_LobbyIO Create(const sp_Connection&, const std::string& name);
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
      LobbyIO(const sp_Connection&, const std::string& name);

      lobby::Info info_;
      sp_Connection connection_;
      lobby::Stat::t stat_;
      std::string name_, msgcache_;
      bool info_ready_;
      std::string error_;
    };
  }
}

#endif // ZUGAGAGA_NETWORKIO_HPP
