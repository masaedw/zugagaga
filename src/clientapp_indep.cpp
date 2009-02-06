// last modified <2004/02/09 04:26:26 2004 JST>
#include <stdexcept>
#include <vector>
#include <boost/bind.hpp>
#include "clientapp.hpp"
#include "verbose.hpp"
#include "event.hpp"
#include "utility.hpp"
#include "ztk.hpp"
#include "graphic.hpp"

using namespace std;
using namespace zu3ga;
using namespace zu3ga::graphic;
using namespace zu3ga::lobby;
using namespace zu3ga::ztk;
using namespace edw;
using namespace edw::sdl;
using namespace boost;

const int FPS = 30;

class HogeTimer
{
 private:
  unsigned int interval;
  unsigned int stime;
  unsigned int fpstime;
  unsigned int count;
  unsigned int skip;
 public:
  HogeTimer()
    : interval(0)
    , stime(0)
    , fpstime(0)
    , skip(0)
  {}
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

namespace {
  void Final()
  {
    SmartSurface scr = GetVideoSurface();
    scr.Clear(Color());
    scr.Draw(GetGraphic(Type::InitBackground));
    sp_Graphic rw = GetGraphic(Type::ResultWindow);
    scr.Draw(rw,(Pos(WindowWidth,WindowHeight)-pos_cast<Pos>(rw->GetWH()))/2);
    scr.Update();
    SDLTimer timer;
    timer.SetFPS(FPS);
    Uint16 key;
    LobbyEvent::t eve;
    while (true) {
      timer.Start();
      while(EventManager::GetInstance()->GetEvent(eve,key)) {
	switch (eve) {
	case LobbyEvent::Quit:
	case LobbyEvent::OK:
	  goto endfunc;
	  break;
	default:
	  break;
	}
      }
      timer.Wait();
    }
  endfunc:
    return;
  }
}

bool
ClientApp::Mainloop()
{
  //SDLTimer timer;
  HogeTimer timer;
  timer.SetFPS(FPS);

  verbose::puts( 2, "メインループ開始" );

  sp_Graphic mark = GetGraphic(Type::ResultMark);
  sp_Graphic table = GetGraphic(Type::ResultWindow);
  Pos tpos = (Pos(WindowWidth,WindowHeight)-pos_cast<Pos>(table->GetWH()))/2;
  sp_Graphic bg = GetGraphic(Type::LobbyBackground);
  GameStatus::t pre_stat = GameStatus::Game;

  try {
    while ( true ) {
      bool draw = timer.Start();

      eio_->Update();

      Uint8 eve;
      // 入力を取得してプレイヤーに設定
      while (EventManager::GetInstance()->GetEvent(eve)) {
	switch (eve) {
	case ClientEvent::Quit:
	  return false;
	  break;
	default:
	  eio_->PushEvent(eve);
	  break;
	}
      }

      GameStatus::t stat = eio_->GetStatus();

      if ( pre_stat == GameStatus::Ready && stat == GameStatus::Game )
	dmngr_->LetsStart();

      if (draw) {
	SmartSurface scr = GetVideoSurface();
	if ( stat == GameStatus::Game || stat == GameStatus::Ready ) {
	  SendDataPair data = eio_->GetData();
	  // 描画
	  if ( !data.second.HP.empty() )
	    dmngr_->DrawAll( &data.first, &data.second );
	} else if ( stat == GameStatus::Result ) {
	  scr.Draw(bg);
	  scr.Draw(table, tpos);
	} else if ( stat == GameStatus::Final ) {
	  break;
	}
	scr.Update();
      }

      pre_stat = stat;
      // FPSを実現するために待つ
      timer.Wait();
    }
    Final();
    eio_.reset();
  }
  catch (const Zu3gaError& e) {
    errmsg_ = e.JaMsg();
    return false;
  }
  catch (const std::exception& e) {
    errmsg_ = e.what();
    return false;
  }
  verbose::puts( 2, "メインループ終了" );
  return true;
}

//======================================================================
//======================================================================

namespace {
  class Arrow : public ClickableBase, public UpdaterBase, public GraphicBase
  {
  public:
    Arrow( const sp_Graphic& nomal,
	   const sp_Graphic& pushed,
	   const function<void()>& f )
      : nomal_(nomal), pushed_(pushed), cb_(f), count_(0)
    {
      nomal_->SetOrigin( Pos() );
      Uint32 w, h;
      nomal_->GetWH( w, h );
    }

    void Click()
    {
      if (cb_) {
	cb_();
	count_++;
      }
    }

    void Update()
    {
      if ( count_ )
	++count_ &= 7;
    }

    Rect GetRect()
    {
      Uint32 w, h;
      nomal_->GetWH(w, h);
      return Rect( p_.x, p_.y, w, h );
    }

    void Draw( const SDL_Rect& r, const SmartSurface& s, const Pos& p ) const
    {
      p_ = p;
      if ( !count_ )
	nomal_->Draw( r, s, p );
      else
	pushed_->Draw( r, s, p );
    }

    void Draw( const SmartSurface& s, const Pos& p ) const
    {
      p_ = p;
      if ( !count_ )
	nomal_->Draw( s, p );
      else
	pushed_->Draw( s, p );
    }

    BasicPos<Uint32> GetWH() const { return nomal_->GetWH(); }
    void GetWH( Uint32& w, Uint32& h ) const { nomal_->GetWH( w, h ); }
    void GetOrigin( Pos& p ) const { p = Pos(); }
    void SetOrigin( const Pos& ) {}

  private:
    sp_Graphic nomal_, pushed_;
    mutable Pos p_;
    function<void()> cb_;//call back
    int count_;
  };
  typedef boost::shared_ptr<Arrow> sp_Arrow;

  using namespace Type;

  class Window : public UpdaterBase, public FocusPart
  {
  public:
    virtual void Action( LobbyEvent::t ) = 0;
    void SetInfo( const Info* info )
    {
      info_ = info;
    }

    Window() : info_(0) {}
    virtual ~Window() {}

  protected:
    const Info* info_;
  };

  class StageSelection : public Window
  {
  public:
    static Pos spos;

    StageSelection( const function<void()>& cb_prev, const function<void()>& cb_next )
      : bgf_( GetGraphic(LobbyStageWindowFocused) )
      , bguf_( GetGraphic(LobbyStageWindowUnfocused) )
      , pa_( sp_Arrow(new Arrow(GetGraphic(LobbyPrev), GetGraphic(LobbyPrevPush), cb_prev)) )
      , na_( sp_Arrow(new Arrow(GetGraphic(LobbyNext), GetGraphic(LobbyNextPush), cb_next)) )
      , size_( pos_cast<Pos>(bgf_->GetWH()) )
      , stpos_( bgf_->GetWH().x/2, 115 )
    {
      ss_.push_back(GetGraphic(LobbyForest));
      ss_.push_back(GetGraphic(LobbyFuture));
      ss_[0]->SetOrigin( pos_cast<Pos>(ss_[0]->GetWH())/2 );
      ss_[1]->SetOrigin( pos_cast<Pos>(ss_[1]->GetWH())/2 );
    }

    void Action( LobbyEvent::t eve )
    {
      if ( info_->GetMyData().ok )
	return;

      switch (eve) {
      case LobbyEvent::Left:
	pa_->Click();
	break;
      case LobbyEvent::Right:
	na_->Click();
	break;
      default:
	break;
      }
    }

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      pa_->Update();
      na_->Update();
      if ( HaveFocus() )
	scr.Draw(bgf_, spos);
      else
	scr.Draw(bguf_, spos);
      if ( HaveFocus() ) {
	scr.Draw(pa_, Pos(370,67));
	scr.Draw(na_, Pos(556,67));
      }
      scr.Draw(ss_[info_->stage], spos+stpos_);
    }

  private:
    sp_Graphic bgf_, bguf_;
    sp_Arrow pa_, na_;
    vector<sp_Graphic> ss_;
    Pos size_, stpos_;
  };

  class CharaSelection : public Window
  {
  public:
    static Pos spos;

  private:
    struct CharaG
    {
      sp_Graphic name, picture, status;
    };

  public:
    CharaSelection( const function<void()>& cb_prev, const function<void()>& cb_next )
      : bgf_( GetGraphic(LobbyCharaSelectWindowFocused) )
      , bguf_( GetGraphic(LobbyCharaSelectWindowUnfocused) )
      , pa_( sp_Arrow( new Arrow(GetGraphic(LobbyPrev), GetGraphic(LobbyPrevPush), cb_prev) ) )
      , na_( sp_Arrow( new Arrow(GetGraphic(LobbyNext), GetGraphic(LobbyNextPush), cb_next) ) )
      , size_( pos_cast<Pos>(bgf_->GetWH()) )
      , name_( size_.x/2+30, 60 )
      , pcpos_( 64, 110 )
    {
      CharaG temp;
      // Planex
      temp.name = GetGraphic(CharaPlanexName);
      temp.name->SetOrigin(pos_cast<Pos>(temp.name->GetWH()/2));
      temp.picture = GetGraphic(CharaPlanexPicture);
      temp.picture->SetOrigin(pos_cast<Pos>(temp.picture->GetWH())/2);
      temp.status = GetGraphic(CharaPlanexStatus);
      ss_.push_back(temp);

      // Yamaha
      temp.name = GetGraphic(CharaYamahaName);
      temp.name->SetOrigin(pos_cast<Pos>(temp.name->GetWH()/2));
      temp.picture = GetGraphic(CharaYamahaPicture);
      Pos tpos = pos_cast<Pos>(temp.picture->GetWH())/2;
      tpos.x += 6;
      temp.picture->SetOrigin(tpos);
      temp.status = GetGraphic(CharaYamahaStatus);
      ss_.push_back(temp);
      stpos_ = pos_cast<Pos>(bgf_->GetWH() - temp.status->GetWH()) - Pos(30, 30);

      // NTT
      temp.name = GetGraphic(CharaNTTName);
      temp.name->SetOrigin(pos_cast<Pos>(temp.name->GetWH()/2));
      temp.picture = GetGraphic(CharaNTTPicture);
      temp.picture->SetOrigin(pos_cast<Pos>(temp.picture->GetWH())/2);
      temp.status = GetGraphic(CharaNTTStatus);
      ss_.push_back(temp);
    }

    void Action( LobbyEvent::t eve )
    {
      if ( info_->GetMyData().ok )
	return;

      switch (eve) {
      case LobbyEvent::Left:
	pa_->Click();
	break;
      case LobbyEvent::Right:
	na_->Click();
	break;
      default:
	break;
      }
    }

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      pa_->Update();
      na_->Update();
      if ( HaveFocus() )
	scr.Draw(bgf_, spos);
      else
	scr.Draw(bguf_, spos);
      CharaG temp = ss_[info_->GetMyData().type];
      Pos tmppos = (size_-pos_cast<Pos>(temp.picture->GetWH()))/2 + spos;
      tmppos.y -= 50;
      scr.Draw( temp.picture, spos+pcpos_ );
      scr.Draw( temp.name, spos+name_ );
      scr.Draw( temp.status, spos+stpos_ );
      if ( HaveFocus() ) {
	scr.Draw(pa_, Pos(349,272));
	scr.Draw(na_, Pos(471,272));
      }
    }

  private:
    sp_Graphic bgf_, bguf_;
    sp_Arrow pa_, na_;
    vector<CharaG> ss_;
    Pos size_, name_, pcpos_, stpos_;
  };

  class TeamSelection : public Window
  {
  public:
    static Pos spos;
    TeamSelection( const function<void()>& cb_prev, const function<void()>& cb_next )
      : pa_( sp_Arrow(new Arrow(GetGraphic(LobbyPrev), GetGraphic(LobbyPrevPush), cb_prev)) )
      , na_( sp_Arrow(new Arrow(GetGraphic(LobbyNext), GetGraphic(LobbyNextPush), cb_next)) ) 
    {
      bgf_[0] = GetGraphic(LobbyTeamWindowFocusedI);
      bgf_[1] = GetGraphic(LobbyTeamWindowFocusedRo);
      bgf_[2] = GetGraphic(LobbyTeamWindowFocusedHa);
      bgf_[3] = GetGraphic(LobbyTeamWindowFocusedNi);
      bguf_[0] = GetGraphic(LobbyTeamWindowUnfocusedI);
      bguf_[1] = GetGraphic(LobbyTeamWindowUnfocusedRo);
      bguf_[2] = GetGraphic(LobbyTeamWindowUnfocusedHa);
      bguf_[3] = GetGraphic(LobbyTeamWindowUnfocusedNi);
    }

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      pa_->Update();
      na_->Update();

      if ( HaveFocus() ) {
	scr.Draw(bgf_[info_->GetMyData().team],spos);
      } else {
	if ( info_->GetMyData().team < 4 )
	  scr.Draw(bguf_[info_->GetMyData().team],spos);
      }

      if ( HaveFocus() ) {
	scr.Draw(pa_, Pos(359,365));
	scr.Draw(na_, Pos(575,365));
      }
    }

    void Action( LobbyEvent::t eve )
    {
      if ( info_->GetMyData().ok )
	return;

      switch (eve) {
      case LobbyEvent::Left:
	pa_->Click();
	break;
      case LobbyEvent::Right:
	na_->Click();
	break;
      default:
	break;
      }
    }

  private:
    sp_Arrow pa_, na_;
    sp_Graphic bgf_[4];
    sp_Graphic bguf_[4];
  };

  class ChatWindow : public Window
  {
  public:
    static Pos spos;

    ChatWindow()
      : bgf_( GetGraphic(LobbyChatWindowFocused) )
      , bguf_( GetGraphic(LobbyChatWindowUnfocused) )
    {}

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      if ( HaveFocus() ) {
	scr.Draw( bgf_, spos );
      } else {
	scr.Draw( bguf_, spos );
      }
    }
    void Action( LobbyEvent::t ) {}
  private:
    sp_Graphic bgf_, bguf_;
  };

  class Background : public UpdaterBase
  {
  public:
    Background()
      : bg_( GetGraphic(LobbyBackground) )
      , zu3ga_( GetGraphic(LobbyZugagaga) )
      , rpos_( WindowWidth - zu3ga_->GetWH().x )
      , y_( 0 )
      , z3h_( zu3ga_->GetWH().y+9 )
    {}

    void Update()
    {
      SmartSurface scr = GetVideoSurface();

      scr.Draw(bg_);
      for (unsigned int y = y_; y < WindowHeight + z3h_; y += z3h_) {
	scr.Draw(zu3ga_, Pos(0, y - z3h_));
	scr.Draw(zu3ga_, Pos(rpos_, y - z3h_));
      }

      if ( y_ )
	y_--;
      else
	y_ = z3h_;
    }

  private:
    sp_Graphic bg_, zu3ga_;
    int rpos_, y_, z3h_;
  };

  class TagWindow : public Window
  {
  public:
    static Pos spos;

    TagWindow()
      : tagnull_( GetGraphic(LobbyTagNull) )
      , gOK_( GetGraphic(LobbyOK) )
      , pp_( GetGraphic(CharaPlanexFuda) )
      , py_( GetGraphic(CharaYamahaFuda) )
      , pn_( GetGraphic(CharaNTTFuda) )
    {
      tag_[0][0] = GetGraphic(LobbyTagI0);
      tag_[0][1] = GetGraphic(LobbyTagI1);
      tag_[0][2] = GetGraphic(LobbyTagI2);
      tag_[0][3] = GetGraphic(LobbyTagI3);
      tag_[1][0] = GetGraphic(LobbyTagRo0);
      tag_[1][1] = GetGraphic(LobbyTagRo1);
      tag_[1][2] = GetGraphic(LobbyTagRo2);
      tag_[1][3] = GetGraphic(LobbyTagRo3);
      tag_[2][0] = GetGraphic(LobbyTagHa0);
      tag_[2][1] = GetGraphic(LobbyTagHa1);
      tag_[2][2] = GetGraphic(LobbyTagHa2);
      tag_[2][3] = GetGraphic(LobbyTagHa3);
      tag_[3][0] = GetGraphic(LobbyTagNi0);
      tag_[3][1] = GetGraphic(LobbyTagNi1);
      tag_[3][2] = GetGraphic(LobbyTagNi2);
      tag_[3][3] = GetGraphic(LobbyTagNi3);
      pp_->SetOrigin(pos_cast<Pos>(pp_->GetWH()));
      py_->SetOrigin(pos_cast<Pos>(py_->GetWH()));
      pn_->SetOrigin(pos_cast<Pos>(pn_->GetWH()));
      gOK_->SetOrigin(pos_cast<Pos>(gOK_->GetWH())/2);
      Uint32 w, h;
      tagnull_->GetWH(w,h);
      twid_ = w;
      picpos_ = Pos(w,h);
      okpos_ = Pos(w/2, h-40);
    }

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      for ( int i = 0; i < 4; i++ ) {
	Pos tpos(spos.x + (twid_+13)*i, spos.y);
	if ( info_->initdata[i].team == Team::None )
	  scr.Draw(tagnull_, tpos);
	else {
	  scr.Draw(tag_[info_->initdata[i].team][i], tpos); // 背景
	  switch (info_->initdata[i].type) { // キャラ
	  case PlayerType::Planex:
	    scr.Draw( pp_, tpos+picpos_ );
	    break;
	  case PlayerType::Yamaha:
	    scr.Draw( py_, tpos+picpos_ );
	    break;
	  case PlayerType::NTT:
	    scr.Draw( pn_, tpos+picpos_ );
	    break;
	  }
	  if ( info_->initdata[i].ok )
	    scr.Draw( gOK_, tpos+okpos_ );
	}
      }
    }

    void Action( LobbyEvent::t ) {}
  private:
    sp_Graphic tag_[4][4];
    sp_Graphic tagnull_, gOK_;
    sp_Graphic pp_, py_, pn_;
    Uint32 twid_;
    Pos picpos_, okpos_;
  };

  class ButtonWindow : public Window
  {
    struct Inner
    {
      sp_Graphic g;
      function<void()> func;
      Inner( const sp_Graphic& ag, const function<void()>& af )
	: g( ag ), func( af ) {}
    };

  public:
    static Pos spos;
    ButtonWindow( bool canstart,
		  const function<void()>& cb_cancel,
		  const function<void()>& cb_ok,
		  const function<void()>& cb_out,
		  const function<void()>& cb_start)
      : fnum_(0)
      , cancel_( GetGraphic(ButtonCancel), cb_cancel )
      , ok_( GetGraphic(ButtonOK), cb_ok )
      , out_( GetGraphic(ButtonOut), cb_out )
      , start_( GetGraphic(ButtonStart), cb_start )
      , focus_( RectGraphic::Create(70, 30, 3, Color(0x83,0x64,0x76), false) )
      , canstart_( canstart )
    {
      il_ = &out_;
      ir_ = &ok_;
    }

    void Action( LobbyEvent::t eve )
    {
      switch (eve) {
      case LobbyEvent::Left:
	++fnum_ %= 2;
	break;
      case LobbyEvent::Right:
	++fnum_ %= 2;
	break;
      case LobbyEvent::OK:
	switch (fnum_) {
	case 0:
	  il_->func();
	  break;
	case 1:
	  ir_->func();
	  break;
	default:
	  break;
	}
	break;
      default:
	break;
      }
    }

    void Update()
    {
      Pos pos1( spos + Pos(45,0) );
      Pos pos2( spos + Pos(125,0) );
      SmartSurface scr = GetVideoSurface();

      if ( info_->GetMyData().ok ) {
	if ( canstart_ && AllOK() ) {
	  il_ = &cancel_;
	  ir_ = &start_;
	} else {
	  il_ = &out_;
	  ir_ = &cancel_;
	}
      } else {
	il_ = &out_;
	ir_ = &ok_;
      }

      if ( HaveFocus() ) {
	if ( fnum_ == 0 )
	  scr.Draw(focus_, pos1);
	else
	  scr.Draw(focus_, pos2);
      }
      scr.Draw( il_->g, pos1 );
      scr.Draw( ir_->g, pos2 );
    }

    bool AllOK()
    {
      bool retvalue = true;
      for ( int i = 0; i < 4; i++ ) {
	if ( info_->initdata[i].team != Team::None ) {
	  retvalue &= info_->initdata[i].ok;
	}
      }
      return retvalue;
    }

  private:
    int fnum_;
    Inner* il_;
    Inner* ir_;
    Inner cancel_, ok_, out_, start_;
    sp_RectGraphic focus_;
    const bool canstart_;
  };

  Pos StageSelection::spos(353, -8);
  Pos CharaSelection::spos(362, 180);
  Pos TeamSelection::spos(370, 358);
  Pos ButtonWindow::spos(362, 445);

  Pos ChatWindow::spos(37, 270);
  Pos TagWindow::spos(45,12);
}

bool
ClientApp::Lobby() // この関数の処理は実にオブジェクト指向的じゃない！
{
  errmsg_.erase();
  LobbyEvent::t eve;
  Uint16 key;
  bool retvalue = true;
  vector<sp_Updater> delarr;
  Window* focus;
  Window* prefocus = 0;
  StageSelection* ssel = new StageSelection(bind(&LobbyIOBase::PrevStage,lobbyio_),
					    bind(&LobbyIOBase::NextStage,lobbyio_));
  CharaSelection* csel = new CharaSelection(bind(&LobbyIOBase::PrevChara,lobbyio_),
					    bind(&LobbyIOBase::NextChara,lobbyio_));
  TeamSelection* tsel = new TeamSelection(bind(&LobbyIOBase::PrevTeam,lobbyio_),
					  bind(&LobbyIOBase::NextTeam,lobbyio_));
  ButtonWindow* bwin = new ButtonWindow(lobbyio_->IsHost(),
					bind(&LobbyIOBase::Cancel,lobbyio_),
					bind(&LobbyIOBase::Ready,lobbyio_),
					bind(&LobbyIOBase::Bye,lobbyio_),
					bind(&LobbyIOBase::Start,lobbyio_));
  Background* bg = new Background;
  ChatWindow* cwin = new ChatWindow;
  TagWindow* tagwin = new TagWindow;

  delarr.push_back(sp_Updater(ssel));
  delarr.push_back(sp_Updater(csel));
  delarr.push_back(sp_Updater(tsel));
  delarr.push_back(sp_Updater(bwin));
  delarr.push_back(sp_Updater(bg));
  delarr.push_back(sp_Updater(cwin));
  delarr.push_back(sp_Updater(tagwin));

  csel->SetFocus();
  focus = csel;

  sp_Graphic wait(GetGraphic(Type::LobbyConnecting));

  //SDLTimer timer;
  HogeTimer timer;
  timer.SetFPS(FPS);

  //cout << "lobby start" << endl;
  //cout << (lobbyio_->IsHost() ? "host" : "connect") << endl;
  while (true) {
    timer.Start();
    GetVideoSurface().Clear(Color(0xff,0xff,0xff));
    lobbyio_->Update();

    const Info* info = lobbyio_->GetData();
    bg->Update();
    if ( info ) {
      ssel->SetInfo(info);
      csel->SetInfo(info);
      tsel->SetInfo(info);
      bwin->SetInfo(info);
      tagwin->SetInfo(info);

      ssel->Update();
      csel->Update();
      tsel->Update();
      bwin->Update();
      tagwin->Update();
      cwin->Update();
      GetVideoSurface().Update();
    } else { // 接続中
      SmartSurface scr = GetVideoSurface();
      scr.Draw(wait,Pos(300,100));
      scr.Update();
      while (EventManager::GetInstance()->GetEvent(eve,key)) {
	switch (eve) {
	case LobbyEvent::Quit:
	  retvalue = false;
	  goto endloop;
	  break;
	default:
	  break;
	}
      }
      goto wait;
    }

    while (EventManager::GetInstance()->GetEvent(eve,key)) {
      switch (eve) {
      case LobbyEvent::Quit:
	lobbyio_->Bye();
	retvalue = false;
	goto endloop;
	break;
      case LobbyEvent::Down:
	if ( info->GetMyData().ok )
	  break;
	if ( focus == ssel ) {
	  ssel->ResetFocus();
	  csel->SetFocus();
	  focus = csel;
	} else if ( focus == csel ) {
	  csel->ResetFocus();
	  tsel->SetFocus();
	  focus = tsel;
	} else if ( focus == tsel ) {
	  tsel->ResetFocus();
	  bwin->SetFocus();
	  focus = bwin;
	} else if ( focus == bwin ) {
	  bwin->ResetFocus();
	  if ( lobbyio_->IsHost() ) {
	    ssel->SetFocus();
	    focus = ssel;
	  } else {
	    csel->SetFocus();
	    focus = csel;
	  }
	}
	break;
      case LobbyEvent::Up:
	if ( info->GetMyData().ok )
	  break;
	if ( focus == ssel ) {
	  ssel->ResetFocus();
	  bwin->SetFocus();
	  focus = bwin;
	} else if ( focus == csel ) {
	  csel->ResetFocus();
	  if ( lobbyio_->IsHost() ) {
	    ssel->SetFocus();
	    focus = ssel;
	  } else {
	    bwin->SetFocus();
	    focus = bwin;
	  }
	} else if ( focus == tsel ) {
	  tsel->ResetFocus();
	  csel->SetFocus();
	  focus = csel;
	} else if ( focus == bwin ) {
	  bwin->ResetFocus();
	  tsel->SetFocus();
	  focus = tsel;
	}
	break;
      case LobbyEvent::Change:
      case LobbyEvent::BackChange:
	if ( focus != cwin ) {
	  focus->ResetFocus();
	  prefocus = focus;
	  cwin->SetFocus();
	  focus = cwin;
	} else {
	  focus->ResetFocus();
	  prefocus->SetFocus();
	  focus = prefocus;
	  prefocus = 0;
	}
	break;
      default:
	if ( focus )
	  focus->Action(eve);
	break;
      }
    }

  wait:
    switch (lobbyio_->GetStatus()) {
    case Stat::Continue:
      break;
    case Stat::Start:
      goto endloop;
      break;
    case Stat::Error:
      {
	string str = lobbyio_->GetError();
	if ( str.empty() )
	  errmsg_ = "接続を拒否されました";
	else
	  errmsg_ = str;
      }
    case Stat::Quit:
      retvalue = false;
      goto endloop;
      break;
    }

    timer.Wait();
  }

 endloop:
  //cout << "lobby end" << endl;

  return retvalue;
}
