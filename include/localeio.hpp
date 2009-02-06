//last modified <2004/02/08 00:23:50 2004 JST>
#ifndef ZUGAGAGA_LOCALEIO_HPP
#define ZUGAGAGA_LOCALEIO_HPP

#include <vector>
#include "gameio.hpp"
#include "objects.hpp"
#include "network.hpp"

namespace zu3ga {
  namespace local {
    class Server;
    typedef boost::shared_ptr<Server> sp_Server;
    class EventIO : public EventIOBase
    {
    public:
      void Update(); // フレーム更新があったら SendDataPair もらう。
      SendDataPair GetData();
      void PushEvent(unsigned char event); // イベントはサーバーを通して入れる
      GameStatus::t GetStatus(); // ゲーム状態はサーバーに聞く

      /**
	 vector<int> にはディスクリプターとをいれとく
	 添字はロビーの順番。つまり、"x人目"のディスクリプターを [x-1] に格納する。
	 自分のばあいは desc は 0 にする。
      */
      static sp_EventIO Create(const lobby::Info&, const std::vector<network::sp_Connection>& descs);
      ~EventIO();
    private:
      EventIO(const lobby::Info&, const std::vector<network::sp_Connection>&);
      sp_Server serv_;
    };
  }
}

#endif // ZUGAGAGA_LOCALEIO_HPP
