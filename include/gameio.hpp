// last modified <2004/02/08 04:22:20 2004 JST>
#ifndef ZUGAGAGA_GAME_IO_HPP
#define ZUGAGAGA_GAME_IO_HPP

#include <utility>
#include <string>
#include "comtypes.hpp"
#include "utility.hpp"
#include "draw.hpp"

/*
  プロトコルを考える

  ------------------------------------------------
  ロビー
  ------------------------------------------------

  方向(dir)
    クライアントからサーバー : -->
    サーバーからクライアント : <--

  コマンド文字列
    有意な文字は\nとスペースだけ。
    行頭、行末、単語間に余分なスペースを含んではいけない。
  文字列データ
    'でかこむ。\'でエスケープ
    \nを含んではいけない

  すること		dir : コマンド文字列
  ------------------------------------------------
  接続 名前(HOGE)送信	--> : 'HOGE'\n

  だめ
    同じ名前がある		<-- : invalidname\n
  いい
  			<-- : info\n
			      name 'HITORIME' type ? team ?\n
			      name 'FUTARIME' type ? team ?\n
			      end\n

  機種変える		--> : nexttype\n
  機種変える		--> : prevtype\n
  チーム変える		--> : nextteam\n
  チーム変える		--> : prevteam\n
  準備完了		--> : ok\n
  やっぱやめ		--> : cancel\n
  さよなら		--> : bye\n
  メッセージ送信		--> : msg 'MSG'\n

  ユーザー追加		<-- : user 'NAMAE' type ? team ?\n
  ステージ変更		<-- : stage ?\n
  機種変更		<-- : type 'NAMAE' ?\n
  チーム変更		<-- : team 'NAMAE' ?\n
  準備完了		<-- : ok 'NAMAE'\n
  やっぱやめ		<-- : cancel 'NAMAE'\n
  さよなら		<-- : bye 'NAMAE'\n
  メッセージ受信		<-- : msg 'NAMAE' 'MSG'\n

  ゲーム開始(ロビー終了)	<-- : good luck!\n
  接続終了		<-- : exit\n
  ------------------------------------------------

  ユーザーが減った場合、隙間はつめられる。
  増えた場合、後ろにつく


  ------------------------------------------------
  ゲーム中
  ------------------------------------------------
  人は番号で管理 0, 1, 2, 3
  番号はロビーの一人目、二人目、、、に対応する。

  プレイヤーは起きたイベントを送信し、
  サーバーはフレーム更新の合図とともに各プレイヤーのイベントを返す。

  イベントの種類は以下の通り
  PUp
  PDown
  PLeft
  PRight
  PBtn1
  PBtn2
  PBtn3
  RUp
  RDown
  RLeft
  RRight
  RBtn1
  RBtn2
  RBtn3

  イベント発生		--> : EVE\n


  テキストで通信してみる？
  ----------------------------------------------------------------------
  ゲーム開始(ready?)	<-- : ready\n
  ラウンド開始		<-- : start\n

  次フレームに更新せよ	<-- : next\n
  			      0 EVE1 EVE2 ...\n
			      1 EVE1\n
			      end\n
  勝敗表示画面に行け	<-- : result\n
  勝敗表示画面終了  次ラウンドがあるとき
			<-- : ready\n
  勝敗表示画面終了  ゲーム終了
			<-- : good job!\n

  終了受信		--> : bye\n

  バイナリで通信してみる？
  ----------------------------------------------------------------------
  ゲーム開始(ready?)	<-- : ready\n
  ラウンド開始		<-- : start\n

  次フレームに更新せよ	<-- : data ???\n
  			      DATA

  勝敗表示画面に行け	<-- : result\n

  勝敗表示画面終了  次ラウンドがあるとき
			<-- : ready\n
  勝敗表示画面終了  ゲーム終了
			<-- : good job!\n

  終了受信		--> : bye\n

  bye受信でコネクションを切断。
*/

namespace zu3ga {
  namespace GameStatus {
    enum t {
      Ready,	///< ラウンド開始するよー
      Game,	///< ラウンド中
      Result,	///< ラウンド結果表示画面
      Final,	///< 最終画面
    };
  }
  /**
     ゲーム中のイベントの入出力
  */
  typedef std::pair<SendObjAry, SendParam> SendDataPair;
  class EventIOBase : public UpdaterBase
  {
  public:
    virtual SendDataPair GetData() = 0;
    virtual void PushEvent(unsigned char event) = 0;
    virtual GameStatus::t GetStatus() = 0;

    virtual ~EventIOBase() {}
  };
  typedef boost::shared_ptr<EventIOBase> sp_EventIO;

  namespace lobby {
    struct PlayerData : public PlayerInitData
    {
      std::string name;
      bool ok;
      PlayerData() : ok( false ) {}

      StatusTag GetDMTag(int i) const
      {
	StatusTagID::t id = StatusTagID::t(i);
	if (team == Team::None )
	  id = StatusTagID::NoUse;
	return StatusTag(id,name,team);
      }
    };

    struct Info
    {
      lobby::PlayerData initdata[4];
      int self;
      StageType::t stage;

      PlayerData GetMyData() const { return initdata[self]; }
      DMInitData GetDMInitData() const
      {
	DMInitData temp;
	temp.stg_type = stage;
	temp.tags[0] = initdata[self].GetDMTag(self);
	for (int i = 0; i < 3; i++) {
	  int j = i;
	  if ( j >= self ) {
	    j++;
	  }
	  temp.tags[i+1] = initdata[j].GetDMTag(j);
	}
	return temp;
      }

      Info() : self(0), stage(StageType::Forest) {}
    };

    namespace Stat {
      enum t {
	Continue,
	Quit,
	Start,
	Error,
      };
    }
  }

  /**
     ロビーの通信
  */
  class LobbyIOBase : public UpdaterBase
  {
  public:
    virtual sp_EventIO GetEventIO() = 0;
    virtual sp_DrawingManager GetDrawingManager() = 0;

    virtual const lobby::Info* GetData() = 0;
    virtual ~LobbyIOBase() {}
    virtual void NextChara() = 0;
    virtual void PrevChara() = 0;
    virtual void NextStage() = 0;
    virtual void PrevStage() = 0;
    virtual void NextTeam() = 0;
    virtual void PrevTeam() = 0;
    virtual void Ready() = 0;
    virtual void Cancel() = 0;
    virtual bool IsHost() = 0;
    virtual void Bye() = 0;
    virtual void Start() = 0;
    virtual lobby::Stat::t GetStatus() = 0;
    virtual void SendMsg(const std::string&) = 0;
    virtual std::string GetMsg() = 0;
    virtual std::string GetError() = 0;
  };
  typedef boost::shared_ptr<LobbyIOBase> sp_LobbyIO;

  inline std::string
  Escape( const std::string& msg )
  {
    std::string temp = msg;
    std::string::size_type pos = 0;
    while ( (pos=temp.find("'",pos)) != temp.npos ) {
      temp.insert(pos,1,'\\');
      pos+=2;
    }
    return temp;
  }

  inline std::string
  Unescape( const std::string& msg )
  {
    std::string temp = msg;
    for ( std::string::size_type i = 0; (i=temp.find("\\'",i)) != temp.npos; ) {
      temp.erase(i,1);
      i++;
    }
    return temp;
  }

  inline std::string::size_type
  FindQuote(const std::string& str, std::string::size_type pos)
  {
    std::string::size_type i = pos - 1;
    do {
      i++;
      i = str.find("'",i);
    } while ( str[i-1] == '\\' && i != str.npos );
    return i;
  }

  inline std::string
  GetString(const std::string& str, std::string::size_type pos = 0)
  {
    std::string::size_type begin = FindQuote(str,pos);
    std::string::size_type end = FindQuote(str,begin+1);
    if ( begin == str.npos || end == str.npos ) {
      throw SyntaxError(str);
    }
    std::string temp = str.substr(begin+1, end-begin-1);
    return Unescape(temp);
  }

  inline std::string
  GetName(const std::string& str, std::string::size_type pos = 0)
  {
    return GetString(str, pos);
  }

  inline std::string
  EveToString(unsigned char event)
  {
    std::string temp;
    unsigned char eve;
    if ( event & ClientEvent::Press ) {
      temp += 'P';
      eve = event^ClientEvent::Press;
    } else {
      temp += 'R';
      eve = event;
    }

    switch (eve) {
    case ClientEvent::Up:
      temp += "Up";
      break;
    case ClientEvent::Down:
      temp += "Down";
      break;
    case ClientEvent::Left:
      temp += "Left";
      break;
    case ClientEvent::Right:
      temp += "Right";
      break;
    case ClientEvent::Button1:
      temp += "Btn1";
      break;
    case ClientEvent::Button2:
      temp += "Btn2";
      break;
    case ClientEvent::Button3:
      temp += "Btn3";
      break;
    default:
      throw std::logic_error(std::string("EventIO::PushEvent: invalid event.") +
			     IntToStr(int(event)));
      break;
    }
    return temp;
  }

}

#endif // ZUGAGAGA_GAME_IO_HPP
