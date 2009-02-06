#ifndef ZUGAGAGA_EFFECT_HPP
#define ZUGAGAGA_EFFECT_HPP

#include <iostream>
#include <cstdlib>
#include "object.hpp"
#include "objects.hpp"

namespace zu3ga {
  /**
     小さい爆発
  */
  class ExplosionA : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::ExplosionA;
    DPos pos;

  public:
    ExplosionA(const DPos& pos) : pos(pos) {}
    void Update();
    SendObject GetSendObject();
  };

  /**
     ExplosionAの各辺2倍の爆発
  */
  class ExplosionAx2 : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::ExplosionAx2;
    DPos pos;

  public:
    ExplosionAx2(const DPos& pos) : pos(pos) {}
    void Update();
    SendObject GetSendObject();
  };

  /**
     ExplosionAの各辺4倍の爆発
  */
  class ExplosionAx4 : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::ExplosionAx4;
    DPos pos;

  public:
    ExplosionAx4(const DPos& pos) : pos(pos) {}
    void Update();
    SendObject GetSendObject();
  };

  //-------------------------
  // ミサイルの煙？
  //-------------------------
  class SmokeA : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::SmokeA;
    DPos pos;

  public:
    SmokeA(const DPos& pos) : pos(pos) {}
    SendObject GetSendObject();
  };

  //-------------------------
  // ズサーの煙
  //-------------------------
  class SmokeB : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::SmokeA;
    DPos pos;

  public:
    SmokeB(const DPos& pos) : pos(pos) {}
    SendObject GetSendObject();
    void Update()
    {
      ++frame;
      pos.y -= 2;
    }
  };

  //------------------------------
  // Planexのエネルギー弾の爆発
  //------------------------------
  class ExplosionB : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::ExplosionB;
    DPos pos;

  public:
    ExplosionB(const DPos& pos) : pos(pos) {}
    SendObject GetSendObject();
  };

  //------------------------------
  // NTTの攻撃の爆発
  //------------------------------
  class ExplosionC : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::ExplosionC;
    DPos pos;

  public:
    ExplosionC(const DPos& pos) : pos(pos) {}
    SendObject GetSendObject();
  };

  //------------------------------
  // Planexの近距離攻撃の切れすじ
  //------------------------------
  class SlashA : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::SlashA;
    DPos pos;
    Direction::t dir;

  public:
    SlashA(const DPos& pos, Direction::t dir);
    SendObject GetSendObject();
  };

  //--------------------------------------------------
  // Planexの近距離攻撃の切れすじと同じグラフィック
  // 向きと完全に対応する^_^;
  //--------------------------------------------------
  class SlashAA : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::SlashA;
    DPos pos;
    Direction::t dir;

  public:
    SlashAA(const DPos& pos, Direction::t dir);
    SendObject GetSendObject();
  };

  //------------------------------
  // 山葉レーザーのエフェクト
  //------------------------------
  class YamahaLaserEffect : public Object
  {
  private:
    Objects* objs;
    DPos pos;
    Direction2::t dir2;

  public:
    YamahaLaserEffect(Objects* objs, const DPos& pos, Direction2::t dir)
      : objs(objs), pos(pos), dir2(dir)
    {}

    SendObject GetSendObject()
    {
      if (frame == 3*2) {
	erase = true;
      }
      return SendObject(SendPos(short(pos.x), short(pos.y)),
			SendObjType::YamahaLaserEffect, dir2 + 16*(frame / 2));
    }
  };

  //----------
  // 残像
  //----------
  class Zanzou : public Object
  {
  private:
    SendObjType::t type;
    SendObjType::t draw_type;
    short state;
    DPos pos;

  public:
    Zanzou(SendObjType::t type, short state, const DPos& pos) 
      : type(type), draw_type(SendObjType::NoGraphic), state(state), pos(pos) {}

    SendObject GetSendObject()
    {
      using namespace SendObjType;
      /*
      if (frame == 2) {
	draw_type = type;
      } else if (frame == 3) {
	draw_type = NoGraphic;
      } else if (frame == 4) {
	draw_type = type;
      } else if (frame == 5) {
	draw_type = NoGraphic;
      } else if (frame == 6) {
	draw_type = type;
      } else if (frame == 7) {
	draw_type = NoGraphic;
	erase = true;
      }
      */
      if (frame == 1) {
	draw_type = type;
      } else if (frame == 5) {
	draw_type = NoGraphic;
	erase = true;
      }

      return SendObject(SendPos(short(pos.x), short(pos.y)), draw_type, state);
    }
  };

  //------------------------------
  // 無造作に飛び散る小さい点
  //------------------------------
  class DotEffect : public Object
  {
  private:
    static const double default_ver_pos = 20;
    DPos pos;
    DPos speed;
    double ver_pos;
    double ver_speed;

  public:
    DotEffect(const DPos& a_pos, double dir)
      : pos(a_pos)
    {
      ver_pos = -default_ver_pos;
      pos.y += default_ver_pos;
      dir += (rand() % 20 - 10) / 10;
      DPos fact = ObjectFunc::GetDirFacter(dir);
      double s = rand() % 15;
      speed = fact * s;
      ver_speed = -(rand() % 12) - 5;
    }

    void Update()
    {
      ver_speed += 1.5;
      ver_pos += ver_speed;
      if (ver_pos > 0) {
	ver_pos = 0;
	erase = true;
      }
      if (frame > 100) {
	ver_pos = 0;
	erase = true;
      }
      pos += speed;
    }

    SendObject GetSendObject()
    {
      return SendObject(SendPos(short(pos.x), short(pos.y + ver_pos)),
			SendObjType::DotEffect, 0);
    }
  };
}

#endif // ZUGAGAGA_EFFECT_HPP
