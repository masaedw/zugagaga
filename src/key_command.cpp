// last modified <2004/02/06 15:42:38 2004 JST>
#include <stdexcept>
#include "key_command.hpp"
#include "motion.hpp"

using namespace std;
using namespace zu3ga;

// プレイヤーの移動形態を得る．歩きかダッシュ
PlayerMotion::Type CommandManager::GetMoveForm()
{
  // 入力数がコマンド認識に不十分な場合
  if (q.Size() <= 2) {
    return PlayerMotion::None;
  }

  PlayerMotion::Type form = PlayerMotion::None;

  // 規定フレーム以内の→・→等の入力でダッシュとみなす
  if (q.Get(1)->key == KeyEvent::Arrow5 &&
      q.Get(0)->key == q.Get(2)->key &&
      q.Get(0)->frame - q.Get(2)->frame <= 10) {
    form = PlayerMotion::Dash;
  }

  return form;
}

// 今入力されている武器のタイプを得る
PlayerMotion::Type CommandManager::GetWeaponType()
{
  InputKeyData* in1 = btn_q.Get(0);
  InputKeyData* in2 = btn_q.Get(1);

  switch (in1->key) {
  case KeyEvent::Button2Press:
    if (in2->key == KeyEvent::Button3Press &&
	in1->frame - in2->frame <= 1) {
      return PlayerMotion::Weapon2;
    }
    return PlayerMotion::Weapon1;

  case KeyEvent::Button3Press:
    if (in2->key == KeyEvent::Button2Press &&
	in1->frame - in2->frame <= 1) {
      return PlayerMotion::Weapon2;
    }
    return PlayerMotion::CloseRange;

  default:
    throw logic_error("攻撃ボタン入力がないのに，使用武器を得ようとした．");
  }
}

// 送られてきた十字キー情報を記憶する
void CommandManager::PushArrowKey(KeyEvent::KeyEvent_t keve)
{
  // 最新の入力を追加する
  q.Push(InputKeyData(keve, frame));
}

//============================================================
// CommandManager::DisposeKeyEvent
// キーイベントから新しい動作を得る
// 新しい動作が生まれたらtrueを返す
//============================================================
bool CommandManager::DisposeKeyEvent(KeyEvent::KeyEvent_t keve, PlayerMotion& motion)
{
  // コマンド入力感知のために入力情報を記憶する
  // 入力がキーなら
  if (IsArrow(keve)) {
    PushArrowKey(keve);
  }
  // 入力がボタンなら
  else {
    btn_q.Push(InputKeyData(keve, frame));
  }

  switch (keve) {
  case KeyEvent::Arrow1:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir1;
    return true;
  case KeyEvent::Arrow2:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir2;
    return true;
  case KeyEvent::Arrow3:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir3;
    return true;
  case KeyEvent::Arrow4:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir4;
    return true;
  case KeyEvent::Arrow5:
    motion.type = GetMoveForm();
    motion.dir = Direction::None;
    return true;
  case KeyEvent::Arrow6:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir6;
    return true;
  case KeyEvent::Arrow7:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir7;
    return true;
  case KeyEvent::Arrow8:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir8;
    return true;
  case KeyEvent::Arrow9:
    motion.type = GetMoveForm();
    motion.dir = Direction::Dir9;
    return true;
  case KeyEvent::Button1Press:
    motion.type = PlayerMotion::SearchEnemy;
    return true;
  case KeyEvent::Button2Press:
    motion.type = GetWeaponType();
    // 移動の方向は変わっていない
    return true;
  case KeyEvent::Button3Press:
    motion.type = GetWeaponType();
    // 移動の方向は変わっていない
    return true;
  default:
    // モーションの変化とみなさない
    return false;
  }
}
