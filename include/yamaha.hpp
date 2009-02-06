// last modified <2004/02/07 20:47:11 2004 JST>
#ifndef ZUGAGAGA_YAMAHA_HPP
#define ZUGAGAGA_YAMAHA_HPP

#include <map>
#include "player.hpp"
#include "objects.hpp"

namespace zu3ga {
  class Yamaha : public Player
  {
  private:
    ///< Yamahaのパラメータ
    static const PlayerParam yamaha_param;

    // 絵の配列
    std::vector<short> stand;
    std::vector<short> ahead;
    std::vector<short> back;
    std::vector<std::vector<short> > kin;
    std::vector<std::vector<short> > laser;
    std::map<Direction::t, short> down;

    // 各オブジェクトの出現場所
    std::vector<std::vector<DPos> > bomb;
    std::vector<std::vector<DPos> > kin_pos;
    std::vector<std::vector<DPos> > laser_pos;

    // 爆弾の出る数
    static const unsigned int bomb_num = 8;
    // 次に爆弾を飛ばす方向
    double t_dir;
    // 後ろ→前と絵を変えるときに,立ちフレームを表示させるフレーム数
    int stand_frame;

    // private method
    void OneFrameProc();
    SendPos GetShadowPos();
    void AddBomb(double target_dir, bool second=false);

    bool stop;

  public:
    Yamaha(Team::t team, Objects*);
    unsigned char GetShadowState(){ return 1; }
  };
}

#endif // ZUGAGAGA_YAMAHA_HPP
