// last modified <2007/01/27 14:17:08 2007>
#include <stdexcept>
#include <cmath>
#include "object.hpp"
#include "motion.hpp"

using namespace std;
using namespace zu3ga;

const double zu3ga::Object::gravity;

// 自分から見たposの方向を返す
Direction::t ObjectFunc::GetDirection(const DPos& src, const DPos& dest)
{
  double rad = atan2(dest.y - src.y, dest.x - src.x);
  return ObjectFunc::GetDirection(rad);
}

// 角度から方向を得る
Direction::t ObjectFunc::GetDirection(double rad)
{
  Direction::t dir = Direction::None;

  // 計算の量を減らすためにπで割り，他の全ての角度からπを消す
  rad /= M_PI;
  // 条件分岐を減らすため，正負にわける
  // 正について
  if (rad > 0) {
    if (rad < 1/8.0) {
      dir = Direction::Dir6;
    }
    else if (rad < 3/8.0) {
      dir = Direction::Dir3;
    }
    else if (rad < 5/8.0) {
      dir = Direction::Dir2;
    }
    else if (rad < 7/8.0) {
      dir = Direction::Dir1;
    } 
    else {
      dir = Direction::Dir4;
    }
  }
  // 負について
  else {
    if (rad > -1/8.0) {
      dir = Direction::Dir6;
    }
    else if (rad > -3/8.0) {
      dir = Direction::Dir9;
    }
    else if (rad > -5/8.0) {
      dir = Direction::Dir8;
    }
    else if (rad > -7/8.0) {
      dir = Direction::Dir7;
    }
    else {
      dir = Direction::Dir4;
    }
  }
  
  return dir;
}

// 角度から方向２(１６方向)を得る
Direction2::t ObjectFunc::GetDirection2(double rad)
{
  Direction2::t dir;

  // 計算の量を減らすためにπで割り，他の全ての角度からπを消す
  rad /= M_PI;
  // 条件分岐を減らすため，正負にわける
  // 正について
  if (rad > 0) {
    if (rad < 1/16.0) {
      dir = Direction2::Dir6;
    }
    else if (rad < 3/16.0) {
      dir = Direction2::Dir36;
    }
    else if (rad < 5/16.0) {
      dir = Direction2::Dir3;
    }
    else if (rad < 7/16.0) {
      dir = Direction2::Dir23;
    } 
    else if (rad < 9/16.0) {
      dir = Direction2::Dir2;
    }
    else if (rad < 11/16.0) {
      dir = Direction2::Dir12;
    }
    else if (rad < 13/16.0) {
      dir = Direction2::Dir1;
    }
    else if (rad < 15/16.0) {
      dir = Direction2::Dir14;
    }
    else {
      dir = Direction2::Dir4;
    }
  }
  // 負について
  else {
    if (rad > -1/16.0) {
      dir = Direction2::Dir6;
    }
    else if (rad > -3/16.0) {
      dir = Direction2::Dir69;
    }
    else if (rad > -5/16.0) {
      dir = Direction2::Dir9;
    }
    else if (rad > -7/16.0) {
      dir = Direction2::Dir89;
    } 
    else if (rad > -9/16.0) {
      dir = Direction2::Dir8;
    }
    else if (rad > -11/16.0) {
      dir = Direction2::Dir78;
    }
    else if (rad > -13/16.0) {
      dir = Direction2::Dir7;
    }
    else if (rad > -15/16.0) {
      dir = Direction2::Dir47;
    }
    else {
      dir = Direction2::Dir4;
    }
  }
  
  return dir;
}

Direction::t ObjectFunc::GetDirection4or6(const DPos& dir)
{
  double rad = GetRadian(DPos(0, 0), dir);
  rad /= M_PI;
  if (rad > 0.5 || rad < -0.5) {
    return Direction::Dir4;
  }
  else {
    return Direction::Dir6;
  }
}

//============================================================
// ObjectFunc::GetDirFacter
// 向きから方向の係数を得る
// 例
// (1, 0) とか (-0.7, 0.7) とか
//============================================================
DPos ObjectFunc::GetDirFacter(Direction::t dir)
{
  // 1/sqrt(2) = 0.7

  switch (dir) {
  case Direction::Dir1:
    return DPos(-0.7, 0.7);
  case Direction::Dir2:
    return DPos(0, 1.0);
  case Direction::Dir3:
    return DPos(0.7, 0.7);
  case Direction::Dir4:
    return DPos(-1.0, 0);
  case Direction::Dir6:
    return DPos(1.0, 0);
  case Direction::Dir7:
    return DPos(-0.7, -0.7);
  case Direction::Dir8:
    return DPos(0, -1.0);
  case Direction::Dir9:
    return DPos(0.7, -0.7);
  case Direction::None:
    return DPos(0, 0);
  default:
    throw logic_error("GetDirFacter: 方向要素の計算ができません．引数が不正です．");
  }
}

DPos ObjectFunc::GetDirFacter(const DPos& src, const DPos& dest)
{
  double x = dest.x - src.x;
  double y = dest.y - src.y;
  double h = hypot(x, y);

  return DPos(x / h, y / h);
}

DPos ObjectFunc::GetDirFacter(const double rad)
{
  return DPos(cos(rad), sin(rad));
}

DPos ObjectFunc::GetTriRatio(const DPos& arg)
{
  double h = hypot(arg.x, arg.y);
  return DPos(arg.x / h, arg.y / h);
}

// srcからみたdestの角度を得る
double ObjectFunc::GetRadian(const DPos& src, const DPos& dest)
{
  return atan2(dest.y - src.y, dest.x - src.x);
}

// srcをdestに(最大で)moveだけ近付ける
void ObjectFunc::DirAdjust(double& src, const double dest, const double move)
{
  if (src == dest) {
    return;
  }

  double diff = dest - src;
  // 差分の方がmoveよりも大きい
  if (abs(diff) > abs(move)) {
    if (diff > 0) {
      src += move;
    }
    else {
      src -= move;
    }
  }
  // 差分よりもmoveのほうが大きい
  else {
    src += diff;
  }
}

void ObjectFunc::DirAdjust(double& src, const DPos& pos, const DPos& dest, const double move)
{
  DirAdjust(src, GetRadian(pos, dest), move);
}

// 二つの座標の距離を得る
double ObjectFunc::GetDistance(const DPos& lhs, const DPos& rhs)
{
  return hypot(lhs.x - rhs.x, lhs.y - rhs.y);
}

// ぶつかり当たり判定を行う
// チームの影響を受けない
bool ObjectFunc::CheckCollDetect(const Detection& ldet, const Detection& rdet)
{
  if (ldet.high < rdet.low) {
    if (ldet.low > rdet.high) {
      if (ldet.left < rdet.right) {
        if (ldet.right > rdet.left) {
          return true;
        }
      }
    }
  }
  return false;
}

// 攻撃当たり判定を行う
// チームの影響を受ける
bool ObjectFunc::CheckHitDetect(const Detection& ldet, const Detection& rdet)
{
  if (ldet.team == rdet.team) {
    return false;
  }
  return CheckCollDetect(ldet, rdet);
}

bool ObjectFunc::CheckStageDetect(const Detection& det, const Pos& end)
{
  if (det.high > end.y || det.low < 0 || det.left > end.x || det.right < 0) {
    return true;
  }
  return false;
}

void ObjectFunc::SetStageDetect(const Detection& det, const Pos& end, DPos& pos)
{
  // 水平の当たり判定
  if (det.left < 0) {
    pos.x -= det.left;
  } 
  else if (det.right > end.x) {
    pos.x -= det.right - end.x;
  }

  // 垂直の当たり判定
  if (det.high < 0) {
    pos.y -= det.high;
  }
  else if (det.low > end.y) {
    pos.y -= det.low - end.y;
  }
}

double ObjectFunc::GetRadian(int dir)
{
  double rad = M_PI;

  switch (dir) {
  case Direction::Dir1:
    rad *= 3/4.0;
    break;
  case Direction::Dir2:
    rad *= 1/2.0;
    break;
  case Direction::Dir3:
    rad *= 1/4.0;
    break;
  case Direction::Dir4:
    break;
  case Direction::Dir6:
    rad = 0;
    break;
  case Direction::Dir7:
    rad *= -3/4.0;
    break;
  case Direction::Dir8:
    rad *= -1/2.0;
    break;
  case Direction::Dir9:
    rad *= -1/4.0;
    break;
  default:
    break;
  }

  return rad;
}

// 重なりの中心を得る(1次元)
double ObjectFunc::GetLapCenter(const double& l1, const double& r1, 
				const double& l2, const double& r2)
{
  if (l1 > l2) {
    if (r1 < r2) {
      // 1が2に含まれている
      return (l1 + r1) / 2;
    } else {
      // 乗っているだけ
      return (l1 + r2) / 2;
    }
  } 
  // l1 < l2
  else {
    if (r2 < r1) {
      // 2が1に含まれている
      return (l2 + r2) / 2;
    } else {
      // 乗っているだけ
      return (l2 + r1) / 2;
    }
  }
}

DPos ObjectFunc::GetLapDetectCenter(const Detection& ldet, const Detection& rdet)
{
  DPos ret;

  // 水平方向について重なりの中心を得る
  ret.x = GetLapCenter(ldet.left, ldet.right, rdet.left, rdet.right);
  // 垂直方向について重なりの中心を得る
  ret.y = GetLapCenter(ldet.high, ldet.low, rdet.high, rdet.low);

  return ret;
}

Direction::t ObjectFunc::GetRotRn(Direction::t dir, int n)
{
  while (n--) {
    dir = ObjectFunc::GetRotR(dir);
  }
  return dir;
}

Direction::t ObjectFunc::GetRotR(Direction::t dir)
{
  using namespace Direction;

  switch (dir) {
  case Dir1:
    return Dir2;
  case Dir2:
    return Dir3;
  case Dir3:
    return Dir6;
  case Dir6:
    return Dir9;
  case Dir9:
    return Dir8;
  case Dir8:
    return Dir7;
  case Dir7:
    return Dir4;
  case Dir4:
    return Dir1;
  default:
    return None;
  }
}
