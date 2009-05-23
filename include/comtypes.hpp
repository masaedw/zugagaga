/** @file
 * 一般に使う型定義
 *
 * last modified <2004/02/09 21:19:23 2004 JST>
 */
#ifndef ZUGAGAGA_COMTYPES_HPP
#define ZUGAGAGA_COMTYPES_HPP

#include <vector>
#include "edw/basic_structs.hpp"

namespace zu3ga {
  using namespace edw::basic_structs;

  /**
   * パラメータ型
   */
  struct SendPos
  {
    short x, y;
    SendPos( short x, short y )
      : x(x), y(y) {}
    SendPos() {}
  };
  typedef std::vector<unsigned short> HitPoints;
  typedef unsigned char DashParam;

  inline Pos toPos(const SendPos spos) { return Pos(spos.x, spos.y); }

  /**
   * クライアントが受信する視覚情報
   */
  struct SendObject
  {
    SendPos pos; // 4
    unsigned char type; // 1
    unsigned char state; // 1
    SendObject()
      : type(0), state(0) {}
    SendObject(const SendPos& pos, unsigned char type, unsigned char state)
      : pos(pos), type(type), state(state) {}
  };

  typedef std::vector<SendObject> Arrows;

  /**
   * Typeを持たないSendObject
   * 状況を限定させて使う
   */
  struct NonTypeSendObject
  {
    SendPos pos;		// 4
    unsigned char state;	// 1
  };

  /**
   * クライアントが受信する視覚情報の配列
   */
  typedef std::vector<SendObject> SendObjAry;
  typedef SendObjAry::iterator SOAIter;
  typedef SendObjAry::const_iterator SOAConstIter;

  namespace SendObjType {
    enum t {
      CharaA = 0,
      Planex = 0,
      PlanexI = Planex,
      PlanexRo,
      PlanexHa,
      PlanexNi,
      Yamaha,
      YamahaI = Yamaha,
      YamahaRo,
      YamahaHa,
      YamahaNi,
      NTT,
      NTTI = NTT,
      NTTRo,
      NTTHa,
      NTTNi,
      PlanexMissile,
      PlanexEnergyShot,
      YamahaBomb,
      YamahaLaserShot,
      NTTEnergyShot,
      YamahaLaserEffect,
      DotEffect,
      ExplosionA,
      ExplosionAx2,
      ExplosionAx4,
      ExplosionB,
      ExplosionC,
      SmokeA,
      SlashA,
      Shadow,
      Arrow,
      TransArrow,
      ArrowLight,
      NoGraphic,		// 画像なしオブジェクト(通信しない)
      End,
    };
  }

  namespace NTTState {
    enum t {
      Stand1,
      Stand2,
      Stand3,
      Stand4,
      Stand6,
      Stand7,
      Stand8,
      Stand9,
      Dash1,
      Dash2,
      Dash3,
      Dash4,
      Dash6,
      Dash7,
      Dash8,
      Dash9,
      Weapon1,
      Weapon2,
      Weapon3,
      Weapon4,
      Weapon6,
      Weapon7,
      Weapon8,
      Weapon9,
      Attack1,
      Attack2,
      Attack3,
      Attack4,
      Attack6,
      Attack7,
      Attack8,
      Attack9,
      Kin1_1,
      Kin1_2,
      Kin1_3,
      Kin1_4,
      Kin1_5,
      Kin1_6,
      Kin2_1,
      Kin2_2,
      Kin2_3,
      Kin2_4,
      Kin2_5,
      Kin2_6,
      Kin3_1,
      Kin3_2,
      Kin3_3,
      Kin3_4,
      Kin3_5,
      Kin3_6,
      Kin4_1,
      Kin4_2,
      Kin4_3,
      Kin4_4,
      Kin4_5,
      Kin4_6,
      Kin6_1,
      Kin6_2,
      Kin6_3,
      Kin6_4,
      Kin6_5,
      Kin6_6,
      Kin7_1,
      Kin7_2,
      Kin7_3,
      Kin7_4,
      Kin7_5,
      Kin7_6,
      Kin8_1,
      Kin8_2,
      Kin8_3,
      Kin8_4,
      Kin8_5,
      Kin8_6,
      Kin9_1,
      Kin9_2,
      Kin9_3,
      Kin9_4,
      Kin9_5,
      Kin9_6,
      Down4,
      Down6,
    };
  }


  /**
     本来はTypeはArrowとTransArrowを分けるために使うが
     IDをついでに扱ってみる
  */
  namespace ArrowType {
    enum t {
      ID0,
      ID1,
      ID2,
      ID3,
      TransID0,
      TransID1,
      TransID2,
      TransID3,
    };
  }

  namespace CharaAState {
    enum t {
      Stand1 = 0,
      Stand2,
      Stand3,
      Stand4,
      Stand6,
      Stand7,
      Stand8,
      Stand9,
      Walk1,
      Walk2,
      Walk3,
      Walk4,
      Walk6,
      Walk7,
      Walk8,
      Walk9,
      Dash1,
      Dash2,
      Dash3,
      Dash4,
      Dash6,
      Dash7,
      Dash8,
      Dash9,
      Weapon1,
      Weapon2,
      Weapon3,
      Weapon4,
      Weapon6,
      Weapon7,
      Weapon8,
      Weapon9,
      Down1,
      Down2,
      Kin1_1,
      Kin1_2,
      Kin1_3,
      Kin1_4,
      Kin1_5,
      Kin1_6,
      Kin1_7,
      Kin1_8,
      Kin2_1,
      Kin2_2,
      Kin2_3,
      Kin2_4,
      Kin2_5,
      Kin2_6,
      Kin2_7,
      Kin2_8,
      Kin3_1,
      Kin3_2,
      Kin3_3,
      Kin3_4,
      Kin3_5,
      Kin3_6,
      Kin3_7,
      Kin3_8,
      Kin4_1,
      Kin4_2,
      Kin4_3,
      Kin4_4,
      Kin4_5,
      Kin4_6,
      Kin4_7,
      Kin4_8,
      Kin6_1,
      Kin6_2,
      Kin6_3,
      Kin6_4,
      Kin6_5,
      Kin6_6,
      Kin6_7,
      Kin6_8,
      Kin7_1,
      Kin7_2,
      Kin7_3,
      Kin7_4,
      Kin7_5,
      Kin7_6,
      Kin7_7,
      Kin7_8,
      Kin8_1,
      Kin8_2,
      Kin8_3,
      Kin8_4,
      Kin8_5,
      Kin8_6,
      Kin8_7,
      Kin8_8,
      Kin9_1,
      Kin9_2,
      Kin9_3,
      Kin9_4,
      Kin9_5,
      Kin9_6,
      Kin9_7,
      Kin9_8,
      End,
    };
  }

  // YamahaのSendObjState
  namespace YamahaState {
    enum t {
      Stand1,
      Stand2,
      Stand3,
      Stand4,
      Stand6,
      Stand7,
      Stand8,
      Stand9,
      Ahead1,
      Ahead2,
      Ahead3,
      Ahead4,
      Ahead6,
      Ahead7,
      Ahead8,
      Ahead9,
      Back1,
      Back2,
      Back3,
      Back4,
      Back6,
      Back7,
      Back8,
      Back9,
      AheadT1,
      AheadT2,
      AheadT3,
      AheadT4,
      AheadT6,
      AheadT7,
      AheadT8,
      AheadT9,
      BackT1,
      BackT2,
      BackT3,
      BackT4,
      BackT6,
      BackT7,
      BackT8,
      BackT9,
      Laser1_1,
      Laser1_2,
      Laser1_3,
      Laser1_4,
      Laser2_1,
      Laser2_2,
      Laser2_3,
      Laser2_4,
      Laser3_1,
      Laser3_2,
      Laser3_3,
      Laser3_4,
      Laser4_1,
      Laser4_2,
      Laser4_3,
      Laser4_4,
      Laser6_1,
      Laser6_2,
      Laser6_3,
      Laser6_4,
      Laser7_1,
      Laser7_2,
      Laser7_3,
      Laser7_4,
      Laser8_1,
      Laser8_2,
      Laser8_3,
      Laser8_4,
      Laser9_1,
      Laser9_2,
      Laser9_3,
      Laser9_4,
      Down4,
      Down6,
      Kin1_1,
      Kin1_2,
      Kin1_3,
      Kin1_4,
      Kin1_5,
      Kin1_6,
      Kin1_7,
      Kin1_8,
      Kin1_9,
      Kin1_10,
      Kin1_11,
      Kin1_12,
      Kin1_13,
      Kin2_1,
      Kin2_2,
      Kin2_3,
      Kin2_4,
      Kin2_5,
      Kin2_6,
      Kin2_7,
      Kin2_8,
      Kin2_9,
      Kin2_10,
      Kin2_11,
      Kin2_12,
      Kin2_13,
      Kin3_1,
      Kin3_2,
      Kin3_3,
      Kin3_4,
      Kin3_5,
      Kin3_6,
      Kin3_7,
      Kin3_8,
      Kin3_9,
      Kin3_10,
      Kin3_11,
      Kin3_12,
      Kin3_13,
      Kin4_1,
      Kin4_2,
      Kin4_3,
      Kin4_4,
      Kin4_5,
      Kin4_6,
      Kin4_7,
      Kin4_8,
      Kin4_9,
      Kin4_10,
      Kin4_11,
      Kin4_12,
      Kin4_13,
      Kin6_1,
      Kin6_2,
      Kin6_3,
      Kin6_4,
      Kin6_5,
      Kin6_6,
      Kin6_7,
      Kin6_8,
      Kin6_9,
      Kin6_10,
      Kin6_11,
      Kin6_12,
      Kin6_13,
      Kin7_1,
      Kin7_2,
      Kin7_3,
      Kin7_4,
      Kin7_5,
      Kin7_6,
      Kin7_7,
      Kin7_8,
      Kin7_9,
      Kin7_10,
      Kin7_11,
      Kin7_12,
      Kin7_13,
      Kin8_1,
      Kin8_2,
      Kin8_3,
      Kin8_4,
      Kin8_5,
      Kin8_6,
      Kin8_7,
      Kin8_8,
      Kin8_9,
      Kin8_10,
      Kin8_11,
      Kin8_12,
      Kin8_13,
      Kin9_1,
      Kin9_2,
      Kin9_3,
      Kin9_4,
      Kin9_5,
      Kin9_6,
      Kin9_7,
      Kin9_8,
      Kin9_9,
      Kin9_10,
      Kin9_11,
      Kin9_12,
      Kin9_13,
      End,
    };
  }

  // 矢印の状態
  namespace ArrowState {
    enum t {
      IBigUp,
      IBigDown,
      IBigLeft,
      IBigRight,
      IMiddleUp,
      IMiddleDown,
      IMiddleLeft,
      IMiddleRight,
      ISmallUp,
      ISmallDown,
      ISmallLeft,
      ISmallRight,
      RoBigUp,
      RoBigDown,
      RoBigLeft,
      RoBigRight,
      RoMiddleUp,
      RoMiddleDown,
      RoMiddleLeft,
      RoMiddleRight,
      RoSmallUp,
      RoSmallDown,
      RoSmallLeft,
      RoSmallRight,
      HaBigUp,
      HaBigDown,
      HaBigLeft,
      HaBigRight,
      HaMiddleUp,
      HaMiddleDown,
      HaMiddleLeft,
      HaMiddleRight,
      HaSmallUp,
      HaSmallDown,
      HaSmallLeft,
      HaSmallRight,
      NiBigUp,
      NiBigDown,
      NiBigLeft,
      NiBigRight,
      NiMiddleUp,
      NiMiddleDown,
      NiMiddleLeft,
      NiMiddleRight,
      NiSmallUp,
      NiSmallDown,
      NiSmallLeft,
      NiSmallRight,
    };
  }

  // Direction::tと数値は対応
  namespace ShellState {
    enum t {
      dir1 = 0,
      dir2,
      dir3,
      dir4,
      dir6,
      dir7,
      dir8,
      dir9,
      End,
    };
  }

  /**
   * クライアントが受信する視覚情報以外の情報
   */
  struct SendParam
  {
    SendPos view_pos;		// 4
    HitPoints HP;		// 2 * n
    DashParam dash;		// 1
    SendPos dash_gauge_pos;	// 4
    NonTypeSendObject site;	// 5
    Arrows arrows;		// 6 * n
  };

  /**
   * クライアントがゲーム中に発するイベント
   */
  namespace ClientEvent {
    enum {
      FlagArea	= 1,
      Press	= 1,
      None	= 0,
    };
    enum t {
      Up	=  1 << FlagArea,
      Down	=  2 << FlagArea,
      Left	=  3 << FlagArea,
      Right	=  4 << FlagArea,
      Button1	=  5 << FlagArea,
      Button2	=  6 << FlagArea,
      Button3	=  7 << FlagArea,
      Quit	=  8 << FlagArea,
    };
  }

  namespace PlayerType {
    enum t {
      Planex = 0,
      Yamaha = 1,
      NTT = 2,
    };
  }

  namespace Team {
    enum t {
      I,
      Ro,
      Ha,
      Ni,
      None,
      Dummy,
    };
  }

  namespace StageType {
    enum t {
      Forest,
      Future,
    };
  }

  ///< ロックオンサイトの状態
  namespace LockState {
    enum t {
      MoveNum = 8,
    };
  }

  ///< プレイヤーの初期化データ
  struct PlayerInitData 
  {
    PlayerType::t type;		///< キャラクターの種類
    Team::t team;		///< プレイヤーのチーム
    PlayerInitData() : type(PlayerType::Planex), team(Team::None) {}
  };

  ///<例外クラス
  class Zu3gaError : public std::runtime_error
  {
  public:
    Zu3gaError()
      : std::runtime_error("zu3ga: unknown error")
      , jamsg("よーわからんエラー") {}
    Zu3gaError(const std::string msg)
      : std::runtime_error("zu3ga error")
      , jamsg(msg) {}
    Zu3gaError(const std::string ja, const std::string eng)
      : std::runtime_error(std::string("zu3ga: ")+eng)
      , jamsg(ja) {}
    std::string JaMsg() const { return jamsg; }
    virtual ~Zu3gaError() throw() {}
  private:
    std::string jamsg;
  };

  class SyntaxError : public Zu3gaError
  {
  public:
    SyntaxError(const std::string& msg)
      : Zu3gaError("構文エラー","syntax error")
    {
      std::cerr << "syntax error: " << msg << std::endl;
    }
    SyntaxError(const std::string& msg0, const std::string& msg1)
      : Zu3gaError("構文エラー: "+msg1,"syntax error")
    {
      std::cerr << "syntax error: " << msg1 << ": " << msg0 << std::endl;
    }
  };

}

#endif // ZUGAGAGA_COMTYPES_HPP
