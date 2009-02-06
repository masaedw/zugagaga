#include <string>
#include <iostream>
#include "draw.hpp"
#include "graphic.hpp"
#include "utility.hpp"

using namespace std;
using namespace zu3ga;
using namespace graphic;
using namespace edw::sdl;
using graphic::ScreenWidth;
using graphic::ScreenHeight;

//============================================================
// DrawingManager::DrawingManager
// 絵を扱う２次元配列の初期化を行う
//============================================================
DrawingManager::DrawingManager(const DMInitData& data)
  : screen(GetVideoSurface()), frame(0), elap_draw(false), time_font(GetFont(FontType::Tag100))
{
  // ステージ
  stg_type = data.stg_type;
  switch (stg_type) {
  case StageType::Forest:
    stg_back = GetGraphic(Type::ForestBack);
    break;
  case StageType::Future:
    stg_back = GetGraphic(Type::FutureBack);
    break;
  }

  client_num = 0;
  // 御札
  for (unsigned int i = 0; i < 4; ++i) {
    if (i == 0) {
      tags.push_back(GetStatusTagGraphic(data.tags[i], true));
    } else {
      tags.push_back(GetStatusTagGraphic(data.tags[i]));
    }
    if (data.tags[i].id != StatusTagID::NoUse) {
      client_num++;
    }
  }
  int by = 90;
  int bh = tags[0]->GetWH().y;
  int sy = by + bh + 12;
  int sw = 40;
  // 御札の描画位置
  tag_pos[0] = Pos(ScreenWidth + 15, by);
  tag_pos[1] = Pos(ScreenWidth + 5, sy);
  tag_pos[2] = Pos(ScreenWidth + 10 + sw, sy);
  tag_pos[3] = Pos(ScreenWidth + 15 + sw*2, sy);
  // 御札に重ねる黒い画像．使ってないけど
  black_big = GetGraphic(Type::StatusTagBlackBig);
  black_small = GetGraphic(Type::StatusTagBlackSmall);
  // ステータスの背景
  status_back = GetGraphic(Type::BattleStatusBack);
  // 焦げと炎
  burnt_big = GetGraphic(Type::BurntBig);
  flame_big.push_back(GetGraphic(Type::FlameBig0));
  flame_big.push_back(GetGraphic(Type::FlameBig1));
  flame_big.push_back(GetGraphic(Type::FlameBig2));
  flame_big.push_back(GetGraphic(Type::FlameBig3));
  flame_big.push_back(GetGraphic(Type::FlameBig4));
  flame_big.push_back(GetGraphic(Type::FlameBig5));
  burnt_small = GetGraphic(Type::BurntSmall);
  flame_small.push_back(GetGraphic(Type::FlameSmall0));
  flame_small.push_back(GetGraphic(Type::FlameSmall1));
  flame_small.push_back(GetGraphic(Type::FlameSmall2));
  flame_small.push_back(GetGraphic(Type::FlameSmall3));
  flame_small.push_back(GetGraphic(Type::FlameSmall4));
  flame_small.push_back(GetGraphic(Type::FlameSmall5));
  // 経過時間
  elapse = GetGraphic(Type::Elapse);

  // 各HPバッファの初期化
  for (unsigned int i = 0; i < 4; ++i) {
    hp_buf.push_back(1000);
    pre_hp.push_back(1000);
    dec_frame_stack.push_back(0);
  }

  GAry ary;
  // SendObjType::PlanexI
  ary.push_back( GetGraphic( Type::CharaAStand1I ) );
  ary.push_back( GetGraphic( Type::CharaAStand2I ) );
  ary.push_back( GetGraphic( Type::CharaAStand3I ) );
  ary.push_back( GetGraphic( Type::CharaAStand4I ) );
  ary.push_back( GetGraphic( Type::CharaAStand6I ) );
  ary.push_back( GetGraphic( Type::CharaAStand7I ) );
  ary.push_back( GetGraphic( Type::CharaAStand8I ) );
  ary.push_back( GetGraphic( Type::CharaAStand9I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk1I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk2I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk3I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk4I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk6I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk7I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk8I ) );
  ary.push_back( GetGraphic( Type::CharaAWalk9I ) );
  ary.push_back( GetGraphic( Type::CharaADash1I ) );
  ary.push_back( GetGraphic( Type::CharaADash2I ) );
  ary.push_back( GetGraphic( Type::CharaADash3I ) );
  ary.push_back( GetGraphic( Type::CharaADash4I ) );
  ary.push_back( GetGraphic( Type::CharaADash6I ) );
  ary.push_back( GetGraphic( Type::CharaADash7I ) );
  ary.push_back( GetGraphic( Type::CharaADash8I ) );
  ary.push_back( GetGraphic( Type::CharaADash9I ) );
  ary.push_back( GetGraphic( Type::CharaAWeapon1I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon2I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon3I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon4I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon6I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon7I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon8I ));
  ary.push_back( GetGraphic( Type::CharaAWeapon9I ));
  ary.push_back( GetGraphic( Type::CharaADown1I ));
  ary.push_back( GetGraphic( Type::CharaADown2I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin1_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin2_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin3_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin4_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin6_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin7_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin8_8I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_1I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_2I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_3I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_4I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_5I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_6I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_7I ));
  ary.push_back( GetGraphic( Type::CharaAKin9_8I ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::PlanexRo
  ary.push_back( GetGraphic( Type::CharaAStand1Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand2Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand3Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand4Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand6Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand7Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand8Ro ) );
  ary.push_back( GetGraphic( Type::CharaAStand9Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk1Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk2Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk3Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk4Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk6Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk7Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk8Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWalk9Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash1Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash2Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash3Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash4Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash6Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash7Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash8Ro ) );
  ary.push_back( GetGraphic( Type::CharaADash9Ro ) );
  ary.push_back( GetGraphic( Type::CharaAWeapon1Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon2Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon3Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon4Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon6Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon7Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon8Ro ));
  ary.push_back( GetGraphic( Type::CharaAWeapon9Ro ));
  ary.push_back( GetGraphic( Type::CharaADown1Ro ));
  ary.push_back( GetGraphic( Type::CharaADown2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin1_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin2_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin3_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin4_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin6_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin7_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin8_8Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_1Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_2Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_3Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_4Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_5Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_6Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_7Ro ));
  ary.push_back( GetGraphic( Type::CharaAKin9_8Ro ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::PlanexHa
  ary.push_back( GetGraphic( Type::CharaAStand1Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand2Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand3Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand4Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand6Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand7Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand8Ha ) );
  ary.push_back( GetGraphic( Type::CharaAStand9Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk1Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk2Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk3Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk4Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk6Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk7Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk8Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWalk9Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash1Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash2Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash3Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash4Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash6Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash7Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash8Ha ) );
  ary.push_back( GetGraphic( Type::CharaADash9Ha ) );
  ary.push_back( GetGraphic( Type::CharaAWeapon1Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon2Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon3Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon4Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon6Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon7Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon8Ha ));
  ary.push_back( GetGraphic( Type::CharaAWeapon9Ha ));
  ary.push_back( GetGraphic( Type::CharaADown1Ha ));
  ary.push_back( GetGraphic( Type::CharaADown2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin1_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin2_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin3_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin4_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin6_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin7_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin8_8Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_1Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_2Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_3Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_4Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_5Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_6Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_7Ha ));
  ary.push_back( GetGraphic( Type::CharaAKin9_8Ha ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::PlanexNi
  ary.push_back( GetGraphic( Type::CharaAStand1Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand2Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand3Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand4Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand6Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand7Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand8Ni ) );
  ary.push_back( GetGraphic( Type::CharaAStand9Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk1Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk2Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk3Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk4Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk6Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk7Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk8Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWalk9Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash1Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash2Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash3Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash4Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash6Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash7Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash8Ni ) );
  ary.push_back( GetGraphic( Type::CharaADash9Ni ) );
  ary.push_back( GetGraphic( Type::CharaAWeapon1Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon2Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon3Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon4Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon6Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon7Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon8Ni ));
  ary.push_back( GetGraphic( Type::CharaAWeapon9Ni ));
  ary.push_back( GetGraphic( Type::CharaADown1Ni ));
  ary.push_back( GetGraphic( Type::CharaADown2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin1_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin2_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin3_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin4_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin6_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin7_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin8_8Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_1Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_2Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_3Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_4Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_5Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_6Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_7Ni ));
  ary.push_back( GetGraphic( Type::CharaAKin9_8Ni ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaI
  ary.push_back( GetGraphic( Type::YamahaStand1I ));
  ary.push_back( GetGraphic( Type::YamahaStand2I ));
  ary.push_back( GetGraphic( Type::YamahaStand3I ));
  ary.push_back( GetGraphic( Type::YamahaStand4I ));
  ary.push_back( GetGraphic( Type::YamahaStand6I ));
  ary.push_back( GetGraphic( Type::YamahaStand7I ));
  ary.push_back( GetGraphic( Type::YamahaStand8I ));
  ary.push_back( GetGraphic( Type::YamahaStand9I ));
  ary.push_back( GetGraphic( Type::YamahaAhead1I ));
  ary.push_back( GetGraphic( Type::YamahaAhead2I ));
  ary.push_back( GetGraphic( Type::YamahaAhead3I ));
  ary.push_back( GetGraphic( Type::YamahaAhead4I ));
  ary.push_back( GetGraphic( Type::YamahaAhead6I ));
  ary.push_back( GetGraphic( Type::YamahaAhead7I ));
  ary.push_back( GetGraphic( Type::YamahaAhead8I ));
  ary.push_back( GetGraphic( Type::YamahaAhead9I ));
  ary.push_back( GetGraphic( Type::YamahaBack1I ));
  ary.push_back( GetGraphic( Type::YamahaBack2I ));
  ary.push_back( GetGraphic( Type::YamahaBack3I ));
  ary.push_back( GetGraphic( Type::YamahaBack4I ));
  ary.push_back( GetGraphic( Type::YamahaBack6I ));
  ary.push_back( GetGraphic( Type::YamahaBack7I ));
  ary.push_back( GetGraphic( Type::YamahaBack8I ));
  ary.push_back( GetGraphic( Type::YamahaBack9I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT1I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT2I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT3I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT4I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT6I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT7I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT8I ));
  ary.push_back( GetGraphic( Type::YamahaAheadT9I ));
  ary.push_back( GetGraphic( Type::YamahaBackT1I ));
  ary.push_back( GetGraphic( Type::YamahaBackT2I ));
  ary.push_back( GetGraphic( Type::YamahaBackT3I ));
  ary.push_back( GetGraphic( Type::YamahaBackT4I ));
  ary.push_back( GetGraphic( Type::YamahaBackT6I ));
  ary.push_back( GetGraphic( Type::YamahaBackT7I ));
  ary.push_back( GetGraphic( Type::YamahaBackT8I ));
  ary.push_back( GetGraphic( Type::YamahaBackT9I ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_4I ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_1I ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_2I ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_3I ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_4I ));
  ary.push_back( GetGraphic( Type::YamahaDown4I ));
  ary.push_back( GetGraphic( Type::YamahaDown6I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin1_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin2_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin3_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin4_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin6_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin7_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin8_13I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_1I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_2I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_3I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_4I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_5I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_6I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_7I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_8I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_9I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_10I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_11I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_12I ));
  ary.push_back( GetGraphic( Type::YamahaKin9_13I ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaRo
  ary.push_back( GetGraphic( Type::YamahaStand1Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand2Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand3Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand4Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand6Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand7Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand8Ro ));
  ary.push_back( GetGraphic( Type::YamahaStand9Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead1Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead2Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead3Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead4Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead6Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead7Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead8Ro ));
  ary.push_back( GetGraphic( Type::YamahaAhead9Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack1Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack2Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack3Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack4Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack6Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack7Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack8Ro ));
  ary.push_back( GetGraphic( Type::YamahaBack9Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT1Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT2Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT3Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT4Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT6Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT7Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT8Ro ));
  ary.push_back( GetGraphic( Type::YamahaAheadT9Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT1Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT2Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT3Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT4Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT6Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT7Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT8Ro ));
  ary.push_back( GetGraphic( Type::YamahaBackT9Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaDown4Ro ));
  ary.push_back( GetGraphic( Type::YamahaDown6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin1_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin2_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin3_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin4_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin6_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin7_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin8_13Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_1Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_2Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_3Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_4Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_5Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_6Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_7Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_8Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_9Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_10Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_11Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_12Ro ));
  ary.push_back( GetGraphic( Type::YamahaKin9_13Ro ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaHa
  ary.push_back( GetGraphic( Type::YamahaStand1Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand2Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand3Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand4Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand6Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand7Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand8Ha ));
  ary.push_back( GetGraphic( Type::YamahaStand9Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead1Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead2Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead3Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead4Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead6Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead7Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead8Ha ));
  ary.push_back( GetGraphic( Type::YamahaAhead9Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack1Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack2Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack3Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack4Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack6Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack7Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack8Ha ));
  ary.push_back( GetGraphic( Type::YamahaBack9Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT1Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT2Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT3Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT4Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT6Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT7Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT8Ha ));
  ary.push_back( GetGraphic( Type::YamahaAheadT9Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT1Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT2Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT3Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT4Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT6Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT7Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT8Ha ));
  ary.push_back( GetGraphic( Type::YamahaBackT9Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaDown4Ha ));
  ary.push_back( GetGraphic( Type::YamahaDown6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin1_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin2_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin3_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin4_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin6_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin7_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin8_13Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_1Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_2Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_3Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_4Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_5Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_6Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_7Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_8Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_9Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_10Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_11Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_12Ha ));
  ary.push_back( GetGraphic( Type::YamahaKin9_13Ha ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaNi
  ary.push_back( GetGraphic( Type::YamahaStand1Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand2Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand3Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand4Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand6Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand7Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand8Ni ));
  ary.push_back( GetGraphic( Type::YamahaStand9Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead1Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead2Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead3Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead4Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead6Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead7Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead8Ni ));
  ary.push_back( GetGraphic( Type::YamahaAhead9Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack1Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack2Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack3Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack4Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack6Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack7Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack8Ni ));
  ary.push_back( GetGraphic( Type::YamahaBack9Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT1Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT2Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT3Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT4Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT6Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT7Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT8Ni ));
  ary.push_back( GetGraphic( Type::YamahaAheadT9Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT1Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT2Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT3Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT4Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT6Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT7Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT8Ni ));
  ary.push_back( GetGraphic( Type::YamahaBackT9Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser1_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser2_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser3_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser4_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser6_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser7_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser8_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaLaser9_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaDown4Ni ));
  ary.push_back( GetGraphic( Type::YamahaDown6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin1_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin2_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin3_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin4_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin6_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin7_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin8_13Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_1Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_2Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_3Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_4Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_5Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_6Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_7Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_8Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_9Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_10Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_11Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_12Ni ));
  ary.push_back( GetGraphic( Type::YamahaKin9_13Ni ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::NTTI
  ary.push_back( GetGraphic( Type::NTTStand1I ));
  ary.push_back( GetGraphic( Type::NTTStand2I ));
  ary.push_back( GetGraphic( Type::NTTStand3I ));
  ary.push_back( GetGraphic( Type::NTTStand4I ));
  ary.push_back( GetGraphic( Type::NTTStand6I ));
  ary.push_back( GetGraphic( Type::NTTStand7I ));
  ary.push_back( GetGraphic( Type::NTTStand8I ));
  ary.push_back( GetGraphic( Type::NTTStand9I ));
  ary.push_back( GetGraphic( Type::NTTDash1I ));
  ary.push_back( GetGraphic( Type::NTTDash2I ));
  ary.push_back( GetGraphic( Type::NTTDash3I ));
  ary.push_back( GetGraphic( Type::NTTDash4I ));
  ary.push_back( GetGraphic( Type::NTTDash6I ));
  ary.push_back( GetGraphic( Type::NTTDash7I ));
  ary.push_back( GetGraphic( Type::NTTDash8I ));
  ary.push_back( GetGraphic( Type::NTTDash9I ));
  ary.push_back( GetGraphic( Type::NTTWeapon1I ));
  ary.push_back( GetGraphic( Type::NTTWeapon2I ));
  ary.push_back( GetGraphic( Type::NTTWeapon3I ));
  ary.push_back( GetGraphic( Type::NTTWeapon4I ));
  ary.push_back( GetGraphic( Type::NTTWeapon6I ));
  ary.push_back( GetGraphic( Type::NTTWeapon7I ));
  ary.push_back( GetGraphic( Type::NTTWeapon8I ));
  ary.push_back( GetGraphic( Type::NTTWeapon9I ));
  ary.push_back( GetGraphic( Type::NTTAttack1I ));
  ary.push_back( GetGraphic( Type::NTTAttack2I ));
  ary.push_back( GetGraphic( Type::NTTAttack3I ));
  ary.push_back( GetGraphic( Type::NTTAttack4I ));
  ary.push_back( GetGraphic( Type::NTTAttack6I ));
  ary.push_back( GetGraphic( Type::NTTAttack7I ));
  ary.push_back( GetGraphic( Type::NTTAttack8I ));
  ary.push_back( GetGraphic( Type::NTTAttack9I ));
  ary.push_back( GetGraphic( Type::NTTKin1_1I ));
  ary.push_back( GetGraphic( Type::NTTKin1_2I ));
  ary.push_back( GetGraphic( Type::NTTKin1_3I ));
  ary.push_back( GetGraphic( Type::NTTKin1_4I ));
  ary.push_back( GetGraphic( Type::NTTKin1_5I ));
  ary.push_back( GetGraphic( Type::NTTKin1_6I ));
  ary.push_back( GetGraphic( Type::NTTKin2_1I ));
  ary.push_back( GetGraphic( Type::NTTKin2_2I ));
  ary.push_back( GetGraphic( Type::NTTKin2_3I ));
  ary.push_back( GetGraphic( Type::NTTKin2_4I ));
  ary.push_back( GetGraphic( Type::NTTKin2_5I ));
  ary.push_back( GetGraphic( Type::NTTKin2_6I ));
  ary.push_back( GetGraphic( Type::NTTKin3_1I ));
  ary.push_back( GetGraphic( Type::NTTKin3_2I ));
  ary.push_back( GetGraphic( Type::NTTKin3_3I ));
  ary.push_back( GetGraphic( Type::NTTKin3_4I ));
  ary.push_back( GetGraphic( Type::NTTKin3_5I ));
  ary.push_back( GetGraphic( Type::NTTKin3_6I ));
  ary.push_back( GetGraphic( Type::NTTKin4_1I ));
  ary.push_back( GetGraphic( Type::NTTKin4_2I ));
  ary.push_back( GetGraphic( Type::NTTKin4_3I ));
  ary.push_back( GetGraphic( Type::NTTKin4_4I ));
  ary.push_back( GetGraphic( Type::NTTKin4_5I ));
  ary.push_back( GetGraphic( Type::NTTKin4_6I ));
  ary.push_back( GetGraphic( Type::NTTKin6_1I ));
  ary.push_back( GetGraphic( Type::NTTKin6_2I ));
  ary.push_back( GetGraphic( Type::NTTKin6_3I ));
  ary.push_back( GetGraphic( Type::NTTKin6_4I ));
  ary.push_back( GetGraphic( Type::NTTKin6_5I ));
  ary.push_back( GetGraphic( Type::NTTKin6_6I ));
  ary.push_back( GetGraphic( Type::NTTKin7_1I ));
  ary.push_back( GetGraphic( Type::NTTKin7_2I ));
  ary.push_back( GetGraphic( Type::NTTKin7_3I ));
  ary.push_back( GetGraphic( Type::NTTKin7_4I ));
  ary.push_back( GetGraphic( Type::NTTKin7_5I ));
  ary.push_back( GetGraphic( Type::NTTKin7_6I ));
  ary.push_back( GetGraphic( Type::NTTKin8_1I ));
  ary.push_back( GetGraphic( Type::NTTKin8_2I ));
  ary.push_back( GetGraphic( Type::NTTKin8_3I ));
  ary.push_back( GetGraphic( Type::NTTKin8_4I ));
  ary.push_back( GetGraphic( Type::NTTKin8_5I ));
  ary.push_back( GetGraphic( Type::NTTKin8_6I ));
  ary.push_back( GetGraphic( Type::NTTKin9_1I ));
  ary.push_back( GetGraphic( Type::NTTKin9_2I ));
  ary.push_back( GetGraphic( Type::NTTKin9_3I ));
  ary.push_back( GetGraphic( Type::NTTKin9_4I ));
  ary.push_back( GetGraphic( Type::NTTKin9_5I ));
  ary.push_back( GetGraphic( Type::NTTKin9_6I ));
  ary.push_back( GetGraphic( Type::NTTDown4I ));
  ary.push_back( GetGraphic( Type::NTTDown6I ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::NTTRo
  ary.push_back( GetGraphic( Type::NTTStand1Ro ));
  ary.push_back( GetGraphic( Type::NTTStand2Ro ));
  ary.push_back( GetGraphic( Type::NTTStand3Ro ));
  ary.push_back( GetGraphic( Type::NTTStand4Ro ));
  ary.push_back( GetGraphic( Type::NTTStand6Ro ));
  ary.push_back( GetGraphic( Type::NTTStand7Ro ));
  ary.push_back( GetGraphic( Type::NTTStand8Ro ));
  ary.push_back( GetGraphic( Type::NTTStand9Ro ));
  ary.push_back( GetGraphic( Type::NTTDash1Ro ));
  ary.push_back( GetGraphic( Type::NTTDash2Ro ));
  ary.push_back( GetGraphic( Type::NTTDash3Ro ));
  ary.push_back( GetGraphic( Type::NTTDash4Ro ));
  ary.push_back( GetGraphic( Type::NTTDash6Ro ));
  ary.push_back( GetGraphic( Type::NTTDash7Ro ));
  ary.push_back( GetGraphic( Type::NTTDash8Ro ));
  ary.push_back( GetGraphic( Type::NTTDash9Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon1Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon2Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon3Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon4Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon6Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon7Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon8Ro ));
  ary.push_back( GetGraphic( Type::NTTWeapon9Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack1Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack2Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack3Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack4Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack6Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack7Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack8Ro ));
  ary.push_back( GetGraphic( Type::NTTAttack9Ro ));
  ary.push_back( GetGraphic( Type::NTTKin1_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin1_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin1_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin1_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin1_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin1_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin2_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin2_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin2_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin2_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin2_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin2_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin3_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin3_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin3_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin3_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin3_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin3_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin4_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin4_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin4_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin4_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin4_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin4_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin6_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin6_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin6_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin6_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin6_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin6_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin7_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin7_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin7_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin7_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin7_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin7_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin8_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin8_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin8_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin8_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin8_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin8_6Ro ));
  ary.push_back( GetGraphic( Type::NTTKin9_1Ro ));
  ary.push_back( GetGraphic( Type::NTTKin9_2Ro ));
  ary.push_back( GetGraphic( Type::NTTKin9_3Ro ));
  ary.push_back( GetGraphic( Type::NTTKin9_4Ro ));
  ary.push_back( GetGraphic( Type::NTTKin9_5Ro ));
  ary.push_back( GetGraphic( Type::NTTKin9_6Ro ));
  ary.push_back( GetGraphic( Type::NTTDown4Ro ));
  ary.push_back( GetGraphic( Type::NTTDown6Ro ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::NTTHa
  ary.push_back( GetGraphic( Type::NTTStand1Ha ));
  ary.push_back( GetGraphic( Type::NTTStand2Ha ));
  ary.push_back( GetGraphic( Type::NTTStand3Ha ));
  ary.push_back( GetGraphic( Type::NTTStand4Ha ));
  ary.push_back( GetGraphic( Type::NTTStand6Ha ));
  ary.push_back( GetGraphic( Type::NTTStand7Ha ));
  ary.push_back( GetGraphic( Type::NTTStand8Ha ));
  ary.push_back( GetGraphic( Type::NTTStand9Ha ));
  ary.push_back( GetGraphic( Type::NTTDash1Ha ));
  ary.push_back( GetGraphic( Type::NTTDash2Ha ));
  ary.push_back( GetGraphic( Type::NTTDash3Ha ));
  ary.push_back( GetGraphic( Type::NTTDash4Ha ));
  ary.push_back( GetGraphic( Type::NTTDash6Ha ));
  ary.push_back( GetGraphic( Type::NTTDash7Ha ));
  ary.push_back( GetGraphic( Type::NTTDash8Ha ));
  ary.push_back( GetGraphic( Type::NTTDash9Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon1Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon2Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon3Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon4Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon6Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon7Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon8Ha ));
  ary.push_back( GetGraphic( Type::NTTWeapon9Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack1Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack2Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack3Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack4Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack6Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack7Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack8Ha ));
  ary.push_back( GetGraphic( Type::NTTAttack9Ha ));
  ary.push_back( GetGraphic( Type::NTTKin1_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin1_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin1_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin1_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin1_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin1_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin2_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin2_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin2_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin2_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin2_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin2_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin3_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin3_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin3_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin3_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin3_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin3_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin4_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin4_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin4_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin4_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin4_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin4_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin6_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin6_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin6_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin6_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin6_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin6_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin7_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin7_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin7_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin7_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin7_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin7_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin8_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin8_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin8_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin8_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin8_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin8_6Ha ));
  ary.push_back( GetGraphic( Type::NTTKin9_1Ha ));
  ary.push_back( GetGraphic( Type::NTTKin9_2Ha ));
  ary.push_back( GetGraphic( Type::NTTKin9_3Ha ));
  ary.push_back( GetGraphic( Type::NTTKin9_4Ha ));
  ary.push_back( GetGraphic( Type::NTTKin9_5Ha ));
  ary.push_back( GetGraphic( Type::NTTKin9_6Ha ));
  ary.push_back( GetGraphic( Type::NTTDown4Ha ));
  ary.push_back( GetGraphic( Type::NTTDown6Ha ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::NTTNi
  ary.push_back( GetGraphic( Type::NTTStand1Ni ));
  ary.push_back( GetGraphic( Type::NTTStand2Ni ));
  ary.push_back( GetGraphic( Type::NTTStand3Ni ));
  ary.push_back( GetGraphic( Type::NTTStand4Ni ));
  ary.push_back( GetGraphic( Type::NTTStand6Ni ));
  ary.push_back( GetGraphic( Type::NTTStand7Ni ));
  ary.push_back( GetGraphic( Type::NTTStand8Ni ));
  ary.push_back( GetGraphic( Type::NTTStand9Ni ));
  ary.push_back( GetGraphic( Type::NTTDash1Ni ));
  ary.push_back( GetGraphic( Type::NTTDash2Ni ));
  ary.push_back( GetGraphic( Type::NTTDash3Ni ));
  ary.push_back( GetGraphic( Type::NTTDash4Ni ));
  ary.push_back( GetGraphic( Type::NTTDash6Ni ));
  ary.push_back( GetGraphic( Type::NTTDash7Ni ));
  ary.push_back( GetGraphic( Type::NTTDash8Ni ));
  ary.push_back( GetGraphic( Type::NTTDash9Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon1Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon2Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon3Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon4Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon6Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon7Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon8Ni ));
  ary.push_back( GetGraphic( Type::NTTWeapon9Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack1Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack2Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack3Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack4Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack6Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack7Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack8Ni ));
  ary.push_back( GetGraphic( Type::NTTAttack9Ni ));
  ary.push_back( GetGraphic( Type::NTTKin1_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin1_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin1_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin1_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin1_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin1_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin2_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin2_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin2_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin2_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin2_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin2_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin3_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin3_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin3_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin3_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin3_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin3_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin4_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin4_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin4_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin4_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin4_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin4_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin6_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin6_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin6_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin6_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin6_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin6_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin7_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin7_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin7_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin7_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin7_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin7_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin8_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin8_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin8_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin8_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin8_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin8_6Ni ));
  ary.push_back( GetGraphic( Type::NTTKin9_1Ni ));
  ary.push_back( GetGraphic( Type::NTTKin9_2Ni ));
  ary.push_back( GetGraphic( Type::NTTKin9_3Ni ));
  ary.push_back( GetGraphic( Type::NTTKin9_4Ni ));
  ary.push_back( GetGraphic( Type::NTTKin9_5Ni ));
  ary.push_back( GetGraphic( Type::NTTKin9_6Ni ));
  ary.push_back( GetGraphic( Type::NTTDown4Ni ));
  ary.push_back( GetGraphic( Type::NTTDown6Ni ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::PlanexMissile
  ary.push_back( GetGraphic( Type::PlanexMissile1 ));
  ary.push_back( GetGraphic( Type::PlanexMissile12 ));
  ary.push_back( GetGraphic( Type::PlanexMissile2 ));
  ary.push_back( GetGraphic( Type::PlanexMissile23 ));
  ary.push_back( GetGraphic( Type::PlanexMissile3 ));
  ary.push_back( GetGraphic( Type::PlanexMissile36 ));
  ary.push_back( GetGraphic( Type::PlanexMissile6 ));
  ary.push_back( GetGraphic( Type::PlanexMissile69 ));
  ary.push_back( GetGraphic( Type::PlanexMissile9 ));
  ary.push_back( GetGraphic( Type::PlanexMissile89 ));
  ary.push_back( GetGraphic( Type::PlanexMissile8 ));
  ary.push_back( GetGraphic( Type::PlanexMissile78 ));
  ary.push_back( GetGraphic( Type::PlanexMissile7 ));
  ary.push_back( GetGraphic( Type::PlanexMissile47 ));
  ary.push_back( GetGraphic( Type::PlanexMissile4 ));
  ary.push_back( GetGraphic( Type::PlanexMissile14 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::PlanexEnergyShot
  ary.push_back( GetGraphic( Type::PlanexEnergyShot1 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot12 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot2 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot23 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot3 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot36 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot6 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot69 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot9 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot89 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot8 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot78 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot7 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot47 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot4 ));
  ary.push_back( GetGraphic( Type::PlanexEnergyShot14 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaBomb
  ary.push_back( GetGraphic( Type::YamahaBombI ));
  ary.push_back( GetGraphic( Type::YamahaBombRo ));
  ary.push_back( GetGraphic( Type::YamahaBombHa ));
  ary.push_back( GetGraphic( Type::YamahaBombNi ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaLaserShot
  ary.push_back( GetGraphic( Type::YamahaLaserShot1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot12 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot23 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot36 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot6 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot69 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot9 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot89 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot8 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot78 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot7 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot47 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserShot14 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::NTTEnergyShot
  ary.push_back( GetGraphic( Type::NTTEnergyShot1 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot12 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot2 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot23 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot3 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot36 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot6 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot69 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot9 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot89 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot8 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot78 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot7 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot47 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot4 ));
  ary.push_back( GetGraphic( Type::NTTEnergyShot14 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::YamahaLaserEffect
  ary.push_back( GetGraphic( Type::YamahaLaserEffect1_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect12_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect2_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect23_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect3_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect36_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect6_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect69_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect9_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect89_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect8_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect78_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect7_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect47_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect4_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect14_1 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect1_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect12_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect2_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect23_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect3_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect36_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect6_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect69_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect9_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect89_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect8_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect78_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect7_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect47_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect4_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect14_2 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect1_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect12_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect2_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect23_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect3_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect36_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect6_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect69_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect9_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect89_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect8_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect78_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect7_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect47_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect4_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect14_3 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect1_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect12_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect2_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect23_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect3_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect36_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect6_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect69_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect9_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect89_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect8_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect78_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect7_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect47_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect4_4 ));
  ary.push_back( GetGraphic( Type::YamahaLaserEffect14_4 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::DotEffect
  ary.push_back( GetGraphic( Type::DotEffect ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::ExplosionA
  ary.push_back( GetGraphic( Type::ExplosionA0 ));
  ary.push_back( GetGraphic( Type::ExplosionA1 ));
  ary.push_back( GetGraphic( Type::ExplosionA2 ));
  ary.push_back( GetGraphic( Type::ExplosionA3 ));
  ary.push_back( GetGraphic( Type::ExplosionA4 ));
  ary.push_back( GetGraphic( Type::ExplosionA5 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::ExplosionAx2
  ary.push_back( GetGraphic( Type::ExplosionAx2_0 ));
  ary.push_back( GetGraphic( Type::ExplosionAx2_1 ));
  ary.push_back( GetGraphic( Type::ExplosionAx2_2 ));
  ary.push_back( GetGraphic( Type::ExplosionAx2_3 ));
  ary.push_back( GetGraphic( Type::ExplosionAx2_4 ));
  ary.push_back( GetGraphic( Type::ExplosionAx2_5 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::ExplosionAx4
  ary.push_back( GetGraphic( Type::ExplosionAx4_0 ));
  ary.push_back( GetGraphic( Type::ExplosionAx4_1 ));
  ary.push_back( GetGraphic( Type::ExplosionAx4_2 ));
  ary.push_back( GetGraphic( Type::ExplosionAx4_3 ));
  ary.push_back( GetGraphic( Type::ExplosionAx4_4 ));
  ary.push_back( GetGraphic( Type::ExplosionAx4_5 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::ExplosionB
  ary.push_back( GetGraphic( Type::ExplosionB0 ));
  ary.push_back( GetGraphic( Type::ExplosionB1 ));
  ary.push_back( GetGraphic( Type::ExplosionB2 ));
  ary.push_back( GetGraphic( Type::ExplosionB3 ));
  ary.push_back( GetGraphic( Type::ExplosionB4 ));
  ary.push_back( GetGraphic( Type::ExplosionB5 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::ExplosionC
  ary.push_back( GetGraphic( Type::ExplosionC0 ));
  ary.push_back( GetGraphic( Type::ExplosionC1 ));
  ary.push_back( GetGraphic( Type::ExplosionC2 ));
  ary.push_back( GetGraphic( Type::ExplosionC3 ));
  ary.push_back( GetGraphic( Type::ExplosionC4 ));
  ary.push_back( GetGraphic( Type::ExplosionC5 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::Smoke
  ary.push_back( GetGraphic( Type::SmokeA0 ));
  ary.push_back( GetGraphic( Type::SmokeA1 ));
  ary.push_back( GetGraphic( Type::SmokeA2 ));
  ary.push_back( GetGraphic( Type::SmokeA3 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::SlashA
  ary.push_back( GetGraphic( Type::SlashA1_1 ));
  ary.push_back( GetGraphic( Type::SlashA2_1 ));
  ary.push_back( GetGraphic( Type::SlashA3_1 ));
  ary.push_back( GetGraphic( Type::SlashA4_1 ));
  ary.push_back( GetGraphic( Type::SlashA6_1 ));
  ary.push_back( GetGraphic( Type::SlashA7_1 ));
  ary.push_back( GetGraphic( Type::SlashA8_1 ));
  ary.push_back( GetGraphic( Type::SlashA9_1 ));
  ary.push_back( GetGraphic( Type::SlashA1_2 ));
  ary.push_back( GetGraphic( Type::SlashA2_2 ));
  ary.push_back( GetGraphic( Type::SlashA3_2 ));
  ary.push_back( GetGraphic( Type::SlashA4_2 ));
  ary.push_back( GetGraphic( Type::SlashA6_2 ));
  ary.push_back( GetGraphic( Type::SlashA7_2 ));
  ary.push_back( GetGraphic( Type::SlashA8_2 ));
  ary.push_back( GetGraphic( Type::SlashA9_2 ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::Shadow
  ary.push_back( GetGraphic( Type::Shadow ));
  ary.push_back( GetGraphic( Type::ShadowYamaha ));
  ary.push_back( GetGraphic( Type::ShadowKirai ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::Arrow
  ary.push_back( GetGraphic( Type::Arrow_IBigUp ));
  ary.push_back( GetGraphic( Type::Arrow_IBigDown ));
  ary.push_back( GetGraphic( Type::Arrow_IBigLeft ));
  ary.push_back( GetGraphic( Type::Arrow_IBigRight ));
  ary.push_back( GetGraphic( Type::Arrow_IMiddleUp ));
  ary.push_back( GetGraphic( Type::Arrow_IMiddleDown ));
  ary.push_back( GetGraphic( Type::Arrow_IMiddleLeft ));
  ary.push_back( GetGraphic( Type::Arrow_IMiddleRight ));
  ary.push_back( GetGraphic( Type::Arrow_ISmallUp ));
  ary.push_back( GetGraphic( Type::Arrow_ISmallDown ));
  ary.push_back( GetGraphic( Type::Arrow_ISmallLeft ));
  ary.push_back( GetGraphic( Type::Arrow_ISmallRight ));
  ary.push_back( GetGraphic( Type::Arrow_RoBigUp ));
  ary.push_back( GetGraphic( Type::Arrow_RoBigDown ));
  ary.push_back( GetGraphic( Type::Arrow_RoBigLeft ));
  ary.push_back( GetGraphic( Type::Arrow_RoBigRight ));
  ary.push_back( GetGraphic( Type::Arrow_RoMiddleUp ));
  ary.push_back( GetGraphic( Type::Arrow_RoMiddleDown ));
  ary.push_back( GetGraphic( Type::Arrow_RoMiddleLeft ));
  ary.push_back( GetGraphic( Type::Arrow_RoMiddleRight ));
  ary.push_back( GetGraphic( Type::Arrow_RoSmallUp ));
  ary.push_back( GetGraphic( Type::Arrow_RoSmallDown ));
  ary.push_back( GetGraphic( Type::Arrow_RoSmallLeft ));
  ary.push_back( GetGraphic( Type::Arrow_RoSmallRight ));
  ary.push_back( GetGraphic( Type::Arrow_HaBigUp ));
  ary.push_back( GetGraphic( Type::Arrow_HaBigDown ));
  ary.push_back( GetGraphic( Type::Arrow_HaBigLeft ));
  ary.push_back( GetGraphic( Type::Arrow_HaBigRight ));
  ary.push_back( GetGraphic( Type::Arrow_HaMiddleUp ));
  ary.push_back( GetGraphic( Type::Arrow_HaMiddleDown ));
  ary.push_back( GetGraphic( Type::Arrow_HaMiddleLeft ));
  ary.push_back( GetGraphic( Type::Arrow_HaMiddleRight ));
  ary.push_back( GetGraphic( Type::Arrow_HaSmallUp ));
  ary.push_back( GetGraphic( Type::Arrow_HaSmallDown ));
  ary.push_back( GetGraphic( Type::Arrow_HaSmallLeft ));
  ary.push_back( GetGraphic( Type::Arrow_HaSmallRight ));
  ary.push_back( GetGraphic( Type::Arrow_NiBigUp ));
  ary.push_back( GetGraphic( Type::Arrow_NiBigDown ));
  ary.push_back( GetGraphic( Type::Arrow_NiBigLeft ));
  ary.push_back( GetGraphic( Type::Arrow_NiBigRight ));
  ary.push_back( GetGraphic( Type::Arrow_NiMiddleUp ));
  ary.push_back( GetGraphic( Type::Arrow_NiMiddleDown ));
  ary.push_back( GetGraphic( Type::Arrow_NiMiddleLeft ));
  ary.push_back( GetGraphic( Type::Arrow_NiMiddleRight ));
  ary.push_back( GetGraphic( Type::Arrow_NiSmallUp ));
  ary.push_back( GetGraphic( Type::Arrow_NiSmallDown ));
  ary.push_back( GetGraphic( Type::Arrow_NiSmallLeft ));
  ary.push_back( GetGraphic( Type::Arrow_NiSmallRight ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::TransArrow
  ary.push_back( GetGraphic( Type::TArrow_IBigUp ));
  ary.push_back( GetGraphic( Type::TArrow_IBigDown ));
  ary.push_back( GetGraphic( Type::TArrow_IBigLeft ));
  ary.push_back( GetGraphic( Type::TArrow_IBigRight ));
  ary.push_back( GetGraphic( Type::TArrow_IMiddleUp ));
  ary.push_back( GetGraphic( Type::TArrow_IMiddleDown ));
  ary.push_back( GetGraphic( Type::TArrow_IMiddleLeft ));
  ary.push_back( GetGraphic( Type::TArrow_IMiddleRight ));
  ary.push_back( GetGraphic( Type::TArrow_ISmallUp ));
  ary.push_back( GetGraphic( Type::TArrow_ISmallDown ));
  ary.push_back( GetGraphic( Type::TArrow_ISmallLeft ));
  ary.push_back( GetGraphic( Type::TArrow_ISmallRight ));
  ary.push_back( GetGraphic( Type::TArrow_RoBigUp ));
  ary.push_back( GetGraphic( Type::TArrow_RoBigDown ));
  ary.push_back( GetGraphic( Type::TArrow_RoBigLeft ));
  ary.push_back( GetGraphic( Type::TArrow_RoBigRight ));
  ary.push_back( GetGraphic( Type::TArrow_RoMiddleUp ));
  ary.push_back( GetGraphic( Type::TArrow_RoMiddleDown ));
  ary.push_back( GetGraphic( Type::TArrow_RoMiddleLeft ));
  ary.push_back( GetGraphic( Type::TArrow_RoMiddleRight ));
  ary.push_back( GetGraphic( Type::TArrow_RoSmallUp ));
  ary.push_back( GetGraphic( Type::TArrow_RoSmallDown ));
  ary.push_back( GetGraphic( Type::TArrow_RoSmallLeft ));
  ary.push_back( GetGraphic( Type::TArrow_RoSmallRight ));
  ary.push_back( GetGraphic( Type::TArrow_HaBigUp ));
  ary.push_back( GetGraphic( Type::TArrow_HaBigDown ));
  ary.push_back( GetGraphic( Type::TArrow_HaBigLeft ));
  ary.push_back( GetGraphic( Type::TArrow_HaBigRight ));
  ary.push_back( GetGraphic( Type::TArrow_HaMiddleUp ));
  ary.push_back( GetGraphic( Type::TArrow_HaMiddleDown ));
  ary.push_back( GetGraphic( Type::TArrow_HaMiddleLeft ));
  ary.push_back( GetGraphic( Type::TArrow_HaMiddleRight ));
  ary.push_back( GetGraphic( Type::TArrow_HaSmallUp ));
  ary.push_back( GetGraphic( Type::TArrow_HaSmallDown ));
  ary.push_back( GetGraphic( Type::TArrow_HaSmallLeft ));
  ary.push_back( GetGraphic( Type::TArrow_HaSmallRight ));
  ary.push_back( GetGraphic( Type::TArrow_NiBigUp ));
  ary.push_back( GetGraphic( Type::TArrow_NiBigDown ));
  ary.push_back( GetGraphic( Type::TArrow_NiBigLeft ));
  ary.push_back( GetGraphic( Type::TArrow_NiBigRight ));
  ary.push_back( GetGraphic( Type::TArrow_NiMiddleUp ));
  ary.push_back( GetGraphic( Type::TArrow_NiMiddleDown ));
  ary.push_back( GetGraphic( Type::TArrow_NiMiddleLeft ));
  ary.push_back( GetGraphic( Type::TArrow_NiMiddleRight ));
  ary.push_back( GetGraphic( Type::TArrow_NiSmallUp ));
  ary.push_back( GetGraphic( Type::TArrow_NiSmallDown ));
  ary.push_back( GetGraphic( Type::TArrow_NiSmallLeft ));
  ary.push_back( GetGraphic( Type::TArrow_NiSmallRight ));
  graphics_.push_back( ary );

  ary.clear();
  // SendObjType::ArrowLight
  if (stg_type == StageType::Forest) {
    ary.push_back( GetGraphic( Type::ArrowLightBigUpP ));
    ary.push_back( GetGraphic( Type::ArrowLightBigDownP ));
    ary.push_back( GetGraphic( Type::ArrowLightBigLeftP ));
    ary.push_back( GetGraphic( Type::ArrowLightBigRightP ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleUpP ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleDownP ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleLeftP ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleRightP ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallUpP ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallDownP ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallLeftP ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallRightP ));
  } else {
    ary.push_back( GetGraphic( Type::ArrowLightBigUp ));
    ary.push_back( GetGraphic( Type::ArrowLightBigDown ));
    ary.push_back( GetGraphic( Type::ArrowLightBigLeft ));
    ary.push_back( GetGraphic( Type::ArrowLightBigRight ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleUp ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleDown ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleLeft ));
    ary.push_back( GetGraphic( Type::ArrowLightMiddleRight ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallUp ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallDown ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallLeft ));
    ary.push_back( GetGraphic( Type::ArrowLightSmallRight ));
  }
  graphics_.push_back( ary );

  // ロックオンサイトの画像取得
  // 赤
  lock_r.push_back(GetGraphic(Type::LockR1));
  lock_r.push_back(GetGraphic(Type::LockR2));
  lock_r.push_back(GetGraphic(Type::LockR3));
  lock_rparts.push_back(GetGraphic(Type::LockRU));
  lock_rparts.push_back(GetGraphic(Type::LockRD));
  lock_rparts.push_back(GetGraphic(Type::LockRL));
  lock_rparts.push_back(GetGraphic(Type::LockRR));
  // 黄
  lock_y.push_back(GetGraphic(Type::LockY1));
  lock_y.push_back(GetGraphic(Type::LockY2));
  lock_y.push_back(GetGraphic(Type::LockY3));
  lock_yparts.push_back(GetGraphic(Type::LockYU));
  lock_yparts.push_back(GetGraphic(Type::LockYD));
  lock_yparts.push_back(GetGraphic(Type::LockYL));
  lock_yparts.push_back(GetGraphic(Type::LockYR));
}

//============================================================
// DrawingManager::DrawAll
// SendObjAryにある情報とステージの内容を描画する
//============================================================
void DrawingManager::DrawAll( const SendObjAry* objs, const SendParam* param )
{
  ++frame;

  Pos scr_pos = Pos(param->view_pos.x - ScreenWidth / 2,
		    param->view_pos.y - ScreenHeight / 2);

  // ステージ描画
  DrawStage(scr_pos);

  // SendObjAry描画
  for ( SOAConstIter i = objs->begin(); i != objs->end(); ++i ) {
    screen.Draw( graphics_[i->type][i->state], toPos(i->pos) - scr_pos );
  }

  // ダッシュゲージ描画
  if (param->dash < 100) {
    Pos pos(param->dash_gauge_pos.x, param->dash_gauge_pos.y);
    RectGraphic( param->dash * 50 / 100, 10, 1, 
		 Color( 0x88, 0xff, 0xff )).Draw( screen, pos - scr_pos );
    RectGraphic( 50, 10, 1, Color( 0x55, 0x55, 0x55 ), false ).Draw( screen, pos - scr_pos );
  }

  // 矢印描画
  for (Arrows::const_iterator i = param->arrows.begin();
       i != param->arrows.end(); ++i) {
    unsigned char arrow_type;
    SendObject arrow_light;
    bool adraw = false;		// 矢印を描画するか
    arrow_light.type = SendObjType::ArrowLight;
    
    // ArrowTypeをSendObjTypeとSendObjStateに反映
    switch (i->type) {
    case ArrowType::ID0:
      arrow_type = SendObjType::Arrow;
      adraw = true;
      break;
    case ArrowType::ID1:
      arrow_type = SendObjType::Arrow;
      adraw = true;
      break;
    case ArrowType::ID2:
      arrow_type = SendObjType::Arrow;
      adraw = true;
      break;
    case ArrowType::ID3:
      arrow_type = SendObjType::Arrow;
      adraw = true;
      break;
    case ArrowType::TransID0:
      arrow_type = SendObjType::TransArrow;
      break;
    case ArrowType::TransID1:
      arrow_type = SendObjType::TransArrow;
      break;
    case ArrowType::TransID2:
      arrow_type = SendObjType::TransArrow;
      break;
    case ArrowType::TransID3:
      arrow_type = SendObjType::TransArrow;
      break;
    }
    // 矢印の光描画
    if (adraw) {
      arrow_light.pos = i->pos;
      arrow_light.state = i->state;
      while (arrow_light.state >= ArrowState::RoBigUp) {
	// big, middle, small + up, down, left, right
	// 3 * 4
	// 光にチームの情報は存在しない
	arrow_light.state -= 12;
      }
      screen.Draw(graphics_[arrow_light.type][arrow_light.state], toPos(arrow_light.pos) - scr_pos);
    }
    // 矢印描画
    screen.Draw( graphics_[arrow_type][i->state], toPos(i->pos) - scr_pos );
  }

  // ロックオンサイト描画
  int st = param->site.state;
  if (st >= LockState::MoveNum + 3) {
    st -= LockState::MoveNum + 3;

    if (st < LockState::MoveNum) {
      int s = LockState::MoveNum - st;
      int d = s * 150 / LockState::MoveNum;
      Pos pos = toPos(param->site.pos);
      screen.Draw(lock_yparts[0], Pos(pos.x, pos.y - d) - scr_pos);
      screen.Draw(lock_yparts[1], Pos(pos.x, pos.y + d) - scr_pos);
      screen.Draw(lock_yparts[2], Pos(pos.x - d, pos.y) - scr_pos);
      screen.Draw(lock_yparts[3], Pos(pos.x + d, pos.y) - scr_pos);
    } else {
      unsigned int i = st - LockState::MoveNum;
      screen.Draw(lock_y[i], toPos(param->site.pos) - scr_pos);
    }
  } else {
    if (param->site.state < LockState::MoveNum) {
      int s = LockState::MoveNum - param->site.state;
      int d = s * 150 / LockState::MoveNum;
      Pos pos = toPos(param->site.pos);
      screen.Draw(lock_rparts[0], Pos(pos.x, pos.y - d) - scr_pos);
      screen.Draw(lock_rparts[1], Pos(pos.x, pos.y + d) - scr_pos);
      screen.Draw(lock_rparts[2], Pos(pos.x - d, pos.y) - scr_pos);
      screen.Draw(lock_rparts[3], Pos(pos.x + d, pos.y) - scr_pos);
    } else {
      unsigned int i = param->site.state - LockState::MoveNum;
      screen.Draw(lock_r[i], toPos(param->site.pos) - scr_pos);
    }
  }

  //-----------------------------------
  // 対戦中のステータス等を描画する
  //-----------------------------------
  // 背景
  screen.Draw(status_back, Pos(WindowWidth - status_back->GetWH().x, 0));

  // 経過時間
  unsigned min = 0;
  unsigned sec = 0;
  unsigned ms10 = 0;

  if (elap_draw) {
    Uint32 etime = SDL_GetTicks() - base_time;
    while (etime > one_min) {
      etime -= one_min;
      ++min;
    }
    while (etime > one_sec) {
      etime -= one_sec;
      ++sec;
    }
    while (etime > ten_ms) {
      etime -= ten_ms;
      ++ms10;
    }
  }
  // 時間の数値を文字列に変換
  string str_time;
  string str_min = IntToStr(min);
  if (str_min.size() < 2) {
    str_min = "0" + str_min;
  } else if (str_min.size() > 2) {
    str_min = "99";
  }
  string str_sec = IntToStr(sec);
  if (str_sec.size() < 2) {
    str_sec = "0" + str_sec;
  }
  string str_ms10 = IntToStr(ms10);
  if (str_ms10.size() < 2) {
    str_ms10 = "0" + str_ms10;
  }
  str_time = str_min + ":" + str_sec + ":" + str_ms10;
  sp_Graphic grap_time = time_font->RenderBlended(str_time, Color(0x11, 0x11, 0x11)).GetGraphic();
  // 分:秒:10ms
  static unsigned elapse_time_width = 0;
  unsigned temp_w = grap_time->GetWH().x;
  if (elapse_time_width == 0) {
    elapse_time_width = temp_w;
  } else {
    if (temp_w > elapse_time_width) {
      elapse_time_width = temp_w;
    }
  }
  
  screen.Draw(grap_time, 
	      Pos(WindowWidth - elapse_time_width - 5, 
		  elapse->GetWH().y + 15));
  // "経過時間"
  screen.Draw(elapse, Pos(WindowWidth - elapse->GetWH().x - 5, 10));
  
  DrawTagsByFlame(param);
  // DrawTagsByBlack(param);
}

// 御札(HP)を描画する
// 炎で御札を燃やすHPの減らしかた
void DrawingManager::DrawTagsByFlame(const SendParam* param)
{
  // HPの順番をかえる
  HitPoints hp;
  // 自分のHPを先頭に
  hp.push_back(param->HP[own_id]);
  for (unsigned int i = 0; i < 4; ++i) {
    // 自分のHP以外を次に
    if (i != unsigned(own_id)) {
      // 値のあるHP
      if (i < param->HP.size()) {
	hp.push_back(param->HP[i]);
      } else {
	// 未使用ならmax_hpをいれる
	hp.push_back(1000);
      }
    }
  }

  // HPバッファ書き換え
  for (unsigned int i = 0; i < 4; ++i) {
    // バッファの方が値が大きいなら
    if (hp_buf[i] > hp[i]) {
      // バッファのHPを減らす
      if (hp_buf[i] - hp[i] > hp_buf_red) {
	hp_buf[i] -= hp_buf_red;
      } else {
	hp_buf[i] = hp[i];
      }
    } 
  }

  // ４つの御札を描画
  for (unsigned int i = 0; i < 4; ++i) {
    // 戦闘不能を描画しない
    if (hp_buf[i] == 0) {
      continue;
    }
    // 御札描画
    SDL_Rect rect;		// 描画する御札の長方形
    rect.x = rect.y = 0;
    rect.w = tags[i]->GetWH().x;
    rect.h = tags[i]->GetWH().y * hp_buf[i] / 1000;

    sp_Graphic burnt;
    GAry* flame;
    bool burnt_draw = false;
    unsigned int down_diff;
    // HPが減っているなら焦げを描画する
    if (0 < hp_buf[i] && hp_buf[i] < 1000) {
      burnt_draw = true;
      // 自分用の大きい御札画像
      if (i == 0) {
	burnt = burnt_big;
	flame = &flame_big;
	// 実際焦げを下にずらす量
	unsigned int down = 5;
	if (rect.h > 10) {
	  down_diff = 10;
	  rect.h -= 10;
	} else {
	  down_diff = rect.h;
	  rect.h = 0;
	}
	down_diff += down;
      } 
      // 他プレイヤーの小さい御札画像
      else {
	burnt = burnt_small;
	flame = &flame_small;
	unsigned int down = 4;
	if (rect.h > 7) {
	  down_diff = 7;
	  rect.h -= 7;
	} else {
	  down_diff = rect.h;
	  rect.h = 0;
	}
	down_diff += down;
      }
    }
    // 御札描画
    tags[i]->Draw(rect, screen, tag_pos[i]);
    // 焦げ描画
    if (burnt_draw) {
      SDL_Rect brect;
      brect.x = 0;
      brect.y = 0;
      brect.w = burnt->GetWH().x;
      brect.h = burnt->GetWH().y;
      int y = tag_pos[i].y + rect.h - brect.h + down_diff;

      if (y < tag_pos[i].y) {
	brect.y = tag_pos[i].y - y;
      }
      burnt->Draw(brect, screen, Pos(tag_pos[i].x, y + brect.y));
    }
    // 炎描画
    if (hp_buf[i] != hp[i]) {
      unsigned int gf = flame_frame;
      unsigned int index = (frame % (gf*6)) / gf;
      sp_Graphic g = (*flame)[index];
      Pos pos;
      pos.x = tag_pos[i].x - g->GetWH().x / 6;
      pos.y = tag_pos[i].y + rect.h - g->GetWH().y * 82 / 100;
      screen.Draw(g, pos);
    }
  }
}

void DrawingManager::DrawTagsByBlack(const SendParam* param)
{
  // HPの順番をかえる
  HitPoints hp;
  // 自分のHPを先頭に
  hp.push_back(param->HP[own_id]);
  for (unsigned int i = 0; i < 4; ++i) {
    // 自分のHP以外を次に
    if (i != unsigned(own_id)) {
      // 値のあるHP
      if (i < param->HP.size()) {
	hp.push_back(param->HP[i]);
      } else {
	// 未使用ならmax_hpをいれる
	hp.push_back(1000);
      }
    }
  }

  // 今回のHPを記憶
  // 前回と違った場合スタックにフレームを記憶
  for (unsigned int i = 0; i < 4; ++i) {
    if (pre_hp[i] > hp[i]) {
      dec_frame_stack[i] = frame;
      pre_hp[i] = hp[i];
    }
  }

  // 描画
  for (unsigned int i = 0; i < 4; ++i) {
    SDL_Rect rect;		// 描画する御札の領域
    int w = tags[i]->GetWH().x;
    int h = tags[i]->GetWH().y;
    // 戦闘不能を描画しない
    if (hp_buf[i] > 0) {
      // 御札描画
      rect.x = rect.y = 0;
      rect.w = w;
      rect.h = h * hp_buf[i] / 1000;
      tags[i]->Draw(rect, screen, tag_pos[i]);
    }
    // 減りかけの部分描画
    if (hp_buf[i] > hp[i]) {
      rect.x = 0;
      rect.w = w;
      rect.y = h * hp[i] / 1000;
      rect.h = h * hp_buf[i] / 1000 - rect.y;
      Pos dpos = tag_pos[i];
      dpos.y += rect.y;
      if (i == 0) {
	black_big->Draw(rect, screen, dpos);
      } else {
	black_small->Draw(rect, screen, dpos);
      }
    }
  }

  // HPのバッファを減らす
  for (unsigned int i = 0; i < 4; ++i) {
    if (frame - dec_frame_stack[i] > 25 || hp[i] == 0) {
      unsigned short dec = hp_buf_red * 5;
      if (hp_buf[i] - hp[i] > dec) {
	hp_buf[i] -= dec;
      } else {
	hp_buf[i] = hp[i];
      }
    }
  }
}

// ステージを描画する．関数分ける必要ないけどなんとなく
void DrawingManager::DrawStage(const Pos& scr_pos)
{
  stg_back->Draw(mkRect(scr_pos.x, scr_pos.y, ScreenWidth, ScreenHeight), screen, Pos(0, 0));
  switch (stg_type) {
  case StageType::Forest:
    break;
  case StageType::Future:
    break;
  }
}

//============================================================
// DrawingManager::GetScreenPos
// 実際に描画するスクリーンの絶対座標を得る
//============================================================
Pos DrawingManager::GetScreenPos( const Pos& view_pos )
{
  // ステージ外が見えないように描画点調整
  // てきとーな実装
  // ステージ 1600x1200
  // ビデオ 640x480
  int w, h;
  w = 1600;
  h = 1200;

  int hww = ScreenWidth / 2;
  int hwh = ScreenHeight / 2;
  Pos draw_pos = view_pos;
  // 自分のキャラクタの位置
  if (draw_pos.x < hww) {
    draw_pos.x = hww;
  }
  if (draw_pos.y < hwh) {
    draw_pos.y = hwh;
  }
  if (w - hww < draw_pos.x) {
    draw_pos.x = w - hww;
  }
  if (h - hwh < hwh) {
    draw_pos.y = h - hwh;
  }

  Pos scr_pos(draw_pos.x - ScreenWidth / 2, draw_pos.y - ScreenHeight / 2);

  return scr_pos;
}

// ステータスを表示する御札のグラフィックを得る
sp_Graphic DrawingManager::GetStatusTagGraphic(const StatusTag& tag, bool myself)
{
  // 自分用の大きい画像
  if (myself) {
    switch (tag.id) {
    case StatusTagID::ID0:
      // 自分自身のIDを記憶しておく
      own_id = 0;
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI1Big);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo1Big);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa1Big);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi1Big);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;
    case StatusTagID::ID1:
      // 自分自身のIDを記憶しておく
      own_id = 1;
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI2Big);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo2Big);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa2Big);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi2Big);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;
    case StatusTagID::ID2:
      // 自分自身のIDを記憶しておく
      own_id = 2;
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI3Big);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo3Big);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa3Big);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi3Big);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;
    case StatusTagID::ID3:
      // 自分自身のIDを記憶しておく
      own_id = 3;
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI4Big);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo4Big);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa4Big);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi4Big);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;
    case StatusTagID::NoUse:
      throw logic_error("StatusTag::GetStatusTagGraphic: 未使用を示す大きい御札は存在しません．");
    }
  }
  // 敵用の小さい画像
  else {
    switch (tag.id) {
    case StatusTagID::ID0:
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI1Small);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo1Small);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa1Small);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi1Small);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;

    case StatusTagID::ID1:
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI2Small);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo2Small);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa2Small);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi2Small);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;

    case StatusTagID::ID2:
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI3Small);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo3Small);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa3Small);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi3Small);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;

    case StatusTagID::ID3:
      switch (tag.team) {
      case Team::I:
	return GetGraphic(Type::StatusTagI4Small);
      case Team::Ro:
	return GetGraphic(Type::StatusTagRo4Small);
      case Team::Ha:
	return GetGraphic(Type::StatusTagHa4Small);
      case Team::Ni:
	return GetGraphic(Type::StatusTagNi4Small);
      default:
	throw logic_error("StatusTag::GetStatusTagGraphic: 不正なチームタイプ");
      }
      break;

    case StatusTagID::NoUse:
      return GetGraphic(Type::StatusTagKuro);
    }
  }
  throw logic_error("DrawingManager::GetStatusTagGraphic: 画像を得ることができません．");
}

// SendObjAryのみ描画する
// テスト用
void DrawingManager::DrawSendObjAry(const SendObjAry* objs)
{
  // ステージ描画
  DrawStage(Pos(0, 0));

  // SendObjAry描画
  for ( SOAConstIter i = objs->begin(); i != objs->end(); ++i ) {
    screen.Draw( graphics_[i->type][i->state], toPos(i->pos));
  }
}
