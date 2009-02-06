#include <cmath>
#include "yamaha.hpp"
#include "yamaha_weapon.hpp"
#include "effect.hpp"
#include "shell.hpp"

using namespace std;
using namespace zu3ga;

const double zu3ga::YamahaBomb::diff_speed;
const double zu3ga::YamahaBomb::vms;

const PlayerParam Yamaha::yamaha_param = PlayerParam(2100, // max_hp
						     160, // dash_gauge
						     12.0, // walk_speed
						     23.0, // dash_speed
						     3.5, // slow_damage_speed
						     7.0, // fast_damage_speed
						     26.0, // close_range_speed
						     1.5, // tracking
						     ObjDetection(DetectionType::Coll,
								  25, 8, 17, 17)); // own_obj_det


Yamaha::Yamaha(Team::t team, Objects* objs)
  : Player(team, objs, yamaha_param)
  , stand_frame(0)
  , stop(false)
{
  // SendObjTypeの変更
  switch (team) {
  case Team::I:
    type = SendObjType::YamahaI;
    break;
  case Team::Ro:
    type = SendObjType::YamahaRo;
    break;
  case Team::Ha:
    type = SendObjType::YamahaHa;
    break;
  case Team::Ni:
    type = SendObjType::YamahaNi;
    break;
  default:
    throw logic_error("Yamaha::Yamaha: 正しくないチームでプレイヤーが初期化されました．team = "
		      + int(team));
    break;
  }

  // 画像IDのmapの作成
  using namespace Direction;

  // 立ち絵
  stand.reserve(End);
  stand[Dir1] = YamahaState::Stand1;
  stand[Dir2] = YamahaState::Stand2;
  stand[Dir3] = YamahaState::Stand3;
  stand[Dir4] = YamahaState::Stand4;
  stand[Dir6] = YamahaState::Stand6;
  stand[Dir7] = YamahaState::Stand7;
  stand[Dir8] = YamahaState::Stand8;
  stand[Dir9] = YamahaState::Stand9;

  // 前に進む絵
  ahead.reserve(End);
  ahead[Dir1] = YamahaState::Ahead1;
  ahead[Dir2] = YamahaState::Ahead2;
  ahead[Dir3] = YamahaState::Ahead3;
  ahead[Dir4] = YamahaState::Ahead4;
  ahead[Dir6] = YamahaState::Ahead6;
  ahead[Dir7] = YamahaState::Ahead7;
  ahead[Dir8] = YamahaState::Ahead8;
  ahead[Dir9] = YamahaState::Ahead9;

  // 後ろに進む絵
  back.reserve(End);
  back[Dir1] = YamahaState::Back1;
  back[Dir2] = YamahaState::Back2;
  back[Dir3] = YamahaState::Back3;
  back[Dir4] = YamahaState::Back4;
  back[Dir6] = YamahaState::Back6;
  back[Dir7] = YamahaState::Back7;
  back[Dir8] = YamahaState::Back8;
  back[Dir9] = YamahaState::Back9;

  // レーザー絵
  vector<short> l;
  l.push_back(YamahaState::Laser1_1);
  l.push_back(YamahaState::Laser1_2);
  l.push_back(YamahaState::Laser1_3);
  l.push_back(YamahaState::Laser1_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser2_1);
  l.push_back(YamahaState::Laser2_2);
  l.push_back(YamahaState::Laser2_3);
  l.push_back(YamahaState::Laser2_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser3_1);
  l.push_back(YamahaState::Laser3_2);
  l.push_back(YamahaState::Laser3_3);
  l.push_back(YamahaState::Laser3_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser4_1);
  l.push_back(YamahaState::Laser4_2);
  l.push_back(YamahaState::Laser4_3);
  l.push_back(YamahaState::Laser4_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser6_1);
  l.push_back(YamahaState::Laser6_2);
  l.push_back(YamahaState::Laser6_3);
  l.push_back(YamahaState::Laser6_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser7_1);
  l.push_back(YamahaState::Laser7_2);
  l.push_back(YamahaState::Laser7_3);
  l.push_back(YamahaState::Laser7_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser8_1);
  l.push_back(YamahaState::Laser8_2);
  l.push_back(YamahaState::Laser8_3);
  l.push_back(YamahaState::Laser8_4);
  laser.push_back(l);
  l.clear();
  l.push_back(YamahaState::Laser9_1);
  l.push_back(YamahaState::Laser9_2);
  l.push_back(YamahaState::Laser9_3);
  l.push_back(YamahaState::Laser9_4);
  laser.push_back(l);
  l.clear();

  // 近距離攻撃絵
  vector<short> temp;
  temp.push_back(YamahaState::Kin1_1);
  temp.push_back(YamahaState::Kin1_2);
  temp.push_back(YamahaState::Kin1_3);
  temp.push_back(YamahaState::Kin1_4);
  temp.push_back(YamahaState::Kin1_5);
  temp.push_back(YamahaState::Kin1_6);
  temp.push_back(YamahaState::Kin1_7);
  temp.push_back(YamahaState::Kin1_8);
  temp.push_back(YamahaState::Kin1_9);
  temp.push_back(YamahaState::Kin1_10);
  temp.push_back(YamahaState::Kin1_11);
  temp.push_back(YamahaState::Kin1_12);
  temp.push_back(YamahaState::Kin1_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin2_1);
  temp.push_back(YamahaState::Kin2_2);
  temp.push_back(YamahaState::Kin2_3);
  temp.push_back(YamahaState::Kin2_4);
  temp.push_back(YamahaState::Kin2_5);
  temp.push_back(YamahaState::Kin2_6);
  temp.push_back(YamahaState::Kin2_7);
  temp.push_back(YamahaState::Kin2_8);
  temp.push_back(YamahaState::Kin2_9);
  temp.push_back(YamahaState::Kin2_10);
  temp.push_back(YamahaState::Kin2_11);
  temp.push_back(YamahaState::Kin2_12);
  temp.push_back(YamahaState::Kin2_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin3_1);
  temp.push_back(YamahaState::Kin3_2);
  temp.push_back(YamahaState::Kin3_3);
  temp.push_back(YamahaState::Kin3_4);
  temp.push_back(YamahaState::Kin3_5);
  temp.push_back(YamahaState::Kin3_6);
  temp.push_back(YamahaState::Kin3_7);
  temp.push_back(YamahaState::Kin3_8);
  temp.push_back(YamahaState::Kin3_9);
  temp.push_back(YamahaState::Kin3_10);
  temp.push_back(YamahaState::Kin3_11);
  temp.push_back(YamahaState::Kin3_12);
  temp.push_back(YamahaState::Kin3_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin4_1);
  temp.push_back(YamahaState::Kin4_2);
  temp.push_back(YamahaState::Kin4_3);
  temp.push_back(YamahaState::Kin4_4);
  temp.push_back(YamahaState::Kin4_5);
  temp.push_back(YamahaState::Kin4_6);
  temp.push_back(YamahaState::Kin4_7);
  temp.push_back(YamahaState::Kin4_8);
  temp.push_back(YamahaState::Kin4_9);
  temp.push_back(YamahaState::Kin4_10);
  temp.push_back(YamahaState::Kin4_11);
  temp.push_back(YamahaState::Kin4_12);
  temp.push_back(YamahaState::Kin4_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin6_1);
  temp.push_back(YamahaState::Kin6_2);
  temp.push_back(YamahaState::Kin6_3);
  temp.push_back(YamahaState::Kin6_4);
  temp.push_back(YamahaState::Kin6_5);
  temp.push_back(YamahaState::Kin6_6);
  temp.push_back(YamahaState::Kin6_7);
  temp.push_back(YamahaState::Kin6_8);
  temp.push_back(YamahaState::Kin6_9);
  temp.push_back(YamahaState::Kin6_10);
  temp.push_back(YamahaState::Kin6_11);
  temp.push_back(YamahaState::Kin6_12);
  temp.push_back(YamahaState::Kin6_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin7_1);
  temp.push_back(YamahaState::Kin7_2);
  temp.push_back(YamahaState::Kin7_3);
  temp.push_back(YamahaState::Kin7_4);
  temp.push_back(YamahaState::Kin7_5);
  temp.push_back(YamahaState::Kin7_6);
  temp.push_back(YamahaState::Kin7_7);
  temp.push_back(YamahaState::Kin7_8);
  temp.push_back(YamahaState::Kin7_9);
  temp.push_back(YamahaState::Kin7_10);
  temp.push_back(YamahaState::Kin7_11);
  temp.push_back(YamahaState::Kin7_12);
  temp.push_back(YamahaState::Kin7_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin8_1);
  temp.push_back(YamahaState::Kin8_2);
  temp.push_back(YamahaState::Kin8_3);
  temp.push_back(YamahaState::Kin8_4);
  temp.push_back(YamahaState::Kin8_5);
  temp.push_back(YamahaState::Kin8_6);
  temp.push_back(YamahaState::Kin8_7);
  temp.push_back(YamahaState::Kin8_8);
  temp.push_back(YamahaState::Kin8_9);
  temp.push_back(YamahaState::Kin8_10);
  temp.push_back(YamahaState::Kin8_11);
  temp.push_back(YamahaState::Kin8_12);
  temp.push_back(YamahaState::Kin8_13);
  kin.push_back(temp);
  temp.clear();
  temp.push_back(YamahaState::Kin9_1);
  temp.push_back(YamahaState::Kin9_2);
  temp.push_back(YamahaState::Kin9_3);
  temp.push_back(YamahaState::Kin9_4);
  temp.push_back(YamahaState::Kin9_5);
  temp.push_back(YamahaState::Kin9_6);
  temp.push_back(YamahaState::Kin9_7);
  temp.push_back(YamahaState::Kin9_8);
  temp.push_back(YamahaState::Kin9_9);
  temp.push_back(YamahaState::Kin9_10);
  temp.push_back(YamahaState::Kin9_11);
  temp.push_back(YamahaState::Kin9_12);
  temp.push_back(YamahaState::Kin9_13);
  kin.push_back(temp);

  // downの画像
  down[Dir4] = YamahaState::Down4;
  down[Dir6] = YamahaState::Down6;

  // 爆弾の出る位置
  for (unsigned int i = 0; i != Direction::End; ++i) {
    double dist = 55;
    double rad = ObjectFunc::GetRadian(i);
    double diff = 0.77;
    vector<DPos> temp;
    for (unsigned int j = 0; j < bomb_num; ++j) {
      temp.push_back(ObjectFunc::GetDirFacter(rad + diff * (-int(bomb_num) / 2 + int(j))) * dist);
    }
    bomb.push_back(temp);
  }

  // 近距離攻撃の判定の出る位置
  for (unsigned int i = 0; i != Direction::End; ++i) {
    double dist = 40;
    double rad = ObjectFunc::GetRadian(i);
    double diff = 0.25;
    DPos fact1 = ObjectFunc::GetDirFacter(rad - diff);
    DPos fact2 = ObjectFunc::GetDirFacter(rad + diff);
    vector<DPos> temp;
    temp.push_back(fact1 * dist);
    temp.push_back(fact2 * dist);
    kin_pos.push_back(temp);
  }
  kin_pos[Direction::Dir1][0].x -= 5;
  kin_pos[Direction::Dir3][0].x -= 5;
  kin_pos[Direction::Dir3][0].y -= 5;
  kin_pos[Direction::Dir4][0].y -= 30;
  kin_pos[Direction::Dir6][0].y -= 20;
  kin_pos[Direction::Dir7][0].y -= 20;
  kin_pos[Direction::Dir8][0].y -= 20;
  kin_pos[Direction::Dir9][0].y -= 20;
  kin_pos[Direction::Dir1][1].x -= 5;
  kin_pos[Direction::Dir3][1].x -= 5;
  kin_pos[Direction::Dir3][1].y -= 5;
  kin_pos[Direction::Dir4][1].y -= 20;
  kin_pos[Direction::Dir6][1].y -= 30;
  kin_pos[Direction::Dir7][1].y -= 20;
  kin_pos[Direction::Dir8][1].y -= 20;
  kin_pos[Direction::Dir9][1].y -= 20;

  // レーザーの出現位置
  for (unsigned int i = 0; i != Direction::End; ++i) {
    vector<DPos> temp;
    double dist = 60;
    double rad = ObjectFunc::GetRadian(i);
    double diff = 0.4;
    temp.push_back(ObjectFunc::GetDirFacter(rad+diff) * dist);
    temp.push_back(ObjectFunc::GetDirFacter(rad-diff) * dist);
    laser_pos.push_back(temp);
    temp.clear();
  }
}

// １フレームの処理
void Yamaha::OneFrameProc()
{
  // 前から後ろに移るとき立ちモーションを表示する残りフレーム数
  --stand_frame;

  static double fms = 4.0;	// float_max_speed
  static double acc = 0.7;

  switch (motion.type) {
  case PlayerMotion::None:
  case PlayerMotion::Dash:
    // 移動方向がない場合
    if (motion.dir == Direction::None) {
      // 立ちモーションとみなす
      state = stand[body_dir];
      break;
    }

    if (ver_pos > -25) {
      ver_speed -= gravity + acc;
      if (ver_speed + gravity < -fms) {
	ver_speed = -fms - gravity;
      }
    } else {
      ver_speed -= gravity - acc;
      if (ver_speed + gravity > fms) {
	ver_speed = fms - gravity;
      }
    }

    // 歩きかダッシュ
    // 立ちを表示するフレームが残っているなら
    if (stand_frame > 0) {
      state = stand[body_dir];
    }
    // 前向きの場合
    else if (IsGettingAhead()) {
      // 後ろ向きから直接前にはならない
      if (unsigned(back[Direction::Dir1]) <= state &&
	  state <= unsigned(back[Direction::Dir9])) {
	//	stand_frame = 7;
	state = stand[body_dir];
      } else {
	state = ahead[body_dir];
      }
    } 
    // 後ろ向きの場合
    else {
      if (unsigned(ahead[Direction::Dir1]) <= state &&
	  state <= unsigned(ahead[Direction::Dir9])) {
	//	stand_frame = 7;
	state = stand[body_dir];
      } else {
	state = back[body_dir];
      }
    }

    // ダッシュの場合に残像を追加する
    if (motion.type == PlayerMotion::Dash) {
      // 並びかた依存
      // 今と同じ、透明の状態を得る
      short z_state = state + 16;
      DPos z_pos = pos;
      z_pos.y += ver_pos;
      objs->AddObject(new Zanzou(type, z_state, z_pos));
    }
    break;
  case PlayerMotion::Weapon1:
  case PlayerMotion::DashWeapon1:
    {
      speed *= 0.9;
      if (frame == 2) {
	t_dir = ObjectFunc::GetRadian(pos, target->GetPos());
	BodyDirLock();
	state = back[ObjectFunc::GetRotRn(body_dir, 0)];
      } else if (frame == 3) {
	state = back[ObjectFunc::GetRotRn(body_dir, 1)];
      } else if (frame == 4) {
	state = back[ObjectFunc::GetRotRn(body_dir, 2)];
      } else if (frame == 5) {
	state = back[ObjectFunc::GetRotRn(body_dir, 3)];
      } else if (frame == 6) {
	state = back[ObjectFunc::GetRotRn(body_dir, 4)];
      }	else if (frame == 7) {
	state = back[ObjectFunc::GetRotRn(body_dir, 5)];
      }	else if (frame == 8) {
	state = back[ObjectFunc::GetRotRn(body_dir, 6)];
      }	else if (frame == 9) {
	state = back[ObjectFunc::GetRotRn(body_dir, 7)];
      } else if (frame == 10) {
	AddBomb(t_dir, true);
	speed += ObjectFunc::GetDirFacter(t_dir) * -3.0;
	state = back[ObjectFunc::GetRotRn(body_dir, 0)];
      } 
      else if (frame == 14) {
	BodyDirUnlock();
	TurnOnTarget();
	t_dir = ObjectFunc::GetRadian(pos, target->GetPos());
	BodyDirLock();
      }
      else if (frame == 22) {
	AddBomb(t_dir);
	speed += ObjectFunc::GetDirFacter(t_dir) * -3.0;
	state = back[body_dir];
      } else if (frame == 45) {
	BeginMove();
      }
    }
    break;
  case PlayerMotion::Weapon2:
  case PlayerMotion::DashWeapon2:
    // 速度更新
    speed *= 0.95;

    // 絵を変更
    switch (frame) {
    case 1:
      state = laser[body_dir][0];
      break;
    case 3:
      state = laser[body_dir][1];
      break;
    case 5:
      t_dir = ObjectFunc::GetRadian(pos, target->GetPos());
      BodyDirLock();
      state = laser[body_dir][2];
      break;
    case 7:
      {
	state =laser[body_dir][3];
	DPos fact = ObjectFunc::GetDirFacter(body_dir);
	speed += fact * -2.0;
	//ver_speed -= 8.0;
	objs->AddObject(new YamahaLaserShot(t_dir,
					    objs,  pos + laser_pos[body_dir][0], target, team));
	objs->AddObject(new YamahaLaserShot(t_dir,
					    objs, pos + laser_pos[body_dir][1], target, team));
      }
      break;
    case 15:
      {
	state =laser[body_dir][3];
	DPos fact = ObjectFunc::GetDirFacter(body_dir);
	speed += fact * -2.0;
	//ver_speed -= 8.0;
	objs->AddObject(new YamahaLaserShot(t_dir,
					    objs,  pos + laser_pos[body_dir][0], target, team));
	objs->AddObject(new YamahaLaserShot(t_dir,
					    objs, pos + laser_pos[body_dir][1], target, team));
      }
      break;
    case 37:
      BeginMove();
      break;
    }
    break;

  case PlayerMotion::DashSpatia:
    BeginMove();
    break;
  case PlayerMotion::CloseRange:
  case PlayerMotion::DashCloseRange:
    {
      // 絵変更
      switch (frame) {
      case 1:
	stop = false;
	state = kin[body_dir][0];
	break;
      case 6:
	BodyDirLock();
	state = kin[body_dir][1];
	break;
      case 7:
	state = kin[body_dir][2];
	break;
      case 8:
	state = kin[body_dir][3];
	break;
      case 9:
	speed.x = speed.y = 0;
	stop = true;
	state = kin[body_dir][4];
	objs->AddObject(new YamahaCloseRangeHitDetect(objs, pos+kin_pos[body_dir][0], 
						      target, body_dir, team, pos));
	break;
      case 12:
	stop = false;
	state = kin[body_dir][5];
	break;
      case 13:
	state = kin[body_dir][6];
	break;
      case 14:
	state = kin[body_dir][7];
	break;
      case 15:
	stop = true;
	state = kin[body_dir][8];
	objs->AddObject(new YamahaCloseRangeHitDetect(objs, pos+kin_pos[body_dir][1],
						      target, body_dir, team, pos, true));
	break;
      case 19:
	state = kin[body_dir][9];
	break;
      case 20:
	state = kin[body_dir][10];
	break;
      case 21:
	state = kin[body_dir][11];
	break;
      case 22:
	state = kin[body_dir][12];
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
    state = back[body_dir];
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

SendPos Yamaha::GetShadowPos()
{
  SendPos spos(short(pos.x), short(pos.y));
  switch (state) {
  case YamahaState::Ahead1:
    spos.y += 5;
    spos.x -= 5;
    break;
  case YamahaState::Ahead2:
    spos.y += 10;
    break;
  case YamahaState::Ahead3:
    spos.y += 5;
    spos.x += 5;
    break;
  case YamahaState::Ahead4:
    spos.x -= 10;
    break;
  case YamahaState::Ahead6:
    spos.x += 10;
    break;
  case YamahaState::Ahead7:
    spos.y -= 5;
    spos.x -= 5;
    break;
  case YamahaState::Ahead8:
    spos.y -= 3;
    break;
  case YamahaState::Ahead9:
    spos.y -= 5;
    spos.x += 5;
    break;
  default:
    break;
  }
  return spos;
}

void Yamaha::AddBomb(double target_dir, bool second)
{
  DPos dpos = pos;
  dpos.y += 20;
  if (second) {
    for (unsigned int i = 0; i < 4; ++i) {
      objs->AddObject(new YamahaBomb(target_dir, objs, dpos, team, M_PI + 0.5 * (i-2.0)));
    }
  } else {
    for (unsigned int i = 0; i < 3; ++i) {
      objs->AddObject(new YamahaBomb(target_dir, objs, dpos, team, M_PI + 0.5 * (i-1.5)));
    }
  } 

  /*
    for (unsigned int i = 0; i < bomb_num; ++i) {
    objs->AddObject(new YamahaBomb(target_dir,
    objs, pos + bomb[body_dir][i],
    team));
    }
  */
}
