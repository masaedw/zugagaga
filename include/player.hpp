// last modified <2007/01/27 14:11:34 2007>
#ifndef ZUGAGAGA_PLAYER_HPP
#define ZUGAGAGA_PLAYER_HPP

#include <queue>
#include <stack>
#include <vector>
#include "event.hpp"
#include "key_command.hpp"
#include "objects.hpp"
#include "motion.hpp"
#include "comtypes.hpp"

namespace zu3ga {
  // コンパイラのご機嫌とり
  class Objects;

  /**
     プレイヤーのパラメータ構造体
     キャラクター毎に違うので，継承先で値は決まる
  */
  struct PlayerParam
  {
    short int max_hp;	///< 最大HP
    short int dash_gauge; ///< 最大ダッシュゲージ量
    double walk_speed;	///< 歩く速度
    double dash_speed;	///< ダッシュの速度
    double slow_damage_speed; ///< 弱攻撃をくらったときのノックバック速度
    double fast_damage_speed; ///< 強攻撃をくらったときのノックバック速度
    double close_range_speed; ///< 近距離攻撃の移動速度
    double tracking;	///< 近距離攻撃の追尾能力(rad/frame)
    ObjDetection own_obj_det; ///< 当たり判定

    PlayerParam(){}
    PlayerParam(short int max_hp,
		short int dash_gauge,
		double walk_speed,
		double dash_speed,
		double slow_damage_speed,
		double fast_damage_speed,
		double close_range_speed,
		double tracking,
		ObjDetection own_obj_det)
      : max_hp(max_hp)
      , dash_gauge(dash_gauge)
      , walk_speed(walk_speed)
      , dash_speed(dash_speed)
      , slow_damage_speed(slow_damage_speed)
      , fast_damage_speed(fast_damage_speed)
      , close_range_speed(close_range_speed)
      , tracking(tracking)
      , own_obj_det(own_obj_det)
    {}
  };

  /**
     プレイヤーの基底クラス．

     継承して作成するときの注意
     OneFrameProcで処理を書くとき，1フレーム目が動作のはじまりを表している．(0ではない)
     武器やダウン等の，
     動作を受け付けない状態が終わるフレームには必ずBeginMoveを呼び出すこと．
  */
  class Player : public Object
  {
  protected:
    SendObjType::t type;

    //static const double down_float = -gravity * 9; // cannot appear in a constant-expression と言われてコンパイルできない
    static const double down_float = -1.5 * 9; ///< ダウン時の浮き(高さ方向の初速)
    static const double e = 0.6; ///< バウンドするときの反発係数
    ///< 敵の位置に応じて描画位置をずらす．その最大の距離
    static const double draw_correct_max_dist = 200;
    static const unsigned stagger_frame = 20; ///< よろけの硬直フレーム数
    static const unsigned combo_frame = 23; ///< コンボとみなすフレーム数
    static const unsigned charge_damage_frame = 20; ///< LightAttackを蓄積するフレーム数
    static const unsigned down_frame = 20; ///< ダウンしているフレーム数
    static const unsigned situp_invincib = 15; ///< 起き上がりの無敵時間
    static const unsigned char stagger_num = 4; ///< よろけるまでの蓄積数
    static const unsigned int combo_down_num = 4; ///< 何ヒットでダウンするか
    static const int next_target_frame = 7; ///< 次のターゲットと認識するまでの時間
    static const double lock_near_dist = 3; ///< ロックオンサイトを瞬間移動させても問題のない距離

    ///< ロックオンサイトをボタンで移動した後，次に自動移動するまでに時間
    static const int lock_site_frame = 60;
    bool new_target_moving;	///< ターゲットが新しいものに移った瞬間ならtrue
    bool pre_explosion;		///< 爆発に以降する寸前
    DPos pre_diff;		///< 前回のフレームの描画点のずれ
    bool search;		///< 今回のフレーム敵をサーチするか否か
    std::vector<Player*> enemy_list; ///< 敵プレイヤー達のポインタ
    ///< ロックオンサイトを動かさない残りフレーム数(自動照準モードでサーチボタンを押した後)
    int lock_site;
    ///< ロックオンしている敵の情報．ロックオンして何フレーム立ったか記憶する．
    std::map<Player*, int> lock_map;
    bool weapon_lock;

    PlayerParam param;	///< 継承先キャラクタに設定されるプレイヤーのパラメータ
    bool auto_search;		///< 自動サーチをするか否か
    unsigned int abs_frame;	///< プレイヤーが起動してからの経過フレーム数
    int invincib;		///< 残り無敵フレーム数．普段は0
    short int hp;		///< 現在のHP
    short int dash_gauge;	///< 現在のダッシュゲージの量
    PlayerMotion motion;	///< 現在のモーション
    Direction::t body_dir;	///< 体の向き
    unsigned int state;		///< 現在のSendObjState
    DPos pos;			///< 現在の座標
    DPos speed;			///< 現在の速度
    Team::t team;		///< 自分が属しているチーム
    Objects* objs;		///< 物体管理クラスとの関連
    Player* target;		///< 今狙っている敵
    Player* next_tar;		///< ターゲット変更候補
    ///< 次のターゲット候補を認識している時間．これが蓄積してはじめてターゲットが変更される
    int nt_frame;
    double appr_dir;		///< 近付いている方向(radian)
    double ver_pos;		///< 現在の高さ．宙に浮くモーションのときのみ使用
    double ver_speed;		///< 高さ方向への速度．
    bool body_dir_lock;		///< 体の向きを固定するか否か

    ///< クライアントイベントをキー情報に変換するクラス
    KeyEventCreater kecreater;	
    ///< キー入力を必殺技(笑)に変換するクラス
    CommandManager cmanager;
    /**
       連射系武器の蓄積数．蓄積するとよろける
       格納する値はframe
    */
    std::queue<unsigned int> ldq;
    /**
       よろける武器の蓄積数．コンボ補正に影響する
       格納する値はframe
    */
    std::stack<unsigned int> combo_stack;

    // private method
    Player* FindNearestEnemy();	///< 最も近い敵を検索する
    void BeginMove();		///< OneFrameProcで武器の動作が終わったときに呼ぶべき関数
    void TurnOnTarget();	///< 敵の方向を向く
    bool SetCollDetect(const Detection&); ///< ぶつかり判定を設定する
    bool SetAttackDetect(const Detection&); ///< 攻撃判定を設定する
    short int GetHorizonLap(const Detection&); ///< 引数の判定との水平方向の重なりを得る
    short int GetVerticalLap(const Detection&); ///< 引数の判定との垂直方向の重なりを得る
    void SetSpeed();		///< 速度を設定する
    DPos GetWalkSpeed();	///< 歩きのときの速度を得る
    ///< 体の向きの固定を開始する
    void BodyDirLock(){ body_dir_lock = true; }
    ///< 体の向きの固定を解除する
    void BodyDirUnlock(){ body_dir_lock = false; }
    ///< 現在の状態(SendObjState)を返す
    unsigned char GetSendObjState(){ return state; }
    ///< 新しいモーションを設定する
    bool SetNewMotion(PlayerMotion& pre, const PlayerMotion& new_m);
    void DecHP(short int damage); ///< HPをdamage分減らす
    void AddLightDamage(const Detection&); ///< 連射系武器のダメージを蓄積させる
    void SetStaggerDamage(const Detection&); ///< よろけダメージを設定する
    void MakeEnemyList();
    Player* GetNextEnemy(Player*);
    bool AdjustStageEnd(SendPos&);

    virtual void UpdateSpeed(); ///< 動作の決まっている速度の更新する
    unsigned char GetSendObjType() { return type; }
    virtual void OneFrameProc() = 0; ///< １フレームの処理を書く
    virtual unsigned char GetShadowState()
    {
      return 0;
    }
    virtual SendPos GetShadowPos()
    {
      SendPos spos(short(pos.x), short(pos.y));
      return spos;
    }
    bool IsGettingAhead();

  public:
    Player(Team::t team, Objects*, const PlayerParam&, bool auto_search = true);
    virtual ~Player(){}
    void SetPos(const DPos&);		///< プレイヤーの座標を設定する

    ///< クライアントイベントから自分の状態を変更する
    virtual void ChangeState(unsigned char event);
    virtual void Update();		///< 時間を更新する
    SendObject GetSendObject(); ///< クライアントに送信する構造体を得る
    virtual SendPos GetSendPos();
    virtual bool GetSendParamPos(SendPos&);
    virtual SendPos GetSendParamPos();
    virtual short GetHitPoint() { return hp * 1000 / param.max_hp; }
    virtual DashParam GetDashParam()
    { 
      if (pre_explosion) {
	return 0;
      } else {
	return dash_gauge * 100 / param.dash_gauge;
      }
    }
    DPos GetPos(){ return pos; }
    Team::t GetTeam(){ return team; }

    void WeaponLockRelease(){ weapon_lock = false; }
    void CheckStageEnd();
    bool GetDetection(Detection&);
    Detection GetDetection();
    virtual bool SetDetection(const Detection&);
    double GetAbsSpeed();
    virtual void SetLockOnInfo(Player*);
    virtual NonTypeSendObject GetLockOnSite();
    virtual SendPos GetLockOnSitePos()
    {
      SendPos spos;
      spos.x = short(pos.x);
      spos.y = short(pos.y);
      spos.y -= 10;
      return spos;
    }
    unsigned GetInv() { return invincib; }
    virtual bool GetShadow(SendObject& so)
    {
      so.type = SendObjType::Shadow;
      so.pos = GetShadowPos();
      so.state = GetShadowState();
      return true;
    }
    bool GetArrow(SendObject& arrow, int id, const SendPos& view_pos, bool focus = false);
    unsigned char Player::GetArrowState(Team::t, double, Direction::t);
    int GetSiteSize(double dist);
    Player* GetTarget(){ return target; }
    SendPos GetDashGaugePos(bool up_draw = false)
    { 
      if (up_draw) {
	return SendPos(short(pos.x) - 25, short(pos.y) - 50);
      } else {
	return SendPos(short(pos.x) - 25, short(pos.y) + 20);
      }
    }
    short GetIntersectingPointY(const Pos& src, const Pos& vec, short x)
    {
      return short(src.y + ((double(x - src.x) / vec.x) * vec.y));
    }
    short GetIntersectingPointX(const Pos& src, const Pos& vec, short y)
    {
      return short(src.x + ((double(y - src.y) / vec.y) * vec.x));
    }
    void UpdateTarget();
  };

  ///< やられた後のプレイヤー
  class DeadPlayer : public Player
  {
  private:
    // 純粋仮想関数をとりあえず書いとく
    virtual void OneFrameProc(){};
    virtual SendPos GetShadowPos(){ return SendPos(); }
    
  public:
    DeadPlayer(const DPos& a_pos, Objects* objs)
      : Player(Team::I, objs, PlayerParam())
    { SetPos(a_pos); }
    bool GetSendParamPos(SendPos& spos)
    {
      spos.x = short(pos.x);
      spos.y = short(pos.y);
      AdjustStageEnd(spos);

      return true;
    }
    SendPos GetSendParamPos()
    {
      SendPos spos;
      GetSendParamPos(spos);
      return spos;
    }
    bool GetDetection(Detection&){ return false; }
    unsigned char GetSendObjType() { return SendObjType::NoGraphic; }
    short GetHitPoint() { return 0; }
    DashParam GetDashParam(){ return 100; }
    SendPos GetLockOnSitePos() // ロックオンされることはないが…
    {
      SendPos spos;
      spos.x = short(pos.x);
      spos.y = short(pos.y);
      spos.y -= 10;
      return spos;
    }
    NonTypeSendObject GetLockOnSite() // 見えないように…
    { 
      NonTypeSendObject so;
      so.pos.x = -1000;
      so.pos.y = -1000;
      so.state = 0;
      return so;
    }
    void ChangeState(unsigned char event); 
    void Update();
  };

  ///< ロックオン対象が誰もいない時のダミープレイヤー
  class DummyPlayer: public Player
  {
  private:
    // 純粋仮想関数をとりあえず書いとく
    virtual void OneFrameProc(){};
    virtual SendPos GetShadowPos(){ return SendPos(); }

  public:
    DummyPlayer()
      : Player(Team::I, 0, PlayerParam()){}
    SendPos GetLockOnSitePos() // 見えないように…
    {
      SendPos spos;
      spos.x = -1000;
      spos.y = -1000;
      return spos;
    }
    void SetPos(const Pos& p)
    {
      pos.x = double(p.x);
      pos.y = double(p.y);
    }
  };
}

#endif // ZUGAGAGA_PLAYER_HPP
