// last modified <2004/02/02 14:42:57 2004 JST>
#ifndef ZUGAGAGA_MOTION_HPP
#define ZUGAGAGA_MOTION_HPP

#include <vector>

namespace zu3ga {
  /**
     方向を表す定数
  */
  namespace Direction {
    enum t {
      Dir1 = 0,
      Dir2,
      Dir3,
      Dir4,
      Dir6,
      Dir7,
      Dir8,
      Dir9,
      None,
      End,
    };
  }

  /**
     方向を表す定数２(16方向)
  */
  namespace Direction2 {
    enum t {
      Dir1 = 0,
      Dir12,
      Dir2,
      Dir23,
      Dir3,
      Dir36,
      Dir6,
      Dir69,
      Dir9,
      Dir89,
      Dir8,
      Dir78,
      Dir7,
      Dir47,
      Dir4,
      Dir14,
    };
  }

  /**
     プレイヤーの現在の状態．
     状態と移動している方向を持つ．
     コマンドもこれを吐き出す．
   */
  struct PlayerMotion {
    enum Type {
      None,			///< 歩きと立ちをかねる
      Dash,
      DashSpatia,
      SearchEnemy,
      Weapon1,
      Weapon2,
      DashWeapon1,
      DashWeapon2,
      CloseRange,
      DashCloseRange,
      StaggerDamage,
      FlyDamage,
      Bound,
      Down,
      Explosion,
    };

    Type type;			///< 現在の状態
    Direction::t dir;		///< 移動している方向

    PlayerMotion() : type(None), dir(Direction::None) {}    // コンストラクタ
    ///< モーションが攻撃タイプであるか否か
    bool WeaponType() const
    {
      switch (type) {
      case Weapon1:
      case Weapon2:
      case DashWeapon1:
      case DashWeapon2:
      case CloseRange:
	return true;
      default:
	return false;
      }
    }
  };
}

#endif // ZUGAGAGA_MOTION_HPP
