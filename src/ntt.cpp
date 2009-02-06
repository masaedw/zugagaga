/// last modified <2007/01/27 14:21:09 2007>
#include "ntt.hpp"
#include "ntt_weapon.hpp"
#include "effect.hpp"
#include "shell.hpp"

using namespace std;
using namespace zu3ga;

const double zu3ga::NTT::attack_speed;

const PlayerParam NTT::ntt_param = PlayerParam(2400, // max_hp
					       150, // dash_gauge
					       13.0, // walk_speed
					       26.0, // dash_speed
					       3.5, // slow_damage_speed
					       7.0, // fast_damage_speed
					       26.0, // close_range_speed
					       1.5, // tracking
					       ObjDetection(DetectionType::Coll,
							    25, 8, 16, 16)); // own_obj_det


NTT::NTT(Team::t team, Objects* objs)
  : Player(team, objs, ntt_param)
  , stop(false)
{
  type = SendObjType::NTT;

  // SendObjTypeの変更
  switch (team) {
  case Team::I:
    type = SendObjType::NTTI;
    break;
  case Team::Ro:
    type = SendObjType::NTTRo;
    break;
  case Team::Ha:
    type = SendObjType::NTTHa;
    break;
  case Team::Ni:
    type = SendObjType::NTTNi;
    break;
  default:
    throw logic_error("NTT::NTT: 正しくないチームでプレイヤーが初期化されました．team = "
		      + int(team));
    break;
  }

  // 画像IDのmapの作成
  using namespace Direction;

  // 立ち絵
  stand.reserve(End);
  stand[Dir1] = NTTState::Stand1;
  stand[Dir2] = NTTState::Stand2;
  stand[Dir3] = NTTState::Stand3;
  stand[Dir4] = NTTState::Stand4;
  stand[Dir6] = NTTState::Stand6;
  stand[Dir7] = NTTState::Stand7;
  stand[Dir8] = NTTState::Stand8;
  stand[Dir9] = NTTState::Stand9;
  // ダッシュ
  dash.reserve(End);
  dash[Dir1] = NTTState::Dash1;
  dash[Dir2] = NTTState::Dash2;
  dash[Dir3] = NTTState::Dash3;
  dash[Dir4] = NTTState::Dash4;
  dash[Dir6] = NTTState::Dash6;
  dash[Dir7] = NTTState::Dash7;
  dash[Dir8] = NTTState::Dash8;
  dash[Dir9] = NTTState::Dash9;
  // 武器を持っている姿
  weapon.reserve(End);
  weapon[Dir1] = NTTState::Weapon1;
  weapon[Dir2] = NTTState::Weapon2;
  weapon[Dir3] = NTTState::Weapon3;
  weapon[Dir4] = NTTState::Weapon4;
  weapon[Dir6] = NTTState::Weapon6;
  weapon[Dir7] = NTTState::Weapon7;
  weapon[Dir8] = NTTState::Weapon8;
  weapon[Dir9] = NTTState::Weapon9;
  // 突進
  attack.reserve(End);
  attack[Dir1] = NTTState::Attack1;
  attack[Dir2] = NTTState::Attack2;
  attack[Dir3] = NTTState::Attack3;
  attack[Dir4] = NTTState::Attack4;
  attack[Dir6] = NTTState::Attack6;
  attack[Dir7] = NTTState::Attack7;
  attack[Dir8] = NTTState::Attack8;
  attack[Dir9] = NTTState::Attack9;

  // 近距離
  {
    vector<short> temp;
    temp.push_back(NTTState::Kin1_1);
    temp.push_back(NTTState::Kin1_2);
    temp.push_back(NTTState::Kin1_3);
    temp.push_back(NTTState::Kin1_4);
    temp.push_back(NTTState::Kin1_5);
    temp.push_back(NTTState::Kin1_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin2_1);
    temp.push_back(NTTState::Kin2_2);
    temp.push_back(NTTState::Kin2_3);
    temp.push_back(NTTState::Kin2_4);
    temp.push_back(NTTState::Kin2_5);
    temp.push_back(NTTState::Kin2_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin3_1);
    temp.push_back(NTTState::Kin3_2);
    temp.push_back(NTTState::Kin3_3);
    temp.push_back(NTTState::Kin3_4);
    temp.push_back(NTTState::Kin3_5);
    temp.push_back(NTTState::Kin3_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin4_1);
    temp.push_back(NTTState::Kin4_2);
    temp.push_back(NTTState::Kin4_3);
    temp.push_back(NTTState::Kin4_4);
    temp.push_back(NTTState::Kin4_5);
    temp.push_back(NTTState::Kin4_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin6_1);
    temp.push_back(NTTState::Kin6_2);
    temp.push_back(NTTState::Kin6_3);
    temp.push_back(NTTState::Kin6_4);
    temp.push_back(NTTState::Kin6_5);
    temp.push_back(NTTState::Kin6_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin7_1);
    temp.push_back(NTTState::Kin7_2);
    temp.push_back(NTTState::Kin7_3);
    temp.push_back(NTTState::Kin7_4);
    temp.push_back(NTTState::Kin7_5);
    temp.push_back(NTTState::Kin7_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin8_1);
    temp.push_back(NTTState::Kin8_2);
    temp.push_back(NTTState::Kin8_3);
    temp.push_back(NTTState::Kin8_4);
    temp.push_back(NTTState::Kin8_5);
    temp.push_back(NTTState::Kin8_6);
    kin.push_back(temp);
    temp.clear();
    temp.push_back(NTTState::Kin9_1);
    temp.push_back(NTTState::Kin9_2);
    temp.push_back(NTTState::Kin9_3);
    temp.push_back(NTTState::Kin9_4);
    temp.push_back(NTTState::Kin9_5);
    temp.push_back(NTTState::Kin9_6);
    kin.push_back(temp);
    temp.clear();
  }

  // downの画像
  down[Dir4] = NTTState::Down4;
  down[Dir6] = NTTState::Down6;

  // 武器の出る場所
  w1_pos.reserve(End);
  w1_pos[Dir1] = DPos(-1, -18);
  w1_pos[Dir2] = DPos(8, -13);
  w1_pos[Dir3] = DPos(2, -20);
  w1_pos[Dir4] = DPos(-6, -23);
  w1_pos[Dir6] = DPos(10, -23);
  w1_pos[Dir7] = DPos(-9, -29);
  w1_pos[Dir8] = DPos(-9, -34);
  w1_pos[Dir9] = DPos(5, -35);

  // 突進の当たり判定の出る場所
  at_pos.reserve(End);
  at_pos[Dir1] = DPos(-12, 4);
  at_pos[Dir2] = DPos(-1, 10);
  at_pos[Dir3] = DPos(16, 5);
  at_pos[Dir4] = DPos(-15, -1);
  at_pos[Dir6] = DPos(17, -3);
  at_pos[Dir7] = DPos(-13, -10);
  at_pos[Dir8] = DPos(0, -15);
  at_pos[Dir9] = DPos(13, -10);

  // 近距離攻撃の判定の出る位置
  for (unsigned int i = 0; i != Direction::End; ++i) {
    double dist1 = 40;
    double dist2 = 110;
    double dist3 = 155;
    vector<DPos> temp;
    temp.push_back(ObjectFunc::GetDirFacter(Direction::t(i)) * dist1);
    temp.push_back(ObjectFunc::GetDirFacter(Direction::t(i)) * dist2);
    temp.push_back(ObjectFunc::GetDirFacter(Direction::t(i)) * dist3);
    kin_pos.push_back(temp);
  }
  for (unsigned int i = 0; i != Direction::End; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      kin_pos[i][j].y -= 10;
    }
  }
}

// １フレームの処理
void NTT::OneFrameProc()
{
  switch (motion.type) {
  case PlayerMotion::None:
    state = stand[body_dir];
    break;
  case PlayerMotion::Dash:
    if (frame % 2 == 0) {
      state = dash[body_dir];
    } else {
      state = stand[body_dir];
    }
    break;
  case PlayerMotion::Weapon1:
  case PlayerMotion::DashWeapon1:
    state = weapon[body_dir];
    speed *= 0.91;
    if (frame == 2) {
      objs->AddObject(new NTTEnergyShot(objs, pos + w1_pos[body_dir], target, team));
    }
    if (frame == 5) {
      objs->AddObject(new NTTEnergyShot(objs, pos + w1_pos[body_dir], target, team));
    }
    if (frame == 8) {
      objs->AddObject(new NTTEnergyShot(objs, pos + w1_pos[body_dir], target, team));
    }
    if (frame == 11) {
      objs->AddObject(new NTTEnergyShot(objs, pos + w1_pos[body_dir], target, team));
    }
    if (frame == 14) {
      objs->AddObject(new NTTEnergyShot(objs, pos + w1_pos[body_dir], target, team));
    }
    if (frame == 17) {
      objs->AddObject(new NTTEnergyShot(objs, pos + w1_pos[body_dir], target, team));
    }
    if (frame == 30) {
      BeginMove();
    }
    break;
  case PlayerMotion::Weapon2:
  case PlayerMotion::DashWeapon2:
    // はじめのフレーム
    if (frame == 1) {
      attack_end = 0;
      state = stand[body_dir];
    }
    //---------------
    //特殊な状況
    //---------------
    if (attack_end) {
      //----------------------------------------
      // スライディング状態がはじまったところ
      //----------------------------------------
      if (attack_end == 1) {
	// スライディング状態途中にする
	attack_end = 2;
	frame = 100;
      }
      if (attack_end == 2) {
	speed *= 0.9;
	objs->AddObject(new SmokeB(pos));
	if (frame >= 120) {
	  BeginMove();
	}
      }
      //--------------------
      // 物体に当たった瞬間
      //--------------------
      if (attack_end == 3) {
	attack_end = 4;
	DPos s = ObjectFunc::GetDirFacter(body_dir);
	s *= -1;
	speed = s * 5;
	ver_speed = -10;
	state = stand[body_dir];
      } else if (attack_end == 4) {
	if (ver_pos > -10) {
	  BeginMove();
	}
      }
    }
    //--------------------
    // 普通に移動する状態
    //--------------------
    else {
      // 入力の向きによって突進方向を変える
      if (frame >= 2) {
	BodyDirLock();
	Direction::t d = kecreater.GetKeyDirection();
	if (d == Direction::None) {
	  d = body_dir;
	}
	body_dir = d;
	speed = ObjectFunc::GetDirFacter(d) * attack_speed;
	// 絵の変更
	state = attack[d];
	// 当たり判定追加
	DPos dpos = pos;
	dpos.y += ver_pos;
	objs->AddObject(new NTTAttackHitDetect(this, objs, dpos + at_pos[d], target, team));
      }

      // 浮き
      // 出始めに浮く
      if (frame == 2) {
	ver_pos = -1;
	ver_speed = -3.0;
      }
      // 重力を打ち消す
      if (2 <= frame && frame <= 18) {
	ver_speed -= gravity;
      }
      // 微妙に落下
      if (frame >= 13) {
	ver_speed += 0.5;
	// 地面についたら終了
	if (ver_pos >= 0) {
	  attack_end = 1;
	}
      }
    }
    break;
  case PlayerMotion::DashSpatia:
    BeginMove();
    break;
  case PlayerMotion::CloseRange:
  case PlayerMotion::DashCloseRange:
    {
      if (frame == 1) {
	speed *= 0;
	BodyDirLock();
	stop = true;
      } else if (frame == 2) {
	state = kin[body_dir][1];
      } else if (frame == 3) {
	state = kin[body_dir][2];
      } else if (frame == 4) {
	state = kin[body_dir][3];
      } else if (5 <= frame && frame <= 15) {
	state = kin[body_dir][4];
	objs->AddObject(new NTTKinHitDetect(objs, pos + kin_pos[body_dir][0], target, team));
	objs->AddObject(new NTTKinHitDetect(objs, pos + kin_pos[body_dir][1], target, team, 1));
	objs->AddObject(new NTTKinHitDetect(objs, pos + kin_pos[body_dir][2], target, team, 2));
      } else if (frame == 16) {
	state = kin[body_dir][5];
      } else if (frame == 20) {
	state = stand[body_dir];
      } else if (frame == 26) {
	BeginMove();
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
    state = stand[body_dir];
    break;

  case PlayerMotion::Explosion:
    state = down[body_dir];
    if (frame == 30) {
      erase = true;
      DPos dpos[4];
      double d = 15;
      dpos[0] = DPos(-d, -d);
      dpos[1] = DPos(-d, d);
      dpos[2] = DPos(d, -d);
      dpos[3] = DPos(d, d);
      objs->AddObject(new ExplosionA(pos + dpos[0]));
      objs->AddObject(new ExplosionA(pos + dpos[1]));
      objs->AddObject(new ExplosionA(pos));
      objs->AddObject(new ExplosionA(pos + dpos[2]));
      objs->AddObject(new ExplosionA(pos + dpos[3]));
    }
    break;

  default:
    break;
  }
}

bool NTT::SetDetection(const Detection& det)
{
  if (motion.type == PlayerMotion::Weapon2) {
    if (attack_end == 0) {
      if (det.type == DetectionType::Hit) {
	return true;
      }
    }
  }
  return Player::SetDetection(det);
}

void NTT::SetAttackEnd()
{
  attack_end = 3;
  objs->AddObject(new DotEffect(pos, ObjectFunc::GetRadian(body_dir)));
  objs->AddObject(new DotEffect(pos, ObjectFunc::GetRadian(body_dir)));
  objs->AddObject(new DotEffect(pos, ObjectFunc::GetRadian(body_dir)));
  objs->AddObject(new DotEffect(pos, ObjectFunc::GetRadian(body_dir)));
}
