#ifndef ZUGAGAGA_OBJECT_HPP
#define ZUGAGAGA_OBJECT_HPP

#include <iostream>
#include "motion.hpp"
#include "detect.hpp"

namespace zu3ga {
  // 物体関係でよく使う関数郡
  // 基底クラスにstaticで持っても良いかも
  namespace ObjectFunc {
    Direction::t GetDirection(const DPos& src, const DPos& dest); ///< 二つの座標から方向を得る
    Direction::t GetDirection(double rad); ///< 角度から方向を得る
    Direction2::t GetDirection2(double rad); ///< 角度から方向２(１６方向)を得る
    Direction::t GetDirection4or6(const DPos& dir); ///< xyから左か右の向きを得る
    DPos GetDirFacter(const DPos& src, const DPos& dest); ///< 二つのPosから方向要素を得る
    DPos GetDirFacter(Direction::t); ///< 向きから方向の要素(三角比)を得る
    DPos GetDirFacter(const double rad); ///< 角度から方向の要素を得る
    DPos GetTriRatio(const DPos&); ///< XYの値から，その三角比を得る
    double GetRadian(const DPos& src, const DPos& dest); ///< srcからみたdestの角度を得る
    double GetRadian(const int dir); ///< Directionから角度を得る

    ///< srcをdestにmoveだけ近付ける(単位:radian)
    void DirAdjust(double& src, const double dest, const double move); 
    void DirAdjust(double& src, const DPos& pos, const DPos& dest, const double move);
    double GetDistance(const DPos&, const DPos&); ///< 二つの座標の距離を得る

    ///< アタリ判定を調べる．重なっているならtrue
    bool CheckCollDetect(const Detection&, const Detection&); 
    bool CheckHitDetect(const Detection&, const Detection&);
    ///< ステージとの当たり判定を調べる
    bool CheckStageDetect(const Detection& det, const Pos& end);
    ///< ステージの当たり判定を設定する
    void SetStageDetect(const Detection& det, const Pos& end, DPos& pos);
    ///< 当たり判定の重なっている長方形の中心の座標を得る
    DPos GetLapDetectCenter(const Detection& ldet, const Detection& rdet);
    ///< 直線(?)の重なりの中心を得る
    double GetLapCenter(const double& l1, const double& r1, 
			const double& l2, const double& r2);

    ///< dirをnだけ右回転させた方向を得る
    Direction::t GetRotRn(Direction::t dir, int n);
    ///< dirを一つだけ右回転させた方向を得る
    Direction::t GetRotR(Direction::t dir);
  }

  class Object
  {
  protected:
    static const double gravity = 1.5; ///< 重力
    bool erase;			///< trueならばこの物体は消えようとしている
    unsigned frame;

  public:
    Object() : erase(false), frame(0) { }
    bool Erase(){ return erase; } ///< この物体が消えようとしているかどうかを返す
    virtual ~Object(){}
    virtual void Update(){ ++frame; };
    virtual SendObject GetSendObject()
    {
      SendObject so;
      so.type = SendObjType::NoGraphic;
      return so;
    }
    virtual void CheckStageEnd(){}
    virtual bool GetDetection(Detection&){ return false; }
    virtual bool GetDetection(Detection*&) { return false; }
    virtual bool SetDetection(const Detection&){ return true; }
    virtual double GetAbsSpeed(){ return 0; }
    virtual bool GetDrawableDetect(Detection& det){ return GetDetection(det); }
    virtual bool GetShadow(SendObject&){ return false; }
  };

  ///< 当たり、ぶつかり判定のみ持った物体
  class CollObject : public Object
  {
  private:
    Detection det;

  public:
    CollObject(const Pos& start, const Pos& end, bool stage_obj=true)
      : det(stage_obj ? DetectionType::StageColl : DetectionType::Coll,
	    Team::Dummy, start.y, end.y, start.x, end.x)
    {}
    bool GetDetection(Detection*& a_det)
    {
      a_det = &det;
      return true;
    }
    bool GetDrawableDetect(Detection& det)
    {
      Detection* pdet;
      GetDetection(pdet);
      det = *pdet;
      return true;
    }
  };
}

#endif // ZUGAGAGA_OBJECT_HPP
