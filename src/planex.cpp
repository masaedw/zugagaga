// last modified <2004/02/09 04:32:32 2004 JST>
#include "planex.hpp"
#include "planex_weapon.hpp"
#include "shell.hpp"

using namespace std;
using namespace zu3ga;

const PlayerParam Planex::planex_param = PlayerParam(2300, // max_hp
						     120, // dash_gauge
						     11.0, // walk_speed
						     22.0, // dash_speed
						     3.5, // slow_damage_speed
						     7.0, // fast_damage_speed
						     24.0, // close_range_speed
						     1.5, // tracking
						     ObjDetection(DetectionType::Coll,
								  25, 8, 18, 18)); // own_obj_det

Planex::Planex(Team::t team, Objects* objs)
  : Player(team, objs, planex_param)
  , stop(false)
{
  // SendObjTypeの変更
  switch (team) {
  case Team::I:
    type = SendObjType::PlanexI;
    break;
  case Team::Ro:
    type = SendObjType::PlanexRo;
    break;
  case Team::Ha:
    type = SendObjType::PlanexHa;
    break;
  case Team::Ni:
    type = SendObjType::PlanexNi;
    break;
  default:
    throw logic_error("Planex::Planex: 正しくないチームでプレイヤーが初期化されました． team=" + int(team));
    break;
  }

  // 画像IDのmapの作成
  using namespace Direction;

  // 立ち絵
  stand.reserve(End);
  stand[Dir1] = CharaAState::Stand1;
  stand[Dir2] = CharaAState::Stand2;
  stand[Dir3] = CharaAState::Stand3;
  stand[Dir4] = CharaAState::Stand4;
  stand[Dir6] = CharaAState::Stand6;
  stand[Dir7] = CharaAState::Stand7;
  stand[Dir8] = CharaAState::Stand8;
  stand[Dir9] = CharaAState::Stand9;

  // 歩き絵
  walk.reserve(End);
  walk[Dir1] = CharaAState::Walk1;
  walk[Dir2] = CharaAState::Walk2;
  walk[Dir3] = CharaAState::Walk3;
  walk[Dir4] = CharaAState::Walk4;
  walk[Dir6] = CharaAState::Walk6;
  walk[Dir7] = CharaAState::Walk7;
  walk[Dir8] = CharaAState::Walk8;
  walk[Dir9] = CharaAState::Walk9;

  // ダッシュ絵
  dash.reserve(End);
  dash[Dir1] = CharaAState::Dash1;
  dash[Dir2] = CharaAState::Dash2;
  dash[Dir3] = CharaAState::Dash3;
  dash[Dir4] = CharaAState::Dash4;
  dash[Dir6] = CharaAState::Dash6;
  dash[Dir7] = CharaAState::Dash7;
  dash[Dir8] = CharaAState::Dash8;
  dash[Dir9] = CharaAState::Dash9;

  // 武器絵
  weapon.reserve(End);
  weapon[Dir1] = CharaAState::Weapon1;
  weapon[Dir2] = CharaAState::Weapon2;
  weapon[Dir3] = CharaAState::Weapon3;
  weapon[Dir4] = CharaAState::Weapon4;
  weapon[Dir6] = CharaAState::Weapon6;
  weapon[Dir7] = CharaAState::Weapon7;
  weapon[Dir8] = CharaAState::Weapon8;
  weapon[Dir9] = CharaAState::Weapon9;

  // 近距離攻撃絵
  vector<short> temp;
  temp.push_back(CharaAState::Kin1_1);
  temp.push_back(CharaAState::Kin1_2);
  temp.push_back(CharaAState::Kin1_3);
  temp.push_back(CharaAState::Kin1_4);
  temp.push_back(CharaAState::Kin1_5);
  temp.push_back(CharaAState::Kin1_6);
  temp.push_back(CharaAState::Kin1_7);
  temp.push_back(CharaAState::Kin1_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin2_1);
  temp.push_back(CharaAState::Kin2_2);
  temp.push_back(CharaAState::Kin2_3);
  temp.push_back(CharaAState::Kin2_4);
  temp.push_back(CharaAState::Kin2_5);
  temp.push_back(CharaAState::Kin2_6);
  temp.push_back(CharaAState::Kin2_7);
  temp.push_back(CharaAState::Kin2_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin3_1);
  temp.push_back(CharaAState::Kin3_2);
  temp.push_back(CharaAState::Kin3_3);
  temp.push_back(CharaAState::Kin3_4);
  temp.push_back(CharaAState::Kin3_5);
  temp.push_back(CharaAState::Kin3_6);
  temp.push_back(CharaAState::Kin3_7);
  temp.push_back(CharaAState::Kin3_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin4_1);
  temp.push_back(CharaAState::Kin4_2);
  temp.push_back(CharaAState::Kin4_3);
  temp.push_back(CharaAState::Kin4_4);
  temp.push_back(CharaAState::Kin4_5);
  temp.push_back(CharaAState::Kin4_6);
  temp.push_back(CharaAState::Kin4_7);
  temp.push_back(CharaAState::Kin4_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin6_1);
  temp.push_back(CharaAState::Kin6_2);
  temp.push_back(CharaAState::Kin6_3);
  temp.push_back(CharaAState::Kin6_4);
  temp.push_back(CharaAState::Kin6_5);
  temp.push_back(CharaAState::Kin6_6);
  temp.push_back(CharaAState::Kin6_7);
  temp.push_back(CharaAState::Kin6_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin7_1);
  temp.push_back(CharaAState::Kin7_2);
  temp.push_back(CharaAState::Kin7_3);
  temp.push_back(CharaAState::Kin7_4);
  temp.push_back(CharaAState::Kin7_5);
  temp.push_back(CharaAState::Kin7_6);
  temp.push_back(CharaAState::Kin7_7);
  temp.push_back(CharaAState::Kin7_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin8_1);
  temp.push_back(CharaAState::Kin8_2);
  temp.push_back(CharaAState::Kin8_3);
  temp.push_back(CharaAState::Kin8_4);
  temp.push_back(CharaAState::Kin8_5);
  temp.push_back(CharaAState::Kin8_6);
  temp.push_back(CharaAState::Kin8_7);
  temp.push_back(CharaAState::Kin8_8);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(CharaAState::Kin9_1);
  temp.push_back(CharaAState::Kin9_2);
  temp.push_back(CharaAState::Kin9_3);
  temp.push_back(CharaAState::Kin9_4);
  temp.push_back(CharaAState::Kin9_5);
  temp.push_back(CharaAState::Kin9_6);
  temp.push_back(CharaAState::Kin9_7);
  temp.push_back(CharaAState::Kin9_8);
  kin.push_back(temp);

  // map
  down[Dir6] = CharaAState::Down1;
  down[Dir4] = CharaAState::Down2;

  // ミサイル出現位置
  for (unsigned int i = 0; i != Direction::End; ++i) {
    double dist = 20;
    double rad = ObjectFunc::GetRadian(i);
    double diff = 1.5;
    DPos fact1 = ObjectFunc::GetDirFacter(rad + diff);
    DPos fact2 = ObjectFunc::GetDirFacter(rad - diff);

    vector<DPos> temp;
    temp.push_back(DPos(dist * fact1.x, dist * fact1.y));
    temp.push_back(DPos(dist * fact2.x, dist * fact2.y));
    missile_pos.push_back(temp);
  }

  // エネルギー弾出現位置
  ene_pos.push_back(DPos(-36, 6));
  ene_pos.push_back(DPos(-11, 20));
  ene_pos.push_back(DPos(27, 18));
  ene_pos.push_back(DPos(-49, -20));
  ene_pos.push_back(DPos(59, -14));
  ene_pos.push_back(DPos(-20, -50));
  ene_pos.push_back(DPos(12, -57));
  ene_pos.push_back(DPos(34, -51));
}

void Planex::AddWeapon1()
{
  static const double mis_dir = 0.2;
  objs->AddObject(new PlanexMissile(mis_dir, objs, pos + missile_pos[body_dir][0], target, team));
  objs->AddObject(new PlanexMissile(-mis_dir, objs, pos + missile_pos[body_dir][1], target, team));
}

// １フレームの処理
void Planex::OneFrameProc()
{
  switch (motion.type) {
  case PlayerMotion::None:
    // 移動方向がない場合
    if (motion.dir == Direction::None) {
      // 立ちモーションとみなす
      state = stand[body_dir];
    }
    // 歩き
    state = walk[body_dir];
    break;

  case PlayerMotion::Weapon1:
  case PlayerMotion::DashWeapon1:
    // 速度更新
    // 歩きと同様の移動
    if (motion.type == PlayerMotion::Weapon1) {
      speed = GetWalkSpeed();
      speed *= 0.90;
    } else {
      speed.x *= 0.95;
      speed.y *= 0.95;
    }

    // 絵の更新
    state = stand[body_dir];

    if (frame == 2) {
      AddWeapon1();
    }
    else if (frame == 5) {
      AddWeapon1();
    }
    else if (frame == 8) {
      AddWeapon1();
    }
    else if (frame >= 30) {
      BeginMove();
    }
    break;

  case PlayerMotion::Weapon2:
  case PlayerMotion::DashWeapon2:
    // 速度更新
    speed *= 0.8;

    // 絵を変更
    state = weapon[body_dir];

    if (frame <= 1) {
      
    } else if (frame == 2) {
      t_dir = ObjectFunc::GetRadian(pos, target->GetPos());
      BodyDirLock();
    } else if (frame == 5) {
      objs->AddObject(new PlanexEnergyShot(objs, pos + ene_pos[body_dir], target, team, t_dir));
      DPos fact = ObjectFunc::GetDirFacter(body_dir);
      speed += fact * -12.0;
      break;
    } else if (frame >= 25) {
      BeginMove();      
    }
    break;

  case PlayerMotion::Dash:
  case PlayerMotion::DashSpatia:
    // 絵を変更
    {
      static bool ahya = true;;
      if (ahya) {
	state = walk[body_dir];
	ahya = false;
      }
      else {
	state = dash[body_dir];
	ahya = true;
      }
      break;
    }

  case PlayerMotion::CloseRange:
  case PlayerMotion::DashCloseRange:
    {
      // 絵変更
      switch (frame) {
      case 1:
	stop = false;
	state = kin[body_dir][0];
	break;
      case 3:
	state = kin[body_dir][1];
	break;
      case 8:
	BodyDirLock();
	state = kin[body_dir][2];
	break;
      case 9:
	state = kin[body_dir][3];
	break;
      case 10:
	speed.x = speed.y = 0;
	stop = true;
	state = kin[body_dir][4];
	objs->AddObject(new PlanexCloseRangeHitDetect(objs, pos, target, body_dir, team));
	break;
      case 12:
	state = kin[body_dir][5];
	break;
      case 14:
	state = kin[body_dir][6];
	break;
      case 16:
	state = kin[body_dir][7];
	break;
      case 27:
	BeginMove();
	break;
      }

      // 敵の方向に調節しつつ速度を更新
      if (frame >= 2) {
	if (stop == false) {
	  // 角度調整
	  double rad = ObjectFunc::GetRadian(pos, target->GetPos());
	  ObjectFunc::DirAdjust(appr_dir, rad, param.tracking);

	  // 速度と向き更新
	  DPos dir_fact = ObjectFunc::GetDirFacter(appr_dir);
	  speed.x = dir_fact.x * param.close_range_speed;
	  speed.y = dir_fact.y * param.close_range_speed;
	  if (body_dir_lock == false) {
	    body_dir = ObjectFunc::GetDirection(appr_dir);
	  }
	  break;
	}
      }
    }
    break;

  case PlayerMotion::FlyDamage:
  case PlayerMotion::Bound:
  case PlayerMotion::Down:
    state = down[body_dir];
    break;

  case PlayerMotion::StaggerDamage:
    state = kin[body_dir][1];
    break;

  case PlayerMotion::Explosion:
    state = down[body_dir];
    if (frame == 30) {
      erase = true;
      objs->AddObject(new ExplosionAx4(pos));
    }
    break;

  default:
    break;
  }
}
