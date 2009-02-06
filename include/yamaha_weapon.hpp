// last modified <2004/02/09 00:06:24 2004 JST>
#ifndef ZUGAGAGA_YAMAHA_WEAPON_HPP
#define ZUGAGAGA_YAMAHA_WEAPON_HPP

#include <iostream>
#include <cmath>
#include "object.hpp"
#include "objects.hpp"
#include "shell.hpp"
#include "effect.hpp"

namespace zu3ga {
  /**
     Yamahaの機雷の爆発
   */
  class YamahaBombExplosion : public Object
  {
  private:
    static const SendObjType::t type = SendObjType::ExplosionAx2;
    ObjDetection own_obj_det;
    DPos pos;
    bool hit;			// プレイヤーに当たったか否か
    Team::t team;

  public:
    YamahaBombExplosion(const DPos& pos)
      : own_obj_det(ObjDetection(DetectionType::HardHit, 22, 22, 22, 22)), pos(pos)
      , hit(false)
      , team(Team::Dummy)
    {}
    void Update()
    {
      ++frame;
      if (frame >= 12) {
	erase = true;
      }
    }

    SendObject GetSendObject()
    {
      short state;
      if (frame <= 2) {
	state = 0;
      } else if (frame <= 4) {
	state = 1;
      } else if (frame <= 6) {
	state = 2;
      } else if (frame <= 8) {
	state = 3;
      } else if (frame <= 10) {
	state = 4;
      } else if (frame >= 11) {
	state = 5;
      }
      return SendObject(SendPos(short(pos.x), short(pos.y)), type, state);
    }

    bool GetDetection(Detection& det)
    {
      if (hit == false && frame > 0) {
	det = Detection(own_obj_det, Pos(short(pos.x), short(pos.y)),
			team, DetectParam(DetectParamType::HeavyAttack,
					  DPos(0, 0), 60));
	return true;
      }
      return false;
    }

    bool SetDetection(const Detection& det)
    {
      switch (det.type) {
      case DetectionType::Coll:
	hit = true;
	break;
      default:
	break;
      }

      return true;
    }
  };

  /**
     機雷本体
  */
  class YamahaBomb : public StraightShell
  {
  private:
    static const double diff_speed = 8;
    static const double vms = 100.0; // vertical_max_speed
    static const double vacc = 0.3; // vertical_acceleration

  public:
    YamahaBomb(double a_dir, Objects* objs, const DPos& pos, Team::t team, double diff_dir = 0)
      : StraightShell(ShellParam(ShellType::YamahaBomb,
				 0, // damage
				 26, // v0
				 1000, // v_max
				 0, // acc
				 0, // tracking
				 DetectParamType::LightAttack,
				 ObjDetection(DetectionType::Hit, 6, 6, 6, 6)),
		      a_dir, objs, pos, team)
    {
      ver_pos = -25;
      ver_speed = -1;

      DPos xy_speed;
      xy_speed = ObjectFunc::GetDirFacter(dir) * speed;
      // 広がりを考慮
      xy_speed += ObjectFunc::GetDirFacter(dir + diff_dir) * diff_speed;

      speed = hypot(xy_speed.x, xy_speed.y);
      dir = ObjectFunc::GetRadian(DPos(0, 0), xy_speed);
    }

    // 固有の処理
    // 速度を落とす,消える処理
    void OneFrameProc()
    {
      //---------------
      // 高さの変化
      //---------------
      // 基本位置より高い
      if (ver_pos < -25) {
	// 下に加速
	ver_speed += -gravity + vacc;
	if (ver_speed > vms) {
	  ver_speed = vms;
	}
      } 
      // 基本位置より低い
      else {
	// 上に加速
	ver_speed += -gravity - vacc;
	if (ver_speed < -vms) {
	  ver_speed = -vms;
	}
      }
      ver_pos += ver_speed;

      speed *= 0.88;
      if (speed < 5) {
	speed = 5;
      }
      if (frame > 120) {
	erase = true;
	objs->AddObject(new YamahaBombExplosion(pos));
      }
    }

    // 当たったときに出すオブジェクト
    bool GetHitObject(Object*& obj, const DPos& a_pos)
    {
      obj = new YamahaBombExplosion(a_pos);
      return true;
    }

    // チームの数だけ画像がある
    virtual unsigned short GetSendObjState(){ return short(team); }

    bool GetShadow(SendObject& so)
    {
      so.type = SendObjType::Shadow;
      so.pos = SendPos(short(pos.x), short(pos.y));
      so.state = 2;
      return true;
    }
  };

  /**
     山葉レーザー
  */
  class YamahaLaserShot : public Shell
  {
  private:
    static const int ef = 15;	// 攻撃判定が当たってから、消えるまでのフレーム数
    int erase_frame;

  public:
    YamahaLaserShot(double t_dir, Objects* objs, const DPos& pos, Player* target, Team::t team)
      : Shell(ShellParam(ShellType::YamahaLaserShot, // type
			 80,	// damage
			 26,	// v_0
			 26,	// v_max
			 0,	// acc
			 0,     // tracking
			 DetectParamType::HeavyAttack,
			 ObjDetection(DetectionType::HardHit, 12, 12, 12, 12)),
	      objs,
	      pos,
	      target,
	      team)
      , erase_frame(10000)
    { 
      dir = t_dir;
    }

    bool GetHitObject(Object*& obj, const DPos& a_pos)
    {
      erase = false;
      erase_frame = ef;
      // Warning消し^_^;
      if (obj) {
	if (a_pos.x) {
	  return false;
	}
      }
      return false;
    }


    bool GetDetection(Detection& det)
    {
      // 消えようとしている
      // 判定を消す
      if (erase_frame <= ef) {
	return false;
      }
      return Shell::GetDetection(det);
    }

    void OneFrameProc()
    {
      if (erase_frame <= 0) {
	erase = true;
      } 
      // 消える前の一時
      else if (erase_frame <= ef) {
	objs->AddObject(new DotEffect(pos, dir));
	speed *= 0.60;
      }
      --erase_frame;
    }
  };


  /**
     近距離攻撃当たり判定
   */
  class YamahaCloseRangeHitDetect : public UnmovingHitDetectObj
  {
  private:
    Direction::t slash_dir;

  public:
    YamahaCloseRangeHitDetect(Objects* objs, const DPos& pos,
			      Player* target, Direction::t body_dir, Team::t team, 
			      const DPos& mypos, bool down=false)
      : UnmovingHitDetectObj(DetectParamType::HeavyAttack,
			     110,
			     ObjDetection(),
			     objs, pos, target, team, 
			     ObjectFunc::GetRadian(mypos, target->GetPos()))
      , slash_dir(body_dir)
    {
      if (down) {
	param.det_type = DetectParamType::HeavyDownAttack;
      }
      param.own_obj_det = ObjDetection(DetectionType::HardHit,
				       18, 18, 18, 18);
    }
    void OneFrameProc()
    {
      if (frame >= 6) {
	erase = true;
      }
    }
    bool GetHitObject(Object*& obj, const DPos& pos)
    {
      obj = new SlashAA(pos, slash_dir);
      return true;
    }
  };
}

#endif // ZUGAGAGA_YAMAHA_WEAPON_HPP
