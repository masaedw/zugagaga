// last modified <2004/02/08 20:36:47 2004 JST>
#include "shell.hpp"
#include "effect.hpp"
#include "player.hpp"

using namespace std;
using namespace zu3ga;

// 弾基底クラスのコンストラクタ
Shell::Shell(const ShellParam& param, Objects* objs, 
	     const DPos& pos, Player* target, Team::t team)
  : ver_pos(0)
  , ver_speed(0)
  , param(param)
  , target(target)
  , objs(objs)
  , team(team)
  , pos(pos)
  , speed(param.v0)
  , dir(target ? ObjectFunc::GetRadian(pos, target->GetPos()) : 0)
{}

bool Shell::SetDetection(const Detection& det)
{
  bool temp = false;
  // 自分のDetectionType
  switch (param.own_obj_det.type) {
  case DetectionType::Hit:
    // ぶつかったDetectionType
    switch (det.type) {
    case DetectionType::Coll:
    case DetectionType::StageColl:
    case DetectionType::Hit:
    case DetectionType::HardHit:
      temp = true;
      break;
    default:
      break;
    }
    break;
  case DetectionType::HardHit:
    // ぶつかったDetectionType
    switch (det.type) {
    case DetectionType::Coll:
    case DetectionType::StageColl:
    case DetectionType::HardHit:
      temp = true;
    default:
      break;
    }
    break;
  default:
    break;
  }

  if (temp) {
    erase = true;
    AddHitObject(det);
  }
  return true;
}

// 時間を更新する
void Shell::Update()
{
  ++frame;

  // はじめのフレームは移動をしない
  if (frame == 1) {
    return;
  }

  // 継承先固有の処理
  OneFrameProc();

  // 敵を追跡(向き調整)
  ObjectFunc::DirAdjust(dir, pos, target->GetPos(), param.tracking);

  // 速度の更新
  ver_speed += gravity;
  ver_pos += ver_speed;
  if (ver_pos > 0) {
    ver_pos = 0;
    ver_speed = 0;
  }
  DPos temp_sp;
  DPos fact = ObjectFunc::GetDirFacter(dir);
  speed += param.acc;
  if (speed > param.v_max) {
    speed = param.v_max;
  }
  temp_sp.x = speed * fact.x;
  temp_sp.y = speed * fact.y;

  // 座標の更新
  pos += temp_sp;
}

void Shell::CheckStageEnd()
{
  if (ObjectFunc::CheckStageDetect(GetDetection(), objs->GetStageEnd())) {
    erase = true;
  }
}

// SendObject構造体を得る
SendObject Shell::GetSendObject()
{
  SendObject sobj;

  sobj.pos = SendPos(short(pos.x), short(pos.y + ver_pos));
  sobj.type = param.type;
  sobj.state = GetSendObjState();

  return sobj;
}

// 当たり判定を得る
bool Shell::GetDetection(Detection& det)
{
  if (erase) {
    return false;
  }
  det = GetDetection();
  return true;
}
Detection Shell::GetDetection()
{
  return Detection(param.own_obj_det,
		   Pos(short(pos.x), short(pos.y + ver_pos)),
		   team,
		   DetectParam(param.det_type,
			       ObjectFunc::GetDirFacter(dir),
			       param.damage));
}

// 物体に当たったときに出現させるオブジェクトを追加する
void Shell::AddHitObject(const Detection& det)
{
  Object* hobj;
  if (GetHitObject(hobj, ObjectFunc::GetLapDetectCenter(det, GetDetection()))) {
    objs->AddObject(hobj);
  }
}

//--------------------------------------------------
// まっすぐ飛ぶだけの弾クラスのコンストラクタ
//--------------------------------------------------
StraightShell::StraightShell(const ShellParam& param,
			     double a_dir, Objects* objs,
			     const DPos& pos, Team::t team)
  : Shell(param, objs, pos, 0, team)
{
  dir = a_dir;
}

// 時間を更新する
void StraightShell::Update()
{
  ++frame;

  // はじめのフレームは移動をしない
  if (frame == 1) {
    return;
  }

  // 継承先固有の処理
  OneFrameProc();

  // 速度の更新
  ver_speed += gravity;
  ver_pos += ver_speed;
  if (ver_pos > 0) {
    ver_pos = 0;
    ver_speed = 0;
  }
  DPos temp_sp;
  DPos fact = ObjectFunc::GetDirFacter(dir);
  speed += param.acc;
  if (speed > param.v_max) {
    speed = param.v_max;
  }
  temp_sp = fact * speed;

  // 座標の更新
  pos += temp_sp;
}
