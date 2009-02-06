// last modified <2004/02/09 00:41:25 2004 JST>
#ifndef ZUGAGAGA_COLL_DETECT_HPP
#define ZUGAGAGA_COLL_DETECT_HPP

#include "edw/sdl.hpp"
#include "comtypes.hpp"

namespace zu3ga {
  namespace DetectionType {
    enum t {
      Coll,			///< ぶつかり判定 かつ やられ判定
      CollOnly,			///< ぶつかり判定
      StageColl,		///< ステージのオブジェクトのぶつかり,当たり判定
      Hit,			///< 当たり判定
      HardHit,			///< 硬い当たり判定．Hitを貫通する．自身や，Collとぶつかると消える
    };
  }

  /**
     当たり判定パラメータの種類
  */
  namespace DetectParamType {
    enum t {
      LightAttack,
      HeavyAttack,
      LightDownAttack,
      HeavyDownAttack,
    };
  }
  /**
     当たり判定のパラメータ構造体
     主に攻撃判定で使用される
  */
  struct DetectParam
  {
    DetectParamType::t type;	///< 種類
    DPos dir;			///< 方向(三角比)
    unsigned char point;	///< 数値．大抵はダメージかと．

    DetectParam(){}
    DetectParam(DetectParamType::t type, DPos dir, unsigned char point)
      : type(type), dir(dir), point(point) {}
  };

  /**
     物体が持つ当たり判定長方形構造体．
     自分の座標からどれだけ判定が広がっているか，という情報で持つ．
  */
  struct ObjDetection
  {
    DetectionType::t type;
    short high;
    short low;
    short left;
    short right;

    ObjDetection(){}
    ObjDetection(DetectionType::t type,
		 short h, short lo, short le, short r)
      : type(type), high(h), low(lo), left(le), right(r) {}
  };

  /**
     あたり判定長方形構造体
  */
  struct Detection
  {
    DetectionType::t type;
    Team::t team;
    short high;
    short low;
    short left;
    short right;
    DetectParam param;		

    Detection(){}
    Detection(DetectionType::t type,
	      Team::t team,
	      short high,
	      short low,
	      short left,
	      short right,
	      DetectParam param = DetectParam())
      : type(type)
      , team(team)
      , high(high)
      , low(low)
      , left(left)
      , right(right)
      , param(param) 
    {}
    Detection(const ObjDetection& obj_det,
	      const Pos& pos,
	      Team::t team,
	      DetectParam param = DetectParam())
      : type(obj_det.type)
      , team(team)
      , high(pos.y - obj_det.high)
      , low(pos.y + obj_det.low)
      , left(pos.x - obj_det.left)
      , right(pos.x + obj_det.right)
      , param(param) 
    {}

    void PrintData()
    {
      std::cout << "type = " << type << std::endl;
      std::cout << "team = " << team << std::endl;
      std::cout << "high = " << high << std::endl;
      std::cout << "low  = " << low << std::endl;
      std::cout << "left = " << left << std::endl;
      std::cout << "right= " << right << std::endl;
      //    DetectParam param;		
    }
  };

  /**
     描画するための当たり判定構造体
  */
  struct DrawableDetect
  {
    edw::sdl::sp_Graphic rect;
    Pos pos;

    DrawableDetect()
      : rect(edw::sdl::RectGraphic::Create(1, 1, 0, edw::sdl::Color(0x00, 0x00, 0x00)))
      , pos(-10, -10)
    {}

    DrawableDetect(const Detection& det)
      : pos(det.left, det.high)
    {
      edw::sdl::Color color;
      switch (det.type) {
      case DetectionType::Hit:
      case DetectionType::HardHit:
	color = edw::sdl::Color(0xff, 0x22, 0x22);
	break;
      case DetectionType::Coll:
      case DetectionType::StageColl:
	color = edw::sdl::Color(0x22, 0xff, 0xff);
	break;
      case DetectionType::CollOnly:
	color = edw::sdl::Color(0x22, 0xff, 0x22);
	break;
      }
      rect = edw::sdl::RectGraphic::Create(det.right - det.left, 
					   det.low - det.high, 
					   2, color, false);
    }
  };
}

#endif // ZUGAGAGA_COLL_DETECT_HPP
