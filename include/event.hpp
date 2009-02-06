//============================================================
// イベント処理全般
// last modified <2004/02/07 22:11:15 2004 JST>
//============================================================
#ifndef ZUGAGAGA_CLIENT_EVENT_HPP
#define ZUGAGAGA_CLIENT_EVENT_HPP

#include <map>
#include <memory>
#include "edw/sdl.hpp"
#include "comtypes.hpp"
#include "motion.hpp"

namespace zu3ga {

  namespace LobbyEvent {
    enum t {
      None,
      Up,
      Down,
      Left,
      Right,
      Change,
      BackChange,
      OK,
      Key,
      Quit,
    };
  }

  class EventManager
  {
  public:
    static void Init();
    static void Quit();
    static EventManager* GetInstance();

    /**
       イベント取得 ゲーム操作モード
       @param eve ClientEvent が入って返される
       @return イベントがあったなら true 無ければ false 。
    */
    bool GetEvent(Uint8& eve);

    /**
       イベント取得 キー入力(ロビー)モード
       @param eve LobbyEvent が入って返される。
       @param unicode 入力された文字
       @return イベントがあったなら true 無ければ false 。
    */
    bool GetEvent(LobbyEvent::t& eve, Uint16& unicode);

    void SetKeyEvent(SDLKey,Uint8);

    void SetVerticalAxis(Uint8);
    void SetHorizontalAxis(Uint8);
    void SetButton1Button(Uint8);
    void SetButton2Button(Uint8);
    void SetButton3Button(Uint8);

    void DumpConfigFile();
  private:
    EventManager();
    ~EventManager();

    void GameMode();
    void KeyMode();

    enum Mode {
      Key,
      Game,
    };
    // joystick 軸、ボタン番号対応用
    enum {
      Vertical,
      Horizontal,
    };
    enum {
      Button1,
      Button2,
      Button3,
    };

    struct KeyData
    {
      LobbyEvent::t event;
      Uint16 unicode;
      KeyData()
	: event(LobbyEvent::None), unicode(0) {}
    };

    struct JoyData
    {
      LobbyEvent::t lobby;
      Uint8 client;
      JoyData()
	: lobby(LobbyEvent::None), client(ClientEvent::None) {}
    };

    class Stick
    {
    public:
      Stick()
	: value_(0) {}
      void Up()
      {
	if (value_ & SDL_HAT_DOWN)
	  value_ ^= SDL_HAT_DOWN;
	else
	  value_ |= SDL_HAT_UP;
      }

      bool IsUp()
      {
	return value_ & SDL_HAT_UP;
      }

      void Down()
      {
	if (value_ & SDL_HAT_UP)
	  value_ ^= SDL_HAT_UP;
	else
	  value_ |= SDL_HAT_DOWN;
      }

      bool IsDown()
      {
	return value_ & SDL_HAT_DOWN;
      }

      void VerticalZero()
      {
	value_ &= ~(SDL_HAT_UP|SDL_HAT_DOWN);
      }

      void Left()
      {
	if (value_ & SDL_HAT_RIGHT)
	  value_ ^= SDL_HAT_RIGHT;
	else
	  value_ |= SDL_HAT_LEFT;
      }

      bool IsLeft()
      {
	return value_ & SDL_HAT_LEFT;
      }

      void Right()
      {
	if (value_ & SDL_HAT_LEFT)
	  value_ ^= SDL_HAT_LEFT;
	else
	  value_ |= SDL_HAT_RIGHT;
      }

      bool IsRight()
      {
	return value_ & SDL_HAT_RIGHT;
      }

      void HorizontalZero()
      {
	value_ &= ~(SDL_HAT_RIGHT|SDL_HAT_LEFT);
      }

      int Get() { return value_; }

    private:
      Uint8 value_;
    };

    KeyData GetLobbyKeyEvent(const SDL_KeyboardEvent& key);
    JoyData GetJoyEvent(const SDL_JoyAxisEvent& jaxis);
    JoyData GetJoyEvent(const SDL_JoyButtonEvent& jbutton);

    typedef std::map<SDLKey,Uint8> Keymap;
    Keymap key_;//quitkey_, upkey_, downkey_, leftkey_, rightkey_, b1key_, b2key_, b3key_;
    std::vector<Uint8> padstick_, padbutton_;
    Stick stickvalue_;
    Mode mode_;
    SDL_Joystick* stick_;
  };

  /**
     キーのイベントを表す定数．
     Arrowの数値はテンキーと対応．
  */
  namespace KeyEvent {
    enum KeyEvent_t {
      // Arrowイベントとはキーボードの状態が変化したというイベント
      Arrow1,
      Arrow2,
      Arrow3,
      Arrow4,
      Arrow5, ///< 何も押されていない状態に変化，というイベント
      Arrow6,
      Arrow7,
      Arrow8,
      Arrow9,
      Button1Press,
      Button2Press,
      Button3Press,
      Button1Release,
      Button2Release,
      Button3Release,
    };
  }

  // ArrowKeyManager::GetKeyStateで使用する要素
  namespace Vertical {
    enum t {
      Up,
      Down,
      None,
    };
  }
  namespace Horizon {
    enum t {
      Left,
      Right,
      None,
    };
  }
  /**
     現在押されている十字キーを管理するクラス．
  */
  class ArrowKeyManager
  {
    // 登録する十字キーの方向
    enum ArrowKey_t {
      None = 0,
      Up,
      Down, 
      Left,
      Right,
    };

    typedef std::vector<ArrowKey_t> ArrowKeyAry;
    typedef ArrowKeyAry::iterator AKAIter;

  private:
    ArrowKeyAry keys;

    // private method
    ArrowKey_t ClientEventToArrowKey(const unsigned char event);

  public:
    KeyEvent::KeyEvent_t GetKeyState();
    void Add(unsigned char clie_eve);
    void Delete(unsigned char clie_eve);
  };

  /**
     クライアントイベントからキーイベントを生み出すクラス
  */
  class KeyEventCreater
  {
  private:
    ArrowKeyManager akmanager;

  public:
    KeyEvent::KeyEvent_t ConvertClientEvent(unsigned char client_event);
    Direction::t GetKeyDirection();
  };

}

#endif // ZUGAGAGA_CLIENT_EVENT_HPP
