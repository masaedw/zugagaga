/** @file
   問題領域クラス

   last modified <2004/01/29 22:16:03 2004 JST>
 */
#ifndef ZUGAGAGA_OBJECTS_HPP
#define ZUGAGAGA_OBJECTS_HPP

#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include "comtypes.hpp"
#include "object.hpp"
#include "detect.hpp"

namespace zu3ga {
  // コンパイラのご機嫌とり
  class Player;

  typedef std::map<int, Player*> PlayerMap;
  typedef PlayerMap::iterator PMIter;
  typedef std::list<Object*> ObjectAry;
  typedef ObjectAry::iterator OAIter;

  class Objects
  {
  private:
    Team::t winner;

    ObjectAry obj_ary;
    ObjectAry stage_obj_ary;
    DPos start_pos[4];

    SendObjAry* send_obj_ary;
    PlayerMap players;
    PlayerMap dead_players;
    int next_id;		///< 次に追加されるプレイヤーに割り振られるID
    Pos stage_end;		///< ステージの端の座標

    // 当たり判定のためのメモリをあらかじめ確保(処理を軽くする目的)
    Detection ldet;
    Detection rdet;
    Detection* sdet;

    // private method
    void Hoge( ); ///< 二つの物体のぶつかり判定を行う
    void aCollCheck(Object*, Object*);
    void aStageObjCollCheck(Object*, Object*);
    void aHitCheck(Object*, Object*);
    void aStageObjHitCheck(Object*, Object*);
    bool CheckRect(Detection&, Detection&); ///< 当たり判定長方形の重なりを調べる
    void EraseObj(OAIter&);

  public:
    Objects(StageType::t stage_t = StageType::Forest);
    ~Objects();

    /**
       プレイヤーイベントをセット
     */
    Objects& ChangePlayerState(unsigned char event, int id);
    /**
       全ての情報更新
     */
    Objects& UpdateAll();
    /**
       当たり判定
     */
    Objects& HitCheck();
    /**
       送信すべきデータをつくって返す

       @return 送信すべきデータ delete しちゃいけない
     */
    const SendObjAry* GetSendData();
    /**
       各ユーザが取得できるオブジェクトの位置
       以外のパラメータを返す.
       @param id ソケット？
     */
    SendParam GetSendParam(int id);

    /**
       よーいはじめの合図

       Getなんとかのあと(updateのまえ)
     */
    void LetsStart();

    /**
       もう終わった？
    */
    Team::t IsFinished();

    int AddPlayer(const PlayerInitData&); ///< プレイヤーを追加する．追加したプレイヤーのIDを返す
    void AddObject(Object*);	///< プレイヤー以外の物体を追加する
    void AddStageObject(Object*); ///< ステージの物体(移動しない)を追加する
    PlayerMap* GetPlayerMap(){ return &players; } ///< IDと対応したプレイヤーマップを得る
    void SetPlayerPos(int id, const DPos& pos); ///< プレイヤーの座標を設定する．テスト用
    const Pos& GetStageEnd(){ return stage_end; } ///< ステージの端の座標を得る
    std::vector<DrawableDetect> GetDrawableDetectAry();
  };
  typedef boost::shared_ptr<Objects> sp_Objects;
}

#endif // ZUGAGAGA_OBJECTS_HPP
