// last modified <2004/02/07 21:57:45 2004 JST>
//============================================================
// イベント処理全般の実装
//============================================================
#include "event.hpp"
#include <stdexcept>
using namespace std;
using namespace zu3ga;

//============================================================
// KeyEventCreater::ConvertClientEvent
// クライアントイベントからキーイベント生成
//============================================================
KeyEvent::KeyEvent_t
KeyEventCreater::ConvertClientEvent(unsigned char eve)
{
  bool press = false;
  if (eve & ClientEvent::Press) {
    // eveのPress領域を削除
    eve ^= ClientEvent::Press;
    press = true;
  }

  switch (eve) {
  case ClientEvent::Up:
  case ClientEvent::Down:
  case ClientEvent::Left:
  case ClientEvent::Right:	// 十字キーのイベントだった場合
    {
      // 十字キー管理クラスに設定
	if (press) {
	  akmanager.Add(eve);
	} else {
	  akmanager.Delete(eve);
	}
      return akmanager.GetKeyState(); // 十字キー状態を得る
    }
  case ClientEvent::Button1:	// ボタン１の場合
    if (press) {
      return KeyEvent::Button1Press;
    } else {
      return KeyEvent::Button1Release;
    }
  case ClientEvent::Button2:	// ボタン２の場合
    if (press) {
      return KeyEvent::Button2Press;
    } else {
      return KeyEvent::Button2Release;
    }
  case ClientEvent::Button3:	// ボタン３の場合
    if (press) {
      return KeyEvent::Button3Press;
    } else {
      return KeyEvent::Button3Release;
    }
  }

  // キーイベントでない
  throw logic_error("キーイベントでないイベントが戦闘中プレイヤーに渡されました．");
}

//============================================================
// ArrowKeyManager::ClientEventToArrowKey
// クライアントイベントを十字キー情報に変換する
// イベントが十字キーのイベントでない場合ArrowKey::Noneを返す
//============================================================
ArrowKeyManager::ArrowKey_t 
ArrowKeyManager::ClientEventToArrowKey(const unsigned char clie_eve)
{
  switch (clie_eve) {
  case ClientEvent::Up:
    return Up;
  case ClientEvent::Down:
    return Down;
  case ClientEvent::Left:
    return Left;
  case ClientEvent::Right:
    return Right;
  default:
    return None;
  }
}

//============================================================
// ArrowKeyManager::Add
// 十字キー情報を追加する
// 十字キーが押されたときに呼ぶ
//============================================================
void ArrowKeyManager::Add(unsigned char eve)
{
  ArrowKey_t arrow = ClientEventToArrowKey(eve);
  // 十字キーのイベントでないなら終了
  if (arrow == None) {
    return;
  }

  // すでに登録されていないか検索
  // 
  // 正しく十字キーが放されたイベントを感知し，
  // そのつどDeleteされていれば
  // すでに登録されていることはありえないが．
  for (AKAIter i = keys.begin();
       i != keys.end(); ++i) {
    // 同じ方向の登録を発見
    if (*i == arrow) {
      return;
    }
  }

  keys.push_back(arrow);
}

//============================================================
// ArrowKeyManager::Delete
// 十字キー情報を消去する
// 十字キーが放されたときに呼ぶ
//============================================================
void ArrowKeyManager::Delete(unsigned char eve)
{
  ArrowKey_t arrow = ClientEventToArrowKey(eve);
  // 十字キーのイベントでないなら終了
  if (arrow == None) {
    return;
  }

  for (AKAIter i = keys.begin();
       i != keys.end(); ) {
    // 消去したい情報を発見
    if (*i == arrow) {
      i = keys.erase(i);
    }
    else {
      ++i;
    }
  }
}

//============================================================
// ArrowKeyManager::GetKeyState
// 現在押されている十字キーの方向を得る
//============================================================
KeyEvent::KeyEvent_t
ArrowKeyManager::GetKeyState()
{

  Vertical::t ver(Vertical::None);
  Horizon::t hor(Horizon::None);

  // 垂直，水平方向の情報を各自調べる
  // 後に登録された情報が優先される
  for (AKAIter i = keys.begin();
       i != keys.end(); ++i) {
    switch (*i) {
    case Up:
      ver = Vertical::Up;
      break;
    case Down:
      ver = Vertical::Down;
      break;
    case Left:
      hor = Horizon::Left;
      break;
    case Right:
      hor = Horizon::Right;
      break;
    case None:
      break;
    }
  }

  // 実際のキーボードの状態を返す
  switch (ver) {
  case Vertical::Up:
    switch (hor) {
    case Horizon::Left:
      return KeyEvent::Arrow7;
    case Horizon::Right:
      return KeyEvent::Arrow9;
    case Horizon::None:
      return KeyEvent::Arrow8;
    }
    break;
  case Vertical::Down:
    switch (hor) {
    case Horizon::Left:
      return KeyEvent::Arrow1;
    case Horizon::Right:
      return KeyEvent::Arrow3;
    case Horizon::None:
      return KeyEvent::Arrow2;
    }
    break;
  case Vertical::None:
    switch (hor) {
    case Horizon::Left:
      return KeyEvent::Arrow4;
    case Horizon::Right:
      return KeyEvent::Arrow6;
    case Horizon::None:
      return KeyEvent::Arrow5;
    }
    break;
  }

  throw logic_error("クライアントのキーボードの状態を得ることに失敗しました．");
}

// 今のキーボードの向きを得る
// 何も押されていない場合Direction::Noneを返す
Direction::t KeyEventCreater::GetKeyDirection()
{
  switch (akmanager.GetKeyState()) {
  case KeyEvent::Arrow1:
    return Direction::Dir1;
  case KeyEvent::Arrow2:
    return Direction::Dir2;
  case KeyEvent::Arrow3:
    return Direction::Dir3;
  case KeyEvent::Arrow4:
    return Direction::Dir4;
  case KeyEvent::Arrow5:
    return Direction::None;
  case KeyEvent::Arrow6:
    return Direction::Dir6;
  case KeyEvent::Arrow7:
    return Direction::Dir7;
  case KeyEvent::Arrow8:
    return Direction::Dir8;
  case KeyEvent::Arrow9:
    return Direction::Dir9;
  default:
    return Direction::None;
  }
}
