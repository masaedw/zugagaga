// last modified <2007/01/27 14:16:56 2007>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <memory>
#include "player.hpp"
#include "comtypes.hpp"
#include "graphic.hpp"

using namespace std;
using namespace zu3ga;
using graphic::ScreenWidth;
using graphic::ScreenHeight;

const double zu3ga::Player::down_float;
const double zu3ga::Player::lock_near_dist;
const double zu3ga::Player::e;
const double zu3ga::Player::draw_correct_max_dist;

static DummyPlayer dummy;

//============================================================
// Player::Player
//============================================================
Player::Player(Team::t team, Objects* objs, const PlayerParam& param, bool auto_search)
  : new_target_moving(false)
  , pre_explosion(false)
  , search(false)
  , lock_site(0)
  , weapon_lock(true)
  , param(param)
  , auto_search(auto_search)
  , abs_frame(0)
  , invincib(0)
  , hp(param.max_hp)
  , dash_gauge(param.dash_gauge)
  , body_dir(Direction::Dir2)
  , state(0)
  , pos(DPos(0, 0))
  , speed(DPos(0, 0))
  , team(team)
  , objs(objs)
  , target(0)
  , next_tar(0)
  , nt_frame(0)
  , ver_pos(0)
  , ver_speed(0)
  , body_dir_lock(false)
{}

//============================================================
// Player::ChangeState
// クライアントイベントからプレイヤーの状態を変更する
//============================================================
void Player::ChangeState(unsigned char eve)
{
  PlayerMotion new_motion;
  // 新しいモーションが生まれたら
  if (cmanager.DisposeKeyEvent(kecreater.ConvertClientEvent(eve), new_motion)) {
    // 武器の使用が認められていない場合
    if (weapon_lock) {
      if (new_motion.WeaponType()) {
	return;
      }
    }

    // 動作が新しくなった場合，フレームを0にする
    if (SetNewMotion(motion, new_motion)) {
      frame = 0;
      if (motion.type == PlayerMotion::CloseRange ||
	  motion.type == PlayerMotion::DashCloseRange) {
	appr_dir = ObjectFunc::GetRadian(pos, target->GetPos());
      }
    }
  }
}

// 歩きのときの速度を得る
DPos Player::GetWalkSpeed()
{
  DPos ret = ObjectFunc::GetDirFacter(motion.dir);
  ret *= param.walk_speed;
  return ret;
}

// 速度を更新する
void Player::UpdateSpeed()
{
  ver_speed += gravity;
  if (ver_speed > 0 && ver_pos >= 0) {
    ver_speed = 0;
  }
  switch (motion.type) {
  case PlayerMotion::None:	// 歩きの場合
    speed = GetWalkSpeed();
    break;
  case PlayerMotion::Dash:
    {
      // 0で割ることを防止
      if (dash_gauge == 0) {
	DPos dir_fact = ObjectFunc::GetDirFacter(motion.dir);
	speed = dir_fact * param.walk_speed;
	break;
      }

      // ゲージによって遅くする
      // double ratio;
      // ratio = double(dash_gauge) / param.dash_gauge;
      // ratio = sqrt(ratio); 
      // double spd = param.walk_speed + param.dash_speed * ratio;
      double spd = param.dash_speed;
      DPos dir_fact = ObjectFunc::GetDirFacter(motion.dir);
      speed = dir_fact * spd;
      break;
    }
  case PlayerMotion::DashSpatia:
    speed *= 0.90;
    break;

  case PlayerMotion::StaggerDamage:
    speed *= 0.94;
    break;

  case PlayerMotion::FlyDamage:
  case PlayerMotion::Bound:

    break;

  case PlayerMotion::Down:
  case PlayerMotion::Explosion:
    if (frame == 1) {
      speed.x = speed.y = 0;
    }
    break;

  default:
    break;
  }
}

// 敵リストを作る
void Player::MakeEnemyList()
{
  if (enemy_list.size()) {
    enemy_list.clear();
  }

  PlayerMap* map = objs->GetPlayerMap();

  for (PMIter i = map->begin();
       i != map->end(); ++i) {
    // 違うチームなら
    if (team != i->second->GetTeam()) {
      enemy_list.push_back(i->second);
    }
  }
}

// 引数の次の敵へのポインタを得る
Player* Player::GetNextEnemy(Player* p)
{
  for (vector<Player*>::iterator i = enemy_list.begin();
       i != enemy_list.end(); ++i) {
    if (*i == p) {
      if (++i == enemy_list.end()) {
	i = enemy_list.begin();
      }
      return *i;
    }
  }
  if (enemy_list.size() == 0) {
    return &dummy;
  }
  throw logic_error("次の敵が発見できません");
}

// 最も近い敵の方向を向く
void Player::TurnOnTarget()
{
  // 今回の最も近い敵
  Player* temp = FindNearestEnemy();
  // 一番はじめ
  if (target == 0) {
    target = temp;
    MakeEnemyList();
  }

  if (auto_search) {
    // ロックオンサイトを固定している
    if (lock_site != 0) {
      nt_frame = 0;
      lock_site--;
    }
    // 今のターゲットとは違うターゲットが近い
    else if (target != temp) {
      // 前回の最も近い敵と同じなら
      if (next_tar == temp) {
	++nt_frame;
	if (nt_frame >= next_target_frame) {
	  nt_frame = 0;
	  // 新しい敵にターゲットを合わせる
	  target = temp;
	  new_target_moving = true;
	  SetLockOnInfo(target);
	} else {
	  next_tar = temp;
	}
      } else {
	next_tar = temp;
      }
    } else {
      nt_frame = 0;
    }
  }

  // サーチボタンが押されている
  if (search) {
    target = GetNextEnemy(target);
    search = false;
    lock_site = lock_site_frame;
    new_target_moving = true;
    SetLockOnInfo(target);
  }

  // ロックオン対象が誰もいないよ
  if (target == 0) {
    target = &dummy;
    dummy.SetPos(objs->GetStageEnd() / 2);
    new_target_moving = true;
    SetLockOnInfo(target);
  }

  // 体の向きが固定されている設定の場合
  if (body_dir_lock) {
    return;
  }

  // 向きが変更されない状態の場合
  if (motion.type == PlayerMotion::FlyDamage ||
      motion.type == PlayerMotion::Bound ||
      motion.type == PlayerMotion::Down ||
      motion.type == PlayerMotion::Explosion) {
    return;
  }

  // 体の向きを敵の方向に向ける
  body_dir = ObjectFunc::GetDirection(pos, target->GetPos());
}

// OneFrameProcで武器などの硬直する動作が終わったときに呼ぶべき関数
// プレイヤーからの入力を受け付けない状態から復帰するときに呼ばれる
void Player::BeginMove()
{
  // 爆発に移行
  if (pre_explosion) {
    if (motion.type != PlayerMotion::Down) {
      body_dir = ObjectFunc::GetDirection4or6(-speed);
      speed *= 0;
    }
    frame = 0;
    motion.type = PlayerMotion::Explosion;
    return;
  }

  // 硬直解除
  motion.type = PlayerMotion::None;
  frame = 0;
  body_dir_lock = false;
  TurnOnTarget();
  UpdateSpeed();
}

//============================================================
// Player::Update
// １フレーム，時を進める
//============================================================
void Player::Update()
{
  // 敵の方向を向く
  TurnOnTarget();

  ++frame;			// フレームをカウント
  ++abs_frame;
  --invincib;
  if (invincib < 0) {
    invincib = 0;
  }
  cmanager.UpdateFrame();

  // モーション固有の更新
  switch (motion.type) {
  case PlayerMotion::FlyDamage:
  case PlayerMotion::Bound:
    // 地面まで達しているなら
    if (ver_speed > 0 && ver_pos >= 0) {
      if (motion.type == PlayerMotion::FlyDamage) {
	// Boundに移行する
	frame = 0;
	motion.type = PlayerMotion::Bound;
	ver_pos = 0;
	ver_speed *= -e;
	// 速度を落とす
	speed.x *= 0.8;
	speed.y *= 0.8;
      }
      else if (motion.type == PlayerMotion::Bound) {
	// Downに移行する
	frame = 0;
	motion.type = PlayerMotion::Down;
	invincib = 10000;
	ver_pos = 0;
	ver_speed = 0;
      }
    }
    break;

  case PlayerMotion::Down:
    if (frame >= down_frame) {
      invincib = situp_invincib;
      BeginMove();
    }
    break;

  case PlayerMotion::StaggerDamage:
    if (frame == 1) {
      body_dir_lock = true;
    }
    if (frame >= stagger_frame) {
      BeginMove();
    }
    break;

  case PlayerMotion::Dash:
    break;

  case PlayerMotion::DashSpatia:
    if (frame >= 15) {
      BeginMove();
    }
    break;

  case PlayerMotion::Explosion:
    break;

  default:
    break;
  }

  // ダッシュゲージ更新
  switch (motion.type) {
  case PlayerMotion::Dash:
    dash_gauge -= 4;
    break;
  case PlayerMotion::DashWeapon1:
  case PlayerMotion::DashWeapon2:
  case PlayerMotion::DashCloseRange:
    break;
  default:
    dash_gauge += 4;
    break;
  }

  // ダッシュゲージを正しい範囲内に調整
  if (dash_gauge > param.dash_gauge) {
    dash_gauge = param.dash_gauge;
  }
  else if (dash_gauge < 0) {
    dash_gauge = 0;
    if (motion.type == PlayerMotion::Dash) {
      // motion.type = PlayerMotion::DashSpatia;
      motion.type = PlayerMotion::None;
    }
  }

  // プレイヤー毎の１フレームの固有の処理を書く
  // ex. 弾の出現や状態の変更など
  OneFrameProc();

  // 速度更新
  UpdateSpeed();

  // 座標更新
  pos += speed;
  // 高さ更新
  ver_pos += ver_speed;
  if (ver_pos > 0) {
    ver_pos = 0;
  }
}

//============================================================
// Player::GetSendObject
// クライアントに送信する物体情報を作成する
//============================================================
SendObject Player::GetSendObject() 
{ 
  SendObject sobj;
  
  sobj.type = GetSendObjType();
  sobj.state = GetSendObjState();
  sobj.pos = GetSendPos();

  return sobj;
}

// SendPosを得る
SendPos Player::GetSendPos()
{
  SendPos spos = SendPos(short(pos.x),
			 short(pos.y));
  spos.y += short(ver_pos);

  return spos;
}

bool Player::GetSendParamPos(SendPos& spos)
{
  DPos tpos = target->GetPos();
  double dist = hypot(tpos.x - pos.x, tpos.y - pos.y);
  if (dist > draw_correct_max_dist) {
    dist = draw_correct_max_dist;
  }

  DPos diff = ObjectFunc::GetDirFacter(pos, tpos);
  diff *= dist;			// 今回算出した描画点のずれ
  if (target == &dummy) {
    diff.x = diff.y = 0;
  }
  if (new_target_moving) {
    DPos diff_vec = diff - pre_diff;
    // 目的の位置と近いなら
    if (hypot(diff_vec.x, diff_vec.y) < lock_near_dist) {
      new_target_moving = false;
    }
    else {
      // ひかえめに移動させる
      pre_diff += diff_vec / 12.0;
    }
    diff = pre_diff;
  } else {
    pre_diff = diff;
  }

  spos = GetSendPos();
  bool ret = false;
  if (diff.y < -120) {
    ret = true;
  }
  spos.x += short(diff.x);
  spos.y += short(diff.y);
  spos.y -= short(ver_pos);

  AdjustStageEnd(spos);

  // 画面端付近の調整
  return ret;
}

SendPos Player::GetSendParamPos()
{
  SendPos spos;
  GetSendParamPos(spos);
  return spos;
}

// プレイヤーの座標を設定する
void Player::SetPos(const DPos& a_pos)
{
  pos = a_pos;
}

// 最も近くの敵を探す
Player* Player::FindNearestEnemy()
{
  Player* p = 0;
  PlayerMap* map;

  double dist, nearest = 10000;
  map = objs->GetPlayerMap();
  for (PMIter i = map->begin();
       i != map->end(); ++i) {
    // 違うチームなら
    if (team != i->second->GetTeam()) {
      DPos t_pos = i->second->GetPos();
      // 自分との距離を測る
      dist = hypot(t_pos.x - pos.x, t_pos.y - pos.y);
      // 今回の距離が最も近い場合
      if (dist < nearest) {
	p = i->second;
	nearest = dist;
      }
    }
  }

  return p;
}

// 当たり判定を得る
bool Player::GetDetection(Detection& det)
{
  det = GetDetection();
  return true;
}

Detection Player::GetDetection()
{
  if (invincib) {
    param.own_obj_det.type = DetectionType::CollOnly;
  } else {
    param.own_obj_det.type = DetectionType::Coll;
  }
  return Detection(param.own_obj_det,
		   Pos(short(pos.x), short(pos.y + ver_pos)),
		   team);
}

//--------------------------------------------------
// 当たり判定を設定する
// すでに当たっていることは決定していることが前提
//--------------------------------------------------
bool Player::SetDetection(const Detection& det)
{
  if (motion.type == PlayerMotion::Explosion) {
    return true;
  }

  switch (det.type) {
  case DetectionType::Coll:	// ぶつかり判定を設定
  case DetectionType::CollOnly:
  case DetectionType::StageColl:
    // ぶつかり判定として設定する
    return SetCollDetect(det);
  case DetectionType::Hit:	// あたり判定を設定
  case DetectionType::HardHit:
    return SetAttackDetect(det);
  }
  return true;
}

void Player::DecHP(short int damage)
{
  // 情報が古い場合クリアする
  if (combo_stack.size() > 0) {
    // 最新のデータが規定フレーム数経過しているなら
    if (abs_frame - combo_stack.top() > combo_frame) {
      // クリア
      while (combo_stack.size()) {
	combo_stack.pop();
      }
    }
  }

  unsigned size = combo_stack.size();
  if (size <= 0) {

  }
  else if (size == 1) {
    damage = damage * 6 / 10;
  }
  else if (size == 2) {
    damage = damage * 4 / 10;
  }
  else if (size == 3) {
    damage = damage * 3 / 10;
  }
  else {
    damage = damage * 2 / 10;
  }

  if (hp != 0) {
    hp -= damage;
  }
  if (hp <= 0 && pre_explosion == false) {
    hp = 0;
    // やられた
    pre_explosion = true;
    // frame = 0;
  }
}

void Player::AddLightDamage(const Detection& det)
{
  // キューから古い情報を削除する
  while (ldq.size() > 0) {
    // 最古のデータが規定フレーム数経過しているなら
    if (abs_frame - ldq.front() > charge_damage_frame) {
      // 削除
      ldq.pop();
    }
    else {
      break;
    }
  }

  // 最新のダメージ情報を追加
  ldq.push(abs_frame);

  // 規定数以上蓄積されているなら
  if (ldq.size() >= stagger_num) {
    // 全要素削除
    while (ldq.size()) {
      ldq.pop();
    }
    // よろけダメージを誘発
    SetStaggerDamage(det);
  }
}

// よろけダメージを設定する
void Player::SetStaggerDamage(const Detection& det)
{
  motion.type = PlayerMotion::StaggerDamage;
  frame = 0;
  speed = det.param.dir * param.fast_damage_speed;
  combo_stack.push(abs_frame);

  // 規定コンボ数以上なら
  if (combo_stack.size() >= combo_down_num) {
    // ダウンする
    motion.type = PlayerMotion::FlyDamage;
    // invincib = 10000;
    frame = 0;
    speed = det.param.dir * param.slow_damage_speed;
    body_dir = ObjectFunc::GetDirection4or6(-speed);
    ver_pos = 0;
    ver_speed = down_float;
  }
}

// 攻撃判定を設定する
bool Player::SetAttackDetect(const Detection& det)
{
  // 攻撃があたらない状態
  if (invincib) {
    return true;
  }

  /*
  Detection* pdet = const_cast<Detection*>(&det);
  bool new_det = false;
  bool ret = true;
  */
  /*
  // 方向が指定されていないなら

    // 当たり判定からダメージの方向を算出

    pdet = new Detection(det);
    pdet->param.dir = 
    new_det = true;
  }
  */
  if (det.param.dir.x == 0 && det.param.dir.y == 0) {
    DPos hit_pos = ObjectFunc::GetLapDetectCenter(det, GetDetection());
    const_cast<Detection*>(&det)->param.dir 
      = (pos - hit_pos) / hypot(pos.x - hit_pos.x, pos.y - hit_pos.y);
  }

  // ダウン追いうちの場合
  if (motion.type == PlayerMotion::FlyDamage ||
      motion.type == PlayerMotion::Bound) {
    if (det.param.type == DetectParamType::HeavyDownAttack ||
	det.param.type == DetectParamType::LightDownAttack) {
      ver_speed = -9;
    } else {
      if (motion.type == PlayerMotion::FlyDamage && frame <= 5) {
	ver_speed -= 0.5;
      } else {
	ver_speed -= 6.0;
	if (ver_speed < -4.5) {
	  ver_speed = -4.5;
	}
      }
    }
    while (combo_stack.size()) {
      combo_stack.pop();
    }
    DecHP(det.param.point * 1 / 10);
  }
  // 普通に当たった場合
  else {
    DecHP(det.param.point);
    switch (det.param.type) {
    case DetectParamType::LightAttack:
      // 連続でダメージを受けるとよろける
      AddLightDamage(det);
      break;
    case DetectParamType::HeavyAttack:
      SetStaggerDamage(det);
      break;
    case DetectParamType::LightDownAttack:
      motion.type = PlayerMotion::FlyDamage;
      // invincib = 10000;
      frame = 0;
      speed = det.param.dir * param.slow_damage_speed;
      body_dir = ObjectFunc::GetDirection4or6(-speed);
      ver_pos = 0;
      ver_speed = down_float;
      break;
    case DetectParamType::HeavyDownAttack:
      motion.type = PlayerMotion::FlyDamage;
      // invincib = 10000;
      frame = 0;
      speed = det.param.dir * param.fast_damage_speed;
      body_dir = ObjectFunc::GetDirection4or6(-speed);
      ver_pos = 0;
      ver_speed = down_float;
      break;
    default:
      // 攻撃でないのに攻撃判定設定関数を呼んでいる
      return false;
    }
  }
  /*
  if (new_det) {
    delete pdet;
  }
  */
  return true;
}

// ステージの端との判定を調べる
void Player::CheckStageEnd()
{
  ObjectFunc::SetStageDetect(GetDetection(), objs->GetStageEnd(), pos);
}

// ぶつかり判定を設定する
bool Player::SetCollDetect(const Detection& det)
{
  //------------------------------
  // 重なりが小さい方のみずらす
  //------------------------------

  // 判定のかさなりを調べる
  short int hor_lap = GetHorizonLap(det);
  short int ver_lap = GetVerticalLap(det);

  // 水平の重なりのほうが小さい
  if (abs(hor_lap) < abs(ver_lap)) {
    pos.x -= hor_lap;
  }
  // 垂直の重なりのほうが小さい
  else {
    pos.y -= ver_lap;
  }
  return true;
}

// 引数の判定との水平方向の重なりを得る
short int Player::GetHorizonLap(const Detection& det)
{
  Detection own_det = GetDetection();
  short int lap1 = 0;		// 重なり
  short int lap2 = 0;

  // 自分の右の判定と物体の左の判定を比べる
  lap1 = own_det.right - det.left;

  // 自分の左の判定と物体の右の判定を比べる
  lap2 = own_det.left - det.right;

  // 小さいほうを返す
  if (abs(lap1) < abs(lap2)) {
    return lap1;
  }
  else {
    return lap2;
  }
}

// 引数の判定との垂直方向の重なりを得る
// 重ならないとき，0を返す
short int Player::GetVerticalLap(const Detection& det)
{
  Detection own_det = GetDetection();
  short int lap1 = 0;
  short int lap2 = 0;

  // 自分の下の判定と物体の上の判定を比べる
  lap1 = own_det.low - det.high;

  // 自分の上の判定と物体の下の判定を比べる
  lap2 = own_det.high - det.low;
  
  // 小さいほうを返す
  if (abs(lap1) < abs(lap2)) {
    return lap1;
  }
  else {
    return lap2;
  }      
}

double Player::GetAbsSpeed()
{
  return hypot(speed.x, speed.y);
}

// 状態が変更されたらtrueを返す
// 使うほうでtrueを見たらframeを0にしたりする
bool Player::SetNewMotion(PlayerMotion& pre, const PlayerMotion& new_m)
{
  bool ret = false;

  // 新しい状態がサーチなら
  if (new_m.type == PlayerMotion::SearchEnemy) {
    search = true;
    return ret;
  }

  //--------------------
  // 新しい向きの設定
  //====================
  switch (new_m.type) {
  case PlayerMotion::Weapon1:
  case PlayerMotion::Weapon2:
  case PlayerMotion::CloseRange:
  case PlayerMotion::SearchEnemy:
  case PlayerMotion::Explosion:
    // 武器モーション情報は
    // 向きを設定していないので向きを代入しない
    break;
  default:
    pre.dir = new_m.dir;
    break;
  }

  //--------------------
  // 新しい状態の設定
  //====================
  switch (pre.type) {
  case PlayerMotion::DashSpatia:
    break;
  case PlayerMotion::Weapon1:
    if (frame <= 1) {
      // 次のモーションが武器なら空キャンセルできる
      if (new_m.WeaponType()) {
	goto CaseDefault;
      }
    }
    break;
  case PlayerMotion::DashWeapon1:
    if (frame <= 1) {
      // 次のモーションが武器なら空キャンセルできる
      if (new_m.WeaponType()) {
	goto CaseDash;
      }
    }
    break;
  case PlayerMotion::CloseRange:
    if (frame <= 1) {
      // 次のモーションが武器なら空キャンセルできる
      if (new_m.WeaponType()) {
	goto CaseDefault;
      }
    }
    break;
  case PlayerMotion::DashCloseRange:
    if (frame <= 1) {
      // 次のモーションが武器なら空キャンセルできる
      if (new_m.WeaponType()) {
	goto CaseDash;
      }
    }
    break;
  case PlayerMotion::Weapon2:
  case PlayerMotion::DashWeapon2:
  case PlayerMotion::StaggerDamage:
  case PlayerMotion::FlyDamage:
  case PlayerMotion::Bound:
  case PlayerMotion::Down:
  case PlayerMotion::Explosion:
    // 自分が動けない状態なので状態をコピーしない
    break;

  case PlayerMotion::Dash:		// 自分がダッシュ状態の場合
  CaseDash:
    switch (new_m.type) {
    case PlayerMotion::SearchEnemy:
      goto CaseDefault;
      break;

    case PlayerMotion::None: // すべての十字キーが放された情報のときはダッシュをやめる
      if (new_m.dir == Direction::None) {
	pre.type = new_m.type;
	// type = PlayerMotion::DashSpatia;
      }
      ret = true;
      break;
    case PlayerMotion::Weapon1: // 武器使用状態への移行の場合
      // ダッシュ中武器使用状態へ移行する
      pre.type = PlayerMotion::DashWeapon1;
      ret = true;
      break;
    case PlayerMotion::Weapon2:
      pre.type = PlayerMotion::DashWeapon2;
      ret = true;
      break;
    case PlayerMotion::CloseRange:
      pre.type = PlayerMotion::DashCloseRange;
      ret = true;
      break;
    default:
      goto CaseDefault;
      break;
    }
    break;

  default:
  CaseDefault:
    // 状態を変更
    // 同じ状態の場合変更しない
    if (pre.type != new_m.type) {
      pre.type =  new_m.type;
      ret = true;
      if (new_m.type == PlayerMotion::Dash) {
	UpdateSpeed();
      }
    }
    break;
  }
  return ret;
}

// 敵をロックオンした時に呼ばれる
void Player::SetLockOnInfo(Player* p)
{
  lock_map[p] = 0;
}

// 自分がロックしている敵のロックオンサイトを得る
NonTypeSendObject Player::GetLockOnSite()
{
  NonTypeSendObject so;

  so.pos = target->GetLockOnSitePos();

  // ロックオンしてからのフレーム数を得る
  int s = lock_map[target]++;
  if (s < LockState::MoveNum) {
    so.state = s;
  } else {
    so.state = LockState::MoveNum + s % 3;
  }

  // 無敵アリ(黄色)
  if (target->GetInv()) {
    so.state += LockState::MoveNum + 3;
  }

  return so;
}

// 自分を示す矢印を得る
bool Player::GetArrow(SendObject& arrow, int id, const SendPos& view_pos, bool focus)
{
  Pos v_pos(view_pos.x, view_pos.y);
  Pos m_pos(short(pos.x), short(pos.y));
  Pos begin(v_pos.x - ScreenWidth / 2, v_pos.y - ScreenHeight / 2);
  Pos end(v_pos.x + ScreenWidth / 2, v_pos.y + ScreenHeight / 2);

  double dist;
  SendPos spos;
  Direction::t dir;
  // 自分の座標がスクリーン内にあるか検討する
  // 水平方向
  // 左外側にある場合
  if (m_pos.x < begin.x) {
    // 垂直方向
    // 上外側
    if (m_pos.y < begin.y) {
      // スクリーン外確定
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);

      // v_posからbeginまで何倍か調べて，そのときのy座標を出す
      short y = GetIntersectingPointY(v_pos, m_vec, begin.x);
      if (y > begin.y) {
	spos.y = y;
	spos.x = begin.x;
	dir = Direction::Dir4;
      } else {
	spos.x = GetIntersectingPointX(v_pos, m_vec, begin.y);
	spos.y = begin.y;
	dir = Direction::Dir8;
      }
    }
    // 下外側
    else if (m_pos.y > end.y) {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      short y = GetIntersectingPointY(v_pos, m_vec, begin.x);
      if (y < end.y) {
	spos.y = y;
	spos.x = begin.x;
	dir = Direction::Dir4;
      }
      else {
	spos.y = end.y;
	spos.x = GetIntersectingPointX(v_pos, m_vec, end.y);
	dir = Direction::Dir2;
      }
    }
    // 左外側のみ
    else {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      spos.x = begin.x;
      spos.y = GetIntersectingPointY(v_pos, m_vec, begin.x);
      dir = Direction::Dir4;
    }
  }
  // 右外側
  else if (m_pos.x > end.x) {
    // 上外側
    if (m_pos.y < begin.y) {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      short y = GetIntersectingPointY(v_pos, m_vec, end.x);
      if (y > begin.y) {
	spos.y = y;
	spos.x = end.x;
	dir = Direction::Dir6;
      } else {
	spos.x = GetIntersectingPointX(v_pos, m_vec, begin.y);
	spos.y = begin.y;
	dir = Direction::Dir8;
      }
    }
    // 下外側
    else if (m_pos.y > end.y) {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      short y = GetIntersectingPointY(v_pos, m_vec, end.x);
      if (y < end.y) {
	spos.y = y;
	spos.x = end.x;
	dir = Direction::Dir6;
      }
      else {
	spos.y = end.y;
	spos.x = GetIntersectingPointX(v_pos, m_vec, end.y);
	dir = Direction::Dir2;
      }
    }
    // 右外側のみ
    else {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      spos.y = GetIntersectingPointY(v_pos, m_vec, end.x);
      spos.x = end.x;
      dir = Direction::Dir6;
    }
  } else {
    // 上外側のみ
    if (m_pos.y < begin.y) {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      spos.x = GetIntersectingPointX(v_pos, m_vec, begin.y);
      spos.y = begin.y;
      dir = Direction::Dir8;
    } 
    // 下外側のみ
    else if (m_pos.y > end.y) {
      Pos m_vec = m_pos - v_pos;
      dist = hypot(m_vec.x, m_vec.y);
      spos.x = GetIntersectingPointX(v_pos, m_vec, end.y);
      spos.y = end.y;
      dir = Direction::Dir2;
    }
    else {
      // 内側にある
      return false;
    }
  }

  // 矢印のタイプを得る
  switch (id) {
  case 0:
    if (focus) {
      arrow.type = ArrowType::ID0;
    } else {
      arrow.type = ArrowType::TransID0;
    }
    break;
  case 1:
    if (focus) {
      arrow.type = ArrowType::ID1;
    } else {
      arrow.type = ArrowType::TransID1;
    }
    break;
  case 2:
    if (focus) {
      arrow.type = ArrowType::ID2;
    } else {
      arrow.type = ArrowType::TransID2;
    }
    break;
  case 3:
    if (focus) {
      arrow.type = ArrowType::ID3;
    } else {
      arrow.type = ArrowType::TransID3;
    }
    break;
  default:
    throw logic_error("Player::GetArrow: 不正なID");
  }
  arrow.pos = spos;
  arrow.state = GetArrowState(team, dist, dir);
  return true;
}

int Player::GetSiteSize(double dist)
{
  if (dist < 450) {
    return 0;
  } 
  else if (dist < 800) {
    return 1;
  }
  else {
    return 2;
  }
}

unsigned char Player::GetArrowState(Team::t team, double dist, Direction::t dir)
{
  switch (team) {
  case Team::I:			// い組
    switch (GetSiteSize(dist)) {
    case 0:			// 大
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::IBigUp;
      case Direction::Dir2:
	return ArrowState::IBigDown;
      case Direction::Dir4:
	return ArrowState::IBigLeft;
      case Direction::Dir6:
	return ArrowState::IBigRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 1:			// 中
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::IMiddleUp;
      case Direction::Dir2:
	return ArrowState::IMiddleDown;
      case Direction::Dir4:
	return ArrowState::IMiddleLeft;
      case Direction::Dir6:
	return ArrowState::IMiddleRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 2:			// 小
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::ISmallUp;
      case Direction::Dir2:
	return ArrowState::ISmallDown;
      case Direction::Dir4:
	return ArrowState::ISmallLeft;
      case Direction::Dir6:
	return ArrowState::ISmallRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    default:
      throw logic_error("距離に対応した矢印のサイズが見つかりません．");
    }
    break;

  case Team::Ro:		// ろ組
    switch (GetSiteSize(dist)) {
    case 0:			// 大
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::RoBigUp;
      case Direction::Dir2:
	return ArrowState::RoBigDown;
      case Direction::Dir4:
	return ArrowState::RoBigLeft;
      case Direction::Dir6:
	return ArrowState::RoBigRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 1:			// 中
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::RoMiddleUp;
      case Direction::Dir2:
	return ArrowState::RoMiddleDown;
      case Direction::Dir4:
	return ArrowState::RoMiddleLeft;
      case Direction::Dir6:
	return ArrowState::RoMiddleRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 2:			// 小
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::RoSmallUp;
      case Direction::Dir2:
	return ArrowState::RoSmallDown;
      case Direction::Dir4:
	return ArrowState::RoSmallLeft;
      case Direction::Dir6:
	return ArrowState::RoSmallRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    default:
      throw logic_error("距離に対応した矢印のサイズが見つかりません．");
    }
    break;

  case Team::Ha:		// は組
    switch (GetSiteSize(dist)) {
    case 0:			// 大
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::HaBigUp;
      case Direction::Dir2:
	return ArrowState::HaBigDown;
      case Direction::Dir4:
	return ArrowState::HaBigLeft;
      case Direction::Dir6:
	return ArrowState::HaBigRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 1:			// 中
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::HaMiddleUp;
      case Direction::Dir2:
	return ArrowState::HaMiddleDown;
      case Direction::Dir4:
	return ArrowState::HaMiddleLeft;
      case Direction::Dir6:
	return ArrowState::HaMiddleRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 2:			// 小
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::HaSmallUp;
      case Direction::Dir2:
	return ArrowState::HaSmallDown;
      case Direction::Dir4:
	return ArrowState::HaSmallLeft;
      case Direction::Dir6:
	return ArrowState::HaSmallRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    default:
      throw logic_error("距離に対応した矢印のサイズが見つかりません．");
    }
    break;

  case Team::Ni:		// に組
    switch (GetSiteSize(dist)) {
    case 0:			// 大
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::NiBigUp;
      case Direction::Dir2:
	return ArrowState::NiBigDown;
      case Direction::Dir4:
	return ArrowState::NiBigLeft;
      case Direction::Dir6:
	return ArrowState::NiBigRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 1:			// 中
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::NiMiddleUp;
      case Direction::Dir2:
	return ArrowState::NiMiddleDown;
      case Direction::Dir4:
	return ArrowState::NiMiddleLeft;
      case Direction::Dir6:
	return ArrowState::NiMiddleRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    case 2:			// 小
      switch (dir) {
      case Direction::Dir8:
	return ArrowState::NiSmallUp;
      case Direction::Dir2:
	return ArrowState::NiSmallDown;
      case Direction::Dir4:
	return ArrowState::NiSmallLeft;
      case Direction::Dir6:
	return ArrowState::NiSmallRight;
      default:
	throw logic_error("不正な矢印の方向を得ようとしました．");
      }
      break;
    default:
      throw logic_error("距離に対応した矢印のサイズが見つかりません．");
    }
    break;
  default:
    break;
  }
  return ArrowState::IBigUp;
}

void Player::UpdateTarget()
{
  MakeEnemyList();
  Player* temp = FindNearestEnemy();
  if (temp == 0) {
    temp = &dummy;
    dummy.SetPos(objs->GetStageEnd() / 2);
  }
  new_target_moving = true;

  if (target != temp) {
    SetLockOnInfo(temp);
  }
  target = temp;
}

bool Player::AdjustStageEnd(SendPos& spos)
{
  bool ret = false;

  // 画面端の場合の調整
  Pos stg_end = objs->GetStageEnd();
  int hsw = ScreenWidth / 2;
  int hsh = ScreenHeight / 2;
  if (spos.x < hsw) {
    spos.x = hsw;
    ret = true;
  }
  if (spos.y < hsh) {
    spos.y = hsh;
    ret = true;
  }
  if (stg_end.x - hsw < spos.x) {
    spos.x = stg_end.x - hsw;
    ret = true;
  }
  if (stg_end.y - hsh < spos.y) {
    spos.y = stg_end.y - hsh;
    ret = true;
  }

  return ret;
}

bool Player::IsGettingAhead()
{
  DPos b_diff = target->GetPos() - pos;
  double b_dist = hypot(b_diff.x, b_diff.y);
  DPos a_diff = target->GetPos() - (pos + GetWalkSpeed());
  double a_dist = hypot(a_diff.x, a_diff.y);

  if (b_dist > a_dist) {
    return true;
  } else {
    return false;
  }
}

//----------------------------------------
// 以下、やられたプレイヤークラスの実装

void DeadPlayer::ChangeState(unsigned char eve)
{
  PlayerMotion new_motion;
  // 新しいモーションが生まれたら
  if (cmanager.DisposeKeyEvent(kecreater.ConvertClientEvent(eve), new_motion)) {
    // 動作が新しくなった場合，フレームを0にする
    if (SetNewMotion(motion, new_motion)) {
      motion.type = PlayerMotion::None;
    }
  }
}
void DeadPlayer::Update()
{
  // 高さ更新
  ver_pos += ver_speed;
  if (ver_pos > 0) {
    ver_pos = 0;
  }

  // 速度更新
  DPos fact = ObjectFunc::GetDirFacter(motion.dir);
  speed = fact * 20;

  // 座標更新
  pos += speed;

  SendPos spos(short(pos.x), short(pos.y));
  if (AdjustStageEnd(spos)) {
    pos.x = double(spos.x);
    pos.y = double(spos.y);
  }
}
