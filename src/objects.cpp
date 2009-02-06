// last modified <2004/02/08 21:45:11 2004 JST>
#include <stdexcept>
#include "objects.hpp"
#include "player.hpp"
#include "planex.hpp"
#include "yamaha.hpp"
#include "ntt.hpp"

using namespace std;
using namespace zu3ga;

// コンストラクタ
Objects::Objects(StageType::t stage_t)
  : winner(Team::None), send_obj_ary(0), next_id(0), stage_end(Pos(800, 700))
{
  switch (stage_t) {
  case StageType::Forest:
    // 画面端座標確定
    stage_end = Pos(1600, 1200);
    // キャラクタの初期位置
    start_pos[0] = DPos(250, 250);
    start_pos[1] = DPos(220, 830);
    start_pos[2] = DPos(1300, 300);
    start_pos[3] = DPos(1330, 920);

    // 左上画面端の森
    AddStageObject(new CollObject(Pos(0, 0), Pos(1600, 78)));
    AddStageObject(new CollObject(Pos(0, 79), Pos(165, 145)));
    AddStageObject(new CollObject(Pos(0, 146), Pos(142, 242)));
    AddStageObject(new CollObject(Pos(0, 243), Pos(80, 325)));
    AddStageObject(new CollObject(Pos(0, 326), Pos(65, 547)));
    AddStageObject(new CollObject(Pos(0, 548), Pos(40, 570)));
    // 上のでっぱり？
    AddStageObject(new CollObject(Pos(448, 79), Pos(463, 106)));
    AddStageObject(new CollObject(Pos(1087, 79), Pos(1400, 102)));
    // 左の森
    AddStageObject(new CollObject(Pos(335, 230), Pos(415, 261)));
    AddStageObject(new CollObject(Pos(314, 262), Pos(520, 299)));
    AddStageObject(new CollObject(Pos(270, 300), Pos(530, 370)));
    AddStageObject(new CollObject(Pos(556, 480), Pos(590, 590))); // 半端
    AddStageObject(new CollObject(Pos(225, 371), Pos(555, 625)));
    AddStageObject(new CollObject(Pos(225, 626), Pos(523, 670)));
    AddStageObject(new CollObject(Pos(225, 671), Pos(485, 720)));
    AddStageObject(new CollObject(Pos(225, 721), Pos(450, 755)));
    AddStageObject(new CollObject(Pos(260, 756), Pos(410, 805)));
    AddStageObject(new CollObject(Pos(280, 806), Pos(355, 855)));
    // 真ん中の森
    AddStageObject(new CollObject(Pos(695, 199), Pos(800, 305)));
    AddStageObject(new CollObject(Pos(801, 240), Pos(855, 305)));
    AddStageObject(new CollObject(Pos(718, 306), Pos(873, 445)));
    AddStageObject(new CollObject(Pos(874, 368), Pos(901, 426))); // 半端
    AddStageObject(new CollObject(Pos(736, 446), Pos(900, 508)));
    AddStageObject(new CollObject(Pos(752, 509), Pos(933, 579)));
    AddStageObject(new CollObject(Pos(802, 580), Pos(937, 613)));
    AddStageObject(new CollObject(Pos(835, 614), Pos(899, 653)));
    // 右上の森
    AddStageObject(new CollObject(Pos(1078, 221), Pos(1235, 291)));
    AddStageObject(new CollObject(Pos(1035, 292), Pos(1216, 407)));
    AddStageObject(new CollObject(Pos(1219, 365), Pos(1330, 407)));
    AddStageObject(new CollObject(Pos(1086, 408), Pos(1371, 455)));
    AddStageObject(new CollObject(Pos(1118, 456), Pos(1343, 488)));
    AddStageObject(new CollObject(Pos(1158, 489), Pos(1343, 519)));
    // 右下の森
    AddStageObject(new CollObject(Pos(1145, 651), Pos(1316, 755)));
    AddStageObject(new CollObject(Pos(1156, 756), Pos(1343, 818)));
    AddStageObject(new CollObject(Pos(1058, 788), Pos(1085, 818))); // 半端
    AddStageObject(new CollObject(Pos(940, 819), Pos(1310, 878)));
    AddStageObject(new CollObject(Pos(1034, 879), Pos(1221, 949)));
    AddStageObject(new CollObject(Pos(1222, 879), Pos(1262, 922))); // 半端
    // 右,下の端の森
    AddStageObject(new CollObject(Pos(1548, 79), Pos(1600, 915)));
    AddStageObject(new CollObject(Pos(1493, 249), Pos(1547, 312))); // 半端
    AddStageObject(new CollObject(Pos(1490, 916), Pos(1600, 1019)));
    AddStageObject(new CollObject(Pos(1445, 1020), Pos(1600, 1083)));
    AddStageObject(new CollObject(Pos(0, 1133), Pos(1600, 1200))); // 一番下
    // 以下、下から２番目
    AddStageObject(new CollObject(Pos(0, 1077), Pos(38, 1132))); // 半端
    AddStageObject(new CollObject(Pos(301, 1106), Pos(349, 1132))); // 半端
    AddStageObject(new CollObject(Pos(386, 1087), Pos(1007, 1132)));
    AddStageObject(new CollObject(Pos(606, 1058), Pos(647, 1086))); // 半端
    AddStageObject(new CollObject(Pos(763, 1048), Pos(808, 1086))); // 半端
    AddStageObject(new CollObject(Pos(1183, 1100), Pos(1232, 1132))); // 半端
    AddStageObject(new CollObject(Pos(1399, 1084), Pos(1600, 1132)));
    break;

  case StageType::Future:
    stage_end = Pos(1600, 1200);
    start_pos[0] = DPos(250, 250);
    start_pos[1] = DPos(250, 1200 - 250);
    start_pos[2] = DPos(1600 - 250, 250);
    start_pos[3] = DPos(1600 - 250, 1200 - 250);
    break;
  }
}

// デストラクタ
Objects::~Objects()
{
  obj_ary.clear();
}

// 試合開始
void Objects::LetsStart()
{
  for (PMIter i = players.begin(); i != players.end(); ++i) {
    i->second->WeaponLockRelease();
  }
}

// 終了合図
Team::t Objects::IsFinished()
{
  int teams[4] = { 0 };
  int num = 0;
  Team::t ret = Team::None;
  for (PMIter i = players.begin(); i != players.end(); ++i) {
    unsigned t = unsigned(i->second->GetTeam());
    if (t <= 3) {
      ++(teams[t]);
    }
  }
  for (unsigned i = 0; i < 4; ++i) {
    // このチームに１人以上存在している
    if (teams[i] > 0) {
      switch (i) {
      case 0:
	ret = Team::I;
	break;
      case 1:
	ret = Team::Ro;
	break;
      case 2:
	ret = Team::Ha;
	break;
      case 3:
	ret = Team::Ni;
	break;
      }
      ++num;
    }
  }
  if (num >= 2) {
    ret = Team::None;
  }
  if (num == 0) {
    ret = Team::Dummy;
  }
  return ret;
}

//========================================
// 全ての物体の情報を１フレーム更新
//========================================
Objects& Objects::UpdateAll()
{
  for (OAIter i = obj_ary.begin(); i != obj_ary.end(); ) {
    // 更新
    (*i)->Update();

    // 物体が消えようとしているならば
    if ((*i)->Erase()) {
      // 削除
      EraseObj(i);
    }
    else {
      ++i;
    }
  }

  // やられたプレイヤー更新
  for (PMIter i = dead_players.begin();
       i != dead_players.end(); ++i) {
    i->second->Update();
  }

  return *this;
}

// 物体配列から物体を削除する
void Objects::EraseObj(OAIter& i)
{
  // プレイヤーが消えようとしている
  if (dynamic_cast<Player*>(*i)) {
    for (PMIter j = players.begin(); j != players.end(); ) {
      if (j->second == *i) {
	// やられたプレイヤーマップに追加する
	dead_players[j->first] = new DeadPlayer(j->second->GetPos(), this);
	players.erase(j++);
	// 他のプレイヤーに敵を探させる
	for (PMIter k = players.begin();
	     k != players.end(); ++k) {
	  k->second->UpdateTarget();
	}
      } else {
	++j;
      }
    }
  }
  delete *i;
  obj_ary.erase(i++);
}

//==================================================
// 当たり判定を行い,必要があれば情報を更新する
//==================================================
Objects& Objects::HitCheck()
{
  // あたり判定の前にぶつかり判定を行う．
  //
  // ぶつかり判定では物体の座標が変化する可能性がある．
  // 当たり判定を行っている最中に，座標が動くと困る．

  // 画面端とのぶつかり判定
  for (OAIter i = obj_ary.begin(); i != obj_ary.end(); ++i) {
    (*i)->CheckStageEnd();
  }

  // ぶつかり判定を行う
  for (OAIter i = obj_ary.begin(); i != obj_ary.end(); ++i) {
    OAIter j = i;
    ++j;
    for ( ; j != obj_ary.end(); ++j) {
      aCollCheck(*i, *j);
    }
    // ステージの物体との判定
    for (OAIter k = stage_obj_ary.begin();
	 k != stage_obj_ary.end(); ++k) {
      aStageObjCollCheck(*i, *k);
    }
  }

  // 当たり判定を行う
  for (OAIter i = obj_ary.begin(); i != obj_ary.end(); ++i) {
    OAIter j = i;
    ++j;
    for ( ; j != obj_ary.end(); ++j) {
      aHitCheck(*i, *j);
    }
    // ステージの物体との判定
    for (OAIter k = stage_obj_ary.begin(); 
	 k != stage_obj_ary.end(); ++k) {
      aStageObjHitCheck(*i, *k);
    }
  }

  // 消えようとしている物体を全て削除
  for (OAIter i = obj_ary.begin(); i != obj_ary.end(); ) {
    if ((*i)->Erase()) {
      EraseObj(i);
    }
    else {
      ++i;
    }
  }

  return *this;
}

// 二つの物体のぶつかり判定を行なう
void Objects::aCollCheck(Object* lobj, Object* robj)
{
  if (lobj->GetDetection(ldet) == false) {
    return;
  }
  if (robj->GetDetection(rdet) == false) {
    return;
  }

  // ぶつかり判定が含まれない判定なら終了
  switch (ldet.type) {
  case DetectionType::Hit:
  case DetectionType::HardHit:
    return;
  default:
    break;
  }
  switch (rdet.type) {
  case DetectionType::Hit:
  case DetectionType::HardHit:
    return;
  default:
    break;
  }

  // 当たり判定が重なっているなら
  if (ObjectFunc::CheckCollDetect(ldet, rdet)) {
    // 速度の大きい方の物体を移動させる
    if (robj->GetAbsSpeed() > lobj->GetAbsSpeed()) {
      robj->SetDetection(ldet);
    } else {
      lobj->SetDetection(rdet);
    }
  }
}

// ステージの当たり判定物体のぶつかり判定を行う
// robjがステージの当たり判定
// ステージの物体どうしの当たり判定を行わないために分ける
void Objects::aStageObjCollCheck(Object* lobj, Object* robj)
{
  if (lobj->GetDetection(ldet) == false) {
    return;
  }
  if (robj->GetDetection(sdet) == false) {
    return;
  }

  // ぶつかり判定が含まれない判定なら終了
  if ((ldet.type == DetectionType::Coll || 
       ldet.type == DetectionType::CollOnly) == false) {
    return;
  }

  // 当たり判定が重なっているなら
  if (ObjectFunc::CheckCollDetect(ldet, *sdet)) {
    // ステージ当たり判定オブジェクトでないほうを移動させる
    lobj->SetDetection(*sdet);
  }
}


// 二つの物体の当たり判定を行なう
void Objects::aHitCheck(Object* lobj, Object* robj)
{
  if (lobj->GetDetection(ldet) == false) {
    return;
  }
  if (robj->GetDetection(rdet) == false) {
    return;
  }

  // ぶつかり判定のみの判定は扱わない
  if (ldet.type == DetectionType::CollOnly ||
      rdet.type == DetectionType::CollOnly) {
    return;
  }

  // 当たっている場合
  if (ObjectFunc::CheckHitDetect(ldet, rdet)) {
    // それぞれ当たり判定を設定する
    lobj->SetDetection(rdet);
    robj->SetDetection(ldet);
  }
}

// ステージの当たり判定物体との当たり判定を行う
// robjがステージの当たり判定
// 処理を軽くするために分けてみる…
void Objects::aStageObjHitCheck(Object* lobj, Object* robj)
{
  if (lobj->GetDetection(ldet) == false) {
    return;
  }
  if (robj->GetDetection(sdet) == false) {
    return;
  }

  // ぶつかり判定のみの判定は扱わない
  if (ldet.type == DetectionType::CollOnly) {
    return;
  }

  // 当たっている場合
  if (ObjectFunc::CheckHitDetect(ldet, *sdet)) {
    // それぞれ当たり判定を設定する
    lobj->SetDetection(*sdet);
    robj->SetDetection(ldet);
  }  
}

// 長方形の重なりを調べる
bool Objects::CheckRect(Detection& det1, Detection& det2)
{
  // 縦の判定を調べる
  // low1 < high2 < high1 || low1 < low2 < high1
  if (((det1.low < det2.high && det1.high < det2.high) ||
       (det1.low < det2.low && det2.low < det1.high)) == false) {
    return false;
  }
  // 横の判定を調べる
  if (((det1.left < det2.right && det2.right < det1.right) ||
      (det1.left < det2.left && det2.left < det1.right)) == false) {
    return false;
  }
  return true;
}

//========================================
// クライアントに送信するデータ配列を作る
//========================================
const SendObjAry* Objects::GetSendData()
{
  delete send_obj_ary;
  send_obj_ary = new SendObjAry();

  for (OAIter i = obj_ary.begin(); i != obj_ary.end(); ++i) {
    SendObject sobj;
    if ((*i)->GetShadow(sobj)) {
      send_obj_ary->push_back(sobj);
    }
    sobj = (*i)->GetSendObject();
    if (sobj.type != SendObjType::NoGraphic) {
      send_obj_ary->push_back(sobj);
    }
  }

  return send_obj_ary;
}

// プレイヤーを物体配列に追加する
int Objects::AddPlayer(const PlayerInitData& data)
{
  Player* player = 0;
  int id = next_id++;

  switch (data.type) {
  case PlayerType::Planex:
    player = new Planex(data.team, this);
    player->SetPos(start_pos[id]);
    break;
  case PlayerType::Yamaha:
    player = new Yamaha(data.team, this);
    player->SetPos(start_pos[id]);
    break;
  case PlayerType::NTT:
    player = new NTT(data.team, this);
    player->SetPos(start_pos[id]);
    break;
  }

  players[id] = player;
  obj_ary.push_front(player);

  return id;
}

// 物体を配列に追加する
void Objects::AddObject(Object* obj)
{
  obj_ary.push_back(obj);
}

// ステージの物体(お互いにぶつからない)を追加する
void Objects::AddStageObject(Object* obj)
{
  stage_obj_ary.push_back(obj);
}

// プレイヤーの状態を変更する
Objects& Objects::ChangePlayerState(unsigned char eve, int id)
{
  PMIter i = players.find(id);
  if (i != players.end()) {
    i->second->ChangeState(eve);
  } else {
    dead_players[id]->ChangeState(eve);
  }
  return *this;
}

// idのプレイヤーのパラメータを得る
SendParam Objects::GetSendParam(int id)
{
  SendParam param;

  Player* p;
  PMIter i = players.find(id);
  if (i != players.end()) {
    p = i->second;
  } else {
    p = dead_players[id];
  }

  bool up_view = p->GetSendParamPos(param.view_pos);
  param.dash = p->GetDashParam();
  param.dash_gauge_pos = p->GetDashGaugePos(up_view);
  param.site = p->GetLockOnSite();

  SendObject arrow;
  // 人数分の情報
  // 矢印取得
  for (PMIter i = players.begin(); i != players.end(); ++i) {
    // 自分自身の矢印は取得しない
    if (p != i->second) {
      // 現在ロックオンしている相手ならば
      if (p->GetTarget() == i->second) {
	// フォーカスtrueの矢印を得る
	if (i->second->GetArrow(arrow, i->first, param.view_pos, true)) {
	  param.arrows.push_back(arrow);
	}
      } else {
	if (i->second->GetArrow(arrow, i->first, param.view_pos)) {
	  param.arrows.push_back(arrow);
	}
      }
    }
  }

  // HP取得
  for (int id = 0; id < 4; ++id) {
    PMIter i = players.find(id);
    if (i != players.end()) {
      param.HP.push_back(i->second->GetHitPoint());
    } 
    else {
      i = dead_players.find(id);
      if (i != dead_players.end()) {
	param.HP.push_back(i->second->GetHitPoint());
      }
    }
  }

  return param;
}

// プレイヤーの座標を設定する．テスト用
void Objects::SetPlayerPos(int id, const DPos& pos)
{
  players[id]->SetPos(pos);
}

vector<DrawableDetect> Objects::GetDrawableDetectAry()
{
  vector<DrawableDetect> ary;
  Detection det;
  for (OAIter i = obj_ary.begin();
       i != obj_ary.end(); ++i) {
    if ((*i)->GetDrawableDetect(det)) {
      ary.push_back(det);
    }
  }
  for (OAIter i = stage_obj_ary.begin();
       i != stage_obj_ary.end(); ++i) {
    if ((*i)->GetDrawableDetect(det)) {
      ary.push_back(det);
    }
  }
  return ary;
}
