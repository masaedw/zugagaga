// last modified <2004/02/08 06:43:34 2004 JST>
#ifndef ZUGAGAGA_NTT_WEAPON_HPP
#define ZUGAGAGA_NTT_WEAPON_HPP

#include <iostream>
#include "object.hpp"
#include "objects.hpp"
#include "shell.hpp"
#include "effect.hpp"
#include "ntt.hpp"

namespace zu3ga {
  /**
  */
  class NTTEnergyShot : public Shell
  {
  public:
    NTTEnergyShot(Objects* objs, const DPos& pos, Player* target, Team::t team)
      : Shell(ShellParam(ShellType::NTTEnergyShot, // type
			 20, // damage
			 18, // v0
			 1000, // v_max
			 0, // acc
			 0.019, // tracking
			 DetectParamType::LightAttack,
			 ObjDetection(DetectionType::Hit, 6, 6, 6, 6)),
	      objs,
	      pos,
	      target,
	      team)
    {}

    bool GetHitObject(Object*& obj, const DPos& pos)
    {
      obj = new ExplosionC(pos);
      return true;
    }
  };

  /**
     突進の当たり判定
   */
  class NTTAttackHitDetect : public UnmovingHitDetectObj
  {
  private:
    NTT* ntt;

  public:
    NTTAttackHitDetect(NTT* ntt, Objects* objs, const DPos& pos, Player* target, Team::t team)
      : UnmovingHitDetectObj(DetectParamType::LightDownAttack,
			     140, // damage
			     ObjDetection(DetectionType::Hit, 17, 17, 17, 17),
			     objs, pos, target, team)
      , ntt(ntt)
    {}

    void OneFrameProc()
    {
      if (frame >= 2) {
	erase = true;
      }
    }
    virtual bool SetDetection(const Detection& det)
    {
      switch (det.type) {
      case DetectionType::Coll:
	ntt->SetAttackEnd();
	break;
      default:
	break;
      }
      erase = true;
      return true;
    }
  };

  /**
     つららの当たり判定
   */
  class NTTKinHitDetect : public UnmovingHitDetectObj
  {
  public:
    NTTKinHitDetect(Objects* objs, const DPos& pos, Player* target, Team::t team, int size=0)
      : UnmovingHitDetectObj(DetectParamType::HeavyAttack,
			     60, // damage
			     ObjDetection(DetectionType::HardHit, 40, 40, 40, 40),
			     objs, pos, target, team)
    {
      if (size == 1) {
	param.own_obj_det = ObjDetection(DetectionType::HardHit, 20, 20, 20, 20);
      } else if (size == 2) {
	param.own_obj_det = ObjDetection(DetectionType::HardHit, 10, 10, 10, 10);
      }
    }
    void OneFrameProc()
    {
      if (frame >= 2) {
	erase = true;
      }
    }
    bool GetHitObject(Object*& obj, const DPos& dpos)
    {
      obj = new DotEffect(dpos, dir);
      return true;
    }
  };
}

#endif
