// last modified <2004/02/02 14:42:45 2004 JST>
#ifndef ZUGAGAGA_KEY_COMMAND_HPP
#define ZUGAGAGA_KEY_COMMAND_HPP

#include <list>
#include "event.hpp"
#include "motion.hpp"

namespace zu3ga {
  struct InputKeyData {
    KeyEvent::KeyEvent_t key;
    unsigned int frame;

    InputKeyData()
      : key(KeyEvent::Arrow1), frame(0) {}
    InputKeyData(KeyEvent::KeyEvent_t key, unsigned int frame)
      : key(key), frame(frame) {}
  };

  class InputQueue {
  private:
    int count;
    enum {
      max_size = 3, // コマンド入力に必要な数
    };
    InputKeyData data[max_size];
    int index;

    int Next(int in)
    {
      in++;
      if (in >= max_size) {
	in = 0;
      }
      return in;
    }
    int RealDig(int in)
    {
      while (in >= max_size) {
	in -= max_size;
      }
      while (in < 0) {
	in += max_size;
      }
      return in;
    }
    
  public:
    InputQueue() : count(0), index(0) {}
    void Push(const InputKeyData& in)
    {
      ++count;

      data[index] = in;
      index = Next(index);
    }
    int Size(){ return count; }
    ///< 過去i番目のデータを得る
    InputKeyData* Get(int i)
    {
      return &data[RealDig(index - i - 1)];
    }

    ///< デバッグ用．登録されているすべてのデータを表示する
    void PrintAllData()
    {
      for (int i = 0; i < max_size; ++i) {
	PrintData(i);
      }
      std::cout << std::endl;
    }
    ///< デバッグ用．Get(i)を実行したときのデータを表示する
    void PrintData(int i)
    {
      std::cout << "Get(" << i << ")" << std::endl;
      std::cout << "key: " << int(Get(i)->key) << std::endl;
      std::cout << "frame: " << Get(i)->frame << std::endl;
    }
  };

  /**
     キーイベントから実際の動作を生むクラス
  */
  class CommandManager
  {
  private:
    InputQueue q;
    InputQueue btn_q;
    unsigned int frame;

    // private method
    void PushArrowKey(KeyEvent::KeyEvent_t keve); ///< 十字キー情報を記憶する
    PlayerMotion::Type GetMoveForm(); ///< 今入力されている移動形態を得る．歩きかダッシュ
    PlayerMotion::Type GetWeaponType();	///< 今入力されている武器のタイプを得る
    bool IsArrow(KeyEvent::KeyEvent_t keve) {
      if (KeyEvent::Arrow9 >= keve && keve >= KeyEvent::Arrow1) {
	return true;
      }
      return false;
    }
    
  public:
    CommandManager() : frame(0) {}
    bool DisposeKeyEvent(KeyEvent::KeyEvent_t, PlayerMotion&); ///< キーイベントからモーションを得る
    void UpdateFrame(){ ++frame; } ///< フレーム数を更新する
  };
}

#endif // ZUGAGAGA_KEY_COMMAND_HPP
