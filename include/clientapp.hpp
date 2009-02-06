#ifndef ZUGAGAGA_CLIENTAPP_HPP
#define ZUGAGAGA_CLIENTAPP_HPP

#include "draw.hpp"
#include "utility.hpp"
#include "gameio.hpp"

namespace zu3ga {
  /**
     クライアントプログラムのアプリケーションクラス
  */
  class ClientApp
  {
  public:
    bool Run();

    ClientApp();
    ~ClientApp();

  private:
    bool Init();
    bool InitGraphic();
    bool InitText();
    bool Lobby();
    /**
       画面出力とインプットデバイス受け持ちのゲーム中メインループ
    */
    bool Mainloop();

    void ResetPointers();

    sp_LobbyIO lobbyio_;
    sp_EventIO eio_;
    sp_DrawingManager dmngr_;
    graphic::VideoInit videoinit__;
    std::string errmsg_, host_, port_, name_;
  };
}

#endif // ZUGAGAGA_CLIENTAPP_HPP
