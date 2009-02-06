// last modified <2004/02/08 17:20:15 2004 JST>
#ifndef ZUGAGAGA_PLANEX_WEAPON_HPP
#define ZUGAGAGA_PLANEX_WEAPON_HPP

#include <iostream>
#include "object.hpp"
#include "objects.hpp"
#include "shell.hpp"
#include "effect.hpp"

namespace zu3ga {
  /** 
  */
  class PlanexMissile : public Shell 
  {
  public:
    PlanexMissile(double dir_diff, Objects* objs,
		  const DPos& pos, Player* target, Team::t team)
      : Shell(ShellParam(ShellType::PlanexMissile, // type
			 28,	// damage
			 1.7,	// v0
			 24.0,	// v_max
			 1.7,	// acc
			 0.030, // tracking
			 DetectParamType::LightAttack,
			 ObjDetection(DetectionType::Hit, 8, 8, 8, 8)),
	      objs,
	      pos,
	      target,
	      team)
    {
      dir += dir_diff;
    }

    bool GetHitObject(Object*& obj, const DPos& pos)
    {
      obj = new ExplosionA(pos);
      return true;
    }

    void OneFrameProc()
    {
      /*
      if (frame % 2 == 0) {
	objs->AddObject(new SmokeA(pos));
      }
      */
    }
  };

  /**
  */
  class PlanexEnergyShot : public Shell
  {
  public:
    PlanexEnergyShot(Objects* objs, const DPos& pos, Player* target, Team::t team, double a_dir = 0)
      : Shell(ShellParam(ShellType::PlanexEnergyShot, // type
			 120, // damage
			 24, // v0
			 24, // v_max
			 0, // acc
			 0.022, // tracking
			 DetectParamType::LightDownAttack,
			 ObjDetection(DetectionType::HardHit, 15, 15, 15, 15)),
	      objs,
	      pos,
	      target,
	      team)
    {
      dir = a_dir;
    }

    bool GetHitObject(Object*& obj, const DPos& pos)
    {
      obj = new ExplosionB(pos);
      return true;
    }
  };

  /**
   */
  class PlanexCloseRangeHitDetect : public UnmovingHitDetectObj
  {
  private:
    Direction::t dir;

  public:
    PlanexCloseRangeHitDetect(Objects* objs, const DPos& pos, 
			      Player* target, Direction::t body_dir, Team::t team)
      : UnmovingHitDetectObj(DetectParamType::LightDownAttack,
			     160, // damage
			     ObjDetection(),
			     objs, pos, target, team)
      , dir(body_dir)
    {
      int d = 2;
      switch (dir) {
      case Direction::Dir1:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 20, 32+d, 37+d, 10);
	break;
      case Direction::Dir2:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 20, 31+d, 31+d, 15);
	break;
      case Direction::Dir3:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 20, 32+d, 10, 37+d);
	break;
      case Direction::Dir4:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 37+d, 10, 46+d, 10);
	break;
      case Direction::Dir6:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 37+d, 10, 10, 46+d);
	break;
      case Direction::Dir7:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 47+d, 5, 32, 15+d);
	break;
      case Direction::Dir8:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 54+d, 5, 20, 31+d);
	break;
      case Direction::Dir9:
	param.own_obj_det = ObjDetection(DetectionType::HardHit,
					 47+d, 5, 15, 32+d);
	break;
      default:
	break;
      }
    }

    void OneFrameProc()
    {
      if (frame >= 6) {
	erase = true;
      }
    }

    bool GetHitObject(Object*& obj, const DPos& pos)
    {
      obj = new SlashA(pos, dir);
      return true;
    }
  };
}

#endif
