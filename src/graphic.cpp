//============================================================
// namespace graphicの実装
// last modified <2004/02/09 01:51:42 2004 JST>
//============================================================
#include <stdexcept>
#include <map>
#include <vector>
#include <fstream>
#include <SDL_image.h>
#include "graphic.hpp"
#include "verbose.hpp"
#include "comtypes.hpp"
#include "utility.hpp"

using namespace std;
using namespace zu3ga;
using namespace graphic;
using namespace edw::sdl;

namespace {
  typedef map<Type::t,sp_Graphic> ImgMap;
  typedef ImgMap::const_iterator IMConstIter;
  ImgMap imgmap_in;
  sp_Font tag100, tag80, tag60, login, errfont;

  void FontInitialize();
  void FontFinalize();
  void ImgDataInitialize();
  void ImgDataFinalize();

  string tag_font_name( "kochi-gothic.ttf" );
  string image_data_path( "images/" );

  inline sp_Graphic
  mkSG( const SmartSurface& suf, const SDL_Rect& rect, const Pos& pos )
  {
    return SurfaceGraphic::Create( suf, rect, pos );
  }

  inline sp_Graphic
  mkSG( const string& name, const SDL_Rect& rect, const Pos& pos )
  {
    SmartSurface temp( load( name ) );
    temp.SwapFormatAlpha();
    return mkSG( temp, rect, pos );
  }

  inline sp_Graphic
  mkSG( const string& name )
  {
    SmartSurface temp( load( name ) );
    temp.SwapFormatAlpha();
    return temp.GetGraphic();
  }

  inline sp_Graphic
  mkSGna( const string& name )
  {
    SmartSurface temp( load( name ) );
    temp.SwapFormat();
    return temp.GetGraphic();
  }
}

SmartSurface graphic::load( const string& name )
{
  verbose::puts(3,name);
  SDL_Surface* suf =  IMG_Load(name.c_str());
  if ( !suf ) {
    throw Error( string("No such file: ")+name );
  }
  return SmartSurface( suf );
}

sp_Font graphic::GetFont( FontType::t type )
{
  switch (type) {
  case FontType::Login:
    return login;
    break;
  case FontType::Tag100:
    return tag100;
    break;
  case FontType::Tag80:
    return tag80;
    break;
  case FontType::Tag60:
    return tag60;
    break;
  case FontType::Error:
    return errfont;
    break;
  }
  return sp_Font();
}

//============================================================
// graphic::画像idに対応する画像を得る
//============================================================
sp_Graphic graphic::GetGraphic( Type::t type )
{
  if ( imgmap_in.empty() )
    throw Zu3gaError( "GraphicManager::GetGraphic: video not intialized." );

  IMConstIter pos = imgmap_in.find(type);
  if ( pos == imgmap_in.end() )
    throw Zu3gaError( "GraphicManager::GetGraphic: おかしい型:" + IntToStr(type));
  return pos->second;
}

void graphic::SetImagesDir( const string& dir )
{
  image_data_path = dir;
}

const unsigned int graphic::WindowWidth = 640;
const unsigned int graphic::WindowHeight = 480;
unsigned int graphic::ScreenWidth;
unsigned int graphic::ScreenHeight;

//======================================================================
// ビデオ初期化
void
graphic::VideoInitialize()
{
  // SDLライブラリを初期化
  if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
    throw Error( "SDL が初期化できませんでした: ");
  }

  // ビデオモード設定
  // Uint32 video_mode = SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF;
  Uint32 video_mode = SDL_HWSURFACE | SDL_DOUBLEBUF;
  if ( SDL_SetVideoMode( WindowWidth, WindowHeight, 16, video_mode ) == 0 ) {
    throw Error( "640x480x16 のビデオモードに設定できませんでした: " );
  }

  SDL_ShowCursor(SDL_DISABLE);

  SmartSurface scr = GetVideoSurface();
  scr.Clear(Color(0xff,0xff,0xff));
  sp_Graphic gl = mkSG(image_data_path + "init/load.jpg");
  Uint32 w, h;
  gl->GetWH(w,h);
  scr.Draw(gl, Pos((WindowWidth-w)/2,(WindowHeight-h)/2));
  scr.Update();

  ImgDataInitialize();
  FontInitialize();
}

//======================================================================
// ビデオ終了
void
graphic::VideoFinalize()
{
  FontFinalize();
  ImgDataFinalize();
  SDL_Quit();
}

namespace {
  void FontInitialize()
  {
    Font::Init();
    tag100 = Font::Create(image_data_path+tag_font_name,25);
    tag80 = Font::Create(image_data_path+tag_font_name, 20);
    tag60 = Font::Create(image_data_path+tag_font_name, 16);
    login = Font::Create(image_data_path+tag_font_name, 22);
    errfont = Font::Create(image_data_path+tag_font_name, 25);
  }

  void FontFinalize()
  {
    tag100.reset();
    tag80.reset();
    tag60.reset();
    login.reset();
    errfont.reset();
    Font::Quit();
  }

  //============================================================
  //画像データ終了
  //============================================================
  void ImgDataFinalize()
  {
    imgmap_in.clear();
  }

  //============================================================
  //画像データ初期化
  //============================================================
  void ImgDataInitialize()
  {
    using namespace graphic::Type;

    string arrow_path   = image_data_path + "arrow/";
    string battle_path  = image_data_path + "battle/";
    string bg_path      = image_data_path + "background/";
    string button_path  = image_data_path + "button/";
    string effect_path  = image_data_path + "effect/";
    string init_path    = image_data_path + "init/";
    string lobby_path   = image_data_path + "lobby/";
    string lockon_path  = image_data_path + "lockon/";
    string map_path     = image_data_path + "map/";
    string planex_path  = image_data_path + "planex/";
    string result_path  = image_data_path + "result/";
    string tag_path     = image_data_path + "tag/";
    string window_path  = image_data_path + "window/";
    string yamaha_path  = image_data_path + "yamaha/";
    string ntt_path     = image_data_path + "ntt/";

    { // Planexの読み込み
      string team_name = "_i";
      {
	int w = 60;
	int h = 70;
	const Pos pos(w / 2, h / 3 * 2);
	SmartSurface temp(load(planex_path + "small" + team_name + ".png"));
	temp.SwapFormatAlpha();
	// 立ち
	imgmap_in[CharaAStand1I] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAStand2I] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAStand3I] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAStand4I] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAStand6I] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAStand7I] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAStand8I] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAStand9I] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// 歩き
	imgmap_in[CharaAWalk1I] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAWalk2I] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAWalk3I] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAWalk4I] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAWalk6I] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAWalk7I] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAWalk8I] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAWalk9I] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// ダッシュ
	SmartSurface temp2(load(planex_path + "small_dash" + team_name + ".png"));
	temp2.SwapFormatAlpha();
	imgmap_in[CharaADash1I] = mkSG(temp2, mkRect(w, h, w, h), pos);
	imgmap_in[CharaADash2I] = mkSG(temp2, mkRect(0, h, w, h), pos);
	imgmap_in[CharaADash3I] = mkSG(temp2, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaADash4I] = mkSG(temp2, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaADash6I] = mkSG(temp2, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaADash7I] = mkSG(temp2, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaADash8I] = mkSG(temp2, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaADash9I] = mkSG(temp2, mkRect(w, 0, w, h), pos);
	// 武器
	w = h = 70;
	SmartSurface ss_shot(load(planex_path + "shot" + team_name + ".png"));
	ss_shot.SwapFormatAlpha();
	imgmap_in[CharaAWeapon1I] = mkSG(ss_shot, mkRect(w, h, w, h), Pos(42, 47));
	imgmap_in[CharaAWeapon2I] = mkSG(ss_shot, mkRect(0, h, w, h), Pos(30, 46));
	imgmap_in[CharaAWeapon3I] = mkSG(ss_shot, mkRect(w * 3, 0, w, h), Pos(37, 48));
	imgmap_in[CharaAWeapon4I] = mkSG(ss_shot, mkRect(w * 2, h, w, h), Pos(45, 47));
	imgmap_in[CharaAWeapon6I] = mkSG(ss_shot, mkRect(w * 2, 0, w, h), Pos(21, 50));
	imgmap_in[CharaAWeapon7I] = mkSG(ss_shot, mkRect(w * 3, h, w, h), Pos(38, 53));
	imgmap_in[CharaAWeapon8I] = mkSG(ss_shot, mkRect(0, 0, w, h), Pos(33, 57));
	imgmap_in[CharaAWeapon9I] = mkSG(ss_shot, mkRect(w, 0, w, h), Pos(33, 53));
	// ダウン
	SmartSurface temp3(load(planex_path + "down" + team_name + ".png"));
	temp3.SwapFormatAlpha();
	imgmap_in[CharaADown1I] = mkSG(temp3, mkRect(0, 0, 52, 47), Pos(25, 25));
	imgmap_in[CharaADown2I] = mkSG(temp3, mkRect(52, 0, 104, 47), Pos(25, 25));
	// 近距離攻撃
	int w2 = 80;
	SmartSurface kin1(load(planex_path + "kin_2" + team_name + ".png"));
	kin1.SwapFormatAlpha();
	imgmap_in[CharaAKin1_1I] = mkSG(kin1, mkRect(0, 0, w, kin1->h), Pos(37, 48));
	imgmap_in[CharaAKin1_2I] = mkSG(kin1, mkRect(w, 0, w, kin1->h), Pos(38, 46));
	imgmap_in[CharaAKin1_3I] = mkSG(kin1, mkRect(w*2, 0, w, kin1->h), Pos(34, 48));
	imgmap_in[CharaAKin1_4I] = mkSG(kin1, mkRect(w*3, 0, w, kin1->h), Pos(43, 47));
	imgmap_in[CharaAKin1_5I] = mkSG(kin1, mkRect(w*4, 0, w, kin1->h), Pos(45, 47));
	imgmap_in[CharaAKin1_6I] = mkSG(kin1, mkRect(w*5, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_7I] = mkSG(kin1, mkRect(w*6, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_8I] = mkSG(kin1, mkRect(w*7, 0, w, kin1->h), Pos(37, 51));
	SmartSurface kin2(load(planex_path + "kin_1" + team_name + ".png"));
	kin2.SwapFormatAlpha();
	imgmap_in[CharaAKin2_1I] = mkSG(kin2, mkRect(0, 0, w, kin2->h), Pos(33, 49));
	imgmap_in[CharaAKin2_2I] = mkSG(kin2, mkRect(w, 0, w, kin2->h), Pos(36, 47));
	imgmap_in[CharaAKin2_3I] = mkSG(kin2, mkRect(w*2, 0, w, kin2->h), Pos(35, 50));
	imgmap_in[CharaAKin2_4I] = mkSG(kin2, mkRect(w*3, 0, w, kin2->h), Pos(40, 49));
	imgmap_in[CharaAKin2_5I] = mkSG(kin2, mkRect(w*4, 0, w, kin2->h), Pos(42, 49));
	imgmap_in[CharaAKin2_6I] = mkSG(kin2, mkRect(w*5, 0, w, kin2->h), Pos(38, 48));
	imgmap_in[CharaAKin2_7I] = mkSG(kin2, mkRect(w*6, 0, w, kin2->h), Pos(39, 47));
	imgmap_in[CharaAKin2_8I] = mkSG(kin2, mkRect(w*7, 0, w, kin2->h), Pos(36, 48));
	SmartSurface kin3(load(planex_path + "kin_6" + team_name + ".png"));
	kin3.SwapFormatAlpha();
	imgmap_in[CharaAKin3_1I] = mkSG(kin3, mkRect(0, 0, w, kin3->h), Pos(32, 44));
	imgmap_in[CharaAKin3_2I] = mkSG(kin3, mkRect(w, 0, w, kin3->h), Pos(34, 44));
	imgmap_in[CharaAKin3_3I] = mkSG(kin3, mkRect(w*2, 0, w, kin3->h), Pos(36, 47));
	imgmap_in[CharaAKin3_4I] = mkSG(kin3, mkRect(w*3, 0, w, kin3->h), Pos(33, 46));
	imgmap_in[CharaAKin3_5I] = mkSG(kin3, mkRect(w*4, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_6I] = mkSG(kin3, mkRect(w*5, 0, w, kin3->h), Pos(30, 46));
	imgmap_in[CharaAKin3_7I] = mkSG(kin3, mkRect(w*6, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_8I] = mkSG(kin3, mkRect(w*7, 0, w, kin3->h), Pos(37, 47));
	SmartSurface kin4(load(planex_path + "kin_3" + team_name + ".png"));
	kin4.SwapFormatAlpha();
	imgmap_in[CharaAKin4_1I] = mkSG(kin4, mkRect(0, 0, w, kin4->h), Pos(30, 41));
	imgmap_in[CharaAKin4_2I] = mkSG(kin4, mkRect(w, 0, w, kin4->h), Pos(37, 42));
	imgmap_in[CharaAKin4_3I] = mkSG(kin4, mkRect(w*2, 0, w, kin4->h), Pos(39, 39));
	imgmap_in[CharaAKin4_4I] = mkSG(kin4, mkRect(w*3, 0, w2, kin4->h), Pos(49, 44));
	imgmap_in[CharaAKin4_5I] = mkSG(kin4, mkRect(w*3+w2, 0, w2, kin4->h), Pos(50, 41));
	imgmap_in[CharaAKin4_6I] = mkSG(kin4, mkRect(w*3+w2*2, 0, w, kin4->h), Pos(49, 42));
	imgmap_in[CharaAKin4_7I] = mkSG(kin4, mkRect(w*4+w2*2, 0, w, kin4->h), Pos(50, 42));
	imgmap_in[CharaAKin4_8I] = mkSG(kin4, mkRect(w*5+w2*2, 0, w, kin4->h), Pos(37, 41));
	SmartSurface kin6(load(planex_path + "kin_7" + team_name + ".png"));
	kin6.SwapFormatAlpha();
	imgmap_in[CharaAKin6_1I] = mkSG(kin6, mkRect(0, 0, w, kin6->h), Pos(28, 41));
	imgmap_in[CharaAKin6_2I] = mkSG(kin6, mkRect(w, 0, w, kin6->h), Pos(28, 42));
	imgmap_in[CharaAKin6_3I] = mkSG(kin6, mkRect(w*2, 0, w, kin6->h), Pos(33, 41));
	imgmap_in[CharaAKin6_4I] = mkSG(kin6, mkRect(w*3, 0, w2, kin6->h), Pos(32, 44));
	imgmap_in[CharaAKin6_5I] = mkSG(kin6, mkRect(w*3+w2, 0, w2, kin6->h), Pos(31, 41));
	imgmap_in[CharaAKin6_6I] = mkSG(kin6, mkRect(w*3+w2*2, 0, w, kin6->h), Pos(20, 42));
	imgmap_in[CharaAKin6_7I] = mkSG(kin6, mkRect(w*4+w2*2, 0, w, kin6->h), Pos(20, 41));
	imgmap_in[CharaAKin6_8I] = mkSG(kin6, mkRect(w*5+w2*2, 0, w, kin6->h), Pos(31, 41));
	SmartSurface kin7(load(planex_path + "kin_4" + team_name + ".png"));
	kin7.SwapFormatAlpha();
	imgmap_in[CharaAKin7_1I] = mkSG(kin7, mkRect(0, 0, w, kin7->h), Pos(32, 54));
	imgmap_in[CharaAKin7_2I] = mkSG(kin7, mkRect(w, 0, w, kin7->h), Pos(35, 56));
	imgmap_in[CharaAKin7_3I] = mkSG(kin7, mkRect(w*2, 0, w, kin7->h), Pos(34, 54));
	imgmap_in[CharaAKin7_4I] = mkSG(kin7, mkRect(w*3, 0, w, kin7->h), Pos(36, 55));
	imgmap_in[CharaAKin7_5I] = mkSG(kin7, mkRect(w*4, 0, w, kin7->h), Pos(37, 55));
	imgmap_in[CharaAKin7_6I] = mkSG(kin7, mkRect(w*5, 0, w, kin7->h), Pos(44, 55));
	imgmap_in[CharaAKin7_7I] = mkSG(kin7, mkRect(w*6, 0, w, kin7->h), Pos(45, 55));
	imgmap_in[CharaAKin7_8I] = mkSG(kin7, mkRect(w*7, 0, w, kin7->h), Pos(36, 54));
	SmartSurface kin8(load(planex_path + "kin_5" + team_name + ".png"));
	kin8.SwapFormatAlpha();
	imgmap_in[CharaAKin8_1I] = mkSG(kin8, mkRect(0, 0, w, kin8->h), Pos(32, 55));
	imgmap_in[CharaAKin8_2I] = mkSG(kin8, mkRect(w, 0, w, kin8->h), Pos(38, 51));
	imgmap_in[CharaAKin8_3I] = mkSG(kin8, mkRect(w*2, 0, w, kin8->h), Pos(39, 52));
	imgmap_in[CharaAKin8_4I] = mkSG(kin8, mkRect(w*3, 0, w, kin8->h), Pos(36, 54));
	imgmap_in[CharaAKin8_5I] = mkSG(kin8, mkRect(w*4, 0, w, kin8->h), Pos(38, 57));
	imgmap_in[CharaAKin8_6I] = mkSG(kin8, mkRect(w*5, 0, w, kin8->h), Pos(42, 58));
	imgmap_in[CharaAKin8_7I] = mkSG(kin8, mkRect(w*6, 0, w, kin8->h), Pos(38, 58));
	imgmap_in[CharaAKin8_8I] = mkSG(kin8, mkRect(w*7, 0, w, kin8->h), Pos(36, 53));
	SmartSurface kin9(load(planex_path + "kin_8" + team_name + ".png"));
	kin9.SwapFormatAlpha();
	imgmap_in[CharaAKin9_1I] = mkSG(kin9, mkRect(0, 0, w, kin9->h), Pos(32, 57));
	imgmap_in[CharaAKin9_2I] = mkSG(kin9, mkRect(w, 0, w, kin9->h), Pos(32, 51));
	imgmap_in[CharaAKin9_3I] = mkSG(kin9, mkRect(w*2, 0, w, kin9->h), Pos(31, 53));
	imgmap_in[CharaAKin9_4I] = mkSG(kin9, mkRect(w*3, 0, w, kin9->h), Pos(40, 52));
	imgmap_in[CharaAKin9_5I] = mkSG(kin9, mkRect(w*4, 0, w, kin9->h), Pos(36, 56));
	imgmap_in[CharaAKin9_6I] = mkSG(kin9, mkRect(w*5, 0, w, kin9->h), Pos(30, 57));
	imgmap_in[CharaAKin9_7I] = mkSG(kin9, mkRect(w*6, 0, w, kin9->h), Pos(31, 56));
	imgmap_in[CharaAKin9_8I] = mkSG(kin9, mkRect(w*7, 0, w, kin9->h), Pos(34, 55));
      }

      {
	team_name = "_ro";
	int w = 60;
	int h = 70;
	const Pos pos(w / 2, h / 3 * 2);
	SmartSurface temp(load(planex_path + "small" + team_name + ".png"));
	temp.SwapFormatAlpha();
	imgmap_in[CharaAStand1Ro] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAStand2Ro] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAStand3Ro] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAStand4Ro] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAStand6Ro] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAStand7Ro] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAStand8Ro] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAStand9Ro] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// 歩き
	imgmap_in[CharaAWalk1Ro] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAWalk2Ro] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAWalk3Ro] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAWalk4Ro] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAWalk6Ro] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAWalk7Ro] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAWalk8Ro] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAWalk9Ro] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// ダッシュ
	SmartSurface temp2(load(planex_path + "small_dash" + team_name + ".png"));
	temp2.SwapFormatAlpha();
	imgmap_in[CharaADash1Ro] = mkSG(temp2, mkRect(w, h, w, h), pos);
	imgmap_in[CharaADash2Ro] = mkSG(temp2, mkRect(0, h, w, h), pos);
	imgmap_in[CharaADash3Ro] = mkSG(temp2, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaADash4Ro] = mkSG(temp2, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaADash6Ro] = mkSG(temp2, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaADash7Ro] = mkSG(temp2, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaADash8Ro] = mkSG(temp2, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaADash9Ro] = mkSG(temp2, mkRect(w, 0, w, h), pos);
	// 武器
	w = h = 70;
	SmartSurface ss_shot(load(planex_path + "shot" + team_name + ".png"));
	ss_shot.SwapFormatAlpha();
	imgmap_in[CharaAWeapon1Ro] = mkSG(ss_shot, mkRect(w, h, w, h), Pos(42, 47));
	imgmap_in[CharaAWeapon2Ro] = mkSG(ss_shot, mkRect(0, h, w, h), Pos(30, 46));
	imgmap_in[CharaAWeapon3Ro] = mkSG(ss_shot, mkRect(w * 3, 0, w, h), Pos(37, 48));
	imgmap_in[CharaAWeapon4Ro] = mkSG(ss_shot, mkRect(w * 2, h, w, h), Pos(45, 47));
	imgmap_in[CharaAWeapon6Ro] = mkSG(ss_shot, mkRect(w * 2, 0, w, h), Pos(21, 50));
	imgmap_in[CharaAWeapon7Ro] = mkSG(ss_shot, mkRect(w * 3, h, w, h), Pos(38, 53));
	imgmap_in[CharaAWeapon8Ro] = mkSG(ss_shot, mkRect(0, 0, w, h), Pos(33, 57));
	imgmap_in[CharaAWeapon9Ro] = mkSG(ss_shot, mkRect(w, 0, w, h), Pos(33, 53));
	// ダウン
	SmartSurface temp3(load(planex_path + "down" + team_name + ".png"));
	temp3.SwapFormatAlpha();
	imgmap_in[CharaADown1Ro] = mkSG(temp3, mkRect(0, 0, 52, 47), Pos(25, 25));
	imgmap_in[CharaADown2Ro] = mkSG(temp3, mkRect(52, 0, 104, 47), Pos(25, 25));
	// 近距離攻撃
	int w2 = 80;
	SmartSurface kin1(load(planex_path + "kin_2" + team_name + ".png"));
	kin1.SwapFormatAlpha();
	imgmap_in[CharaAKin1_1Ro] = mkSG(kin1, mkRect(0, 0, w, kin1->h), Pos(37, 48));
	imgmap_in[CharaAKin1_2Ro] = mkSG(kin1, mkRect(w, 0, w, kin1->h), Pos(38, 46));
	imgmap_in[CharaAKin1_3Ro] = mkSG(kin1, mkRect(w*2, 0, w, kin1->h), Pos(34, 48));
	imgmap_in[CharaAKin1_4Ro] = mkSG(kin1, mkRect(w*3, 0, w, kin1->h), Pos(43, 47));
	imgmap_in[CharaAKin1_5Ro] = mkSG(kin1, mkRect(w*4, 0, w, kin1->h), Pos(45, 47));
	imgmap_in[CharaAKin1_6Ro] = mkSG(kin1, mkRect(w*5, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_7Ro] = mkSG(kin1, mkRect(w*6, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_8Ro] = mkSG(kin1, mkRect(w*7, 0, w, kin1->h), Pos(37, 51));
	SmartSurface kin2(load(planex_path + "kin_1" + team_name + ".png"));
	kin2.SwapFormatAlpha();
	imgmap_in[CharaAKin2_1Ro] = mkSG(kin2, mkRect(0, 0, w, kin2->h), Pos(33, 49));
	imgmap_in[CharaAKin2_2Ro] = mkSG(kin2, mkRect(w, 0, w, kin2->h), Pos(36, 47));
	imgmap_in[CharaAKin2_3Ro] = mkSG(kin2, mkRect(w*2, 0, w, kin2->h), Pos(35, 50));
	imgmap_in[CharaAKin2_4Ro] = mkSG(kin2, mkRect(w*3, 0, w, kin2->h), Pos(40, 49));
	imgmap_in[CharaAKin2_5Ro] = mkSG(kin2, mkRect(w*4, 0, w, kin2->h), Pos(42, 49));
	imgmap_in[CharaAKin2_6Ro] = mkSG(kin2, mkRect(w*5, 0, w, kin2->h), Pos(38, 48));
	imgmap_in[CharaAKin2_7Ro] = mkSG(kin2, mkRect(w*6, 0, w, kin2->h), Pos(39, 47));
	imgmap_in[CharaAKin2_8Ro] = mkSG(kin2, mkRect(w*7, 0, w, kin2->h), Pos(36, 48));
	SmartSurface kin3(load(planex_path + "kin_6" + team_name + ".png"));
	kin3.SwapFormatAlpha();
	imgmap_in[CharaAKin3_1Ro] = mkSG(kin3, mkRect(0, 0, w, kin3->h), Pos(32, 44));
	imgmap_in[CharaAKin3_2Ro] = mkSG(kin3, mkRect(w, 0, w, kin3->h), Pos(34, 44));
	imgmap_in[CharaAKin3_3Ro] = mkSG(kin3, mkRect(w*2, 0, w, kin3->h), Pos(36, 47));
	imgmap_in[CharaAKin3_4Ro] = mkSG(kin3, mkRect(w*3, 0, w, kin3->h), Pos(33, 46));
	imgmap_in[CharaAKin3_5Ro] = mkSG(kin3, mkRect(w*4, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_6Ro] = mkSG(kin3, mkRect(w*5, 0, w, kin3->h), Pos(30, 46));
	imgmap_in[CharaAKin3_7Ro] = mkSG(kin3, mkRect(w*6, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_8Ro] = mkSG(kin3, mkRect(w*7, 0, w, kin3->h), Pos(37, 47));
	SmartSurface kin4(load(planex_path + "kin_3" + team_name + ".png"));
	kin4.SwapFormatAlpha();
	imgmap_in[CharaAKin4_1Ro] = mkSG(kin4, mkRect(0, 0, w, kin4->h), Pos(30, 41));
	imgmap_in[CharaAKin4_2Ro] = mkSG(kin4, mkRect(w, 0, w, kin4->h), Pos(37, 42));
	imgmap_in[CharaAKin4_3Ro] = mkSG(kin4, mkRect(w*2, 0, w, kin4->h), Pos(39, 39));
	imgmap_in[CharaAKin4_4Ro] = mkSG(kin4, mkRect(w*3, 0, w2, kin4->h), Pos(49, 44));
	imgmap_in[CharaAKin4_5Ro] = mkSG(kin4, mkRect(w*3+w2, 0, w2, kin4->h), Pos(50, 41));
	imgmap_in[CharaAKin4_6Ro] = mkSG(kin4, mkRect(w*3+w2*2, 0, w, kin4->h), Pos(49, 42));
	imgmap_in[CharaAKin4_7Ro] = mkSG(kin4, mkRect(w*4+w2*2, 0, w, kin4->h), Pos(50, 42));
	imgmap_in[CharaAKin4_8Ro] = mkSG(kin4, mkRect(w*5+w2*2, 0, w, kin4->h), Pos(37, 41));
	SmartSurface kin6(load(planex_path + "kin_7" + team_name + ".png"));
	kin6.SwapFormatAlpha();
	imgmap_in[CharaAKin6_1Ro] = mkSG(kin6, mkRect(0, 0, w, kin6->h), Pos(28, 41));
	imgmap_in[CharaAKin6_2Ro] = mkSG(kin6, mkRect(w, 0, w, kin6->h), Pos(28, 42));
	imgmap_in[CharaAKin6_3Ro] = mkSG(kin6, mkRect(w*2, 0, w, kin6->h), Pos(33, 41));
	imgmap_in[CharaAKin6_4Ro] = mkSG(kin6, mkRect(w*3, 0, w2, kin6->h), Pos(32, 44));
	imgmap_in[CharaAKin6_5Ro] = mkSG(kin6, mkRect(w*3+w2, 0, w2, kin6->h), Pos(31, 41));
	imgmap_in[CharaAKin6_6Ro] = mkSG(kin6, mkRect(w*3+w2*2, 0, w, kin6->h), Pos(20, 42));
	imgmap_in[CharaAKin6_7Ro] = mkSG(kin6, mkRect(w*4+w2*2, 0, w, kin6->h), Pos(20, 41));
	imgmap_in[CharaAKin6_8Ro] = mkSG(kin6, mkRect(w*5+w2*2, 0, w, kin6->h), Pos(31, 41));
	SmartSurface kin7(load(planex_path + "kin_4" + team_name + ".png"));
	kin7.SwapFormatAlpha();
	imgmap_in[CharaAKin7_1Ro] = mkSG(kin7, mkRect(0, 0, w, kin7->h), Pos(32, 54));
	imgmap_in[CharaAKin7_2Ro] = mkSG(kin7, mkRect(w, 0, w, kin7->h), Pos(35, 56));
	imgmap_in[CharaAKin7_3Ro] = mkSG(kin7, mkRect(w*2, 0, w, kin7->h), Pos(34, 54));
	imgmap_in[CharaAKin7_4Ro] = mkSG(kin7, mkRect(w*3, 0, w, kin7->h), Pos(36, 55));
	imgmap_in[CharaAKin7_5Ro] = mkSG(kin7, mkRect(w*4, 0, w, kin7->h), Pos(37, 55));
	imgmap_in[CharaAKin7_6Ro] = mkSG(kin7, mkRect(w*5, 0, w, kin7->h), Pos(44, 55));
	imgmap_in[CharaAKin7_7Ro] = mkSG(kin7, mkRect(w*6, 0, w, kin7->h), Pos(45, 55));
	imgmap_in[CharaAKin7_8Ro] = mkSG(kin7, mkRect(w*7, 0, w, kin7->h), Pos(36, 54));
	SmartSurface kin8(load(planex_path + "kin_5" + team_name + ".png"));
	kin8.SwapFormatAlpha();
	imgmap_in[CharaAKin8_1Ro] = mkSG(kin8, mkRect(0, 0, w, kin8->h), Pos(32, 55));
	imgmap_in[CharaAKin8_2Ro] = mkSG(kin8, mkRect(w, 0, w, kin8->h), Pos(38, 51));
	imgmap_in[CharaAKin8_3Ro] = mkSG(kin8, mkRect(w*2, 0, w, kin8->h), Pos(39, 52));
	imgmap_in[CharaAKin8_4Ro] = mkSG(kin8, mkRect(w*3, 0, w, kin8->h), Pos(36, 54));
	imgmap_in[CharaAKin8_5Ro] = mkSG(kin8, mkRect(w*4, 0, w, kin8->h), Pos(38, 57));
	imgmap_in[CharaAKin8_6Ro] = mkSG(kin8, mkRect(w*5, 0, w, kin8->h), Pos(42, 58));
	imgmap_in[CharaAKin8_7Ro] = mkSG(kin8, mkRect(w*6, 0, w, kin8->h), Pos(38, 58));
	imgmap_in[CharaAKin8_8Ro] = mkSG(kin8, mkRect(w*7, 0, w, kin8->h), Pos(36, 53));
	SmartSurface kin9(load(planex_path + "kin_8" + team_name + ".png"));
	kin9.SwapFormatAlpha();
	imgmap_in[CharaAKin9_1Ro] = mkSG(kin9, mkRect(0, 0, w, kin9->h), Pos(32, 57));
	imgmap_in[CharaAKin9_2Ro] = mkSG(kin9, mkRect(w, 0, w, kin9->h), Pos(32, 51));
	imgmap_in[CharaAKin9_3Ro] = mkSG(kin9, mkRect(w*2, 0, w, kin9->h), Pos(31, 53));
	imgmap_in[CharaAKin9_4Ro] = mkSG(kin9, mkRect(w*3, 0, w, kin9->h), Pos(40, 52));
	imgmap_in[CharaAKin9_5Ro] = mkSG(kin9, mkRect(w*4, 0, w, kin9->h), Pos(36, 56));
	imgmap_in[CharaAKin9_6Ro] = mkSG(kin9, mkRect(w*5, 0, w, kin9->h), Pos(30, 57));
	imgmap_in[CharaAKin9_7Ro] = mkSG(kin9, mkRect(w*6, 0, w, kin9->h), Pos(31, 56));
	imgmap_in[CharaAKin9_8Ro] = mkSG(kin9, mkRect(w*7, 0, w, kin9->h), Pos(34, 55));
      }

      {
	team_name = "_ha";
	int w = 60;
	int h = 70;
	const Pos pos(w / 2, h / 3 * 2);
	SmartSurface temp(load(planex_path + "small" + team_name + ".png"));
	temp.SwapFormatAlpha();
	imgmap_in[CharaAStand1Ha] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAStand2Ha] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAStand3Ha] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAStand4Ha] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAStand6Ha] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAStand7Ha] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAStand8Ha] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAStand9Ha] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// 歩き
	imgmap_in[CharaAWalk1Ha] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAWalk2Ha] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAWalk3Ha] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAWalk4Ha] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAWalk6Ha] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAWalk7Ha] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAWalk8Ha] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAWalk9Ha] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// ダッシュ
	SmartSurface temp2(load(planex_path + "small_dash" + team_name + ".png"));
	temp2.SwapFormatAlpha();
	imgmap_in[CharaADash1Ha] = mkSG(temp2, mkRect(w, h, w, h), pos);
	imgmap_in[CharaADash2Ha] = mkSG(temp2, mkRect(0, h, w, h), pos);
	imgmap_in[CharaADash3Ha] = mkSG(temp2, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaADash4Ha] = mkSG(temp2, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaADash6Ha] = mkSG(temp2, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaADash7Ha] = mkSG(temp2, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaADash8Ha] = mkSG(temp2, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaADash9Ha] = mkSG(temp2, mkRect(w, 0, w, h), pos);
	// 武器
	w = h = 70;
	SmartSurface ss_shot(load(planex_path + "shot" + team_name + ".png"));
	ss_shot.SwapFormatAlpha();
	imgmap_in[CharaAWeapon1Ha] = mkSG(ss_shot, mkRect(w, h, w, h), Pos(42, 47));
	imgmap_in[CharaAWeapon2Ha] = mkSG(ss_shot, mkRect(0, h, w, h), Pos(30, 46));
	imgmap_in[CharaAWeapon3Ha] = mkSG(ss_shot, mkRect(w * 3, 0, w, h), Pos(37, 48));
	imgmap_in[CharaAWeapon4Ha] = mkSG(ss_shot, mkRect(w * 2, h, w, h), Pos(45, 47));
	imgmap_in[CharaAWeapon6Ha] = mkSG(ss_shot, mkRect(w * 2, 0, w, h), Pos(21, 50));
	imgmap_in[CharaAWeapon7Ha] = mkSG(ss_shot, mkRect(w * 3, h, w, h), Pos(38, 53));
	imgmap_in[CharaAWeapon8Ha] = mkSG(ss_shot, mkRect(0, 0, w, h), Pos(33, 57));
	imgmap_in[CharaAWeapon9Ha] = mkSG(ss_shot, mkRect(w, 0, w, h), Pos(33, 53));
	// ダウン
	SmartSurface temp3(load(planex_path + "down" + team_name + ".png"));
	temp3.SwapFormatAlpha();
	imgmap_in[CharaADown1Ha] = mkSG(temp3, mkRect(0, 0, 52, 47), Pos(25, 25));
	imgmap_in[CharaADown2Ha] = mkSG(temp3, mkRect(52, 0, 104, 47), Pos(25, 25));
	// 近距離攻撃
	int w2 = 80;
	SmartSurface kin1(load(planex_path + "kin_2" + team_name + ".png"));
	kin1.SwapFormatAlpha();
	imgmap_in[CharaAKin1_1Ha] = mkSG(kin1, mkRect(0, 0, w, kin1->h), Pos(37, 48));
	imgmap_in[CharaAKin1_2Ha] = mkSG(kin1, mkRect(w, 0, w, kin1->h), Pos(38, 46));
	imgmap_in[CharaAKin1_3Ha] = mkSG(kin1, mkRect(w*2, 0, w, kin1->h), Pos(34, 48));
	imgmap_in[CharaAKin1_4Ha] = mkSG(kin1, mkRect(w*3, 0, w, kin1->h), Pos(43, 47));
	imgmap_in[CharaAKin1_5Ha] = mkSG(kin1, mkRect(w*4, 0, w, kin1->h), Pos(45, 47));
	imgmap_in[CharaAKin1_6Ha] = mkSG(kin1, mkRect(w*5, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_7Ha] = mkSG(kin1, mkRect(w*6, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_8Ha] = mkSG(kin1, mkRect(w*7, 0, w, kin1->h), Pos(37, 51));
	SmartSurface kin2(load(planex_path + "kin_1" + team_name + ".png"));
	kin2.SwapFormatAlpha();
	imgmap_in[CharaAKin2_1Ha] = mkSG(kin2, mkRect(0, 0, w, kin2->h), Pos(33, 49));
	imgmap_in[CharaAKin2_2Ha] = mkSG(kin2, mkRect(w, 0, w, kin2->h), Pos(36, 47));
	imgmap_in[CharaAKin2_3Ha] = mkSG(kin2, mkRect(w*2, 0, w, kin2->h), Pos(35, 50));
	imgmap_in[CharaAKin2_4Ha] = mkSG(kin2, mkRect(w*3, 0, w, kin2->h), Pos(40, 49));
	imgmap_in[CharaAKin2_5Ha] = mkSG(kin2, mkRect(w*4, 0, w, kin2->h), Pos(42, 49));
	imgmap_in[CharaAKin2_6Ha] = mkSG(kin2, mkRect(w*5, 0, w, kin2->h), Pos(38, 48));
	imgmap_in[CharaAKin2_7Ha] = mkSG(kin2, mkRect(w*6, 0, w, kin2->h), Pos(39, 47));
	imgmap_in[CharaAKin2_8Ha] = mkSG(kin2, mkRect(w*7, 0, w, kin2->h), Pos(36, 48));
	SmartSurface kin3(load(planex_path + "kin_6" + team_name + ".png"));
	kin3.SwapFormatAlpha();
	imgmap_in[CharaAKin3_1Ha] = mkSG(kin3, mkRect(0, 0, w, kin3->h), Pos(32, 44));
	imgmap_in[CharaAKin3_2Ha] = mkSG(kin3, mkRect(w, 0, w, kin3->h), Pos(34, 44));
	imgmap_in[CharaAKin3_3Ha] = mkSG(kin3, mkRect(w*2, 0, w, kin3->h), Pos(36, 47));
	imgmap_in[CharaAKin3_4Ha] = mkSG(kin3, mkRect(w*3, 0, w, kin3->h), Pos(33, 46));
	imgmap_in[CharaAKin3_5Ha] = mkSG(kin3, mkRect(w*4, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_6Ha] = mkSG(kin3, mkRect(w*5, 0, w, kin3->h), Pos(30, 46));
	imgmap_in[CharaAKin3_7Ha] = mkSG(kin3, mkRect(w*6, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_8Ha] = mkSG(kin3, mkRect(w*7, 0, w, kin3->h), Pos(37, 47));
	SmartSurface kin4(load(planex_path + "kin_3" + team_name + ".png"));
	kin4.SwapFormatAlpha();
	imgmap_in[CharaAKin4_1Ha] = mkSG(kin4, mkRect(0, 0, w, kin4->h), Pos(30, 41));
	imgmap_in[CharaAKin4_2Ha] = mkSG(kin4, mkRect(w, 0, w, kin4->h), Pos(37, 42));
	imgmap_in[CharaAKin4_3Ha] = mkSG(kin4, mkRect(w*2, 0, w, kin4->h), Pos(39, 39));
	imgmap_in[CharaAKin4_4Ha] = mkSG(kin4, mkRect(w*3, 0, w2, kin4->h), Pos(49, 44));
	imgmap_in[CharaAKin4_5Ha] = mkSG(kin4, mkRect(w*3+w2, 0, w2, kin4->h), Pos(50, 41));
	imgmap_in[CharaAKin4_6Ha] = mkSG(kin4, mkRect(w*3+w2*2, 0, w, kin4->h), Pos(49, 42));
	imgmap_in[CharaAKin4_7Ha] = mkSG(kin4, mkRect(w*4+w2*2, 0, w, kin4->h), Pos(50, 42));
	imgmap_in[CharaAKin4_8Ha] = mkSG(kin4, mkRect(w*5+w2*2, 0, w, kin4->h), Pos(37, 41));
	SmartSurface kin6(load(planex_path + "kin_7" + team_name + ".png"));
	kin6.SwapFormatAlpha();
	imgmap_in[CharaAKin6_1Ha] = mkSG(kin6, mkRect(0, 0, w, kin6->h), Pos(28, 41));
	imgmap_in[CharaAKin6_2Ha] = mkSG(kin6, mkRect(w, 0, w, kin6->h), Pos(28, 42));
	imgmap_in[CharaAKin6_3Ha] = mkSG(kin6, mkRect(w*2, 0, w, kin6->h), Pos(33, 41));
	imgmap_in[CharaAKin6_4Ha] = mkSG(kin6, mkRect(w*3, 0, w2, kin6->h), Pos(32, 44));
	imgmap_in[CharaAKin6_5Ha] = mkSG(kin6, mkRect(w*3+w2, 0, w2, kin6->h), Pos(31, 41));
	imgmap_in[CharaAKin6_6Ha] = mkSG(kin6, mkRect(w*3+w2*2, 0, w, kin6->h), Pos(20, 42));
	imgmap_in[CharaAKin6_7Ha] = mkSG(kin6, mkRect(w*4+w2*2, 0, w, kin6->h), Pos(20, 41));
	imgmap_in[CharaAKin6_8Ha] = mkSG(kin6, mkRect(w*5+w2*2, 0, w, kin6->h), Pos(31, 41));
	SmartSurface kin7(load(planex_path + "kin_4" + team_name + ".png"));
	kin7.SwapFormatAlpha();
	imgmap_in[CharaAKin7_1Ha] = mkSG(kin7, mkRect(0, 0, w, kin7->h), Pos(32, 54));
	imgmap_in[CharaAKin7_2Ha] = mkSG(kin7, mkRect(w, 0, w, kin7->h), Pos(35, 56));
	imgmap_in[CharaAKin7_3Ha] = mkSG(kin7, mkRect(w*2, 0, w, kin7->h), Pos(34, 54));
	imgmap_in[CharaAKin7_4Ha] = mkSG(kin7, mkRect(w*3, 0, w, kin7->h), Pos(36, 55));
	imgmap_in[CharaAKin7_5Ha] = mkSG(kin7, mkRect(w*4, 0, w, kin7->h), Pos(37, 55));
	imgmap_in[CharaAKin7_6Ha] = mkSG(kin7, mkRect(w*5, 0, w, kin7->h), Pos(44, 55));
	imgmap_in[CharaAKin7_7Ha] = mkSG(kin7, mkRect(w*6, 0, w, kin7->h), Pos(45, 55));
	imgmap_in[CharaAKin7_8Ha] = mkSG(kin7, mkRect(w*7, 0, w, kin7->h), Pos(36, 54));
	SmartSurface kin8(load(planex_path + "kin_5" + team_name + ".png"));
	kin8.SwapFormatAlpha();
	imgmap_in[CharaAKin8_1Ha] = mkSG(kin8, mkRect(0, 0, w, kin8->h), Pos(32, 55));
	imgmap_in[CharaAKin8_2Ha] = mkSG(kin8, mkRect(w, 0, w, kin8->h), Pos(38, 51));
	imgmap_in[CharaAKin8_3Ha] = mkSG(kin8, mkRect(w*2, 0, w, kin8->h), Pos(39, 52));
	imgmap_in[CharaAKin8_4Ha] = mkSG(kin8, mkRect(w*3, 0, w, kin8->h), Pos(36, 54));
	imgmap_in[CharaAKin8_5Ha] = mkSG(kin8, mkRect(w*4, 0, w, kin8->h), Pos(38, 57));
	imgmap_in[CharaAKin8_6Ha] = mkSG(kin8, mkRect(w*5, 0, w, kin8->h), Pos(42, 58));
	imgmap_in[CharaAKin8_7Ha] = mkSG(kin8, mkRect(w*6, 0, w, kin8->h), Pos(38, 58));
	imgmap_in[CharaAKin8_8Ha] = mkSG(kin8, mkRect(w*7, 0, w, kin8->h), Pos(36, 53));
	SmartSurface kin9(load(planex_path + "kin_8" + team_name + ".png"));
	kin9.SwapFormatAlpha();
	imgmap_in[CharaAKin9_1Ha] = mkSG(kin9, mkRect(0, 0, w, kin9->h), Pos(32, 57));
	imgmap_in[CharaAKin9_2Ha] = mkSG(kin9, mkRect(w, 0, w, kin9->h), Pos(32, 51));
	imgmap_in[CharaAKin9_3Ha] = mkSG(kin9, mkRect(w*2, 0, w, kin9->h), Pos(31, 53));
	imgmap_in[CharaAKin9_4Ha] = mkSG(kin9, mkRect(w*3, 0, w, kin9->h), Pos(40, 52));
	imgmap_in[CharaAKin9_5Ha] = mkSG(kin9, mkRect(w*4, 0, w, kin9->h), Pos(36, 56));
	imgmap_in[CharaAKin9_6Ha] = mkSG(kin9, mkRect(w*5, 0, w, kin9->h), Pos(30, 57));
	imgmap_in[CharaAKin9_7Ha] = mkSG(kin9, mkRect(w*6, 0, w, kin9->h), Pos(31, 56));
	imgmap_in[CharaAKin9_8Ha] = mkSG(kin9, mkRect(w*7, 0, w, kin9->h), Pos(34, 55));
      }

      {
	team_name = "_ni";
	int w = 60;
	int h = 70;
	const Pos pos(w / 2, h / 3 * 2);
	SmartSurface temp(load(planex_path + "small" + team_name + ".png"));
	temp.SwapFormatAlpha();
	imgmap_in[CharaAStand1Ni] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAStand2Ni] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAStand3Ni] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAStand4Ni] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAStand6Ni] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAStand7Ni] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAStand8Ni] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAStand9Ni] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// 歩き
	imgmap_in[CharaAWalk1Ni] = mkSG(temp, mkRect(w, h, w, h), pos);
	imgmap_in[CharaAWalk2Ni] = mkSG(temp, mkRect(0, h, w, h), pos);
	imgmap_in[CharaAWalk3Ni] = mkSG(temp, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaAWalk4Ni] = mkSG(temp, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaAWalk6Ni] = mkSG(temp, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaAWalk7Ni] = mkSG(temp, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaAWalk8Ni] = mkSG(temp, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaAWalk9Ni] = mkSG(temp, mkRect(w, 0, w, h), pos);
	// ダッシュ
	SmartSurface temp2(load(planex_path + "small_dash" + team_name + ".png"));
	temp2.SwapFormatAlpha();
	imgmap_in[CharaADash1Ni] = mkSG(temp2, mkRect(w, h, w, h), pos);
	imgmap_in[CharaADash2Ni] = mkSG(temp2, mkRect(0, h, w, h), pos);
	imgmap_in[CharaADash3Ni] = mkSG(temp2, mkRect(w * 3, 0, w, h), pos);
	imgmap_in[CharaADash4Ni] = mkSG(temp2, mkRect(w * 2, h, w, h), pos);
	imgmap_in[CharaADash6Ni] = mkSG(temp2, mkRect(w * 2, 0, w, h), pos);
	imgmap_in[CharaADash7Ni] = mkSG(temp2, mkRect(w * 3, h, w, h), pos);
	imgmap_in[CharaADash8Ni] = mkSG(temp2, mkRect(0, 0, w, h), pos);
	imgmap_in[CharaADash9Ni] = mkSG(temp2, mkRect(w, 0, w, h), pos);
	// 武器
	w = h = 70;
	SmartSurface ss_shot(load(planex_path + "shot" + team_name + ".png"));
	ss_shot.SwapFormatAlpha();
	imgmap_in[CharaAWeapon1Ni] = mkSG(ss_shot, mkRect(w, h, w, h), Pos(42, 47));
	imgmap_in[CharaAWeapon2Ni] = mkSG(ss_shot, mkRect(0, h, w, h), Pos(30, 46));
	imgmap_in[CharaAWeapon3Ni] = mkSG(ss_shot, mkRect(w * 3, 0, w, h), Pos(37, 48));
	imgmap_in[CharaAWeapon4Ni] = mkSG(ss_shot, mkRect(w * 2, h, w, h), Pos(45, 47));
	imgmap_in[CharaAWeapon6Ni] = mkSG(ss_shot, mkRect(w * 2, 0, w, h), Pos(21, 50));
	imgmap_in[CharaAWeapon7Ni] = mkSG(ss_shot, mkRect(w * 3, h, w, h), Pos(38, 53));
	imgmap_in[CharaAWeapon8Ni] = mkSG(ss_shot, mkRect(0, 0, w, h), Pos(33, 57));
	imgmap_in[CharaAWeapon9Ni] = mkSG(ss_shot, mkRect(w, 0, w, h), Pos(33, 53));
	// ダウン
	SmartSurface temp3(load(planex_path + "down" + team_name + ".png"));
	temp3.SwapFormatAlpha();
	imgmap_in[CharaADown1Ni] = mkSG(temp3, mkRect(0, 0, 52, 47), Pos(25, 25));
	imgmap_in[CharaADown2Ni] = mkSG(temp3, mkRect(52, 0, 104, 47), Pos(25, 25));
	// 近距離攻撃
	int w2 = 80;
	SmartSurface kin1(load(planex_path + "kin_2" + team_name + ".png"));
	kin1.SwapFormatAlpha();
	imgmap_in[CharaAKin1_1Ni] = mkSG(kin1, mkRect(0, 0, w, kin1->h), Pos(37, 48));
	imgmap_in[CharaAKin1_2Ni] = mkSG(kin1, mkRect(w, 0, w, kin1->h), Pos(38, 46));
	imgmap_in[CharaAKin1_3Ni] = mkSG(kin1, mkRect(w*2, 0, w, kin1->h), Pos(34, 48));
	imgmap_in[CharaAKin1_4Ni] = mkSG(kin1, mkRect(w*3, 0, w, kin1->h), Pos(43, 47));
	imgmap_in[CharaAKin1_5Ni] = mkSG(kin1, mkRect(w*4, 0, w, kin1->h), Pos(45, 47));
	imgmap_in[CharaAKin1_6Ni] = mkSG(kin1, mkRect(w*5, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_7Ni] = mkSG(kin1, mkRect(w*6, 0, w, kin1->h), Pos(43, 46));
	imgmap_in[CharaAKin1_8Ni] = mkSG(kin1, mkRect(w*7, 0, w, kin1->h), Pos(37, 51));
	SmartSurface kin2(load(planex_path + "kin_1" + team_name + ".png"));
	kin2.SwapFormatAlpha();
	imgmap_in[CharaAKin2_1Ni] = mkSG(kin2, mkRect(0, 0, w, kin2->h), Pos(33, 49));
	imgmap_in[CharaAKin2_2Ni] = mkSG(kin2, mkRect(w, 0, w, kin2->h), Pos(36, 47));
	imgmap_in[CharaAKin2_3Ni] = mkSG(kin2, mkRect(w*2, 0, w, kin2->h), Pos(35, 50));
	imgmap_in[CharaAKin2_4Ni] = mkSG(kin2, mkRect(w*3, 0, w, kin2->h), Pos(40, 49));
	imgmap_in[CharaAKin2_5Ni] = mkSG(kin2, mkRect(w*4, 0, w, kin2->h), Pos(42, 49));
	imgmap_in[CharaAKin2_6Ni] = mkSG(kin2, mkRect(w*5, 0, w, kin2->h), Pos(38, 48));
	imgmap_in[CharaAKin2_7Ni] = mkSG(kin2, mkRect(w*6, 0, w, kin2->h), Pos(39, 47));
	imgmap_in[CharaAKin2_8Ni] = mkSG(kin2, mkRect(w*7, 0, w, kin2->h), Pos(36, 48));
	SmartSurface kin3(load(planex_path + "kin_6" + team_name + ".png"));
	kin3.SwapFormatAlpha();
	imgmap_in[CharaAKin3_1Ni] = mkSG(kin3, mkRect(0, 0, w, kin3->h), Pos(32, 44));
	imgmap_in[CharaAKin3_2Ni] = mkSG(kin3, mkRect(w, 0, w, kin3->h), Pos(34, 44));
	imgmap_in[CharaAKin3_3Ni] = mkSG(kin3, mkRect(w*2, 0, w, kin3->h), Pos(36, 47));
	imgmap_in[CharaAKin3_4Ni] = mkSG(kin3, mkRect(w*3, 0, w, kin3->h), Pos(33, 46));
	imgmap_in[CharaAKin3_5Ni] = mkSG(kin3, mkRect(w*4, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_6Ni] = mkSG(kin3, mkRect(w*5, 0, w, kin3->h), Pos(30, 46));
	imgmap_in[CharaAKin3_7Ni] = mkSG(kin3, mkRect(w*6, 0, w, kin3->h), Pos(30, 45));
	imgmap_in[CharaAKin3_8Ni] = mkSG(kin3, mkRect(w*7, 0, w, kin3->h), Pos(37, 47));
	SmartSurface kin4(load(planex_path + "kin_3" + team_name + ".png"));
	kin4.SwapFormatAlpha();
	imgmap_in[CharaAKin4_1Ni] = mkSG(kin4, mkRect(0, 0, w, kin4->h), Pos(30, 41));
	imgmap_in[CharaAKin4_2Ni] = mkSG(kin4, mkRect(w, 0, w, kin4->h), Pos(37, 42));
	imgmap_in[CharaAKin4_3Ni] = mkSG(kin4, mkRect(w*2, 0, w, kin4->h), Pos(39, 39));
	imgmap_in[CharaAKin4_4Ni] = mkSG(kin4, mkRect(w*3, 0, w2, kin4->h), Pos(49, 44));
	imgmap_in[CharaAKin4_5Ni] = mkSG(kin4, mkRect(w*3+w2, 0, w2, kin4->h), Pos(50, 41));
	imgmap_in[CharaAKin4_6Ni] = mkSG(kin4, mkRect(w*3+w2*2, 0, w, kin4->h), Pos(49, 42));
	imgmap_in[CharaAKin4_7Ni] = mkSG(kin4, mkRect(w*4+w2*2, 0, w, kin4->h), Pos(50, 42));
	imgmap_in[CharaAKin4_8Ni] = mkSG(kin4, mkRect(w*5+w2*2, 0, w, kin4->h), Pos(37, 41));
	SmartSurface kin6(load(planex_path + "kin_7" + team_name + ".png"));
	kin6.SwapFormatAlpha();
	imgmap_in[CharaAKin6_1Ni] = mkSG(kin6, mkRect(0, 0, w, kin6->h), Pos(28, 41));
	imgmap_in[CharaAKin6_2Ni] = mkSG(kin6, mkRect(w, 0, w, kin6->h), Pos(28, 42));
	imgmap_in[CharaAKin6_3Ni] = mkSG(kin6, mkRect(w*2, 0, w, kin6->h), Pos(33, 41));
	imgmap_in[CharaAKin6_4Ni] = mkSG(kin6, mkRect(w*3, 0, w2, kin6->h), Pos(32, 44));
	imgmap_in[CharaAKin6_5Ni] = mkSG(kin6, mkRect(w*3+w2, 0, w2, kin6->h), Pos(31, 41));
	imgmap_in[CharaAKin6_6Ni] = mkSG(kin6, mkRect(w*3+w2*2, 0, w, kin6->h), Pos(20, 42));
	imgmap_in[CharaAKin6_7Ni] = mkSG(kin6, mkRect(w*4+w2*2, 0, w, kin6->h), Pos(20, 41));
	imgmap_in[CharaAKin6_8Ni] = mkSG(kin6, mkRect(w*5+w2*2, 0, w, kin6->h), Pos(31, 41));
	SmartSurface kin7(load(planex_path + "kin_4" + team_name + ".png"));
	kin7.SwapFormatAlpha();
	imgmap_in[CharaAKin7_1Ni] = mkSG(kin7, mkRect(0, 0, w, kin7->h), Pos(32, 54));
	imgmap_in[CharaAKin7_2Ni] = mkSG(kin7, mkRect(w, 0, w, kin7->h), Pos(35, 56));
	imgmap_in[CharaAKin7_3Ni] = mkSG(kin7, mkRect(w*2, 0, w, kin7->h), Pos(34, 54));
	imgmap_in[CharaAKin7_4Ni] = mkSG(kin7, mkRect(w*3, 0, w, kin7->h), Pos(36, 55));
	imgmap_in[CharaAKin7_5Ni] = mkSG(kin7, mkRect(w*4, 0, w, kin7->h), Pos(37, 55));
	imgmap_in[CharaAKin7_6Ni] = mkSG(kin7, mkRect(w*5, 0, w, kin7->h), Pos(44, 55));
	imgmap_in[CharaAKin7_7Ni] = mkSG(kin7, mkRect(w*6, 0, w, kin7->h), Pos(45, 55));
	imgmap_in[CharaAKin7_8Ni] = mkSG(kin7, mkRect(w*7, 0, w, kin7->h), Pos(36, 54));
	SmartSurface kin8(load(planex_path + "kin_5" + team_name + ".png"));
	kin8.SwapFormatAlpha();
	imgmap_in[CharaAKin8_1Ni] = mkSG(kin8, mkRect(0, 0, w, kin8->h), Pos(32, 55));
	imgmap_in[CharaAKin8_2Ni] = mkSG(kin8, mkRect(w, 0, w, kin8->h), Pos(38, 51));
	imgmap_in[CharaAKin8_3Ni] = mkSG(kin8, mkRect(w*2, 0, w, kin8->h), Pos(39, 52));
	imgmap_in[CharaAKin8_4Ni] = mkSG(kin8, mkRect(w*3, 0, w, kin8->h), Pos(36, 54));
	imgmap_in[CharaAKin8_5Ni] = mkSG(kin8, mkRect(w*4, 0, w, kin8->h), Pos(38, 57));
	imgmap_in[CharaAKin8_6Ni] = mkSG(kin8, mkRect(w*5, 0, w, kin8->h), Pos(42, 58));
	imgmap_in[CharaAKin8_7Ni] = mkSG(kin8, mkRect(w*6, 0, w, kin8->h), Pos(38, 58));
	imgmap_in[CharaAKin8_8Ni] = mkSG(kin8, mkRect(w*7, 0, w, kin8->h), Pos(36, 53));
	SmartSurface kin9(load(planex_path + "kin_8" + team_name + ".png"));
	kin9.SwapFormatAlpha();
	imgmap_in[CharaAKin9_1Ni] = mkSG(kin9, mkRect(0, 0, w, kin9->h), Pos(32, 57));
	imgmap_in[CharaAKin9_2Ni] = mkSG(kin9, mkRect(w, 0, w, kin9->h), Pos(32, 51));
	imgmap_in[CharaAKin9_3Ni] = mkSG(kin9, mkRect(w*2, 0, w, kin9->h), Pos(31, 53));
	imgmap_in[CharaAKin9_4Ni] = mkSG(kin9, mkRect(w*3, 0, w, kin9->h), Pos(40, 52));
	imgmap_in[CharaAKin9_5Ni] = mkSG(kin9, mkRect(w*4, 0, w, kin9->h), Pos(36, 56));
	imgmap_in[CharaAKin9_6Ni] = mkSG(kin9, mkRect(w*5, 0, w, kin9->h), Pos(30, 57));
	imgmap_in[CharaAKin9_7Ni] = mkSG(kin9, mkRect(w*6, 0, w, kin9->h), Pos(31, 56));
	imgmap_in[CharaAKin9_8Ni] = mkSG(kin9, mkRect(w*7, 0, w, kin9->h), Pos(34, 55));
      }
    }

    // Yamahaの読み込み
    {
      string team_name = "_i";
      {
	SmartSurface ss = load(yamaha_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	unsigned w = 30;
	int pos_up = 5;
	// 立ち
	imgmap_in[YamahaStand2I] = mkSG(ss, mkRect(0, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand1I] = mkSG(ss, mkRect(w, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand4I] = mkSG(ss, mkRect(w*2, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand7I] = mkSG(ss, mkRect(w*3, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand8I] = mkSG(ss, mkRect(w*4, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand9I] = mkSG(ss, mkRect(w*5, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand6I] = mkSG(ss, mkRect(w*6, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand3I] = mkSG(ss, mkRect(w*7, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	ss = load(yamaha_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 42;
	unsigned h = 53;
	// 前
	imgmap_in[YamahaAhead2I] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAhead1I] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAhead4I] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAhead7I] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAhead8I] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAhead9I] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAhead6I] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAhead3I] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBack2I] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBack1I] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBack4I] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBack7I] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBack8I] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBack9I] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBack6I] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBack3I] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));
	// ダッシュ透明版
	ss = load(yamaha_path + "dash_t" + team_name + ".png");
	ss.SwapFormatAlpha();
	// 前
	imgmap_in[YamahaAheadT2I] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAheadT1I] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAheadT4I] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAheadT7I] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAheadT8I] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAheadT9I] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAheadT6I] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAheadT3I] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBackT2I] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBackT1I] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBackT4I] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBackT7I] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBackT8I] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBackT9I] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBackT6I] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBackT3I] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));

	// レーザー
	Pos lpos[8];
	lpos[0] = Pos(41, 50);
	lpos[1] = Pos(31, 56);
	lpos[2] = Pos(23, 52);
	lpos[3] = Pos(33, 59);
	lpos[4] = Pos(28, 61);
	lpos[5] = Pos(31, 68);
	lpos[6] = Pos(30, 59);
	lpos[7] = Pos(24, 61);
	ss = load(yamaha_path + "laser" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 60;
	h = 75;
	imgmap_in[YamahaLaser1_1I] = mkSG(ss, mkRect(w*1, h, w, h), lpos[0]);
	imgmap_in[YamahaLaser1_2I] = mkSG(ss, mkRect(w*2+3, h, w, h), lpos[0] + Pos(-3, 0));
	imgmap_in[YamahaLaser1_3I] = mkSG(ss, mkRect(w*3, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser1_4I] = mkSG(ss, mkRect(w*4, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser2_1I] = mkSG(ss, mkRect(w*1, 0, w, h), lpos[1] + Pos(1, 0));
	imgmap_in[YamahaLaser2_2I] = mkSG(ss, mkRect(w*2, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_3I] = mkSG(ss, mkRect(w*3, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_4I] = mkSG(ss, mkRect(w*4, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser3_1I] = mkSG(ss, mkRect(w*1+5, h*5, w, h), lpos[2]);
	imgmap_in[YamahaLaser3_2I] = mkSG(ss, mkRect(w*2, h*5, w, h), lpos[2] + Pos(5, 0));
	imgmap_in[YamahaLaser3_3I] = mkSG(ss, mkRect(w*3, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser3_4I] = mkSG(ss, mkRect(w*4, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser4_1I] = mkSG(ss, mkRect(w*1, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_2I] = mkSG(ss, mkRect(w*2, h*6, w, h), lpos[3] + Pos(-1, 0));
	imgmap_in[YamahaLaser4_3I] = mkSG(ss, mkRect(w*3, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_4I] = mkSG(ss, mkRect(w*4, h*6, w, h), lpos[3] + Pos(3, -1));
	imgmap_in[YamahaLaser6_1I] = mkSG(ss, mkRect(w*1, h*2, w, h), lpos[4]);
	imgmap_in[YamahaLaser6_2I] = mkSG(ss, mkRect(w*2, h*2, w, h), lpos[4] + Pos(-1, 0));
	imgmap_in[YamahaLaser6_3I] = mkSG(ss, mkRect(w*3, h*2, w, h), lpos[4] + Pos(0, 0));
	imgmap_in[YamahaLaser6_4I] = mkSG(ss, mkRect(w*4, h*2, w, h), lpos[4] + Pos(3, -1));
	imgmap_in[YamahaLaser7_1I] = mkSG(ss, mkRect(w*1, h*7, w, h), lpos[5]);
	imgmap_in[YamahaLaser7_2I] = mkSG(ss, mkRect(w*2, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_3I] = mkSG(ss, mkRect(w*3, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_4I] = mkSG(ss, mkRect(w*4, h*7, w, h), lpos[5] + Pos(0, -1));
	imgmap_in[YamahaLaser8_1I] = mkSG(ss, mkRect(w*1, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_2I] = mkSG(ss, mkRect(w*2, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_3I] = mkSG(ss, mkRect(w*3, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_4I] = mkSG(ss, mkRect(w*4, h*4, w, h), lpos[6] + Pos(0, -5));
	imgmap_in[YamahaLaser9_1I] = mkSG(ss, mkRect(w*1, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_2I] = mkSG(ss, mkRect(w*2, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_3I] = mkSG(ss, mkRect(w*3, h*3, w, h), lpos[7] + Pos(-1, 0));
	imgmap_in[YamahaLaser9_4I] = mkSG(ss, mkRect(w*4, h*3, w, h), lpos[7] + Pos(-1, 0));
	// ダウン
	w = 50;
	h = 27;
	int diff = 30;
	ss = load(yamaha_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[YamahaDown6I] = mkSG(ss, mkRect(0, 0, w, h), Pos(diff, 25));
	imgmap_in[YamahaDown4I] = mkSG(ss, mkRect(w, 0, w, h), Pos(w - diff, 25));

	// 近距離攻撃
	Pos pos[8];
	pos[0] = Pos(65, 51);
	pos[1] = Pos(38, 51);
	pos[2] = Pos(25, 51);
	pos[3] = Pos(55, 51);
	pos[4] = Pos(20, 51);
	pos[5] = Pos(57, 74);
	pos[6] = Pos(22, 86);
	pos[7] = Pos(18, 73);
	// dir1
	ss = load(yamaha_path + "y_2_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin1_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[0]);
	// dir2
	ss = load(yamaha_path + "y_1_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 105;
	imgmap_in[YamahaKin2_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[1]);
	// dir3
	ss = load(yamaha_path + "y_8_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin3_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[2]);
	// dir4
	ss = load(yamaha_path + "y_7_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin4_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[3]);
	// dir6
	ss = load(yamaha_path + "y_3_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin6_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[4]);
	// dir7
	ss = load(yamaha_path + "y_6_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin7_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[5]);
	// dir8
	ss = load(yamaha_path + "y_5_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 45;
	h = 90;
	imgmap_in[YamahaKin8_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[6]);
	// dir9
	ss = load(yamaha_path + "y_4_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin9_1I] = mkSG(ss, mkRect(w, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_2I] = mkSG(ss, mkRect(w*2, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_3I] = mkSG(ss, mkRect(w*3, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_4I] = mkSG(ss, mkRect(w*4, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_5I] = mkSG(ss, mkRect(w*5, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_7I] = mkSG(ss, mkRect(w*7, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_8I] = mkSG(ss, mkRect(w*8, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_9I] = mkSG(ss, mkRect(w*9, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_10I] = mkSG(ss, mkRect(w*10, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_11I] = mkSG(ss, mkRect(w*11, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_12I] = mkSG(ss, mkRect(w*12, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_13I] = mkSG(ss, mkRect(w*13, 0, w, h), pos[7]);
      }

      {
	team_name = "_ro";
	SmartSurface ss = load(yamaha_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	unsigned w = 30;
	int pos_up = 5;
	// 立ち
	imgmap_in[YamahaStand2Ro] = mkSG(ss, mkRect(0, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand1Ro] = mkSG(ss, mkRect(w, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand4Ro] = mkSG(ss, mkRect(w*2, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand7Ro] = mkSG(ss, mkRect(w*3, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand8Ro] = mkSG(ss, mkRect(w*4, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand9Ro] = mkSG(ss, mkRect(w*5, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand6Ro] = mkSG(ss, mkRect(w*6, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand3Ro] = mkSG(ss, mkRect(w*7, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	ss = load(yamaha_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 42;
	unsigned h = 53;
	// 前
	imgmap_in[YamahaAhead2Ro] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAhead1Ro] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAhead4Ro] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAhead7Ro] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAhead8Ro] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAhead9Ro] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAhead6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAhead3Ro] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBack2Ro] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBack1Ro] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBack4Ro] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBack7Ro] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBack8Ro] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBack9Ro] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBack6Ro] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBack3Ro] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));
	// ダッシュ透明版
	ss = load(yamaha_path + "dash_t" + team_name + ".png");
	ss.SwapFormatAlpha();
	// 前
	imgmap_in[YamahaAheadT2Ro] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAheadT1Ro] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAheadT4Ro] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAheadT7Ro] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAheadT8Ro] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAheadT9Ro] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAheadT6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAheadT3Ro] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBackT2Ro] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBackT1Ro] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBackT4Ro] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBackT7Ro] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBackT8Ro] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBackT9Ro] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBackT6Ro] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBackT3Ro] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));

	// レーザー
	Pos lpos[8];
	lpos[0] = Pos(41, 50);
	lpos[1] = Pos(31, 56);
	lpos[2] = Pos(23, 52);
	lpos[3] = Pos(33, 59);
	lpos[4] = Pos(28, 61);
	lpos[5] = Pos(31, 68);
	lpos[6] = Pos(30, 59);
	lpos[7] = Pos(24, 61);
	ss = load(yamaha_path + "laser" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 60;
	h = 75;
	imgmap_in[YamahaLaser1_1Ro] = mkSG(ss, mkRect(w*1, h, w, h), lpos[0]);
	imgmap_in[YamahaLaser1_2Ro] = mkSG(ss, mkRect(w*2+3, h, w, h), lpos[0] + Pos(-3, 0));
	imgmap_in[YamahaLaser1_3Ro] = mkSG(ss, mkRect(w*3, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser1_4Ro] = mkSG(ss, mkRect(w*4, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser2_1Ro] = mkSG(ss, mkRect(w*1, 0, w, h), lpos[1] + Pos(1, 0));
	imgmap_in[YamahaLaser2_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser3_1Ro] = mkSG(ss, mkRect(w*1+5, h*5, w, h), lpos[2]);
	imgmap_in[YamahaLaser3_2Ro] = mkSG(ss, mkRect(w*2, h*5, w, h), lpos[2] + Pos(5, 0));
	imgmap_in[YamahaLaser3_3Ro] = mkSG(ss, mkRect(w*3, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser3_4Ro] = mkSG(ss, mkRect(w*4, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser4_1Ro] = mkSG(ss, mkRect(w*1, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_2Ro] = mkSG(ss, mkRect(w*2, h*6, w, h), lpos[3] + Pos(-1, 0));
	imgmap_in[YamahaLaser4_3Ro] = mkSG(ss, mkRect(w*3, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_4Ro] = mkSG(ss, mkRect(w*4, h*6, w, h), lpos[3] + Pos(3, -1));
	imgmap_in[YamahaLaser6_1Ro] = mkSG(ss, mkRect(w*1, h*2, w, h), lpos[4]);
	imgmap_in[YamahaLaser6_2Ro] = mkSG(ss, mkRect(w*2, h*2, w, h), lpos[4] + Pos(-1, 0));
	imgmap_in[YamahaLaser6_3Ro] = mkSG(ss, mkRect(w*3, h*2, w, h), lpos[4] + Pos(0, 0));
	imgmap_in[YamahaLaser6_4Ro] = mkSG(ss, mkRect(w*4, h*2, w, h), lpos[4] + Pos(3, -1));
	imgmap_in[YamahaLaser7_1Ro] = mkSG(ss, mkRect(w*1, h*7, w, h), lpos[5]);
	imgmap_in[YamahaLaser7_2Ro] = mkSG(ss, mkRect(w*2, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_3Ro] = mkSG(ss, mkRect(w*3, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_4Ro] = mkSG(ss, mkRect(w*4, h*7, w, h), lpos[5] + Pos(0, -1));
	imgmap_in[YamahaLaser8_1Ro] = mkSG(ss, mkRect(w*1, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_2Ro] = mkSG(ss, mkRect(w*2, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_3Ro] = mkSG(ss, mkRect(w*3, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_4Ro] = mkSG(ss, mkRect(w*4, h*4, w, h), lpos[6] + Pos(0, -5));
	imgmap_in[YamahaLaser9_1Ro] = mkSG(ss, mkRect(w*1, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_2Ro] = mkSG(ss, mkRect(w*2, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_3Ro] = mkSG(ss, mkRect(w*3, h*3, w, h), lpos[7] + Pos(-1, 0));
	imgmap_in[YamahaLaser9_4Ro] = mkSG(ss, mkRect(w*4, h*3, w, h), lpos[7] + Pos(-1, 0));
	// ダウン
	w = 50;
	h = 27;
	int diff = 30;
	ss = load(yamaha_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[YamahaDown6Ro] = mkSG(ss, mkRect(0, 0, w, h), Pos(diff, 25));
	imgmap_in[YamahaDown4Ro] = mkSG(ss, mkRect(w, 0, w, h), Pos(w - diff, 25));

	// 近距離攻撃
	Pos pos[8];
	pos[0] = Pos(65, 51);
	pos[1] = Pos(38, 51);
	pos[2] = Pos(25, 51);
	pos[3] = Pos(55, 51);
	pos[4] = Pos(20, 51);
	pos[5] = Pos(57, 74);
	pos[6] = Pos(22, 86);
	pos[7] = Pos(18, 73);
	// dir1
	ss = load(yamaha_path + "y_2_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin1_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[0]);
	// dir2
	ss = load(yamaha_path + "y_1_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 105;
	imgmap_in[YamahaKin2_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[1]);
	// dir3
	ss = load(yamaha_path + "y_8_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin3_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[2]);
	// dir4
	ss = load(yamaha_path + "y_7_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin4_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[3]);
	// dir6
	ss = load(yamaha_path + "y_3_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin6_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[4]);
	// dir7
	ss = load(yamaha_path + "y_6_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin7_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[5]);
	// dir8
	ss = load(yamaha_path + "y_5_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 45;
	h = 90;
	imgmap_in[YamahaKin8_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[6]);
	// dir9
	ss = load(yamaha_path + "y_4_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin9_1Ro] = mkSG(ss, mkRect(w, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_2Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_3Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_4Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_5Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_7Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_8Ro] = mkSG(ss, mkRect(w*8, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_9Ro] = mkSG(ss, mkRect(w*9, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_10Ro] = mkSG(ss, mkRect(w*10, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_11Ro] = mkSG(ss, mkRect(w*11, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_12Ro] = mkSG(ss, mkRect(w*12, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_13Ro] = mkSG(ss, mkRect(w*13, 0, w, h), pos[7]);
      }

      {
	team_name = "_ha";
	SmartSurface ss = load(yamaha_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	unsigned w = 30;
	int pos_up = 5;
	// 立ち
	imgmap_in[YamahaStand2Ha] = mkSG(ss, mkRect(0, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand1Ha] = mkSG(ss, mkRect(w, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand4Ha] = mkSG(ss, mkRect(w*2, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand7Ha] = mkSG(ss, mkRect(w*3, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand8Ha] = mkSG(ss, mkRect(w*4, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand9Ha] = mkSG(ss, mkRect(w*5, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand6Ha] = mkSG(ss, mkRect(w*6, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand3Ha] = mkSG(ss, mkRect(w*7, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	ss = load(yamaha_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 42;
	unsigned h = 53;
	// 前
	imgmap_in[YamahaAhead2Ha] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAhead1Ha] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAhead4Ha] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAhead7Ha] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAhead8Ha] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAhead9Ha] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAhead6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAhead3Ha] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBack2Ha] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBack1Ha] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBack4Ha] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBack7Ha] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBack8Ha] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBack9Ha] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBack6Ha] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBack3Ha] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));
	// ダッシュ透明版
	ss = load(yamaha_path + "dash_t" + team_name + ".png");
	ss.SwapFormatAlpha();
	// 前
	imgmap_in[YamahaAheadT2Ha] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAheadT1Ha] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAheadT4Ha] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAheadT7Ha] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAheadT8Ha] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAheadT9Ha] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAheadT6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAheadT3Ha] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBackT2Ha] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBackT1Ha] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBackT4Ha] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBackT7Ha] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBackT8Ha] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBackT9Ha] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBackT6Ha] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBackT3Ha] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));

	// レーザー
	Pos lpos[8];
	lpos[0] = Pos(41, 50);
	lpos[1] = Pos(31, 56);
	lpos[2] = Pos(23, 52);
	lpos[3] = Pos(33, 59);
	lpos[4] = Pos(28, 61);
	lpos[5] = Pos(31, 68);
	lpos[6] = Pos(30, 59);
	lpos[7] = Pos(24, 61);
	ss = load(yamaha_path + "laser" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 60;
	h = 75;
	imgmap_in[YamahaLaser1_1Ha] = mkSG(ss, mkRect(w*1, h, w, h), lpos[0]);
	imgmap_in[YamahaLaser1_2Ha] = mkSG(ss, mkRect(w*2+3, h, w, h), lpos[0] + Pos(-3, 0));
	imgmap_in[YamahaLaser1_3Ha] = mkSG(ss, mkRect(w*3, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser1_4Ha] = mkSG(ss, mkRect(w*4, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser2_1Ha] = mkSG(ss, mkRect(w*1, 0, w, h), lpos[1] + Pos(1, 0));
	imgmap_in[YamahaLaser2_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser3_1Ha] = mkSG(ss, mkRect(w*1+5, h*5, w, h), lpos[2]);
	imgmap_in[YamahaLaser3_2Ha] = mkSG(ss, mkRect(w*2, h*5, w, h), lpos[2] + Pos(5, 0));
	imgmap_in[YamahaLaser3_3Ha] = mkSG(ss, mkRect(w*3, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser3_4Ha] = mkSG(ss, mkRect(w*4, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser4_1Ha] = mkSG(ss, mkRect(w*1, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_2Ha] = mkSG(ss, mkRect(w*2, h*6, w, h), lpos[3] + Pos(-1, 0));
	imgmap_in[YamahaLaser4_3Ha] = mkSG(ss, mkRect(w*3, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_4Ha] = mkSG(ss, mkRect(w*4, h*6, w, h), lpos[3] + Pos(3, -1));
	imgmap_in[YamahaLaser6_1Ha] = mkSG(ss, mkRect(w*1, h*2, w, h), lpos[4]);
	imgmap_in[YamahaLaser6_2Ha] = mkSG(ss, mkRect(w*2, h*2, w, h), lpos[4] + Pos(-1, 0));
	imgmap_in[YamahaLaser6_3Ha] = mkSG(ss, mkRect(w*3, h*2, w, h), lpos[4] + Pos(0, 0));
	imgmap_in[YamahaLaser6_4Ha] = mkSG(ss, mkRect(w*4, h*2, w, h), lpos[4] + Pos(3, -1));
	imgmap_in[YamahaLaser7_1Ha] = mkSG(ss, mkRect(w*1, h*7, w, h), lpos[5]);
	imgmap_in[YamahaLaser7_2Ha] = mkSG(ss, mkRect(w*2, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_3Ha] = mkSG(ss, mkRect(w*3, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_4Ha] = mkSG(ss, mkRect(w*4, h*7, w, h), lpos[5] + Pos(0, -1));
	imgmap_in[YamahaLaser8_1Ha] = mkSG(ss, mkRect(w*1, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_2Ha] = mkSG(ss, mkRect(w*2, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_3Ha] = mkSG(ss, mkRect(w*3, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_4Ha] = mkSG(ss, mkRect(w*4, h*4, w, h), lpos[6] + Pos(0, -5));
	imgmap_in[YamahaLaser9_1Ha] = mkSG(ss, mkRect(w*1, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_2Ha] = mkSG(ss, mkRect(w*2, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_3Ha] = mkSG(ss, mkRect(w*3, h*3, w, h), lpos[7] + Pos(-1, 0));
	imgmap_in[YamahaLaser9_4Ha] = mkSG(ss, mkRect(w*4, h*3, w, h), lpos[7] + Pos(-1, 0));
	// ダウン
	w = 50;
	h = 27;
	int diff = 30;
	ss = load(yamaha_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[YamahaDown6Ha] = mkSG(ss, mkRect(0, 0, w, h), Pos(diff, 25));
	imgmap_in[YamahaDown4Ha] = mkSG(ss, mkRect(w, 0, w, h), Pos(w - diff, 25));

	// 近距離攻撃
	Pos pos[8];
	pos[0] = Pos(65, 51);
	pos[1] = Pos(38, 51);
	pos[2] = Pos(25, 51);
	pos[3] = Pos(55, 51);
	pos[4] = Pos(20, 51);
	pos[5] = Pos(57, 74);
	pos[6] = Pos(22, 86);
	pos[7] = Pos(18, 73);
	// dir1
	ss = load(yamaha_path + "y_2_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin1_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[0]);
	// dir2
	ss = load(yamaha_path + "y_1_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 105;
	imgmap_in[YamahaKin2_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[1]);
	// dir3
	ss = load(yamaha_path + "y_8_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin3_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[2]);
	// dir4
	ss = load(yamaha_path + "y_7_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin4_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[3]);
	// dir6
	ss = load(yamaha_path + "y_3_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin6_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[4]);
	// dir7
	ss = load(yamaha_path + "y_6_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin7_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[5]);
	// dir8
	ss = load(yamaha_path + "y_5_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 45;
	h = 90;
	imgmap_in[YamahaKin8_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[6]);
	// dir9
	ss = load(yamaha_path + "y_4_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin9_1Ha] = mkSG(ss, mkRect(w, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_2Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_3Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_4Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_5Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_7Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_8Ha] = mkSG(ss, mkRect(w*8, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_9Ha] = mkSG(ss, mkRect(w*9, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_10Ha] = mkSG(ss, mkRect(w*10, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_11Ha] = mkSG(ss, mkRect(w*11, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_12Ha] = mkSG(ss, mkRect(w*12, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_13Ha] = mkSG(ss, mkRect(w*13, 0, w, h), pos[7]);
      }

      {
	team_name = "_ni";
	SmartSurface ss = load(yamaha_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	unsigned w = 30;
	int pos_up = 5;
	// 立ち
	imgmap_in[YamahaStand2Ni] = mkSG(ss, mkRect(0, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand1Ni] = mkSG(ss, mkRect(w, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand4Ni] = mkSG(ss, mkRect(w*2, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand7Ni] = mkSG(ss, mkRect(w*3, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand8Ni] = mkSG(ss, mkRect(w*4, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand9Ni] = mkSG(ss, mkRect(w*5, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand6Ni] = mkSG(ss, mkRect(w*6, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	imgmap_in[YamahaStand3Ni] = mkSG(ss, mkRect(w*7, 0, w, ss->h), Pos(w/2, ss->h-pos_up));
	ss = load(yamaha_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 42;
	unsigned h = 53;
	// 前
	imgmap_in[YamahaAhead2Ni] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAhead1Ni] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAhead4Ni] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAhead7Ni] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAhead8Ni] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAhead9Ni] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAhead6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAhead3Ni] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBack2Ni] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBack1Ni] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBack4Ni] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBack7Ni] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBack8Ni] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBack9Ni] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBack6Ni] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBack3Ni] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));
	// ダッシュ透明版
	ss = load(yamaha_path + "dash_t" + team_name + ".png");
	ss.SwapFormatAlpha();
	// 前
	imgmap_in[YamahaAheadT2Ni] = mkSG(ss, mkRect(0, 0, w, h), Pos(21, 33));
	imgmap_in[YamahaAheadT1Ni] = mkSG(ss, mkRect(w, 0, w, h), Pos(27, 33));
	imgmap_in[YamahaAheadT4Ni] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(36, 44));
	imgmap_in[YamahaAheadT7Ni] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(29, 43));
	imgmap_in[YamahaAheadT8Ni] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(22, 53));
	imgmap_in[YamahaAheadT9Ni] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(9, 43));
	imgmap_in[YamahaAheadT6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(4, 48));
	imgmap_in[YamahaAheadT3Ni] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(15, 34));
	// 後ろ
	imgmap_in[YamahaBackT2Ni] = mkSG(ss, mkRect(0, h, w, h-1), Pos(22, 50));
	imgmap_in[YamahaBackT1Ni] = mkSG(ss, mkRect(w, h, w, h-1), Pos(14, 48));
	imgmap_in[YamahaBackT4Ni] = mkSG(ss, mkRect(w*2, h, w, h-1), Pos(13, 45));
	imgmap_in[YamahaBackT7Ni] = mkSG(ss, mkRect(w*3, h, w, h-1), Pos(14, 35));
	imgmap_in[YamahaBackT8Ni] = mkSG(ss, mkRect(w*4, h, w, h-1), Pos(19, 44));
	imgmap_in[YamahaBackT9Ni] = mkSG(ss, mkRect(w*5, h, w, h-1), Pos(26, 35));
	imgmap_in[YamahaBackT6Ni] = mkSG(ss, mkRect(w*6, h, w, h-1), Pos(29, 44));
	imgmap_in[YamahaBackT3Ni] = mkSG(ss, mkRect(w*7, h, w, h-1), Pos(27, 48));

	// レーザー
	Pos lpos[8];
	lpos[0] = Pos(41, 50);
	lpos[1] = Pos(31, 56);
	lpos[2] = Pos(23, 52);
	lpos[3] = Pos(33, 59);
	lpos[4] = Pos(28, 61);
	lpos[5] = Pos(31, 68);
	lpos[6] = Pos(30, 59);
	lpos[7] = Pos(24, 61);
	ss = load(yamaha_path + "laser" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 60;
	h = 75;
	imgmap_in[YamahaLaser1_1Ni] = mkSG(ss, mkRect(w*1, h, w, h), lpos[0]);
	imgmap_in[YamahaLaser1_2Ni] = mkSG(ss, mkRect(w*2+3, h, w, h), lpos[0] + Pos(-3, 0));
	imgmap_in[YamahaLaser1_3Ni] = mkSG(ss, mkRect(w*3, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser1_4Ni] = mkSG(ss, mkRect(w*4, h, w, h), lpos[0] + Pos(1, 0));
	imgmap_in[YamahaLaser2_1Ni] = mkSG(ss, mkRect(w*1, 0, w, h), lpos[1] + Pos(1, 0));
	imgmap_in[YamahaLaser2_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser2_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), lpos[1]);
	imgmap_in[YamahaLaser3_1Ni] = mkSG(ss, mkRect(w*1+5, h*5, w, h), lpos[2]);
	imgmap_in[YamahaLaser3_2Ni] = mkSG(ss, mkRect(w*2, h*5, w, h), lpos[2] + Pos(5, 0));
	imgmap_in[YamahaLaser3_3Ni] = mkSG(ss, mkRect(w*3, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser3_4Ni] = mkSG(ss, mkRect(w*4, h*5, w, h), lpos[2] + Pos(5, 1));
	imgmap_in[YamahaLaser4_1Ni] = mkSG(ss, mkRect(w*1, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_2Ni] = mkSG(ss, mkRect(w*2, h*6, w, h), lpos[3] + Pos(-1, 0));
	imgmap_in[YamahaLaser4_3Ni] = mkSG(ss, mkRect(w*3, h*6, w, h), lpos[3]);
	imgmap_in[YamahaLaser4_4Ni] = mkSG(ss, mkRect(w*4, h*6, w, h), lpos[3] + Pos(3, -1));
	imgmap_in[YamahaLaser6_1Ni] = mkSG(ss, mkRect(w*1, h*2, w, h), lpos[4]);
	imgmap_in[YamahaLaser6_2Ni] = mkSG(ss, mkRect(w*2, h*2, w, h), lpos[4] + Pos(-1, 0));
	imgmap_in[YamahaLaser6_3Ni] = mkSG(ss, mkRect(w*3, h*2, w, h), lpos[4] + Pos(0, 0));
	imgmap_in[YamahaLaser6_4Ni] = mkSG(ss, mkRect(w*4, h*2, w, h), lpos[4] + Pos(3, -1));
	imgmap_in[YamahaLaser7_1Ni] = mkSG(ss, mkRect(w*1, h*7, w, h), lpos[5]);
	imgmap_in[YamahaLaser7_2Ni] = mkSG(ss, mkRect(w*2, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_3Ni] = mkSG(ss, mkRect(w*3, h*7, w, h), lpos[5] + Pos(-1, -1));
	imgmap_in[YamahaLaser7_4Ni] = mkSG(ss, mkRect(w*4, h*7, w, h), lpos[5] + Pos(0, -1));
	imgmap_in[YamahaLaser8_1Ni] = mkSG(ss, mkRect(w*1, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_2Ni] = mkSG(ss, mkRect(w*2, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_3Ni] = mkSG(ss, mkRect(w*3, h*4, w, h), lpos[6]);
	imgmap_in[YamahaLaser8_4Ni] = mkSG(ss, mkRect(w*4, h*4, w, h), lpos[6] + Pos(0, -5));
	imgmap_in[YamahaLaser9_1Ni] = mkSG(ss, mkRect(w*1, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_2Ni] = mkSG(ss, mkRect(w*2, h*3, w, h), lpos[7]);
	imgmap_in[YamahaLaser9_3Ni] = mkSG(ss, mkRect(w*3, h*3, w, h), lpos[7] + Pos(-1, 0));
	imgmap_in[YamahaLaser9_4Ni] = mkSG(ss, mkRect(w*4, h*3, w, h), lpos[7] + Pos(-1, 0));
	// ダウン
	w = 50;
	h = 27;
	int diff = 30;
	ss = load(yamaha_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[YamahaDown6Ni] = mkSG(ss, mkRect(0, 0, w, h), Pos(diff, 25));
	imgmap_in[YamahaDown4Ni] = mkSG(ss, mkRect(w, 0, w, h), Pos(w - diff, 25));

	// 近距離攻撃
	Pos pos[8];
	pos[0] = Pos(65, 51);
	pos[1] = Pos(38, 51);
	pos[2] = Pos(25, 51);
	pos[3] = Pos(55, 51);
	pos[4] = Pos(20, 51);
	pos[5] = Pos(57, 74);
	pos[6] = Pos(22, 86);
	pos[7] = Pos(18, 73);
	// dir1
	ss = load(yamaha_path + "y_2_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin1_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[0]);
	imgmap_in[YamahaKin1_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[0]);
	// dir2
	ss = load(yamaha_path + "y_1_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 105;
	imgmap_in[YamahaKin2_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[1]);
	imgmap_in[YamahaKin2_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[1]);
	// dir3
	ss = load(yamaha_path + "y_8_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 90;
	h = 90;
	imgmap_in[YamahaKin3_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[2]);
	imgmap_in[YamahaKin3_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[2]);
	// dir4
	ss = load(yamaha_path + "y_7_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin4_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[3]);
	imgmap_in[YamahaKin4_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[3]);
	// dir6
	ss = load(yamaha_path + "y_3_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 60;
	imgmap_in[YamahaKin6_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[4]);
	imgmap_in[YamahaKin6_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[4]);
	// dir7
	ss = load(yamaha_path + "y_6_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin7_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[5]);
	imgmap_in[YamahaKin7_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[5]);
	// dir8
	ss = load(yamaha_path + "y_5_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 45;
	h = 90;
	imgmap_in[YamahaKin8_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[6]);
	imgmap_in[YamahaKin8_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[6]);
	// dir9
	ss = load(yamaha_path + "y_4_kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 75;
	h = 75;
	imgmap_in[YamahaKin9_1Ni] = mkSG(ss, mkRect(w, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_2Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_3Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_4Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_5Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_7Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_8Ni] = mkSG(ss, mkRect(w*8, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_9Ni] = mkSG(ss, mkRect(w*9, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_10Ni] = mkSG(ss, mkRect(w*10, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_11Ni] = mkSG(ss, mkRect(w*11, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_12Ni] = mkSG(ss, mkRect(w*12, 0, w, h), pos[7]);
	imgmap_in[YamahaKin9_13Ni] = mkSG(ss, mkRect(w*13, 0, w, h), pos[7]);
      }
    }

    {
      string team_name = "";
      {
	// NTT-Meの読み込み
	//----------
	// い組
	//----------
	SmartSurface ss = load(ntt_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	int w = 30;
	int h = 50;
	Pos pos(15, 43);
	imgmap_in[NTTStand2I] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTStand1I] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTStand4I] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTStand7I] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTStand8I] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTStand9I] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTStand6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTStand3I] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTDash2I] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTDash1I] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTDash4I] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTDash7I] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTDash8I] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTDash9I] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTDash6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTDash3I] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "shot" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTWeapon2I] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTWeapon1I] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTWeapon4I] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTWeapon7I] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTWeapon8I] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTWeapon9I] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTWeapon6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTWeapon3I] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "attack_normal" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 50;
	h = 60;
	pos = Pos(w/2, h/2);
	imgmap_in[NTTAttack2I] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTAttack1I] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTAttack4I] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTAttack7I] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTAttack8I] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTAttack9I] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTAttack6I] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTAttack3I] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 200;
	h = 200;
	Pos kpos[8][6];
	kpos[0][0] = Pos(164, 52);
	kpos[0][1] = Pos(0, 1);
	kpos[0][2] = Pos(-1, 2);
	kpos[0][3] = Pos(-1, 1);
	kpos[0][4] = Pos(-1, 2);
	kpos[0][5] = Pos(-1, 4);
	kpos[1][0] = Pos(100, 47);
	kpos[1][1] = Pos(0, 0);
	kpos[1][2] = Pos(0, 1);
	kpos[1][3] = Pos(0, 1);
	kpos[1][4] = Pos(0, 1);
	kpos[1][5] = Pos(0, 1);
	kpos[2][0] = Pos(47, 65);
	kpos[2][1] = Pos(0, -3);
	kpos[2][2] = Pos(-1, -2);
	kpos[2][3] = Pos(-1, -3);
	kpos[2][4] = Pos(-1, -2);
	kpos[2][5] = Pos(-1, 0);
	kpos[3][0] = Pos(182, 115);
	kpos[3][1] = Pos(0, -3);
	kpos[3][2] = Pos(-1, -2);
	kpos[3][3] = Pos(-1, -3);
	kpos[3][4] = Pos(-1, -2);
	kpos[3][5] = Pos(-1, 0);
	kpos[4][0] = Pos(19, 117);
	kpos[4][1] = Pos(0, -3);
	kpos[4][2] = Pos(-1, -2);
	kpos[4][3] = Pos(-1, -3);
	kpos[4][4] = Pos(-1, -2);
	kpos[4][5] = Pos(-1, 0);
	kpos[5][0] = Pos(157, 173);
	kpos[5][1] = Pos(0, -3);
	kpos[5][2] = Pos(-1, -2);
	kpos[5][3] = Pos(-1, -3);
	kpos[5][4] = Pos(-1, -2);
	kpos[5][5] = Pos(-1, 0);
	kpos[6][0] = Pos(99, 197);
	kpos[6][1] = Pos(0, -3);
	kpos[6][2] = Pos(-1, -2);
	kpos[6][3] = Pos(-1, -3);
	kpos[6][4] = Pos(-1, -2);
	kpos[6][5] = Pos(-1, 0);
	kpos[7][0] = Pos(42, 167);
	kpos[7][1] = Pos(0, -3);
	kpos[7][2] = Pos(-1, -2);
	kpos[7][3] = Pos(-1, -3);
	kpos[7][4] = Pos(-1, -2);
	kpos[7][5] = Pos(-1, 0);

	imgmap_in[NTTKin1_1I] = mkSG(ss, mkRect(w, h*0, w, h), kpos[0][0]);
	imgmap_in[NTTKin1_2I] = mkSG(ss, mkRect(w, h*1, w, h), kpos[0][0] + kpos[0][1]);
	imgmap_in[NTTKin1_3I] = mkSG(ss, mkRect(w, h*2, w, h), kpos[0][0] + kpos[0][2]);
	imgmap_in[NTTKin1_4I] = mkSG(ss, mkRect(w, h*3, w, h), kpos[0][0] + kpos[0][3]);
	imgmap_in[NTTKin1_5I] = mkSG(ss, mkRect(w, h*4, w, h), kpos[0][0] + kpos[0][4]);
	imgmap_in[NTTKin1_6I] = mkSG(ss, mkRect(w, h*5, w, h), kpos[0][0] + kpos[0][5]);
	imgmap_in[NTTKin2_1I] = mkSG(ss, mkRect(0, h*0, w, h), kpos[1][0]);
	imgmap_in[NTTKin2_2I] = mkSG(ss, mkRect(0, h*1, w, h), kpos[1][0] + kpos[1][1]);
	imgmap_in[NTTKin2_3I] = mkSG(ss, mkRect(0, h*2, w, h), kpos[1][0] + kpos[1][2]);
	imgmap_in[NTTKin2_4I] = mkSG(ss, mkRect(0, h*3, w, h), kpos[1][0] + kpos[1][3]);
	imgmap_in[NTTKin2_5I] = mkSG(ss, mkRect(0, h*4, w, h), kpos[1][0] + kpos[1][4]);
	imgmap_in[NTTKin2_6I] = mkSG(ss, mkRect(0, h*5, w, h), kpos[1][0] + kpos[1][5]);
	imgmap_in[NTTKin3_1I] = mkSG(ss, mkRect(w*7, h*0, w, h), kpos[2][0]);
	imgmap_in[NTTKin3_2I] = mkSG(ss, mkRect(w*7, h*1, w, h), kpos[2][0] + kpos[2][1]);
	imgmap_in[NTTKin3_3I] = mkSG(ss, mkRect(w*7, h*2, w, h), kpos[2][0] + kpos[2][2]);
	imgmap_in[NTTKin3_4I] = mkSG(ss, mkRect(w*7, h*3, w, h), kpos[2][0] + kpos[2][3]);
	imgmap_in[NTTKin3_5I] = mkSG(ss, mkRect(w*7, h*4, w, h), kpos[2][0] + kpos[2][4]);
	imgmap_in[NTTKin3_6I] = mkSG(ss, mkRect(w*7, h*5, w, h), kpos[2][0] + kpos[2][5]);
	imgmap_in[NTTKin4_1I] = mkSG(ss, mkRect(w*2, h*0, w, h), kpos[3][0]);
	imgmap_in[NTTKin4_2I] = mkSG(ss, mkRect(w*2, h*1, w, h), kpos[3][0] + kpos[3][1]);
	imgmap_in[NTTKin4_3I] = mkSG(ss, mkRect(w*2, h*2, w, h), kpos[3][0] + kpos[3][2]);
	imgmap_in[NTTKin4_4I] = mkSG(ss, mkRect(w*2, h*3, w, h), kpos[3][0] + kpos[3][3]);
	imgmap_in[NTTKin4_5I] = mkSG(ss, mkRect(w*2, h*4, w, h), kpos[3][0] + kpos[3][4]);
	imgmap_in[NTTKin4_6I] = mkSG(ss, mkRect(w*2, h*5, w, h), kpos[3][0] + kpos[3][5]);
	imgmap_in[NTTKin6_1I] = mkSG(ss, mkRect(w*6, h*0, w, h), kpos[4][0]);
	imgmap_in[NTTKin6_2I] = mkSG(ss, mkRect(w*6, h*1, w, h), kpos[4][0] + kpos[4][1]);
	imgmap_in[NTTKin6_3I] = mkSG(ss, mkRect(w*6, h*2, w, h), kpos[4][0] + kpos[4][2]);
	imgmap_in[NTTKin6_4I] = mkSG(ss, mkRect(w*6, h*3, w, h), kpos[4][0] + kpos[4][3]);
	imgmap_in[NTTKin6_5I] = mkSG(ss, mkRect(w*6, h*4, w, h), kpos[4][0] + kpos[4][4]);
	imgmap_in[NTTKin6_6I] = mkSG(ss, mkRect(w*6, h*5, w, h), kpos[4][0] + kpos[4][5]);
	imgmap_in[NTTKin7_1I] = mkSG(ss, mkRect(w*3, h*0, w, h), kpos[5][0]);
	imgmap_in[NTTKin7_2I] = mkSG(ss, mkRect(w*3, h*1, w, h), kpos[5][0] + kpos[5][1]);
	imgmap_in[NTTKin7_3I] = mkSG(ss, mkRect(w*3, h*2, w, h), kpos[5][0] + kpos[5][2]);
	imgmap_in[NTTKin7_4I] = mkSG(ss, mkRect(w*3, h*3, w, h), kpos[5][0] + kpos[5][3]);
	imgmap_in[NTTKin7_5I] = mkSG(ss, mkRect(w*3, h*4, w, h), kpos[5][0] + kpos[5][4]);
	imgmap_in[NTTKin7_6I] = mkSG(ss, mkRect(w*3, h*5, w, h), kpos[5][0] + kpos[5][5]);
	imgmap_in[NTTKin8_1I] = mkSG(ss, mkRect(w*4, h*0, w, h), kpos[6][0]);
	imgmap_in[NTTKin8_2I] = mkSG(ss, mkRect(w*4, h*1, w, h), kpos[6][0] + kpos[6][1]);
	imgmap_in[NTTKin8_3I] = mkSG(ss, mkRect(w*4, h*2, w, h), kpos[6][0] + kpos[6][2]);
	imgmap_in[NTTKin8_4I] = mkSG(ss, mkRect(w*4, h*3, w, h), kpos[6][0] + kpos[6][3]);
	imgmap_in[NTTKin8_5I] = mkSG(ss, mkRect(w*4, h*4, w, h), kpos[6][0] + kpos[6][4]);
	imgmap_in[NTTKin8_6I] = mkSG(ss, mkRect(w*4, h*5, w, h), kpos[6][0] + kpos[6][5]);
	imgmap_in[NTTKin9_1I] = mkSG(ss, mkRect(w*5, h*0, w, h), kpos[7][0]);
	imgmap_in[NTTKin9_2I] = mkSG(ss, mkRect(w*5, h*1, w, h), kpos[7][0] + kpos[7][1]);
	imgmap_in[NTTKin9_3I] = mkSG(ss, mkRect(w*5, h*2, w, h), kpos[7][0] + kpos[7][2]);
	imgmap_in[NTTKin9_4I] = mkSG(ss, mkRect(w*5, h*3, w, h), kpos[7][0] + kpos[7][3]);
	imgmap_in[NTTKin9_5I] = mkSG(ss, mkRect(w*5, h*4, w, h), kpos[7][0] + kpos[7][4]);
	imgmap_in[NTTKin9_6I] = mkSG(ss, mkRect(w*5, h*5, w, h), kpos[7][0] + kpos[7][5]);
	ss = load(ntt_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 48;
	h = 24;
	imgmap_in[NTTDown6I] = mkSG(ss, mkRect(0, 0, w, h), Pos(28, 15));
	imgmap_in[NTTDown4I] = mkSG(ss, mkRect(w, 0, w, h), Pos(20, 15));
      }
      team_name = "_ro";
      {
	//----------
	// ろ組
	//----------
	SmartSurface ss = load(ntt_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	int w = 30;
	int h = 50;
	Pos pos(15, 43);
	imgmap_in[NTTStand2Ro] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTStand1Ro] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTStand4Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTStand7Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTStand8Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTStand9Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTStand6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTStand3Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTDash2Ro] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTDash1Ro] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTDash4Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTDash7Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTDash8Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTDash9Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTDash6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTDash3Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "shot" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTWeapon2Ro] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTWeapon1Ro] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTWeapon4Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTWeapon7Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTWeapon8Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTWeapon9Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTWeapon6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTWeapon3Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "attack_normal" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 50;
	h = 60;
	pos = Pos(w/2, h/2);
	imgmap_in[NTTAttack2Ro] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTAttack1Ro] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTAttack4Ro] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTAttack7Ro] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTAttack8Ro] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTAttack9Ro] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTAttack6Ro] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTAttack3Ro] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 200;
	h = 200;
	Pos kpos[8][6];
	kpos[0][0] = Pos(164, 52);
	kpos[0][1] = Pos(0, 1);
	kpos[0][2] = Pos(-1, 2);
	kpos[0][3] = Pos(-1, 1);
	kpos[0][4] = Pos(-1, 2);
	kpos[0][5] = Pos(-1, 4);
	kpos[1][0] = Pos(100, 47);
	kpos[1][1] = Pos(0, 0);
	kpos[1][2] = Pos(0, 1);
	kpos[1][3] = Pos(0, 1);
	kpos[1][4] = Pos(0, 1);
	kpos[1][5] = Pos(0, 1);
	kpos[2][0] = Pos(47, 65);
	kpos[2][1] = Pos(0, -3);
	kpos[2][2] = Pos(-1, -2);
	kpos[2][3] = Pos(-1, -3);
	kpos[2][4] = Pos(-1, -2);
	kpos[2][5] = Pos(-1, 0);
	kpos[3][0] = Pos(182, 115);
	kpos[3][1] = Pos(0, -3);
	kpos[3][2] = Pos(-1, -2);
	kpos[3][3] = Pos(-1, -3);
	kpos[3][4] = Pos(-1, -2);
	kpos[3][5] = Pos(-1, 0);
	kpos[4][0] = Pos(19, 117);
	kpos[4][1] = Pos(0, -3);
	kpos[4][2] = Pos(-1, -2);
	kpos[4][3] = Pos(-1, -3);
	kpos[4][4] = Pos(-1, -2);
	kpos[4][5] = Pos(-1, 0);
	kpos[5][0] = Pos(157, 173);
	kpos[5][1] = Pos(0, -3);
	kpos[5][2] = Pos(-1, -2);
	kpos[5][3] = Pos(-1, -3);
	kpos[5][4] = Pos(-1, -2);
	kpos[5][5] = Pos(-1, 0);
	kpos[6][0] = Pos(99, 197);
	kpos[6][1] = Pos(0, -3);
	kpos[6][2] = Pos(-1, -2);
	kpos[6][3] = Pos(-1, -3);
	kpos[6][4] = Pos(-1, -2);
	kpos[6][5] = Pos(-1, 0);
	kpos[7][0] = Pos(42, 167);
	kpos[7][1] = Pos(0, -3);
	kpos[7][2] = Pos(-1, -2);
	kpos[7][3] = Pos(-1, -3);
	kpos[7][4] = Pos(-1, -2);
	kpos[7][5] = Pos(-1, 0);

	imgmap_in[NTTKin1_1Ro] = mkSG(ss, mkRect(w, h*0, w, h), kpos[0][0]);
	imgmap_in[NTTKin1_2Ro] = mkSG(ss, mkRect(w, h*1, w, h), kpos[0][0] + kpos[0][1]);
	imgmap_in[NTTKin1_3Ro] = mkSG(ss, mkRect(w, h*2, w, h), kpos[0][0] + kpos[0][2]);
	imgmap_in[NTTKin1_4Ro] = mkSG(ss, mkRect(w, h*3, w, h), kpos[0][0] + kpos[0][3]);
	imgmap_in[NTTKin1_5Ro] = mkSG(ss, mkRect(w, h*4, w, h), kpos[0][0] + kpos[0][4]);
	imgmap_in[NTTKin1_6Ro] = mkSG(ss, mkRect(w, h*5, w, h), kpos[0][0] + kpos[0][5]);
	imgmap_in[NTTKin2_1Ro] = mkSG(ss, mkRect(0, h*0, w, h), kpos[1][0]);
	imgmap_in[NTTKin2_2Ro] = mkSG(ss, mkRect(0, h*1, w, h), kpos[1][0] + kpos[1][1]);
	imgmap_in[NTTKin2_3Ro] = mkSG(ss, mkRect(0, h*2, w, h), kpos[1][0] + kpos[1][2]);
	imgmap_in[NTTKin2_4Ro] = mkSG(ss, mkRect(0, h*3, w, h), kpos[1][0] + kpos[1][3]);
	imgmap_in[NTTKin2_5Ro] = mkSG(ss, mkRect(0, h*4, w, h), kpos[1][0] + kpos[1][4]);
	imgmap_in[NTTKin2_6Ro] = mkSG(ss, mkRect(0, h*5, w, h), kpos[1][0] + kpos[1][5]);
	imgmap_in[NTTKin3_1Ro] = mkSG(ss, mkRect(w*7, h*0, w, h), kpos[2][0]);
	imgmap_in[NTTKin3_2Ro] = mkSG(ss, mkRect(w*7, h*1, w, h), kpos[2][0] + kpos[2][1]);
	imgmap_in[NTTKin3_3Ro] = mkSG(ss, mkRect(w*7, h*2, w, h), kpos[2][0] + kpos[2][2]);
	imgmap_in[NTTKin3_4Ro] = mkSG(ss, mkRect(w*7, h*3, w, h), kpos[2][0] + kpos[2][3]);
	imgmap_in[NTTKin3_5Ro] = mkSG(ss, mkRect(w*7, h*4, w, h), kpos[2][0] + kpos[2][4]);
	imgmap_in[NTTKin3_6Ro] = mkSG(ss, mkRect(w*7, h*5, w, h), kpos[2][0] + kpos[2][5]);
	imgmap_in[NTTKin4_1Ro] = mkSG(ss, mkRect(w*2, h*0, w, h), kpos[3][0]);
	imgmap_in[NTTKin4_2Ro] = mkSG(ss, mkRect(w*2, h*1, w, h), kpos[3][0] + kpos[3][1]);
	imgmap_in[NTTKin4_3Ro] = mkSG(ss, mkRect(w*2, h*2, w, h), kpos[3][0] + kpos[3][2]);
	imgmap_in[NTTKin4_4Ro] = mkSG(ss, mkRect(w*2, h*3, w, h), kpos[3][0] + kpos[3][3]);
	imgmap_in[NTTKin4_5Ro] = mkSG(ss, mkRect(w*2, h*4, w, h), kpos[3][0] + kpos[3][4]);
	imgmap_in[NTTKin4_6Ro] = mkSG(ss, mkRect(w*2, h*5, w, h), kpos[3][0] + kpos[3][5]);
	imgmap_in[NTTKin6_1Ro] = mkSG(ss, mkRect(w*6, h*0, w, h), kpos[4][0]);
	imgmap_in[NTTKin6_2Ro] = mkSG(ss, mkRect(w*6, h*1, w, h), kpos[4][0] + kpos[4][1]);
	imgmap_in[NTTKin6_3Ro] = mkSG(ss, mkRect(w*6, h*2, w, h), kpos[4][0] + kpos[4][2]);
	imgmap_in[NTTKin6_4Ro] = mkSG(ss, mkRect(w*6, h*3, w, h), kpos[4][0] + kpos[4][3]);
	imgmap_in[NTTKin6_5Ro] = mkSG(ss, mkRect(w*6, h*4, w, h), kpos[4][0] + kpos[4][4]);
	imgmap_in[NTTKin6_6Ro] = mkSG(ss, mkRect(w*6, h*5, w, h), kpos[4][0] + kpos[4][5]);
	imgmap_in[NTTKin7_1Ro] = mkSG(ss, mkRect(w*3, h*0, w, h), kpos[5][0]);
	imgmap_in[NTTKin7_2Ro] = mkSG(ss, mkRect(w*3, h*1, w, h), kpos[5][0] + kpos[5][1]);
	imgmap_in[NTTKin7_3Ro] = mkSG(ss, mkRect(w*3, h*2, w, h), kpos[5][0] + kpos[5][2]);
	imgmap_in[NTTKin7_4Ro] = mkSG(ss, mkRect(w*3, h*3, w, h), kpos[5][0] + kpos[5][3]);
	imgmap_in[NTTKin7_5Ro] = mkSG(ss, mkRect(w*3, h*4, w, h), kpos[5][0] + kpos[5][4]);
	imgmap_in[NTTKin7_6Ro] = mkSG(ss, mkRect(w*3, h*5, w, h), kpos[5][0] + kpos[5][5]);
	imgmap_in[NTTKin8_1Ro] = mkSG(ss, mkRect(w*4, h*0, w, h), kpos[6][0]);
	imgmap_in[NTTKin8_2Ro] = mkSG(ss, mkRect(w*4, h*1, w, h), kpos[6][0] + kpos[6][1]);
	imgmap_in[NTTKin8_3Ro] = mkSG(ss, mkRect(w*4, h*2, w, h), kpos[6][0] + kpos[6][2]);
	imgmap_in[NTTKin8_4Ro] = mkSG(ss, mkRect(w*4, h*3, w, h), kpos[6][0] + kpos[6][3]);
	imgmap_in[NTTKin8_5Ro] = mkSG(ss, mkRect(w*4, h*4, w, h), kpos[6][0] + kpos[6][4]);
	imgmap_in[NTTKin8_6Ro] = mkSG(ss, mkRect(w*4, h*5, w, h), kpos[6][0] + kpos[6][5]);
	imgmap_in[NTTKin9_1Ro] = mkSG(ss, mkRect(w*5, h*0, w, h), kpos[7][0]);
	imgmap_in[NTTKin9_2Ro] = mkSG(ss, mkRect(w*5, h*1, w, h), kpos[7][0] + kpos[7][1]);
	imgmap_in[NTTKin9_3Ro] = mkSG(ss, mkRect(w*5, h*2, w, h), kpos[7][0] + kpos[7][2]);
	imgmap_in[NTTKin9_4Ro] = mkSG(ss, mkRect(w*5, h*3, w, h), kpos[7][0] + kpos[7][3]);
	imgmap_in[NTTKin9_5Ro] = mkSG(ss, mkRect(w*5, h*4, w, h), kpos[7][0] + kpos[7][4]);
	imgmap_in[NTTKin9_6Ro] = mkSG(ss, mkRect(w*5, h*5, w, h), kpos[7][0] + kpos[7][5]);
	ss = load(ntt_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 48;
	h = 24;
	imgmap_in[NTTDown6Ro] = mkSG(ss, mkRect(0, 0, w, h), Pos(28, 15));
	imgmap_in[NTTDown4Ro] = mkSG(ss, mkRect(w, 0, w, h), Pos(20, 15));
      }
      team_name = "_ha";
      {
	//----------
	// に組
	//----------
	SmartSurface ss = load(ntt_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	int w = 30;
	int h = 50;
	Pos pos(15, 43);
	imgmap_in[NTTStand2Ha] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTStand1Ha] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTStand4Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTStand7Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTStand8Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTStand9Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTStand6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTStand3Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTDash2Ha] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTDash1Ha] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTDash4Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTDash7Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTDash8Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTDash9Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTDash6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTDash3Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "shot" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTWeapon2Ha] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTWeapon1Ha] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTWeapon4Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTWeapon7Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTWeapon8Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTWeapon9Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTWeapon6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTWeapon3Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "attack_normal" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 50;
	h = 60;
	pos = Pos(w/2, h/2);
	imgmap_in[NTTAttack2Ha] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTAttack1Ha] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTAttack4Ha] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTAttack7Ha] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTAttack8Ha] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTAttack9Ha] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTAttack6Ha] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTAttack3Ha] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 200;
	h = 200;
	Pos kpos[8][6];
	kpos[0][0] = Pos(164, 52);
	kpos[0][1] = Pos(0, 1);
	kpos[0][2] = Pos(-1, 2);
	kpos[0][3] = Pos(-1, 1);
	kpos[0][4] = Pos(-1, 2);
	kpos[0][5] = Pos(-1, 4);
	kpos[1][0] = Pos(100, 47);
	kpos[1][1] = Pos(0, 0);
	kpos[1][2] = Pos(0, 1);
	kpos[1][3] = Pos(0, 1);
	kpos[1][4] = Pos(0, 1);
	kpos[1][5] = Pos(0, 1);
	kpos[2][0] = Pos(47, 65);
	kpos[2][1] = Pos(0, -3);
	kpos[2][2] = Pos(-1, -2);
	kpos[2][3] = Pos(-1, -3);
	kpos[2][4] = Pos(-1, -2);
	kpos[2][5] = Pos(-1, 0);
	kpos[3][0] = Pos(182, 115);
	kpos[3][1] = Pos(0, -3);
	kpos[3][2] = Pos(-1, -2);
	kpos[3][3] = Pos(-1, -3);
	kpos[3][4] = Pos(-1, -2);
	kpos[3][5] = Pos(-1, 0);
	kpos[4][0] = Pos(19, 117);
	kpos[4][1] = Pos(0, -3);
	kpos[4][2] = Pos(-1, -2);
	kpos[4][3] = Pos(-1, -3);
	kpos[4][4] = Pos(-1, -2);
	kpos[4][5] = Pos(-1, 0);
	kpos[5][0] = Pos(157, 173);
	kpos[5][1] = Pos(0, -3);
	kpos[5][2] = Pos(-1, -2);
	kpos[5][3] = Pos(-1, -3);
	kpos[5][4] = Pos(-1, -2);
	kpos[5][5] = Pos(-1, 0);
	kpos[6][0] = Pos(99, 197);
	kpos[6][1] = Pos(0, -3);
	kpos[6][2] = Pos(-1, -2);
	kpos[6][3] = Pos(-1, -3);
	kpos[6][4] = Pos(-1, -2);
	kpos[6][5] = Pos(-1, 0);
	kpos[7][0] = Pos(42, 167);
	kpos[7][1] = Pos(0, -3);
	kpos[7][2] = Pos(-1, -2);
	kpos[7][3] = Pos(-1, -3);
	kpos[7][4] = Pos(-1, -2);
	kpos[7][5] = Pos(-1, 0);

	imgmap_in[NTTKin1_1Ha] = mkSG(ss, mkRect(w, h*0, w, h), kpos[0][0]);
	imgmap_in[NTTKin1_2Ha] = mkSG(ss, mkRect(w, h*1, w, h), kpos[0][0] + kpos[0][1]);
	imgmap_in[NTTKin1_3Ha] = mkSG(ss, mkRect(w, h*2, w, h), kpos[0][0] + kpos[0][2]);
	imgmap_in[NTTKin1_4Ha] = mkSG(ss, mkRect(w, h*3, w, h), kpos[0][0] + kpos[0][3]);
	imgmap_in[NTTKin1_5Ha] = mkSG(ss, mkRect(w, h*4, w, h), kpos[0][0] + kpos[0][4]);
	imgmap_in[NTTKin1_6Ha] = mkSG(ss, mkRect(w, h*5, w, h), kpos[0][0] + kpos[0][5]);
	imgmap_in[NTTKin2_1Ha] = mkSG(ss, mkRect(0, h*0, w, h), kpos[1][0]);
	imgmap_in[NTTKin2_2Ha] = mkSG(ss, mkRect(0, h*1, w, h), kpos[1][0] + kpos[1][1]);
	imgmap_in[NTTKin2_3Ha] = mkSG(ss, mkRect(0, h*2, w, h), kpos[1][0] + kpos[1][2]);
	imgmap_in[NTTKin2_4Ha] = mkSG(ss, mkRect(0, h*3, w, h), kpos[1][0] + kpos[1][3]);
	imgmap_in[NTTKin2_5Ha] = mkSG(ss, mkRect(0, h*4, w, h), kpos[1][0] + kpos[1][4]);
	imgmap_in[NTTKin2_6Ha] = mkSG(ss, mkRect(0, h*5, w, h), kpos[1][0] + kpos[1][5]);
	imgmap_in[NTTKin3_1Ha] = mkSG(ss, mkRect(w*7, h*0, w, h), kpos[2][0]);
	imgmap_in[NTTKin3_2Ha] = mkSG(ss, mkRect(w*7, h*1, w, h), kpos[2][0] + kpos[2][1]);
	imgmap_in[NTTKin3_3Ha] = mkSG(ss, mkRect(w*7, h*2, w, h), kpos[2][0] + kpos[2][2]);
	imgmap_in[NTTKin3_4Ha] = mkSG(ss, mkRect(w*7, h*3, w, h), kpos[2][0] + kpos[2][3]);
	imgmap_in[NTTKin3_5Ha] = mkSG(ss, mkRect(w*7, h*4, w, h), kpos[2][0] + kpos[2][4]);
	imgmap_in[NTTKin3_6Ha] = mkSG(ss, mkRect(w*7, h*5, w, h), kpos[2][0] + kpos[2][5]);
	imgmap_in[NTTKin4_1Ha] = mkSG(ss, mkRect(w*2, h*0, w, h), kpos[3][0]);
	imgmap_in[NTTKin4_2Ha] = mkSG(ss, mkRect(w*2, h*1, w, h), kpos[3][0] + kpos[3][1]);
	imgmap_in[NTTKin4_3Ha] = mkSG(ss, mkRect(w*2, h*2, w, h), kpos[3][0] + kpos[3][2]);
	imgmap_in[NTTKin4_4Ha] = mkSG(ss, mkRect(w*2, h*3, w, h), kpos[3][0] + kpos[3][3]);
	imgmap_in[NTTKin4_5Ha] = mkSG(ss, mkRect(w*2, h*4, w, h), kpos[3][0] + kpos[3][4]);
	imgmap_in[NTTKin4_6Ha] = mkSG(ss, mkRect(w*2, h*5, w, h), kpos[3][0] + kpos[3][5]);
	imgmap_in[NTTKin6_1Ha] = mkSG(ss, mkRect(w*6, h*0, w, h), kpos[4][0]);
	imgmap_in[NTTKin6_2Ha] = mkSG(ss, mkRect(w*6, h*1, w, h), kpos[4][0] + kpos[4][1]);
	imgmap_in[NTTKin6_3Ha] = mkSG(ss, mkRect(w*6, h*2, w, h), kpos[4][0] + kpos[4][2]);
	imgmap_in[NTTKin6_4Ha] = mkSG(ss, mkRect(w*6, h*3, w, h), kpos[4][0] + kpos[4][3]);
	imgmap_in[NTTKin6_5Ha] = mkSG(ss, mkRect(w*6, h*4, w, h), kpos[4][0] + kpos[4][4]);
	imgmap_in[NTTKin6_6Ha] = mkSG(ss, mkRect(w*6, h*5, w, h), kpos[4][0] + kpos[4][5]);
	imgmap_in[NTTKin7_1Ha] = mkSG(ss, mkRect(w*3, h*0, w, h), kpos[5][0]);
	imgmap_in[NTTKin7_2Ha] = mkSG(ss, mkRect(w*3, h*1, w, h), kpos[5][0] + kpos[5][1]);
	imgmap_in[NTTKin7_3Ha] = mkSG(ss, mkRect(w*3, h*2, w, h), kpos[5][0] + kpos[5][2]);
	imgmap_in[NTTKin7_4Ha] = mkSG(ss, mkRect(w*3, h*3, w, h), kpos[5][0] + kpos[5][3]);
	imgmap_in[NTTKin7_5Ha] = mkSG(ss, mkRect(w*3, h*4, w, h), kpos[5][0] + kpos[5][4]);
	imgmap_in[NTTKin7_6Ha] = mkSG(ss, mkRect(w*3, h*5, w, h), kpos[5][0] + kpos[5][5]);
	imgmap_in[NTTKin8_1Ha] = mkSG(ss, mkRect(w*4, h*0, w, h), kpos[6][0]);
	imgmap_in[NTTKin8_2Ha] = mkSG(ss, mkRect(w*4, h*1, w, h), kpos[6][0] + kpos[6][1]);
	imgmap_in[NTTKin8_3Ha] = mkSG(ss, mkRect(w*4, h*2, w, h), kpos[6][0] + kpos[6][2]);
	imgmap_in[NTTKin8_4Ha] = mkSG(ss, mkRect(w*4, h*3, w, h), kpos[6][0] + kpos[6][3]);
	imgmap_in[NTTKin8_5Ha] = mkSG(ss, mkRect(w*4, h*4, w, h), kpos[6][0] + kpos[6][4]);
	imgmap_in[NTTKin8_6Ha] = mkSG(ss, mkRect(w*4, h*5, w, h), kpos[6][0] + kpos[6][5]);
	imgmap_in[NTTKin9_1Ha] = mkSG(ss, mkRect(w*5, h*0, w, h), kpos[7][0]);
	imgmap_in[NTTKin9_2Ha] = mkSG(ss, mkRect(w*5, h*1, w, h), kpos[7][0] + kpos[7][1]);
	imgmap_in[NTTKin9_3Ha] = mkSG(ss, mkRect(w*5, h*2, w, h), kpos[7][0] + kpos[7][2]);
	imgmap_in[NTTKin9_4Ha] = mkSG(ss, mkRect(w*5, h*3, w, h), kpos[7][0] + kpos[7][3]);
	imgmap_in[NTTKin9_5Ha] = mkSG(ss, mkRect(w*5, h*4, w, h), kpos[7][0] + kpos[7][4]);
	imgmap_in[NTTKin9_6Ha] = mkSG(ss, mkRect(w*5, h*5, w, h), kpos[7][0] + kpos[7][5]);
	ss = load(ntt_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 48;
	h = 24;
	imgmap_in[NTTDown6Ha] = mkSG(ss, mkRect(0, 0, w, h), Pos(28, 15));
	imgmap_in[NTTDown4Ha] = mkSG(ss, mkRect(w, 0, w, h), Pos(20, 15));
      }
      team_name = "_ni";
      {
	// NTT-Meの読み込み
	SmartSurface ss = load(ntt_path + "stand" + team_name + ".png");
	ss.SwapFormatAlpha();
	int w = 30;
	int h = 50;
	Pos pos(15, 43);
	imgmap_in[NTTStand2Ni] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTStand1Ni] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTStand4Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTStand7Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTStand8Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTStand9Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTStand6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTStand3Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "dash" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTDash2Ni] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTDash1Ni] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTDash4Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTDash7Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTDash8Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTDash9Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTDash6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTDash3Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "shot" + team_name + ".png");
	ss.SwapFormatAlpha();
	imgmap_in[NTTWeapon2Ni] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTWeapon1Ni] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTWeapon4Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTWeapon7Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTWeapon8Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTWeapon9Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTWeapon6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTWeapon3Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "attack_normal" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 50;
	h = 60;
	pos = Pos(w/2, h/2);
	imgmap_in[NTTAttack2Ni] = mkSG(ss, mkRect(w*0, 0, w, h), pos);
	imgmap_in[NTTAttack1Ni] = mkSG(ss, mkRect(w*1, 0, w, h), pos);
	imgmap_in[NTTAttack4Ni] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
	imgmap_in[NTTAttack7Ni] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
	imgmap_in[NTTAttack8Ni] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
	imgmap_in[NTTAttack9Ni] = mkSG(ss, mkRect(w*5, 0, w, h), pos);
	imgmap_in[NTTAttack6Ni] = mkSG(ss, mkRect(w*6, 0, w, h), pos);
	imgmap_in[NTTAttack3Ni] = mkSG(ss, mkRect(w*7, 0, w, h), pos);
	ss = load(ntt_path + "kin" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 200;
	h = 200;
	Pos kpos[8][6];
	kpos[0][0] = Pos(164, 52);
	kpos[0][1] = Pos(0, 1);
	kpos[0][2] = Pos(-1, 2);
	kpos[0][3] = Pos(-1, 1);
	kpos[0][4] = Pos(-1, 2);
	kpos[0][5] = Pos(-1, 4);
	kpos[1][0] = Pos(100, 47);
	kpos[1][1] = Pos(0, 0);
	kpos[1][2] = Pos(0, 1);
	kpos[1][3] = Pos(0, 1);
	kpos[1][4] = Pos(0, 1);
	kpos[1][5] = Pos(0, 1);
	kpos[2][0] = Pos(47, 65);
	kpos[2][1] = Pos(0, -3);
	kpos[2][2] = Pos(-1, -2);
	kpos[2][3] = Pos(-1, -3);
	kpos[2][4] = Pos(-1, -2);
	kpos[2][5] = Pos(-1, 0);
	kpos[3][0] = Pos(182, 115);
	kpos[3][1] = Pos(0, -3);
	kpos[3][2] = Pos(-1, -2);
	kpos[3][3] = Pos(-1, -3);
	kpos[3][4] = Pos(-1, -2);
	kpos[3][5] = Pos(-1, 0);
	kpos[4][0] = Pos(19, 117);
	kpos[4][1] = Pos(0, -3);
	kpos[4][2] = Pos(-1, -2);
	kpos[4][3] = Pos(-1, -3);
	kpos[4][4] = Pos(-1, -2);
	kpos[4][5] = Pos(-1, 0);
	kpos[5][0] = Pos(157, 173);
	kpos[5][1] = Pos(0, -3);
	kpos[5][2] = Pos(-1, -2);
	kpos[5][3] = Pos(-1, -3);
	kpos[5][4] = Pos(-1, -2);
	kpos[5][5] = Pos(-1, 0);
	kpos[6][0] = Pos(99, 197);
	kpos[6][1] = Pos(0, -3);
	kpos[6][2] = Pos(-1, -2);
	kpos[6][3] = Pos(-1, -3);
	kpos[6][4] = Pos(-1, -2);
	kpos[6][5] = Pos(-1, 0);
	kpos[7][0] = Pos(42, 167);
	kpos[7][1] = Pos(0, -3);
	kpos[7][2] = Pos(-1, -2);
	kpos[7][3] = Pos(-1, -3);
	kpos[7][4] = Pos(-1, -2);
	kpos[7][5] = Pos(-1, 0);

	imgmap_in[NTTKin1_1Ni] = mkSG(ss, mkRect(w, h*0, w, h), kpos[0][0]);
	imgmap_in[NTTKin1_2Ni] = mkSG(ss, mkRect(w, h*1, w, h), kpos[0][0] + kpos[0][1]);
	imgmap_in[NTTKin1_3Ni] = mkSG(ss, mkRect(w, h*2, w, h), kpos[0][0] + kpos[0][2]);
	imgmap_in[NTTKin1_4Ni] = mkSG(ss, mkRect(w, h*3, w, h), kpos[0][0] + kpos[0][3]);
	imgmap_in[NTTKin1_5Ni] = mkSG(ss, mkRect(w, h*4, w, h), kpos[0][0] + kpos[0][4]);
	imgmap_in[NTTKin1_6Ni] = mkSG(ss, mkRect(w, h*5, w, h), kpos[0][0] + kpos[0][5]);
	imgmap_in[NTTKin2_1Ni] = mkSG(ss, mkRect(0, h*0, w, h), kpos[1][0]);
	imgmap_in[NTTKin2_2Ni] = mkSG(ss, mkRect(0, h*1, w, h), kpos[1][0] + kpos[1][1]);
	imgmap_in[NTTKin2_3Ni] = mkSG(ss, mkRect(0, h*2, w, h), kpos[1][0] + kpos[1][2]);
	imgmap_in[NTTKin2_4Ni] = mkSG(ss, mkRect(0, h*3, w, h), kpos[1][0] + kpos[1][3]);
	imgmap_in[NTTKin2_5Ni] = mkSG(ss, mkRect(0, h*4, w, h), kpos[1][0] + kpos[1][4]);
	imgmap_in[NTTKin2_6Ni] = mkSG(ss, mkRect(0, h*5, w, h), kpos[1][0] + kpos[1][5]);
	imgmap_in[NTTKin3_1Ni] = mkSG(ss, mkRect(w*7, h*0, w, h), kpos[2][0]);
	imgmap_in[NTTKin3_2Ni] = mkSG(ss, mkRect(w*7, h*1, w, h), kpos[2][0] + kpos[2][1]);
	imgmap_in[NTTKin3_3Ni] = mkSG(ss, mkRect(w*7, h*2, w, h), kpos[2][0] + kpos[2][2]);
	imgmap_in[NTTKin3_4Ni] = mkSG(ss, mkRect(w*7, h*3, w, h), kpos[2][0] + kpos[2][3]);
	imgmap_in[NTTKin3_5Ni] = mkSG(ss, mkRect(w*7, h*4, w, h), kpos[2][0] + kpos[2][4]);
	imgmap_in[NTTKin3_6Ni] = mkSG(ss, mkRect(w*7, h*5, w, h), kpos[2][0] + kpos[2][5]);
	imgmap_in[NTTKin4_1Ni] = mkSG(ss, mkRect(w*2, h*0, w, h), kpos[3][0]);
	imgmap_in[NTTKin4_2Ni] = mkSG(ss, mkRect(w*2, h*1, w, h), kpos[3][0] + kpos[3][1]);
	imgmap_in[NTTKin4_3Ni] = mkSG(ss, mkRect(w*2, h*2, w, h), kpos[3][0] + kpos[3][2]);
	imgmap_in[NTTKin4_4Ni] = mkSG(ss, mkRect(w*2, h*3, w, h), kpos[3][0] + kpos[3][3]);
	imgmap_in[NTTKin4_5Ni] = mkSG(ss, mkRect(w*2, h*4, w, h), kpos[3][0] + kpos[3][4]);
	imgmap_in[NTTKin4_6Ni] = mkSG(ss, mkRect(w*2, h*5, w, h), kpos[3][0] + kpos[3][5]);
	imgmap_in[NTTKin6_1Ni] = mkSG(ss, mkRect(w*6, h*0, w, h), kpos[4][0]);
	imgmap_in[NTTKin6_2Ni] = mkSG(ss, mkRect(w*6, h*1, w, h), kpos[4][0] + kpos[4][1]);
	imgmap_in[NTTKin6_3Ni] = mkSG(ss, mkRect(w*6, h*2, w, h), kpos[4][0] + kpos[4][2]);
	imgmap_in[NTTKin6_4Ni] = mkSG(ss, mkRect(w*6, h*3, w, h), kpos[4][0] + kpos[4][3]);
	imgmap_in[NTTKin6_5Ni] = mkSG(ss, mkRect(w*6, h*4, w, h), kpos[4][0] + kpos[4][4]);
	imgmap_in[NTTKin6_6Ni] = mkSG(ss, mkRect(w*6, h*5, w, h), kpos[4][0] + kpos[4][5]);
	imgmap_in[NTTKin7_1Ni] = mkSG(ss, mkRect(w*3, h*0, w, h), kpos[5][0]);
	imgmap_in[NTTKin7_2Ni] = mkSG(ss, mkRect(w*3, h*1, w, h), kpos[5][0] + kpos[5][1]);
	imgmap_in[NTTKin7_3Ni] = mkSG(ss, mkRect(w*3, h*2, w, h), kpos[5][0] + kpos[5][2]);
	imgmap_in[NTTKin7_4Ni] = mkSG(ss, mkRect(w*3, h*3, w, h), kpos[5][0] + kpos[5][3]);
	imgmap_in[NTTKin7_5Ni] = mkSG(ss, mkRect(w*3, h*4, w, h), kpos[5][0] + kpos[5][4]);
	imgmap_in[NTTKin7_6Ni] = mkSG(ss, mkRect(w*3, h*5, w, h), kpos[5][0] + kpos[5][5]);
	imgmap_in[NTTKin8_1Ni] = mkSG(ss, mkRect(w*4, h*0, w, h), kpos[6][0]);
	imgmap_in[NTTKin8_2Ni] = mkSG(ss, mkRect(w*4, h*1, w, h), kpos[6][0] + kpos[6][1]);
	imgmap_in[NTTKin8_3Ni] = mkSG(ss, mkRect(w*4, h*2, w, h), kpos[6][0] + kpos[6][2]);
	imgmap_in[NTTKin8_4Ni] = mkSG(ss, mkRect(w*4, h*3, w, h), kpos[6][0] + kpos[6][3]);
	imgmap_in[NTTKin8_5Ni] = mkSG(ss, mkRect(w*4, h*4, w, h), kpos[6][0] + kpos[6][4]);
	imgmap_in[NTTKin8_6Ni] = mkSG(ss, mkRect(w*4, h*5, w, h), kpos[6][0] + kpos[6][5]);
	imgmap_in[NTTKin9_1Ni] = mkSG(ss, mkRect(w*5, h*0, w, h), kpos[7][0]);
	imgmap_in[NTTKin9_2Ni] = mkSG(ss, mkRect(w*5, h*1, w, h), kpos[7][0] + kpos[7][1]);
	imgmap_in[NTTKin9_3Ni] = mkSG(ss, mkRect(w*5, h*2, w, h), kpos[7][0] + kpos[7][2]);
	imgmap_in[NTTKin9_4Ni] = mkSG(ss, mkRect(w*5, h*3, w, h), kpos[7][0] + kpos[7][3]);
	imgmap_in[NTTKin9_5Ni] = mkSG(ss, mkRect(w*5, h*4, w, h), kpos[7][0] + kpos[7][4]);
	imgmap_in[NTTKin9_6Ni] = mkSG(ss, mkRect(w*5, h*5, w, h), kpos[7][0] + kpos[7][5]);
	ss = load(ntt_path + "down" + team_name + ".png");
	ss.SwapFormatAlpha();
	w = 48;
	h = 24;
	imgmap_in[NTTDown6Ni] = mkSG(ss, mkRect(0, 0, w, h), Pos(28, 15));
	imgmap_in[NTTDown4Ni] = mkSG(ss, mkRect(w, 0, w, h), Pos(20, 15));
      }
    }

    { // Planexのミサイル読み込み
      const Pos pos(16, 16);
      const unsigned w = 32;
      const unsigned h = 32;
      SmartSurface temp = load(effect_path + "missile.png");
      temp.SwapFormatAlpha();
      imgmap_in[PlanexMissile1]  = mkSG(temp, mkRect(0, 0, w, h), pos);
      imgmap_in[PlanexMissile12] = mkSG(temp, mkRect(w, 0, w, h), pos);
      imgmap_in[PlanexMissile2]  = mkSG(temp, mkRect(w*2, 0, w, h), pos);
      imgmap_in[PlanexMissile23] = mkSG(temp, mkRect(w*3, 0, w, h), pos);
      imgmap_in[PlanexMissile3]  = mkSG(temp, mkRect(0, h, w, h), pos);
      imgmap_in[PlanexMissile36] = mkSG(temp, mkRect(w, h, w, h), pos);
      imgmap_in[PlanexMissile6]  = mkSG(temp, mkRect(w*2, h, w, h), pos);
      imgmap_in[PlanexMissile69] = mkSG(temp, mkRect(w*3, h, w, h), pos);
      imgmap_in[PlanexMissile9]  = mkSG(temp, mkRect(0, h*2, w, h), pos);
      imgmap_in[PlanexMissile89] = mkSG(temp, mkRect(w, h*2, w, h), pos);
      imgmap_in[PlanexMissile8]  = mkSG(temp, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[PlanexMissile78] = mkSG(temp, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[PlanexMissile7]  = mkSG(temp, mkRect(0, h*3, w, h), pos);
      imgmap_in[PlanexMissile47] = mkSG(temp, mkRect(w, h*3, w, h), pos);
      imgmap_in[PlanexMissile4]  = mkSG(temp, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[PlanexMissile14] = mkSG(temp, mkRect(w*3, h*3, w, h), pos);
    }

    {
      unsigned w, h;
      w = h = 24;
      const Pos pos(w / 2 , h / 2);
      SmartSurface ss = load(effect_path + "kirai_i.png");
      ss.SwapFormatAlpha();
      imgmap_in[YamahaBombI] = mkSG(ss, mkRect(0, 0, ss->w, ss->h), pos);
      ss = load(effect_path + "kirai_ro.png");
      ss.SwapFormatAlpha();
      imgmap_in[YamahaBombRo] = mkSG(ss, mkRect(0, 0, ss->w, ss->h), pos);
      ss = load(effect_path + "kirai_ha.png");
      ss.SwapFormatAlpha();
      imgmap_in[YamahaBombHa] = mkSG(ss, mkRect(0, 0, ss->w, ss->h), pos);
      ss = load(effect_path + "kirai_ni.png");
      ss.SwapFormatAlpha();
      imgmap_in[YamahaBombNi] = mkSG(ss, mkRect(0, 0, ss->w, ss->h), pos);
    }

    { // 煙の読み込み
      unsigned w, h;
      w = h = 32;
      const Pos pos(w / 2, h / 2);
      SmartSurface temp = load(effect_path + "kemuri.png");
      temp.SwapFormatAlpha();
      imgmap_in[SmokeA0] = mkSG(temp, mkRect(0, 0, w, h), pos);
      imgmap_in[SmokeA1] = mkSG(temp, mkRect(w, 0, w, h), pos);
      imgmap_in[SmokeA2] = mkSG(temp, mkRect(w*2, 0, w, h), pos);
      imgmap_in[SmokeA3] = mkSG(temp, mkRect(w*3, 0, w, h), pos);
    }

    { // Planexのエネルギー弾の読み込み
      const unsigned w = 50;
      const unsigned h = 50;
      const Pos pos(w / 2, h / 2);
      SmartSurface temp = load(effect_path + "ene.png");
      temp.SwapFormatAlpha();
      imgmap_in[PlanexEnergyShot1] = mkSG(temp, mkRect(0, 0, w, h), pos);
      imgmap_in[PlanexEnergyShot2] = mkSG(temp, mkRect(w, 0, w, h), pos);
      imgmap_in[PlanexEnergyShot3] = mkSG(temp, mkRect(w*2, 0, w, h), pos);
      imgmap_in[PlanexEnergyShot6] = mkSG(temp, mkRect(w*3, 0, w, h), pos);
      imgmap_in[PlanexEnergyShot9] = mkSG(temp, mkRect(0, h, w, h), pos);
      imgmap_in[PlanexEnergyShot8] = mkSG(temp, mkRect(w, h, w, h), pos);
      imgmap_in[PlanexEnergyShot7] = mkSG(temp, mkRect(w*2, h, w, h), pos);
      imgmap_in[PlanexEnergyShot4] = mkSG(temp, mkRect(w*3, h, w, h), pos);
      imgmap_in[PlanexEnergyShot12] = mkSG(temp, mkRect(0, h*2, w, h), pos);
      imgmap_in[PlanexEnergyShot23] = mkSG(temp, mkRect(w, h*2, w, h), pos);
      imgmap_in[PlanexEnergyShot36] = mkSG(temp, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[PlanexEnergyShot69] = mkSG(temp, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[PlanexEnergyShot89] = mkSG(temp, mkRect(0, h*3, w, h), pos);
      imgmap_in[PlanexEnergyShot78] = mkSG(temp, mkRect(w, h*3, w, h), pos);
      imgmap_in[PlanexEnergyShot47] = mkSG(temp, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[PlanexEnergyShot14] = mkSG(temp, mkRect(w*3, h*3, w, h), pos);

      // 爆発Bの読み込み
      imgmap_in[ExplosionB0] = mkSG(temp, mkRect(0, h*4, w, h), pos);
      imgmap_in[ExplosionB1] = mkSG(temp, mkRect(w, h*4, w, h), pos);
      imgmap_in[ExplosionB2] = mkSG(temp, mkRect(w*2, h*4, w, h), pos);
      imgmap_in[ExplosionB3] = mkSG(temp, mkRect(w*3, h*4, w, h), pos);
      SmartSurface temp2(load(effect_path + "ene2.png"));
      temp2.SwapFormatAlpha();
      const unsigned int w2 = 100;
      const unsigned int h2 = 100;
      const Pos pos2(w2 / 2, h2 / 2);
      imgmap_in[ExplosionB4] = mkSG(temp2, mkRect(0, 0, w2, h2), pos2);
      imgmap_in[ExplosionB5] = mkSG(temp2, mkRect(w2, 0, w2, h2), pos2);
    }

    {
      // NTTのエネルギー弾の読み込み
      int w = 20;
      int h = 20;
      Pos pos(w/2, h/2);
      SmartSurface ss = load(effect_path + "ene_blue.png");
      ss.SwapFormatAlpha();
      imgmap_in[NTTEnergyShot1] = mkSG(ss, mkRect(0, 0, w, h), pos);
      imgmap_in[NTTEnergyShot2] = mkSG(ss, mkRect(w, 0, w, h), pos);
      imgmap_in[NTTEnergyShot3] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
      imgmap_in[NTTEnergyShot6] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
      imgmap_in[NTTEnergyShot9] = mkSG(ss, mkRect(0, h, w, h), pos);
      imgmap_in[NTTEnergyShot8] = mkSG(ss, mkRect(w, h, w, h), pos);
      imgmap_in[NTTEnergyShot7] = mkSG(ss, mkRect(w*2, h, w, h), pos);
      imgmap_in[NTTEnergyShot4] = mkSG(ss, mkRect(w*3, h, w, h), pos);
      imgmap_in[NTTEnergyShot12] = mkSG(ss, mkRect(0, h*2, w, h), pos);
      imgmap_in[NTTEnergyShot23] = mkSG(ss, mkRect(w, h*2, w, h), pos);
      imgmap_in[NTTEnergyShot36] = mkSG(ss, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[NTTEnergyShot69] = mkSG(ss, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[NTTEnergyShot89] = mkSG(ss, mkRect(0, h*3, w, h), pos);
      imgmap_in[NTTEnergyShot78] = mkSG(ss, mkRect(w, h*3, w, h), pos);
      imgmap_in[NTTEnergyShot47] = mkSG(ss, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[NTTEnergyShot14] = mkSG(ss, mkRect(w*3, h*3, w, h), pos);

      // 爆発Cの読み込み
      imgmap_in[ExplosionC0] = mkSG(ss, mkRect(0, h*4, w, h), pos);
      imgmap_in[ExplosionC1] = mkSG(ss, mkRect(w, h*4, w, h), pos);
      imgmap_in[ExplosionC2] = mkSG(ss, mkRect(w*2, h*4, w, h), pos);
      imgmap_in[ExplosionC3] = mkSG(ss, mkRect(w*3, h*4, w, h), pos);
      ss = load(effect_path + "ene2_blue.png");
      ss.SwapFormatAlpha();
      w = h = 40;
      pos = Pos(w/2, h/2);
      imgmap_in[ExplosionC4] = mkSG(ss, mkRect(0, 0, w, h), pos);
      imgmap_in[ExplosionC5] = mkSG(ss, mkRect(w, 0, w, h), pos);
    }

    // YamahaLaserの弾読み込み
    {
      SmartSurface ss = load(effect_path + "laser_tama.png");
      int w = 90;
      int h = 90;
      int d = 20;
      imgmap_in[YamahaLaserShot2] = mkSG(ss, mkRect(0, 0, w, h), Pos(w/2, h/2+d));
      imgmap_in[YamahaLaserShot12] = mkSG(ss, mkRect(w, 0, w, h), Pos(short(w/2-d*0.38), short(h/2+d*0.92)));
      imgmap_in[YamahaLaserShot1] = mkSG(ss, mkRect(w*2, 0, w, h), Pos(short(w/2-d*0.7), short(h/2+d*0.7)));
      imgmap_in[YamahaLaserShot14] = mkSG(ss, mkRect(w*3, 0, w, h), Pos(short(w/2-d*0.92), short(h/2+d*0.38)));
      imgmap_in[YamahaLaserShot4] = mkSG(ss, mkRect(w*4, 0, w, h), Pos(short(w/2-d), short(h/2)));
      imgmap_in[YamahaLaserShot47] = mkSG(ss, mkRect(w*5, 0, w, h), Pos(short(w/2-d*0.92), short(h/2-d*0.38)));
      imgmap_in[YamahaLaserShot7] = mkSG(ss, mkRect(w*6, 0, w, h), Pos(short(w/2-d*0.7), short(h/2-d*0.7)));
      imgmap_in[YamahaLaserShot78] = mkSG(ss, mkRect(w*7, 0, w, h), Pos(short(w/2-d*0.38), short(h/2-d*0.92)));
      imgmap_in[YamahaLaserShot8] = mkSG(ss, mkRect(w*8, 0, w, h), Pos(short(w/2), short(h/2-d)));      
      imgmap_in[YamahaLaserShot89] = mkSG(ss, mkRect(w*9, 0, w, h), Pos(short(w/2+d*0.38), short(h/2-d*0.92)));
      imgmap_in[YamahaLaserShot9] = mkSG(ss, mkRect(w*10, 0, w, h), Pos(short(w/2+d*0.7), short(h/2-d*0.7)));
      imgmap_in[YamahaLaserShot69] = mkSG(ss, mkRect(w*11, 0, w, h), Pos(short(w/2+d*0.92), short(h/2-d*0.38)));
      imgmap_in[YamahaLaserShot6] = mkSG(ss, mkRect(w*12, 0, w, h), Pos(short(w/2+d), short(h/2)));
      imgmap_in[YamahaLaserShot36] = mkSG(ss, mkRect(w*13, 0, w, h), Pos(short(w/2+d*0.92), short(h/2+d*0.38)));
      imgmap_in[YamahaLaserShot3] = mkSG(ss, mkRect(w*14, 0, w, h), Pos(short(w/2+d*0.7), short(h/2+d*0.7)));
      imgmap_in[YamahaLaserShot23] = mkSG(ss, mkRect(w*15, 0, w, h), Pos(short(w/2+d*0.38), short(h/2+d*0.92)));
    }

    // 山葉レーザーのエフェクト読み込み
    {
      SmartSurface ss;
      int w, h;
      Pos pos;

      ss = load(effect_path + "laser_effect0.png");
      ss.SwapFormatAlpha();
      w = h = 36;
      pos = Pos(w / 2, h / 2);
      imgmap_in[YamahaLaserEffect1_1]  = mkSG(ss, mkRect(w*0, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect2_1]  = mkSG(ss, mkRect(w*1, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect3_1]  = mkSG(ss, mkRect(w*2, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect6_1]  = mkSG(ss, mkRect(w*3, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect9_1]  = mkSG(ss, mkRect(w*0, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect8_1]  = mkSG(ss, mkRect(w*1, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect7_1]  = mkSG(ss, mkRect(w*2, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect4_1]  = mkSG(ss, mkRect(w*3, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect12_1] = mkSG(ss, mkRect(w*0, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect23_1] = mkSG(ss, mkRect(w*1, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect36_1] = mkSG(ss, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect69_1] = mkSG(ss, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect89_1] = mkSG(ss, mkRect(w*0, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect78_1] = mkSG(ss, mkRect(w*1, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect47_1] = mkSG(ss, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect14_1] = mkSG(ss, mkRect(w*3, h*3, w, h), pos);

      ss = load(effect_path + "laser_effect1.png");
      ss.SwapFormatAlpha();
      w = h = 36;
      w = w * 5 / 4;
      h = h * 5 / 4;
      pos = Pos(w / 2, h / 2);
      imgmap_in[YamahaLaserEffect1_2]  = mkSG(ss, mkRect(w*0, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect2_2]  = mkSG(ss, mkRect(w*1, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect3_2]  = mkSG(ss, mkRect(w*2, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect6_2]  = mkSG(ss, mkRect(w*3, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect9_2]  = mkSG(ss, mkRect(w*0, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect8_2]  = mkSG(ss, mkRect(w*1, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect7_2]  = mkSG(ss, mkRect(w*2, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect4_2]  = mkSG(ss, mkRect(w*3, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect12_2] = mkSG(ss, mkRect(w*0, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect23_2] = mkSG(ss, mkRect(w*1, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect36_2] = mkSG(ss, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect69_2] = mkSG(ss, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect89_2] = mkSG(ss, mkRect(w*0, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect78_2] = mkSG(ss, mkRect(w*1, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect47_2] = mkSG(ss, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect14_2] = mkSG(ss, mkRect(w*3, h*3, w, h), pos);

      ss = load(effect_path + "laser_effect2.png");
      ss.SwapFormatAlpha();
      w = h = 36;
      w = w * 6 / 4;
      h = h * 6 / 4;
      pos = Pos(w / 2, h / 2);
      imgmap_in[YamahaLaserEffect1_3]  = mkSG(ss, mkRect(w*0, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect2_3]  = mkSG(ss, mkRect(w*1, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect3_3]  = mkSG(ss, mkRect(w*2, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect6_3]  = mkSG(ss, mkRect(w*3, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect9_3]  = mkSG(ss, mkRect(w*0, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect8_3]  = mkSG(ss, mkRect(w*1, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect7_3]  = mkSG(ss, mkRect(w*2, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect4_3]  = mkSG(ss, mkRect(w*3, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect12_3] = mkSG(ss, mkRect(w*0, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect23_3] = mkSG(ss, mkRect(w*1, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect36_3] = mkSG(ss, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect69_3] = mkSG(ss, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect89_3] = mkSG(ss, mkRect(w*0, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect78_3] = mkSG(ss, mkRect(w*1, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect47_3] = mkSG(ss, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect14_3] = mkSG(ss, mkRect(w*3, h*3, w, h), pos);

      ss = load(effect_path + "laser_effect3.png");
      ss.SwapFormatAlpha();
      w = h = 36;
      w = w * 2;
      h = h * 2;
      pos = Pos(w / 2, h / 2);
      imgmap_in[YamahaLaserEffect1_4]  = mkSG(ss, mkRect(w*0, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect2_4]  = mkSG(ss, mkRect(w*1, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect3_4]  = mkSG(ss, mkRect(w*2, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect6_4]  = mkSG(ss, mkRect(w*3, h*0, w, h), pos);
      imgmap_in[YamahaLaserEffect9_4]  = mkSG(ss, mkRect(w*0, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect8_4]  = mkSG(ss, mkRect(w*1, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect7_4]  = mkSG(ss, mkRect(w*2, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect4_4]  = mkSG(ss, mkRect(w*3, h*1, w, h), pos);
      imgmap_in[YamahaLaserEffect12_4] = mkSG(ss, mkRect(w*0, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect23_4] = mkSG(ss, mkRect(w*1, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect36_4] = mkSG(ss, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect69_4] = mkSG(ss, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[YamahaLaserEffect89_4] = mkSG(ss, mkRect(w*0, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect78_4] = mkSG(ss, mkRect(w*1, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect47_4] = mkSG(ss, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[YamahaLaserEffect14_4] = mkSG(ss, mkRect(w*3, h*3, w, h), pos);
    }

    {
      imgmap_in[DotEffect] = mkSG(effect_path + "dot_effect.png");
    }

    { // 切れすじAの読み込み
      SmartSurface temp(load(effect_path + "slashA.png"));
      temp.SwapFormatAlpha();
      const unsigned int w = 80;
      const unsigned int h = 80;
      const Pos pos(w / 2, h / 2);

      imgmap_in[SlashA1_1] = mkSG(temp, mkRect(0, 0, w, h), pos);
      imgmap_in[SlashA1_2] = mkSG(temp, mkRect(w, 0, w, h), pos);
      imgmap_in[SlashA2_1] = mkSG(temp, mkRect(w*2, 0, w, h), pos);
      imgmap_in[SlashA2_2] = mkSG(temp, mkRect(w*3, 0, w, h), pos);
      imgmap_in[SlashA3_1] = mkSG(temp, mkRect(0, h, w, h), pos);
      imgmap_in[SlashA3_2] = mkSG(temp, mkRect(w, h, w, h), pos);
      imgmap_in[SlashA4_1] = mkSG(temp, mkRect(w*2, h, w, h), pos);
      imgmap_in[SlashA4_2] = mkSG(temp, mkRect(w*3, h, w, h), pos);
      imgmap_in[SlashA6_1] = mkSG(temp, mkRect(0, h*2, w, h), pos);
      imgmap_in[SlashA6_2] = mkSG(temp, mkRect(w, h*2, w, h), pos);
      imgmap_in[SlashA7_1] = mkSG(temp, mkRect(w*2, h*2, w, h), pos);
      imgmap_in[SlashA7_2] = mkSG(temp, mkRect(w*3, h*2, w, h), pos);
      imgmap_in[SlashA8_1] = mkSG(temp, mkRect(0, h*3, w, h), pos);
      imgmap_in[SlashA8_2] = mkSG(temp, mkRect(w, h*3, w, h), pos);
      imgmap_in[SlashA9_1] = mkSG(temp, mkRect(w*2, h*3, w, h), pos);
      imgmap_in[SlashA9_2] = mkSG(temp, mkRect(w*3, h*3, w, h), pos);
    }

    { // 影の読み込み
      SmartSurface temp(load(effect_path + "kage.png"));
      temp.SwapFormatAlpha();
      imgmap_in[Shadow] = mkSG(temp, mkRect(0, 0, temp->w, temp->h), Pos(temp->w / 2, temp->h / 2));
      temp = load(effect_path + "kage_yamaha.png");
      temp.SwapFormatAlpha();
      imgmap_in[ShadowYamaha] = mkSG(temp, mkRect(0, 0, temp->w, temp->h), Pos(temp->w/2, temp->h/2));
      temp = load(effect_path + "kage_kirai.png");
      temp.SwapFormatAlpha();
      imgmap_in[ShadowKirai] = mkSG(temp, mkRect(0, 0, temp->w, temp->h), Pos(temp->w/2, temp->h/2));
    }

    {  // 爆発Aの読み込み
      const unsigned int w = 32;
      const unsigned int h = 32;
      const Pos pos(16, 16);
      SmartSurface temp(load(effect_path + "bakuhatu.png"));
      temp.SwapFormatAlpha();

      imgmap_in[ExplosionA0] = mkSG(temp, mkRect(0, 0, w, h), pos);
      imgmap_in[ExplosionA1] = mkSG(temp, mkRect(w, 0, w, h), pos);
      imgmap_in[ExplosionA2] = mkSG(temp, mkRect(w*2, 0, w, h), pos);
      imgmap_in[ExplosionA3] = mkSG(temp, mkRect(w*3, 0, w, h), pos);
      imgmap_in[ExplosionA4] = mkSG(temp, mkRect(w*4, 0, w, h), pos);
      imgmap_in[ExplosionA5] = mkSG(temp, mkRect(w*5, 0, w, h), pos);
    }

    {
      // 爆発Aの読み込み
      const unsigned int w = 64;
      const unsigned int h = 64;
      const Pos pos(w/2, h/2);
      SmartSurface temp(load(effect_path + "bakuhatu_x2.png"));
      temp.SwapFormatAlpha();

      imgmap_in[ExplosionAx2_0] = mkSG(temp, mkRect(0, 0, w, h), pos);
      imgmap_in[ExplosionAx2_1] = mkSG(temp, mkRect(w, 0, w, h), pos);
      imgmap_in[ExplosionAx2_2] = mkSG(temp, mkRect(w*2, 0, w, h), pos);
      imgmap_in[ExplosionAx2_3] = mkSG(temp, mkRect(w*3, 0, w, h), pos);
      imgmap_in[ExplosionAx2_4] = mkSG(temp, mkRect(w*4, 0, w, h), pos);
      imgmap_in[ExplosionAx2_5] = mkSG(temp, mkRect(w*5, 0, w, h), pos);
    }

    {
      // 爆発Ax4
      int w = 128;
      int h = 128;
      Pos pos(w/2, h/2);
      SmartSurface ss(load(effect_path + "bakuhatu_x4.png"));
      ss.SwapFormatAlpha();

      imgmap_in[ExplosionAx4_0] = mkSG(ss, mkRect(0, 0, w, h), pos);
      imgmap_in[ExplosionAx4_1] = mkSG(ss, mkRect(w, 0, w, h), pos);
      imgmap_in[ExplosionAx4_2] = mkSG(ss, mkRect(w*2, 0, w, h), pos);
      imgmap_in[ExplosionAx4_3] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
      imgmap_in[ExplosionAx4_4] = mkSG(ss, mkRect(w*3, 0, w, h), pos);
      imgmap_in[ExplosionAx4_5] = mkSG(ss, mkRect(w*4, 0, w, h), pos);
    }

    { // 矢印の読み込み
      int big = 30;
      int mid = 20;
      int sml = 15;
      int bw = 60;
      int mw = 40;
      int sw = 30;

      //-----
      // 大
      //-----
      SmartSurface temp = load(arrow_path + "0.png");
      temp.SwapFormatAlpha();
      imgmap_in[Arrow_IBigUp] = mkSG(temp, mkRect(0, 0, big*2, big), Pos(big, 0));
      imgmap_in[Arrow_IBigDown] = mkSG(temp, mkRect(0, big, big*2, big), Pos(big, big));
      imgmap_in[Arrow_IBigLeft] = mkSG(temp, mkRect(0, big*2, big, big*2), Pos(0, big));
      imgmap_in[Arrow_IBigRight] = mkSG(temp, mkRect(big, big*2, big, big*2), Pos(big, big));
      imgmap_in[Arrow_RoBigUp] = mkSG(temp, mkRect(bw, 0, big*2, big), Pos(big, 0));
      imgmap_in[Arrow_RoBigDown] = mkSG(temp, mkRect(bw, big, big*2, big), Pos(big, big));
      imgmap_in[Arrow_RoBigLeft] = mkSG(temp, mkRect(bw, big*2, big, big*2), Pos(0, big));
      imgmap_in[Arrow_RoBigRight] = mkSG(temp, mkRect(bw+big, big*2, big, big*2), Pos(big, big));
      imgmap_in[Arrow_HaBigUp] = mkSG(temp, mkRect(bw*2, 0, big*2, big), Pos(big, 0));
      imgmap_in[Arrow_HaBigDown] = mkSG(temp, mkRect(bw*2, big, big*2, big), Pos(big, big));
      imgmap_in[Arrow_HaBigLeft] = mkSG(temp, mkRect(bw*2, big*2, big, big*2), Pos(0, big));
      imgmap_in[Arrow_HaBigRight] = mkSG(temp, mkRect(bw*2+big, big*2, big, big*2), Pos(big, big));
      imgmap_in[Arrow_NiBigUp] = mkSG(temp, mkRect(bw*3, 0, big*2, big), Pos(big, 0));
      imgmap_in[Arrow_NiBigDown] = mkSG(temp, mkRect(bw*3, big, big*2, big), Pos(big, big));
      imgmap_in[Arrow_NiBigLeft] = mkSG(temp, mkRect(bw*3, big*2, big, big*2), Pos(0, big));
      imgmap_in[Arrow_NiBigRight] = mkSG(temp, mkRect(bw*3+big, big*2, big, big*2), Pos(big, big));

      //-----
      // 中
      //-----
      temp = load(arrow_path + "1.png");
      temp.SwapFormatAlpha();
      imgmap_in[Arrow_IMiddleUp] = mkSG(temp, mkRect(0, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[Arrow_IMiddleDown] = mkSG(temp, mkRect(0, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[Arrow_IMiddleLeft] = mkSG(temp, mkRect(0, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[Arrow_IMiddleRight] = mkSG(temp, mkRect(mid, mid*2, mid, mid*2), Pos(mid, mid));
      imgmap_in[Arrow_RoMiddleUp] = mkSG(temp, mkRect(mw, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[Arrow_RoMiddleDown] = mkSG(temp, mkRect(mw, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[Arrow_RoMiddleLeft] = mkSG(temp, mkRect(mw, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[Arrow_RoMiddleRight] = mkSG(temp, mkRect(mw+mid, mid*2, mid, mid*2), Pos(mid, mid));
      imgmap_in[Arrow_HaMiddleUp] = mkSG(temp, mkRect(mw*2, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[Arrow_HaMiddleDown] = mkSG(temp, mkRect(mw*2, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[Arrow_HaMiddleLeft] = mkSG(temp, mkRect(mw*2, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[Arrow_HaMiddleRight] = mkSG(temp, mkRect(mw*2+mid, mid*2, mid, mid*2), Pos(mid, mid));
      imgmap_in[Arrow_NiMiddleUp] = mkSG(temp, mkRect(mw*3, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[Arrow_NiMiddleDown] = mkSG(temp, mkRect(mw*3, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[Arrow_NiMiddleLeft] = mkSG(temp, mkRect(mw*3, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[Arrow_NiMiddleRight] = mkSG(temp, mkRect(mw*3+mid, mid*2, mid, mid*2), Pos(mid, mid));

      //-----
      // 小
      //-----
      temp = load(arrow_path + "2.png");
      temp.SwapFormatAlpha();
      imgmap_in[Arrow_ISmallUp] = mkSG(temp, mkRect(0, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[Arrow_ISmallDown] = mkSG(temp, mkRect(0, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[Arrow_ISmallLeft] = mkSG(temp, mkRect(0, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[Arrow_ISmallRight] = mkSG(temp, mkRect(sml, sml*2, sml, sml*2), Pos(sml, sml));
      imgmap_in[Arrow_RoSmallUp] = mkSG(temp, mkRect(sw, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[Arrow_RoSmallDown] = mkSG(temp, mkRect(sw, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[Arrow_RoSmallLeft] = mkSG(temp, mkRect(sw, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[Arrow_RoSmallRight] = mkSG(temp, mkRect(sw+sml, sml*2, sml, sml*2), Pos(sml, sml));
      imgmap_in[Arrow_HaSmallUp] = mkSG(temp, mkRect(sw*2, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[Arrow_HaSmallDown] = mkSG(temp, mkRect(sw*2, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[Arrow_HaSmallLeft] = mkSG(temp, mkRect(sw*2, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[Arrow_HaSmallRight] = mkSG(temp, mkRect(sw*2+sml, sml*2, sml, sml*2), Pos(sml, sml));
      imgmap_in[Arrow_NiSmallUp] = mkSG(temp, mkRect(sw*3, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[Arrow_NiSmallDown] = mkSG(temp, mkRect(sw*3, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[Arrow_NiSmallLeft] = mkSG(temp, mkRect(sw*3, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[Arrow_NiSmallRight] = mkSG(temp, mkRect(sw*3+sml, sml*2, sml, sml*2), Pos(sml, sml));

      //----------
      // 大透明
      //----------
      temp = load(arrow_path + "0t.png");
      temp.SwapFormatAlpha();
      imgmap_in[TArrow_IBigUp] = mkSG(temp, mkRect(0, 0, big*2, big), Pos(big, 0));
      imgmap_in[TArrow_IBigDown] = mkSG(temp, mkRect(0, big, big*2, big), Pos(big, big));
      imgmap_in[TArrow_IBigLeft] = mkSG(temp, mkRect(0, big*2, big, big*2), Pos(0, big));
      imgmap_in[TArrow_IBigRight] = mkSG(temp, mkRect(big, big*2, big, big*2), Pos(big, big));
      imgmap_in[TArrow_RoBigUp] = mkSG(temp, mkRect(bw, 0, big*2, big), Pos(big, 0));
      imgmap_in[TArrow_RoBigDown] = mkSG(temp, mkRect(bw, big, big*2, big), Pos(big, big));
      imgmap_in[TArrow_RoBigLeft] = mkSG(temp, mkRect(bw, big*2, big, big*2), Pos(0, big));
      imgmap_in[TArrow_RoBigRight] = mkSG(temp, mkRect(bw+big, big*2, big, big*2), Pos(big, big));
      imgmap_in[TArrow_HaBigUp] = mkSG(temp, mkRect(bw*2, 0, big*2, big), Pos(big, 0));
      imgmap_in[TArrow_HaBigDown] = mkSG(temp, mkRect(bw*2, big, big*2, big), Pos(big, big));
      imgmap_in[TArrow_HaBigLeft] = mkSG(temp, mkRect(bw*2, big*2, big, big*2), Pos(0, big));
      imgmap_in[TArrow_HaBigRight] = mkSG(temp, mkRect(bw*2+big, big*2, big, big*2), Pos(big, big));
      imgmap_in[TArrow_NiBigUp] = mkSG(temp, mkRect(bw*3, 0, big*2, big), Pos(big, 0));
      imgmap_in[TArrow_NiBigDown] = mkSG(temp, mkRect(bw*3, big, big*2, big), Pos(big, big));
      imgmap_in[TArrow_NiBigLeft] = mkSG(temp, mkRect(bw*3, big*2, big, big*2), Pos(0, big));
      imgmap_in[TArrow_NiBigRight] = mkSG(temp, mkRect(bw*3+big, big*2, big, big*2), Pos(big, big));

      //----------
      // 中透明
      //----------
      temp = load(arrow_path + "1t.png");
      temp.SwapFormatAlpha();
      imgmap_in[TArrow_IMiddleUp] = mkSG(temp, mkRect(0, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[TArrow_IMiddleDown] = mkSG(temp, mkRect(0, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[TArrow_IMiddleLeft] = mkSG(temp, mkRect(0, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[TArrow_IMiddleRight] = mkSG(temp, mkRect(mid, mid*2, mid, mid*2), Pos(mid, mid));
      imgmap_in[TArrow_RoMiddleUp] = mkSG(temp, mkRect(mw, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[TArrow_RoMiddleDown] = mkSG(temp, mkRect(mw, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[TArrow_RoMiddleLeft] = mkSG(temp, mkRect(mw, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[TArrow_RoMiddleRight] = mkSG(temp, mkRect(mw+mid, mid*2, mid, mid*2), Pos(mid, mid));
      imgmap_in[TArrow_HaMiddleUp] = mkSG(temp, mkRect(mw*2, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[TArrow_HaMiddleDown] = mkSG(temp, mkRect(mw*2, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[TArrow_HaMiddleLeft] = mkSG(temp, mkRect(mw*2, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[TArrow_HaMiddleRight] = mkSG(temp, mkRect(mw*2+mid, mid*2, mid, mid*2), Pos(mid, mid));
      imgmap_in[TArrow_NiMiddleUp] = mkSG(temp, mkRect(mw*3, 0, mid*2, mid), Pos(mid, 0));
      imgmap_in[TArrow_NiMiddleDown] = mkSG(temp, mkRect(mw*3, mid, mid*2, mid), Pos(mid, mid));
      imgmap_in[TArrow_NiMiddleLeft] = mkSG(temp, mkRect(mw*3, mid*2, mid, mid*2), Pos(0, mid));
      imgmap_in[TArrow_NiMiddleRight] = mkSG(temp, mkRect(mw*3+mid, mid*2, mid, mid*2), Pos(mid, mid));

      //----------
      // 小透明
      //----------
      temp = load(arrow_path + "2t.png");
      temp.SwapFormatAlpha();
      imgmap_in[TArrow_ISmallUp] = mkSG(temp, mkRect(0, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[TArrow_ISmallDown] = mkSG(temp, mkRect(0, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[TArrow_ISmallLeft] = mkSG(temp, mkRect(0, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[TArrow_ISmallRight] = mkSG(temp, mkRect(sml, sml*2, sml, sml*2), Pos(sml, sml));
      imgmap_in[TArrow_RoSmallUp] = mkSG(temp, mkRect(sw, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[TArrow_RoSmallDown] = mkSG(temp, mkRect(sw, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[TArrow_RoSmallLeft] = mkSG(temp, mkRect(sw, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[TArrow_RoSmallRight] = mkSG(temp, mkRect(sw+sml, sml*2, sml, sml*2), Pos(sml, sml));
      imgmap_in[TArrow_HaSmallUp] = mkSG(temp, mkRect(sw*2, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[TArrow_HaSmallDown] = mkSG(temp, mkRect(sw*2, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[TArrow_HaSmallLeft] = mkSG(temp, mkRect(sw*2, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[TArrow_HaSmallRight] = mkSG(temp, mkRect(sw*2+sml, sml*2, sml, sml*2), Pos(sml, sml));
      imgmap_in[TArrow_NiSmallUp] = mkSG(temp, mkRect(sw*3, 0, sml*2, sml), Pos(sml, 0));
      imgmap_in[TArrow_NiSmallDown] = mkSG(temp, mkRect(sw*3, sml, sml*2, sml), Pos(sml, sml));
      imgmap_in[TArrow_NiSmallLeft] = mkSG(temp, mkRect(sw*3, sml*2, sml, sml*2), Pos(0, sml));
      imgmap_in[TArrow_NiSmallRight] = mkSG(temp, mkRect(sw*3+sml, sml*2, sml, sml*2), Pos(sml, sml));

      //------------------------------
      // 矢印の後ろに描画する光
      //------------------------------
      int l = 84;
      int s = 54;
      int cl = l / 2;
      int cs = 12 + 30;

      // 光大
      temp = load(arrow_path + "light_ver0.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightBigDown] = mkSG(temp, mkRect(0, 0, l, s), Pos(cl, cs));
      imgmap_in[ArrowLightBigUp] = mkSG(temp, mkRect(0, s, l, s), Pos(cl, s-cs));
      temp = load(arrow_path + "light_hor0.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightBigRight] = mkSG(temp, mkRect(0, 0, s, l), Pos(cs, cl));
      imgmap_in[ArrowLightBigLeft] = mkSG(temp, mkRect(s, 0, s, l), Pos(s-cs, cl));

      // 光中
      l = l * 2 / 3;
      s = s * 2 / 3;
      cl = cl * 2 / 3;
      cs = cs * 2 / 3;
      temp = load(arrow_path + "light_ver1.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightMiddleDown] = mkSG(temp, mkRect(0, 0, l, s), Pos(cl, cs));
      imgmap_in[ArrowLightMiddleUp] = mkSG(temp, mkRect(0, s, l, s), Pos(cl, s-cs));
      temp = load(arrow_path + "light_hor1.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightMiddleRight] = mkSG(temp, mkRect(0, 0, s, l), Pos(cs, cl));
      imgmap_in[ArrowLightMiddleLeft] = mkSG(temp, mkRect(s, 0, s, l), Pos(s-cs, cl));

      // 光小
      l = l *  3 / 4;
      s = s * 3 / 4;
      cl = cl * 3 / 4;
      cs = cs * 3 / 4 + 2;
      temp = load(arrow_path + "light_ver2.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightSmallDown] = mkSG(temp, mkRect(0, 0, l, s), Pos(cl-3, cs-1));
      imgmap_in[ArrowLightSmallUp] = mkSG(temp, mkRect(0, s, l, s), Pos(cl-3, s-cs-7));
      temp = load(arrow_path + "light_hor2.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightSmallRight] = mkSG(temp, mkRect(0, 0, s, l), Pos(cs-1, cl-2));
      imgmap_in[ArrowLightSmallLeft] = mkSG(temp, mkRect(s, 0, s, l), Pos(s-cs-6, cl-1));

      // 光紫
      l = 84;
      s = 54;
      cl = l / 2;
      cs = 12 + 30;

      // 光大
      temp = load(arrow_path + "light_ver0p.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightBigDownP] = mkSG(temp, mkRect(0, 0, l, s), Pos(cl, cs));
      imgmap_in[ArrowLightBigUpP] = mkSG(temp, mkRect(0, s, l, s), Pos(cl, s-cs));
      temp = load(arrow_path + "light_hor0p.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightBigRightP] = mkSG(temp, mkRect(0, 0, s, l), Pos(cs, cl));
      imgmap_in[ArrowLightBigLeftP] = mkSG(temp, mkRect(s, 0, s, l), Pos(s-cs, cl));

      // 光中
      l = l * 2 / 3;
      s = s * 2 / 3;
      cl = cl * 2 / 3;
      cs = cs * 2 / 3;
      temp = load(arrow_path + "light_ver1p.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightMiddleDownP] = mkSG(temp, mkRect(0, 0, l, s), Pos(cl, cs));
      imgmap_in[ArrowLightMiddleUpP] = mkSG(temp, mkRect(0, s, l, s), Pos(cl, s-cs));
      temp = load(arrow_path + "light_hor1p.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightMiddleRightP] = mkSG(temp, mkRect(0, 0, s, l), Pos(cs, cl));
      imgmap_in[ArrowLightMiddleLeftP] = mkSG(temp, mkRect(s, 0, s, l), Pos(s-cs, cl));

      // 光小
      l = l *  3 / 4;
      s = s * 3 / 4;
      cl = cl * 3 / 4;
      cs = cs * 3 / 4;
      temp = load(arrow_path + "light_ver2p.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightSmallDownP] = mkSG(temp, mkRect(0, 0, l, s), Pos(cl-3, cs-1));
      imgmap_in[ArrowLightSmallUpP] = mkSG(temp, mkRect(0, s, l, s), Pos(cl-3, s-cs-7));
      temp = load(arrow_path + "light_hor2p.png");
      temp.SwapFormatAlpha();
      imgmap_in[ArrowLightSmallRightP] = mkSG(temp, mkRect(0, 0, s, l), Pos(cs-1, cl-2));
      imgmap_in[ArrowLightSmallLeftP] = mkSG(temp, mkRect(s, 0, s, l), Pos(s-cs-6, cl-1));
    }

    { // ロックオンサイトの読み込み
      Pos rpos(40, 40);
      sp_Graphic temp;
      // 赤
      temp = mkSG(lockon_path + "r1.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockR1] = temp;
      temp = mkSG(lockon_path + "r2.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockR2] = temp;
      temp = mkSG(lockon_path + "r3.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockR3] = temp;
      temp = mkSG(lockon_path + "ru.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockRU] = temp;
      temp = mkSG(lockon_path + "rd.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockRD] = temp;
      temp = mkSG(lockon_path + "rl.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockRL] = temp;
      temp = mkSG(lockon_path + "rr.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockRR] = temp;
      // 黄色
      temp = mkSG(lockon_path + "y1.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockY1] = temp;
      temp = mkSG(lockon_path + "y2.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockY2] = temp;
      temp = mkSG(lockon_path + "y3.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockY3] = temp;
      temp = mkSG(lockon_path + "yu.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockYU] = temp;
      temp = mkSG(lockon_path + "yd.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockYD] = temp;
      temp = mkSG(lockon_path + "yl.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockYL] = temp;
      temp = mkSG(lockon_path + "yr.png");
      temp->SetOrigin(rpos);
      imgmap_in[LockYR] = temp;
    }

    // ステージの読み込み
    imgmap_in[ForestBack] = mkSGna( map_path + "mori.jpg" );
    imgmap_in[FutureBack] = mkSGna( map_path + "future.jpg" );

    // 初期画面の読み込み
    imgmap_in[InitBackground]		= mkSG( bg_path + "init.jpg" );
    imgmap_in[InitLogo]			= mkSG( init_path + "logo.png" );
    imgmap_in[InitConnectFocused]	= mkSG( button_path + "connect_f.png" );
    imgmap_in[InitConnectUnfocused]	= mkSG( button_path + "connect_uf.png" );
    imgmap_in[InitNewGameFocused]	= mkSG( button_path + "newgame_f.png" );
    imgmap_in[InitNewGameUnfocused]	= mkSG( button_path + "newgame_uf.png" );
    imgmap_in[InitExitFocused]		= mkSG( button_path + "exit_f.png" );
    imgmap_in[InitExitUnfocused]	= mkSG( button_path + "exit_uf.png" );
    imgmap_in[InitInputConnect]		= mkSG( window_path + "login_connect.png" );
    imgmap_in[InitInputNewGame]		= mkSG( window_path + "login_local.png" );

    // ロビー画像の読み込み
    imgmap_in[CharaPlanexFuda]		= mkSG( planex_path + "fuda.png" );
    imgmap_in[CharaPlanexName]		= mkSG( planex_path + "name.png" );
    imgmap_in[CharaPlanexPicture]	= mkSG( planex_path + "picture.png" );
    imgmap_in[CharaPlanexStatus]	= mkSG( planex_path + "status.png" );

    imgmap_in[CharaYamahaFuda]		= mkSG( yamaha_path + "fuda.png" );
    imgmap_in[CharaYamahaName]		= mkSG( yamaha_path + "name.png" );
    imgmap_in[CharaYamahaPicture]	= mkSG( yamaha_path + "picture.png" );
    imgmap_in[CharaYamahaStatus]	= mkSG( yamaha_path + "status.png" );

    imgmap_in[CharaNTTFuda]		= mkSG( ntt_path + "fuda.png" );
    imgmap_in[CharaNTTName]		= mkSG( ntt_path + "name.png" );
    imgmap_in[CharaNTTPicture]		= mkSG( ntt_path + "picture.png" );
    imgmap_in[CharaNTTStatus]		= mkSG( ntt_path + "status.png" );

    imgmap_in[LobbyForest]		= mkSG( map_path + "mori_mini.png" );
    imgmap_in[LobbyFuture]		= mkSG( map_path + "future_mini.png" );

    imgmap_in[LobbyChatWindowFocused]		= mkSG( window_path + "chat_f.png" );
    imgmap_in[LobbyChatWindowUnfocused]		= mkSG( window_path + "chat_uf.png" );
    imgmap_in[LobbyCharaSelectWindowFocused]	= mkSG( window_path + "info_f.png" );
    imgmap_in[LobbyCharaSelectWindowUnfocused]	= mkSG( window_path + "info_uf.png" );
    imgmap_in[LobbyStageWindowFocused]		= mkSG( window_path + "stage_f.png" );
    imgmap_in[LobbyStageWindowUnfocused]	= mkSG( window_path + "stage_uf.png" );
    imgmap_in[LobbyTeamWindowFocusedI]		= mkSG( window_path + "team_i_f.png" );
    imgmap_in[LobbyTeamWindowUnfocusedI]	= mkSG( window_path + "team_i_uf.png" );
    imgmap_in[LobbyTeamWindowFocusedRo]		= mkSG( window_path + "team_ro_f.png" );
    imgmap_in[LobbyTeamWindowUnfocusedRo]	= mkSG( window_path + "team_ro_uf.png" );
    imgmap_in[LobbyTeamWindowFocusedHa]		= mkSG( window_path + "team_ha_f.png" );
    imgmap_in[LobbyTeamWindowUnfocusedHa]	= mkSG( window_path + "team_ha_uf.png" );
    imgmap_in[LobbyTeamWindowFocusedNi]		= mkSG( window_path + "team_ni_f.png" );
    imgmap_in[LobbyTeamWindowUnfocusedNi]	= mkSG( window_path + "team_ni_uf.png" );
    imgmap_in[LobbyConnecting]                  = mkSG( lobby_path + "connecting.png" );

    imgmap_in[LobbyBackground]	= mkSGna( bg_path + "lobby_all.jpg" );
    imgmap_in[LobbyZugagaga]	= mkSGna( bg_path + "side_zu3ga.png" );
    imgmap_in[LobbyOK]		= mkSG( lobby_path + "ok.png" );
    {
      SmartSurface temp = load( button_path + "np.png" );
      temp.SwapFormatAlpha();
      imgmap_in[LobbyPrev]	= mkSG( temp, mkRect(0, 0, 32, 83), Pos(0,0));
      imgmap_in[LobbyNext]	= mkSG( temp, mkRect(33, 0, 33, 83), Pos(0,0));
      temp = load( button_path + "np_push.png" );
      temp.SwapFormatAlpha();
      imgmap_in[LobbyPrevPush]	= mkSG( temp, mkRect(0, 0, 32, 83), Pos(0,0));
      imgmap_in[LobbyNextPush]	= mkSG( temp, mkRect(33, 0, 33, 83), Pos(0,0));
    }

    imgmap_in[LobbyTagI0]	= mkSG( tag_path + "i_1.png" );
    imgmap_in[LobbyTagI1]	= mkSG( tag_path + "i_2.png" );
    imgmap_in[LobbyTagI2]	= mkSG( tag_path + "i_3.png" );
    imgmap_in[LobbyTagI3]	= mkSG( tag_path + "i_4.png" );
    imgmap_in[LobbyTagRo0]	= mkSG( tag_path + "ro_1.png" );
    imgmap_in[LobbyTagRo1]	= mkSG( tag_path + "ro_2.png" );
    imgmap_in[LobbyTagRo2]	= mkSG( tag_path + "ro_3.png" );
    imgmap_in[LobbyTagRo3]	= mkSG( tag_path + "ro_4.png" );
    imgmap_in[LobbyTagHa0]	= mkSG( tag_path + "ha_1.png" );
    imgmap_in[LobbyTagHa1]	= mkSG( tag_path + "ha_2.png" );
    imgmap_in[LobbyTagHa2]	= mkSG( tag_path + "ha_3.png" );
    imgmap_in[LobbyTagHa3]	= mkSG( tag_path + "ha_4.png" );
    imgmap_in[LobbyTagNi0]	= mkSG( tag_path + "ni_1.png" );
    imgmap_in[LobbyTagNi1]	= mkSG( tag_path + "ni_2.png" );
    imgmap_in[LobbyTagNi2]	= mkSG( tag_path + "ni_3.png" );
    imgmap_in[LobbyTagNi3]	= mkSG( tag_path + "ni_4.png" );
    imgmap_in[LobbyTagNull]	= mkSG( tag_path + "siro.png" );

    imgmap_in[ButtonCancel]	= mkSG( button_path + "cancel.png" );
    imgmap_in[ButtonOK]		= mkSG( button_path + "ok.png" );
    imgmap_in[ButtonOut]	= mkSG( button_path + "out.png" );
    imgmap_in[ButtonStart]	= mkSG( button_path + "start.png" );

    // 対戦結果表示画面に使う画像の読み込み
    imgmap_in[ResultWindow]	= mkSG( window_path + "table.png" );
    imgmap_in[ResultMark]	= mkSG( result_path + "mark.png" );

    // 対戦画面用小物の読み込み
    imgmap_in[BattleStatusBack] = mkSG( battle_path + "status.jpg" );
    // 対戦中御札画像読み込み
    // い組
    imgmap_in[StatusTagI1Big] = mkSG( battle_path + "team_i_1_myself.png" );
    imgmap_in[StatusTagI2Big] = mkSG( battle_path + "team_i_2_myself.png" );
    imgmap_in[StatusTagI3Big] = mkSG( battle_path + "team_i_3_myself.png" );
    imgmap_in[StatusTagI4Big] = mkSG( battle_path + "team_i_4_myself.png" );
    imgmap_in[StatusTagI1Small] = mkSG( battle_path + "team_i_1_enemy.png" );
    imgmap_in[StatusTagI2Small] = mkSG( battle_path + "team_i_2_enemy.png" );
    imgmap_in[StatusTagI3Small] = mkSG( battle_path + "team_i_3_enemy.png" );
    imgmap_in[StatusTagI4Small] = mkSG( battle_path + "team_i_4_enemy.png" );
    // ろ組
    imgmap_in[StatusTagRo1Big] = mkSG( battle_path + "team_ro_1_myself.png" );
    imgmap_in[StatusTagRo2Big] = mkSG( battle_path + "team_ro_2_myself.png" );
    imgmap_in[StatusTagRo3Big] = mkSG( battle_path + "team_ro_3_myself.png" );
    imgmap_in[StatusTagRo4Big] = mkSG( battle_path + "team_ro_4_myself.png" );
    imgmap_in[StatusTagRo1Small] = mkSG( battle_path + "team_ro_1_enemy.png" );
    imgmap_in[StatusTagRo2Small] = mkSG( battle_path + "team_ro_2_enemy.png" );
    imgmap_in[StatusTagRo3Small] = mkSG( battle_path + "team_ro_3_enemy.png" );
    imgmap_in[StatusTagRo4Small] = mkSG( battle_path + "team_ro_4_enemy.png" );
    // は組
    imgmap_in[StatusTagHa1Big] = mkSG( battle_path + "team_ha_1_myself.png" );
    imgmap_in[StatusTagHa2Big] = mkSG( battle_path + "team_ha_2_myself.png" );
    imgmap_in[StatusTagHa3Big] = mkSG( battle_path + "team_ha_3_myself.png" );
    imgmap_in[StatusTagHa4Big] = mkSG( battle_path + "team_ha_4_myself.png" );
    imgmap_in[StatusTagHa1Small] = mkSG( battle_path + "team_ha_1_enemy.png" );
    imgmap_in[StatusTagHa2Small] = mkSG( battle_path + "team_ha_2_enemy.png" );
    imgmap_in[StatusTagHa3Small] = mkSG( battle_path + "team_ha_3_enemy.png" );
    imgmap_in[StatusTagHa4Small] = mkSG( battle_path + "team_ha_4_enemy.png" );
    // に組
    imgmap_in[StatusTagNi1Big] = mkSG( battle_path + "team_ni_1_myself.png" );
    imgmap_in[StatusTagNi2Big] = mkSG( battle_path + "team_ni_2_myself.png" );
    imgmap_in[StatusTagNi3Big] = mkSG( battle_path + "team_ni_3_myself.png" );
    imgmap_in[StatusTagNi4Big] = mkSG( battle_path + "team_ni_4_myself.png" );
    imgmap_in[StatusTagNi1Small] = mkSG( battle_path + "team_ni_1_enemy.png" );
    imgmap_in[StatusTagNi2Small] = mkSG( battle_path + "team_ni_2_enemy.png" );
    imgmap_in[StatusTagNi3Small] = mkSG( battle_path + "team_ni_3_enemy.png" );
    imgmap_in[StatusTagNi4Small] = mkSG( battle_path + "team_ni_4_enemy.png" );
    imgmap_in[StatusTagDeadBig] = mkSG( battle_path + "status_tag_dead_myself.png" );
    // その他御札関係
    imgmap_in[StatusTagKuro] = mkSG( battle_path + "team_kuro.png" );
    imgmap_in[StatusTagDeadSmall] = mkSG( battle_path + "status_tag_dead_enemy.png" );
    imgmap_in[StatusTagNoUse] = mkSG( battle_path + "status_tag_nouse.png" );
    imgmap_in[StatusTagBlackBig] = mkSG( battle_path + "black_myself.png" );
    imgmap_in[StatusTagBlackSmall] = mkSG( battle_path + "black_enemy.png" );

    // 炎と焦げ
    SmartSurface temp = load( battle_path + "fire.png" );
    unsigned int w = 80;
    imgmap_in[BurntBig] = mkSG( battle_path + "burnt_myself.png" );
    imgmap_in[FlameBig0] = mkSG( temp, mkRect(0, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameBig1] = mkSG( temp, mkRect(w, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameBig2] = mkSG( temp, mkRect(w*2, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameBig3] = mkSG( temp, mkRect(w*3, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameBig4] = mkSG( temp, mkRect(w*4, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameBig5] = mkSG( temp, mkRect(w*5, 0, w, temp->h), Pos(0, 0));
    temp = load( battle_path + "fire_enemy.png" );
    w = 60;
    imgmap_in[BurntSmall] = mkSG( battle_path + "burnt_enemy.png" );
    imgmap_in[FlameSmall0] = mkSG( temp, mkRect(0, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameSmall1] = mkSG( temp, mkRect(w, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameSmall2] = mkSG( temp, mkRect(w*2, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameSmall3] = mkSG( temp, mkRect(w*3, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameSmall4] = mkSG( temp, mkRect(w*4, 0, w, temp->h), Pos(0, 0));
    imgmap_in[FlameSmall5] = mkSG( temp, mkRect(w*5, 0, w, temp->h), Pos(0, 0));
    // 経過時間
    imgmap_in[Elapse] = mkSG( battle_path + "elapsed_time.png" );

    // 対戦スクリーンの幅を確定
    ScreenWidth = WindowWidth - imgmap_in[BattleStatusBack]->GetWH().x;
    ScreenHeight = WindowHeight;
  } // ImgDataInitialize()

} // namespace {}
