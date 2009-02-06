/** @file
   class Objectsをテストするためのプログラム
*/
// last modified <2004/02/09 01:17:20 2004 JST>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "boost/shared_ptr.hpp"
#include "objects.hpp"
#include "draw.hpp"
#include "event.hpp"
#include "verbose.hpp"

using namespace edw;
using namespace edw::sdl;
using namespace std;
using namespace zu3ga;

int FPS = 30;

class HogeTimer
{
 private:
  unsigned int interval;
  unsigned int stime;
  unsigned int fpstime;
  unsigned int count;
  unsigned int skip;
 public:
  HogeTimer() : skip(0) {}
  ~HogeTimer() {}
  void SetFPS( unsigned int in )
  {
    interval = 1000 / in;
  }
  void SetInterval( double in )
  {
    interval = (unsigned long)( in * 1000 );
  }
  bool Start()
  {
    unsigned int ntime = SDL_GetTicks();
    if (stime + (skip + 1) * interval > ntime) {
      stime = ntime;
      skip = 0;
      count++;
      return true;
    } else {
      skip++;
      return false;
    }
  }
  bool NonSkipStart()
  {
    stime = SDL_GetTicks();
    count++;
    return true;
  }
  double Wait()
  {
    unsigned int ntime = SDL_GetTicks();
    if ( interval > ntime - stime )
      SDL_Delay( interval - ( ntime - stime ) );
    double fps = 0;
    if ( ntime - fpstime > 1000 ) {
      fps = count * 1000.0 / ( ntime - fpstime );
      count = 0;
      fpstime = ntime;
    }
    return fps;
  }
};


struct ButtonHaichi;

void PrintParam(const SendParam&); // テスト用パラメータを出力関数
int ButtonHaichiHenkou();
vector<ButtonHaichi> ReadButtonHaichi();

bool SDLKeyEventToClientEvent2(const SDL_Event& sdl_eve, 
			       unsigned char& clie_eve,
			       int& id);
bool SDLKeyEventToAdjustEvent(const SDL_Event& sdl_eve,
			      unsigned char& adjust_eve);
bool GetAdjustEvent(unsigned char& ad_eve);
bool GetClientEvent2(unsigned char& clie_eve,
		     int& id);
int GetSDLKeySym();
int GraphicPosAdjustMode();

struct ButtonHaichi
{
  int up, down, left, right, w1, w2;
};

struct EventInit
{
  EventInit()
  {
    EventManager::Init();
  }

  ~EventInit()
  {
    EventManager::Quit();
  }
};

int main(int argc, char** argv)
{
  bool teki_move = false;
  bool battle = false;
  bool draw_detect = false;
  bool no_draw = false;
  bool news_fps = false;
  bool much = false;
  bool video_test = false;
  bool yamaha = false;
  bool ntt = false;

  verbose::setVerboseLevel(3);
  graphic::SetImagesDir("../../images/");

  // オプションを設定
  vector<string> options;
  if (argc >= 2) {
    for (int i = 1; i != argc; ++i) {
      options.push_back(string(argv[i]));
    }
  }
  for (vector<string>::iterator i = options.begin();
       i != options.end(); ++i) {
    if (*i == string("-h")) {
      cout << "-h    HELP" << endl;
      cout << "-m    敵が適当に動くようになる" << endl;
      cout << "-b    二人用対戦ゲームがはじまる" << endl;
      cout << "-k    二人用対戦ゲームのキー配置を変更する" << endl;
      cout << "-d    当たり判定表示" << endl;
      cout << "-g    画像調節モード" << endl;
      cout << "-n    描画を行わない" << endl;
      cout << "-f    FPSを知らせる" << endl;
      cout << "-4    敵を多く出現させる" << endl;
      cout << "-v    全力で描画を続けたときのFPSをはかる" << endl;
      cout << "-y    使用キャラが山葉になる" << endl;
      cout << "-p    使用キャラがペンギンになる" << endl;
      return 0;
    }
    if (*i == string("-k")) {
      cout << "***対戦用ボタン配置変更***" << endl;
      return ButtonHaichiHenkou();
    }
    if (*i == string("-m")) {
      cout << "***敵移動***" << endl;
      teki_move = true;
    }
    if (*i == string("-b")) {
      cout << "***対戦***" << endl;
      battle = true;
    }
    if (*i == string("-d")) {
      cout << "***当たり判定描画***" << endl;
      draw_detect = true;
    }
    if (*i == string("-g")) {
      cout << "***画像調節用モード***" << endl;
      return GraphicPosAdjustMode();
    }
    if (*i == string("-n")) {
      cout << "***描画を行わない***" << endl;
      no_draw = true;
      news_fps = true;
      FPS = 100000;
    }
    if (*i == string("-f")) {
      cout << "***FPSを知らせる***" << endl;
      news_fps = true;
    }
    if (*i == string("-4")) {
      cout << "***4体出現***" << endl;
      much = true;
    }
    if (*i == string("-v")) {
      cout << "***全力で動かしてFPSをはかる***" << endl;
      video_test = true;
      news_fps = true;
      FPS = 100000;
    }
    if (*i == string("-y")) {
      cout << "***自キャラ山葉***" << endl;
      yamaha = true;
    }
    if (*i == string("-p")) {
      cout << "***自キャラペンギン***" << endl;
      ntt = true;
    }
  }

  boost::shared_ptr<graphic::VideoInit> video_init;
  boost::shared_ptr<Objects> objs;
  boost::shared_ptr<DrawingManager> dmanager;

  HogeTimer timer;
  timer.SetFPS(FPS);

  try {
    video_init = boost::shared_ptr<graphic::VideoInit>(new graphic::VideoInit);
    objs = boost::shared_ptr<Objects>(new Objects(StageType::Forest));
  } catch (exception& e) {
    cout << "グラフィックの初期化エラー: " << e.what() << endl;
    exit(1);
  }

  // プレイヤー追加
  PlayerInitData data;
  if (yamaha) {
    data.type = PlayerType::Yamaha;
  } else if (ntt) {
    data.type = PlayerType::NTT;
  } else {
    data.type = PlayerType::Planex;
  }
  data.team = Team::I;
  int id = objs->AddPlayer(data);
  
  // 敵追加
  // data.type = PlayerType::Planex;
  data.type = PlayerType::Yamaha;
  data.team = Team::Ro; // 敵チーム
  int teki_id = objs->AddPlayer(data);

  DMInitData dmi;
  dmi.stg_type = StageType::Forest;
  dmi.tags[0] = StatusTag(StatusTagID::ID0, "name0", Team::I);
  dmi.tags[1] = StatusTag(StatusTagID::ID1, "name1", Team::Ro);
  dmi.tags[2].id = StatusTagID::NoUse;
  dmi.tags[3].id = StatusTagID::NoUse;

  int teki_id2;
  int teki_id3;
  // 敵を多く追加
  if (much) {
    data.team = Team::Ha;
    teki_id2 = objs->AddPlayer(data);
    data.team = Team::Ni;
    teki_id3 = objs->AddPlayer(data);

    dmi.tags[2] = StatusTag(StatusTagID::ID2, "name2", Team::Ha);
    dmi.tags[3] = StatusTag(StatusTagID::ID3, "name3", Team::Ni);
  }

  try {
    dmanager = boost::shared_ptr<DrawingManager>(new DrawingManager(dmi));
  } catch (exception& e) {
    cout << e.what() << "描画オブジェクトの初期化エラー" << endl;
  }

  EventInit init__;

  unsigned char clie_eve;
  bool skip = false;
  SmartSurface screen = GetVideoSurface();
  cout << "メインループ開始" << endl;
  try {
    unsigned int frame = 0;
    while (true) {
      ++frame;
      if (video_test) {
	timer.NonSkipStart();
      } else {
	if (timer.Start()) {
	  skip = false;
	} else {
	  skip = true;
	}
      }
      if (frame == 60) {
	cout << "試合開始" << endl;
	objs->LetsStart();
	dmanager->LetsStart();
      }

      //----------------------------------------
      // キー情報を取得してプレイヤーに設定
      //----------------------------------------
      if (battle) {
	int temp_id;
	while (GetClientEvent2(clie_eve, temp_id)) {
	  try {
	    if (int(clie_eve) > 200) {
	      cout << "テストプログラムを終了します" << endl;
	      goto MainLoopEnd;
	    }
	    // プレイヤーにイベントを反映させる
	    objs->ChangePlayerState(clie_eve, temp_id);
	  } catch (std::exception& e) {
	    std::cerr << "error: ChangePlayerState: " << e.what() << endl;
	    goto MainLoopEnd;
	  }
	}
      }
      else {
	while (EventManager::GetInstance()->GetEvent(clie_eve)) {
	  if (clie_eve == ClientEvent::Quit) {
	    cout << "テストプログラムを終了します" << endl;
	    goto MainLoopEnd;
	  }
	  try {
	    // プレイヤーにイベントを反映させる
	    objs->ChangePlayerState(clie_eve, id);
	  } catch (std::exception& e) {
	    std::cerr << "error: ChangePlayerState: " << e.what() << endl;
	    goto MainLoopEnd;
	  }
	}
      }

      // 敵が動くモードならば敵のキー入力をする
      if (teki_move) {
	static unsigned int teki_frame = 0;
	++teki_frame;
	if (teki_frame == 1) {
	  objs->ChangePlayerState(ClientEvent::Right, teki_id);
	  objs->ChangePlayerState(ClientEvent::Left | ClientEvent::Press, teki_id);
	}
	if (teki_frame == 51) {
	  objs->ChangePlayerState(ClientEvent::Left, teki_id);
	  objs->ChangePlayerState(ClientEvent::Right | ClientEvent::Press, teki_id);
	  objs->ChangePlayerState(ClientEvent::Button2 | ClientEvent::Press, teki_id);
	  objs->ChangePlayerState(ClientEvent::Button2, teki_id);
	}
	if (teki_frame == 100) {
	  teki_frame = 0;
	  objs->ChangePlayerState(ClientEvent::Button2 | ClientEvent::Press, teki_id);
	  objs->ChangePlayerState(ClientEvent::Button2, teki_id);
	}

	if (much) {
	  if (teki_frame == 1) {
	    objs->ChangePlayerState(ClientEvent::Right, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Left | ClientEvent::Press, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Right, teki_id3);
	    objs->ChangePlayerState(ClientEvent::Left | ClientEvent::Press, teki_id3);
	  }
	  if (teki_frame == 51) {
	    objs->ChangePlayerState(ClientEvent::Left, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Right | ClientEvent::Press, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Button2 | ClientEvent::Press, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Button2, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Left, teki_id3);
	    objs->ChangePlayerState(ClientEvent::Right | ClientEvent::Press, teki_id3);
	    objs->ChangePlayerState(ClientEvent::Button2 | ClientEvent::Press, teki_id3);
	    objs->ChangePlayerState(ClientEvent::Button2, teki_id3);
	  }
	  if (teki_frame == 100) {
	    teki_frame = 0;
	    objs->ChangePlayerState(ClientEvent::Button2 | ClientEvent::Press, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Button2, teki_id2);
	    objs->ChangePlayerState(ClientEvent::Button2 | ClientEvent::Press, teki_id3);
	    objs->ChangePlayerState(ClientEvent::Button2, teki_id3);
	  }
	}
      }

      // １フレーム分更新と描画
      objs->UpdateAll();
      objs->HitCheck();
      const SendObjAry* soa(objs->GetSendData());
      // id0のプレイヤのパラメータを得る
      SendParam param(objs->GetSendParam(0));
      if (battle) {
	param.view_pos = SendPos(320, 240);
      }
      
      // 描画
      if (skip) {
	continue;
      }
      if (no_draw == false) {
	dmanager->DrawAll(soa, &param);
      }
      // 当たり判定描画
      if (draw_detect) {
	Pos scr_pos = dmanager->GetScreenPos(toPos(param.view_pos));

	vector<DrawableDetect> detects = objs->GetDrawableDetectAry();
	for (vector<DrawableDetect>::iterator i = detects.begin();
	     i != detects.end(); ++i) {
	  i->pos -= scr_pos;
	  screen.Draw(i->rect, i->pos);
	}
      }
      if (no_draw == false) {
	screen.Update();
      }

      // FPSを実現するために待つ
      double t = timer.Wait();
      if (news_fps) {
	if (t) {
	  cout << "FPS= " << t << endl;
	}
      }
    }
  }
  catch ( Zu3gaError& e ) {
    cerr << "err in mainloop: " << e.JaMsg() << endl;
    return 1;
  }
  catch ( exception& e ) {
    cerr << "メインループ中にエラー: " << e.what() << endl;
    return 1;
  }

 MainLoopEnd:
  cout << "メインループ終了" << endl;

  return 0;
}

// 対戦用イベント取得関数
bool GetClientEvent2(unsigned char& clie_eve,
		     int& id)
{
  SDL_Event sdl_eve;

  while (SDL_PollEvent(&sdl_eve)) {
    switch (sdl_eve.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:		// キーボードのイベントの場合
      if (SDLKeyEventToClientEvent2(sdl_eve, clie_eve, id)) {
	return true;
      }
      break;

    default:
      break;
    }

    switch (sdl_eve.key.keysym.sym) {
    case SDLK_ESCAPE:
      clie_eve = 201;
      return true;
    default:
      break;
    }
    break;
  }

  return false;
}

// 対戦用SDLイベント処理関数
bool SDLKeyEventToClientEvent2(const SDL_Event& sdl_eve, 
			       unsigned char& clie_eve,
			       int& id)
{
  static vector<ButtonHaichi> k = ReadButtonHaichi();

  int sym = sdl_eve.key.keysym.sym;
  // プレイヤー１の入力
  if (sym == k[0].up) {
    clie_eve = ClientEvent::Up;
    id = 0;
  }
  else if (sym == k[0].down) {
    clie_eve = ClientEvent::Down;
    id = 0;
  }
  else if (sym == k[0].left) {
    clie_eve = ClientEvent::Left;
    id = 0;
  }
  else if (sym == k[0].right) {
    clie_eve = ClientEvent::Right;
    id = 0;
  }
  else if (sym == k[0].w1) {
    clie_eve = ClientEvent::Button1;
    id = 0;
  }
  else if (sym == k[0].w2) {
    clie_eve = ClientEvent::Button2;
    id = 0;
  }
  // プレイヤー２の入力
  else if (sym == k[1].up) {
    clie_eve = ClientEvent::Up;
    id = 1;
  }
  else if (sym == k[1].down) {
    clie_eve = ClientEvent::Down;
    id = 1;
  }
  else if (sym == k[1].left) {
    clie_eve = ClientEvent::Left;
    id = 1;
  }
  else if (sym == k[1].right) {
    clie_eve = ClientEvent::Right;
    id = 1;
  }
  else if (sym == k[1].w1) {
    clie_eve = ClientEvent::Button1;
    id = 1;
  }
  else if (sym == k[1].w2) {
    clie_eve = ClientEvent::Button2;
    id = 1;
  }
  // プレイヤーの入力とは別物
  else {
    return false;
  }

  if (sdl_eve.type == SDL_KEYDOWN) {
    clie_eve |= ClientEvent::Press;
  }
  return true;
}

int ButtonHaichiHenkou()
{
  boost::shared_ptr<graphic::VideoInit> video_init;
  try {
    video_init = boost::shared_ptr<graphic::VideoInit>(new graphic::VideoInit);
  } catch (exception& e) {
    cout << "ビデオの初期化エラー: " << e.what() << endl;
    exit(1);
  }

  ofstream fout("key_haichi");

  cout << "プレイヤー１" << endl;
  cout << "up:      " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "down:    " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "left:    " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "right:   " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "weapon1: " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "weapon2: " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "プレイヤー２" << endl;
  cout << "up:      " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "down:    " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "left:    " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "right:   " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "weapon1: " << flush;
  fout << GetSDLKeySym() << " ";
  cout << "weapon2: " << flush;
  fout << GetSDLKeySym() << " ";
  fout.close();
  return 0;
}

vector<ButtonHaichi> ReadButtonHaichi()
{
  vector<ButtonHaichi> haichi;
  ButtonHaichi p1, p2;
  ifstream fin("key_haichi");

  fin >> p1.up;
  fin >> p1.down;
  fin >> p1.left;
  fin >> p1.right;
  fin >> p1.w1;
  fin >> p1.w2;
  fin >> p2.up;
  fin >> p2.down;
  fin >> p2.left;
  fin >> p2.right;
  fin >> p2.w1;
  fin >> p2.w2;
  haichi.push_back(p1);
  haichi.push_back(p2);

  return haichi;
}

int GetSDLKeySym()
{
  SDL_Event eve;  

  while (true) {
    SDL_PollEvent(&eve);

    switch (eve.type) {
    case SDL_KEYDOWN:
      {
	int in = eve.key.keysym.sym;
	cout << in << endl;
	return in;
      }
    default:
      break;
    }
  }
  return 0;
}

// 画像位置調整モードで起動したとき呼ばれる
int GraphicPosAdjustMode()
{
  boost::shared_ptr<graphic::VideoInit> video_init;
  boost::shared_ptr<Objects> objs;
  boost::shared_ptr<DrawingManager> dmanager;

  SDLTimer timer;
  timer.SetFPS(FPS);

  try {
    video_init = boost::shared_ptr<graphic::VideoInit>(new graphic::VideoInit);
    objs = boost::shared_ptr<Objects>(new Objects(StageType::Forest));
    DMInitData dmi;
    dmi.stg_type = StageType::Forest;
    dmi.tags[0] = StatusTag(StatusTagID::ID0, "name0", Team::I);
    dmi.tags[1] = StatusTag(StatusTagID::ID1, "name1", Team::Ro);
    dmi.tags[2].id = StatusTagID::NoUse;
    dmi.tags[3].id = StatusTagID::NoUse;
    dmanager = boost::shared_ptr<DrawingManager>(new DrawingManager(dmi));
  } catch (exception& e) {
    cout << "グラフィックの初期化エラー: " << e.what() << endl;
    exit(1);
  }

  try {
    unsigned char ad_eve;
    SmartSurface screen = GetVideoSurface();
    Pos diff(0, 0);
    bool use_diff = false;
    short int state = 0;
    int set_d = 0;
    while (true) {
      timer.Start();
      //----------------------------------------
      // キー情報を取得してプレイヤーに設定
      //----------------------------------------

      while (GetAdjustEvent(ad_eve)) {
	switch (ad_eve) {
	case 0:
	  // 上入力
	  state = 0;
	  if (state < 0) {
	    state = 0;
	  }
	  use_diff = false;
	  break;
	case 1:
	  // 下入力
	  state = YamahaState::Laser1_1 + set_d;
	  if (state >= YamahaState::End) {
	    state = YamahaState::End - 1;
	  }
	  use_diff = true;
	  break;
	case 2:
	  // 左入力
	  state--;
	  //set_d--;
	  if (state < 0) {
	    state = 0;
	  }
	  break;
	case 3:
	  // 右入力
	  state++;
	  //set_d++;
	  break;
	case 99:
	  // ループ終了
	  cout << "プログラムを終了します..." << endl;
	  goto MainLoopEnd;
	case 4:
	  // 上にずらす
	  diff.y -= 1;
	  break;
	case 5:
	  // 下にずらす
	  diff.y += 1;
	  break;
	case 6:
	  // 左にずらす
	  diff.x -= 1;
	  break;
	case 7:
	  // 右にずらす
	  diff.x += 1;
	  break;
	}
      }

      SendObject sobj;
      sobj.type = SendObjType::YamahaLaserEffect;
      sobj.state = state;
      sobj.pos = SendPos(100, 100);
      if (use_diff) {
	sobj.pos.x += diff.x;
	sobj.pos.y += diff.y;
	cout << "diff(" << diff.x << ", " << diff.y << ")" << endl;
      }
      SendObjAry soa;
      soa.push_back(sobj);

      // 描画
      dmanager->DrawSendObjAry(&soa);
      screen.Update();

      // FPSを実現するために待つ
      double t = timer.Wait();
      if (false) {
	cout << "FPS= " << t << endl;
      }
    }
  }
  catch ( exception& e ) {
    cerr << "メインループ中にエラー: " << e.what() << endl;
    return 1;
  }

MainLoopEnd:
  cout << "メインループ終了" << endl;
  return 0;
}

bool SDLKeyEventToAdjustEvent(const SDL_Event& sdl_eve,
			      unsigned char& adjust_eve)
{
  switch (sdl_eve.key.keysym.sym) {
  case SDLK_UP:
    adjust_eve = 0;
    break;
  case SDLK_DOWN:
    adjust_eve = 1;
    break;
  case SDLK_LEFT:
    adjust_eve = 2;
    break;
  case SDLK_RIGHT:
    adjust_eve = 3;
    break;
  case SDLK_ESCAPE:
    adjust_eve = 99;
    break;
  case SDLK_k:
    adjust_eve = 4;
    break;
  case SDLK_j:
    adjust_eve = 5;
    break;
  case SDLK_h:
    adjust_eve = 6;
    break;
  case SDLK_l:
    adjust_eve = 7;
    break;
  default:
    // クライアントイベントではなかった
    return false;
  }
  return true;
}

bool GetAdjustEvent(unsigned char& clie_eve)
{
  SDL_Event sdl_eve;

  while (SDL_PollEvent(&sdl_eve)) {
    switch (sdl_eve.type) {
    case SDL_KEYDOWN: // キーが押された場合
      if (SDLKeyEventToAdjustEvent(sdl_eve, clie_eve)) {
	return true;
      }
      break;
    default:
      break;
    }
  }
  return false;
}
