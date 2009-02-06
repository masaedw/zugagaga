// last modified <2004/02/08 14:52:24 2004 JST>
#ifndef ZUGAGAGA_NTT_HPP
#define ZUGAGAGA_NTT_HPP

#include <map>
#include "player.hpp"
#include "objects.hpp"

namespace zu3ga {
  class NTT : public Player
  {
  private:
    ///< Nttのパラメータ
    static const PlayerParam ntt_param;
    ///< 突進の速度
    static const double attack_speed = 21;

    // 絵の配列
    std::vector<short> stand;
    std::vector<short> dash;
    std::vector<short> weapon;
    std::vector<short> attack;
    std::vector<std::vector<short> > kin;
    std::map<Direction::t, short> down;

    // 攻撃のでる場所
    std::vector<DPos> w1_pos;
    std::vector<DPos> at_pos;
    std::vector<std::vector<DPos> > kin_pos;

    int attack_end;
    bool stop;

    // private method
    void OneFrameProc();

  public:
    NTT(Team::t team, Objects*);
    unsigned char GetShadowState(){ return 1; }
    virtual bool SetDetection(const Detection&);
    void SetAttackEnd();
  };
}

#endif // ZUGAGAGA_NTT_HPP
