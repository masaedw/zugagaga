#ifndef ZUGAGAGA_CLIENT_DRAW_HPP
#define ZUGAGAGA_CLIENT_DRAW_HPP

#include <vector>
#include <stack>
#include <boost/shared_ptr.hpp>
#include "comtypes.hpp"
#include "graphic.hpp"

namespace zu3ga {
  ///< 描画するオブジェクトのデータは送受信するデータと同じ
  typedef SendObject DrawData;

  ///< 対戦中に描画する御札のIDの型
  namespace StatusTagID {
    enum t {
      ID0,
      ID1,
      ID2,
      ID3,
      NoUse,
    };
  }

  ///< 対戦中に描画する御札の情報
  class StatusTag
  {
  public:
    StatusTagID::t id;		///< hoge人目と描画するために必要な
    std::string name;		///< クライアントの名前
    Team::t team;		///< チーム(いろはに)

    StatusTag(){}
    StatusTag(StatusTagID::t id, const std::string& name, Team::t team)
      : id(id), name(name), team(team)
    {}
  };

  ///< 描画マネージャを初期化するのに必要な情報
  struct DMInitData
  {
    StageType::t stg_type;
    StatusTag tags[4];		///< 御札を描画する順番に情報を渡す
  };

  /**
   * 描画マネージャ
   *
   * SendObjとSendParamを描画する
   */
  class DrawingManager
  {
  private:
    typedef std::vector<edw::sdl::sp_Graphic> GAry;
    typedef GAry::iterator GAIter;
    typedef std::vector<GAry> GAryAry;
    typedef GAryAry::iterator GAAIter;

    static const short hp_buf_red = 2; ///< 1フレームでバッファのHPを減らす量
    static const short flame_frame = 3; ///< 炎１枚を表示するフレーム数

    ///< スクリーンのサーフェス
    edw::sdl::SmartSurface screen;
    ///< SendObjAryを描画するためのグラフィク郡
    GAryAry graphics_;

    ///< アニメーションのためにDrawAllした回数を記憶する
    unsigned int frame;

    // ステージ
    StageType::t stg_type;
    edw::sdl::sp_Graphic stg_back;

    // 御札
    GAry tags;
    Pos tag_pos[4];
    HitPoints hp_buf;		///< HPのバッファ．炎のアニメーションに使う
    edw::sdl::sp_Graphic status_back;
    edw::sdl::sp_Graphic black_big;
    edw::sdl::sp_Graphic black_small;
    edw::sdl::sp_Graphic burnt_big;
    edw::sdl::sp_Graphic burnt_small;
    GAry flame_big;
    GAry flame_small;
    int own_id;
    unsigned int client_num;

    // 経過時間
    edw::sdl::sp_Graphic elapse;
    bool elap_draw;		// 経過時間の描画がはじまっているか
    Uint32 base_time;		// 経過時間の計算がはじまったときのms
    const edw::sdl::sp_Font time_font;
    static const unsigned ten_ms = 10;
    static const unsigned one_sec = ten_ms * 100;
    static const unsigned one_min = one_sec * 60;

    // ロックオンサイト
    GAry lock_r;
    GAry lock_rparts;
    GAry lock_y;
    GAry lock_yparts;

    // DrawTagsByBlackで使う変数
    std::vector<unsigned int> dec_frame_stack; // 前回HPが減ったフレーム
    HitPoints pre_hp;		// 前回のHP

    // private_method
    edw::sdl::sp_Graphic GetStatusTagGraphic(const StatusTag&, bool myself=false);
    void DrawTagsByFlame(const SendParam*);
    void DrawTagsByBlack(const SendParam*);

  public:
    DrawingManager(const DMInitData&);
    Pos GetScreenPos(const Pos&);
    void DrawAll(const SendObjAry*, const SendParam*);
    void DrawSendObjAry(const SendObjAry*); // テスト用．パラメータなしで描画できる
    void DrawStage(const Pos& scr_pos);
    void LetsStart()
    {
      elap_draw = true;
      base_time = SDL_GetTicks();
    }
  };
  typedef boost::shared_ptr<DrawingManager> sp_DrawingManager;
}

#endif // ZUGAGAGA_CLIENT_DRAW_HPP
