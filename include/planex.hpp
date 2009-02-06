// last modified <2004/02/07 20:46:27 2004 JST>
#ifndef ZUGAGAGA_PLANEX_HPP
#define ZUGAGAGA_PLANEX_HPP

#include <map>
#include "player.hpp"
#include "objects.hpp"

namespace zu3ga {
  class Planex : public Player
  {
  private:
    ///< Planexのパラメータ
    static const PlayerParam planex_param;

    // 絵を管理する配列
    std::vector<short> stand;
    std::vector<short> walk;
    std::vector<short> dash;
    std::vector<short> weapon;
    std::vector<std::vector<short> > kin;
    std::map<Direction::t, short> down;

    // ミサイルの出現位置
    std::vector<std::vector<DPos> > missile_pos;
    // エネルギー弾の出現位置
    std::vector<DPos> ene_pos;

    // private method
    void AddWeapon1();
    //    unsigned char GetSendObjState(); ///< 現在の状態(SendObjStateを返す
    void OneFrameProc();	///< １フレームの処理を書く
    SendPos GetShadowPos()
    {
      SendPos spos;
      spos.x = short(pos.x);
      spos.y = short(pos.y);
      spos.y += 5;
      return spos;
    }

    // 次に狙う敵の昔の方向
    double t_dir;
    bool stop;

  public:
    Planex(Team::t team, Objects*);
  };
}

#endif // ZUGAGAGA_PLANEX_HPP
