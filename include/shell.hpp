// last modified <2004/02/08 03:44:21 2004 JST>
#ifndef ZUGAGAGA_SHELL_HPP
#define ZUGAGAGA_SHELL_HPP

#include <iostream>
#include "object.hpp"
#include "objects.hpp"

namespace zu3ga {
  namespace ShellType {
    enum t {
      PlanexMissile = SendObjType::PlanexMissile,
      PlanexEnergyShot = SendObjType::PlanexEnergyShot,
      YamahaBomb = SendObjType::YamahaBomb,
      YamahaLaserShot = SendObjType::YamahaLaserShot,
      NTTEnergyShot = SendObjType::NTTEnergyShot,
      NoGraphic = SendObjType::NoGraphic,
    };
  }

  /**
     弾のパラメータ構造体
     継承先で値が決まる
  */
  struct ShellParam
  {
    ShellType::t type;		// SendObjType
    unsigned char damage;
    double v0;
    double v_max;
    double acc;
    double tracking;
    DetectParamType::t det_type;
    ObjDetection own_obj_det;

    ShellParam(const ShellType::t type, const unsigned char damage, 
	       const double v0, const double v_max, const double acc, const double tracking,
	       const DetectParamType::t det_type,
	       const ObjDetection& own_obj_det)
      : type(type)
      , damage(damage)
      , v0(v0)
      , v_max(v_max)
      , acc(acc)
      , tracking(tracking)
      , det_type(det_type)
      , own_obj_det(own_obj_det)
    {}
  };

  ///< 弾の基底クラス
  class Shell : public Object
  {
  protected:
    double ver_pos;
    double ver_speed;

    ShellParam param;

    Player* target;
    Objects* objs;
    Team::t team;
    DPos pos;
    double speed;
    double dir;

    DetectParamType::t GetDetectParamType();
    void AddHitObject(const Detection& det);
    virtual bool GetHitObject(Object*&, const DPos&)
    {
      return false;
    }
    virtual bool SetDetection(const Detection& det);

  public:
    Shell(const ShellParam& param, Objects* objs, const DPos& pos, 
	  Player* target, Team::t team);
    virtual void Update();
    SendObject GetSendObject();
    double GetAbsSpeed(){ return speed; }
    void CheckStageEnd();

    virtual bool GetDetection(Detection&);
    virtual Detection GetDetection();

    ///< 1フレームに1度呼ばれる関数．継承先で特殊な処理をするとき使う
    virtual void OneFrameProc(){}
    ///< SendObjStateを得る．弾は16方向あることを前提とする．
    virtual unsigned short GetSendObjState()
    {
      return ObjectFunc::GetDirection2(dir);
    }
  };

  ///< 動かない,当たり判定だけ持っている物体
  class UnmovingHitDetectObj : public Shell
  {
  public:
    UnmovingHitDetectObj(const DetectParamType::t det_param_type,
			 unsigned char damage,
			 const ObjDetection& own_obj_det,
			 Objects* objs, const DPos& pos, Player* target, Team::t team, 
			 double a_dir=0)
      : Shell(ShellParam(ShellType::NoGraphic, // SendObjType
			 damage,
			 0.0, 0.0, 0.0, 0.0, //速度に関する項目
			 det_param_type, // DetectParamType
			 own_obj_det),
	      objs, pos, target, team)
    { 
      if (a_dir) {
	dir = a_dir;
      }
    }
  };

  ///< まっすぐ飛ぶだけの弾
  class StraightShell : public Shell
  {
  public:
    StraightShell(const ShellParam& param,
		  double dir,
		  Objects* objs, const DPos& pos, Team::t team);
    void Update();
  };
}

#endif // ZUGAGAGA_SHELL_HPP
