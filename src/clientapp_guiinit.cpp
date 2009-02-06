// last modified <2004/02/09 15:25:22 2004 JST>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#include <boost/bind.hpp>
#include "clientapp.hpp"
#include "networkio.hpp"
#include "localio.hpp"
#include "ztk.hpp"
#include "event.hpp"

using namespace std;
using namespace zu3ga;
using namespace zu3ga::network;
using namespace zu3ga::graphic;
using namespace zu3ga::graphic::Type;
using namespace edw::sdl;
using boost::function;
using boost::bind;

namespace {
  const Color c_gray(0x55,0x55,0x55);
  const Color c_black;
  const Color c_error(0xdd,0x33,0x66);
  const Pos helppos(40,200);
  const Pos backpos(200,300);
  const Pos nextpos(300,300);
  const Pos errpos(150,165);

  string DefName()
  {
    const char* name = getenv("USER");
    if ( name )
      return name;
    else
      return "YourName";
  }

  class ActionBase : public ztk::FocusPart
  {
  public:
    virtual void Action() = 0;
    virtual void SetEvent( Uint16 ) = 0;
    virtual ~ActionBase() {}
  };

  //----------------------------------------------------------------------
  // ぼたん
  class Button;
  typedef boost::shared_ptr<Button> sp_Button;
  class Button : public ActionBase
  {
  public:
    static sp_Button Create( const sp_Graphic& f, const sp_Graphic& uf, const function<void()>& cb )
    {
      return sp_Button( new Button( f, uf, cb ) );
    }

    Button( const sp_Graphic& f, const sp_Graphic& uf, const function<void()>& cb )
      : f_(f), uf_(uf), cb_(cb)
    {}

    sp_Graphic GetGraphic()
    {
      if ( HaveFocus() )
	return f_;
      else
	return uf_;
    }

    void Action()
    {
      if (cb_)
	cb_();
    }

    void SetEvent( Uint16 ) {}

    using ztk::FocusPart::onFocused_;
    using ztk::FocusPart::onUnfocused_;

  private:
    sp_Graphic f_, uf_;
    function<void()> cb_;
  };

  //----------------------------------------------------------------------
  class NullGraphic : public GraphicBase
  {
  public:
    void Draw( const SDL_Rect&, const SmartSurface&, const Pos& ) const {};
    void Draw( const SmartSurface&, const Pos& ) const {};
    BasicPos<Uint32> GetWH() const { return BasicPos<Uint32>(); }
    Pos GetOrigin() const { return Pos(); }
    void GetWH( Uint32& w, Uint32& h ) const { w = 0, h = 0; }
    void GetOrigin( Pos& p ) const { p = Pos(); }
    void SetOrigin( const Pos& ) {}
    static sp_Graphic Create() { return sp_Graphic( new NullGraphic ); }
  };

  //----------------------------------------------------------------------
  class Window : public UpdaterBase, public ztk::FocusPart
  {
  public:
    virtual void SetEvent( LobbyEvent::t, Uint16 ) = 0;
    virtual sp_LobbyIO MakeLobby() = 0;
    virtual ~Window() {}
  };
  typedef boost::shared_ptr<Window> sp_Window;

  //----------------------------------------------------------------------
  class TextInput;
  typedef boost::shared_ptr<TextInput> sp_TextInput;
  class TextInput : public GraphicBase, public ActionBase
  {
  public:
    TextInput( const sp_Font& font, const Color cl = Color(0x00,0x00,0x00) )
      : cl_( cl )
      , font_(font)
      , gr_( NullGraphic::Create() )
      , fc_( RectGraphic::Create(10,2,1,cl_) )
      , max_(0)
    {}

    static sp_TextInput Create( const sp_Font& font, const Color cl = Color(0x00,0x00,0x00) )
    {
      return sp_TextInput( new TextInput( font, cl ) );
    }

    void SetMax( string::size_type max ) { max_ = max; }

    void SetEvent( Uint16 key )
    {
      if (key == '\b') {
	if ( !str_.empty() ) {
	  str_.erase(str_.size()-1);
	}
      } else {
	if ( max_ == 0 || max_ > str_.size() ) {
	  str_ += key;
	}
      }
      Refresh();
    }

    Color GetColor() const { return cl_; }
    void SetColor( const Color& cl )
    {
      cl_ = cl;
      fc_->SetColor(cl);
    }

    string GetString() const { return str_; }
    void SetString( const string& c ) { str_ = c; Refresh(); }
    
    void Draw( const SDL_Rect& r, const SmartSurface& s, const Pos& p ) const
    {
      gr_->Draw( r, s, p );
      if ( HaveFocus() ) {
	Pos tp = p;
	tp.x += gr_->GetWH().x;
	tp.y += font_->Height() - 5;
	fc_->Draw( r, s, tp );
      }
    }

    void Draw( const SmartSurface& s, const Pos& p ) const
    {
      gr_->Draw( s, p );
      if ( HaveFocus() ) {
	Pos tp = p;
	tp.x += gr_->GetWH().x;
	tp.y += font_->Height() - 5;
	fc_->Draw( s, tp );
      }
    }

    using GraphicBase::GetOrigin;	// Pos GetOrigin() const;

    BasicPos<Uint32> GetWH()
    {
      return gr_->GetWH();
    }

    void GetWH( Uint32& w, Uint32& h ) const
    {
      gr_->GetWH( w, h );
    }

    void Action()
    {
      if (cb_action)
	cb_action();
    }
    void SetAction( const function<void()>& f ) { cb_action = f; }

    using ztk::FocusPart::onFocused_;
    using ztk::FocusPart::onUnfocused_;

  private:
    Color cl_;
    sp_Font font_;
    mutable sp_Graphic gr_;
    sp_RectGraphic fc_;
    string str_;
    string::size_type max_;
    function<void()> cb_action;

    void Refresh()
    {
      if ( str_.empty() )
	gr_ = NullGraphic::Create();
      else
	gr_ = font_->RenderBlended(str_,cl_).GetGraphic();
    }
  };

  //----------------------------------------------------------------------
  class ConnectWindow : public Window
  {
  public:
    ConnectWindow( const function<void()>& cb_cancel, const function<void()>& cb_make,
		   string& name, string& host, string& port )
      : font_(GetFont(FontType::Login))
      , bg_(GetGraphic(InitInputConnect))
      , errname_(font_->RenderBlended("ユーザー名が必要です",c_error).GetGraphic())
      , errhost_(font_->RenderBlended("ホスト名が不正です",c_error).GetGraphic())
      , errport_(font_->RenderBlended("ポート番号が不正です",c_error).GetGraphic())
      , errconnect_(font_->RenderBlended("接続できません",c_error).GetGraphic())
      , name_(TextInput::Create(font_,c_black))
      , host_(TextInput::Create(font_,c_black))
      , port_(TextInput::Create(font_,c_black))
      , bcan_(Button::Create(font_->RenderBlended("もどる",c_black).GetGraphic(),
			     font_->RenderBlended("もどる",c_gray).GetGraphic(),
			     cb_cancel))
      , bok_(Button::Create(font_->RenderBlended("すすむ",c_black).GetGraphic(),
			    font_->RenderBlended("すすむ",c_gray).GetGraphic(),
			    cb_make))
      , cb_Cancel(cb_cancel)
      , must_check_(None)
      , fi_(true)
      , dname_(&name)
      , dhost_(&host)
      , dport_(&port)
    {
      if ( name.empty() )
	name_->SetString(DefName());
      else
	name_->SetString(name);
      if ( host.empty() )
	host_->SetString("localhost");
      else
	host_->SetString(host);
      if ( port.empty() )
	port_->SetString("9999");
      else
	port_->SetString(port);
      name_->SetAction( bind(&ztk::FocusManager<ActionBase>::Next,&fm_) );
      host_->SetAction( bind(&ztk::FocusManager<ActionBase>::Next,&fm_) );
      port_->SetAction( bind(&ztk::FocusManager<ActionBase>::Next,&fm_) );
      name_->SetMax(13);
      fm_.Add( name_ );
      fm_.Add( host_ );
      fm_.Add( port_ );
      fm_.Add( bok_ );
      fm_.Add( bcan_ );
      onFocused_ = bind(&ConnectWindow::OnFocus,this);
      onUnfocused_ = bind(&ConnectWindow::SaveData,this);
      name_->onUnfocused_ = bind(&ConnectWindow::MustCheck, this, Name);
      host_->onUnfocused_ = bind(&ConnectWindow::MustCheck, this, Host);
      port_->onUnfocused_ = bind(&ConnectWindow::MustCheck, this, Port);
      errname_->SetOrigin(pos_cast<Pos>(errname_->GetWH())/2);
      errhost_->SetOrigin(pos_cast<Pos>(errhost_->GetWH())/2);
      errport_->SetOrigin(pos_cast<Pos>(errport_->GetWH())/2);
      errconnect_->SetOrigin(pos_cast<Pos>(errconnect_->GetWH())/2);
    }

    void GetString( string& name, string& host, string& port )
    {
      name = name_->GetString();
      host = host_->GetString();
      port = port_->GetString();
    }

    void Update()
    {
      if ( !HaveFocus() )
	return;

      CheckInput();

      SmartSurface suf = GetVideoSurface();
      suf.Draw(bg_, spos);
      suf.Draw(name_,spos+Pos(92,38));
      suf.Draw(host_,spos+Pos(117,65));
      suf.Draw(port_,spos+Pos(137,92));
      suf.Draw(bok_->GetGraphic(), spos+backpos);
      suf.Draw(bcan_->GetGraphic(), spos+nextpos);
      if (err_) {
	suf.Draw(err_, spos+errpos);
      }
    }

    sp_LobbyIO MakeLobby()
    {
      SaveData();
      sp_Connection con(new Connection(host_->GetString(), StrToInt(port_->GetString())));
      try {
	con->Open();
      } catch (exception& e) {
	err_ = errconnect_;
	while ( !host_->HaveFocus() ) {
	  fm_.Next();
	}
	return sp_LobbyIO();
      }
      return network::LobbyIO::Create( con, name_->GetString());
    }

    void SetEvent( LobbyEvent::t eve, Uint16 key )
    {
      switch (eve) {
      case LobbyEvent::Quit:
	cb_Cancel();
	break;
      case LobbyEvent::OK:
	fm_.GetFocus()->Action();
	break;
      case LobbyEvent::Up:
      case LobbyEvent::BackChange:
	fm_.Prev();
	break;
      case LobbyEvent::Down:
      case LobbyEvent::Change:
	fm_.Next();
	break;
      case LobbyEvent::Key:
	if ( fi_ && name_->HaveFocus() ) {
	  fi_ = false;
	  name_->SetString("");
	}
	fm_.GetFocus()->SetEvent( key );
	break;
      default:
	break;
      }
    }

    static Pos spos;
  private:
    void OnFocus()
    {
      while ( !name_->HaveFocus() ) {
	fm_.Next();
      }
      err_.reset();
      StoreData();
    }

    void StoreData()
    {
      if ( !dname_->empty() )
	name_->SetString(*dname_);
      if ( !dhost_->empty() )
	host_->SetString(*dhost_);
      if ( !dport_->empty() )
	port_->SetString(*dport_);
    }

    void SaveData()
    {
      *dname_ = name_->GetString();
      *dhost_ = host_->GetString();
      *dport_ = port_->GetString();
    }

    void CheckInput()
    {
      switch (must_check_) {
      case None:
	break;

      case Name:
	if ( name_->GetString() == "" ) {
	  name_->SetString(DefName());
	  err_ = errname_;
	  while ( !name_->HaveFocus() ) {
	    fm_.Next();
	  }
	} else if ( err_ != errconnect_ ) {
	  err_.reset();
	}
	break;

      case Host:
	if ( IsValidHostName(host_->GetString()) == false ) {
	  host_->SetString("");
	  err_ = errhost_;
	  while ( !host_->HaveFocus() ) {
	    fm_.Next();
	  }
	} else if ( err_ != errconnect_ ) {
	  err_.reset();
	}
	break;

      case Port:
	try {
	  port_->SetString(IntToStr(StrToInt(port_->GetString())));
	  if ( err_ != errconnect_ )
	    err_.reset();
	} catch (exception& e) {
	  port_->SetString("");
	  err_ = errport_;
	  while ( !port_->HaveFocus() ) {
	    fm_.Next();
	  }
	}
	break;
      }
      must_check_ = None;
    }

    enum Check {
      Name,
      Port,
      Host,
      None,
    };

    void MustCheck( Check c ) { must_check_ = c; }

    sp_Font font_;
    sp_Graphic bg_, err_, errname_, errhost_, errport_, errconnect_;
    sp_TextInput name_, host_, port_;
    sp_Button bcan_, bok_;
    ztk::FocusManager<ActionBase> fm_;
    function<void()> cb_Cancel;
    Check must_check_;
    bool fi_; // first input switch
    string *dname_, *dhost_, *dport_;
  };
  Pos ConnectWindow::spos(181, 95);

  //----------------------------------------------------------------------
  class NewGameWindow : public Window
  {
  public:
    NewGameWindow( const function<void()>& cb_cancel, const function<void()>& cb_make,
		   string& name, string& port )
      : font_(GetFont(FontType::Login))
      , bg_(GetGraphic(InitInputNewGame))
      , errname_(font_->RenderBlended("ユーザー名が必要です", c_error).GetGraphic())
      , errport_(font_->RenderBlended("ポート番号が不正です", c_error).GetGraphic())
      , errbind_(font_->RenderBlended("bindできません", c_error).GetGraphic())
      , name_(TextInput::Create(font_, c_black))
      , port_(TextInput::Create(font_, c_black))
      , bcan_(Button::Create(font_->RenderBlended("もどる",c_black).GetGraphic(),
			     font_->RenderBlended("もどる",c_gray).GetGraphic(),
			     cb_cancel))
      , bok_(Button::Create(font_->RenderBlended("すすむ",c_black).GetGraphic(),
			    font_->RenderBlended("すすむ",c_gray).GetGraphic(),
			    cb_make))
      , cb_Cancel(cb_cancel)
      , must_check_(None)
      , fi_(true)
      , dname_(&name)
      , dport_(&port)
    {
      if ( name.empty() )
	name_->SetString(DefName());
      else
	name_->SetString(name);
      if ( port.empty() )
	port_->SetString("9999");
      else
	port_->SetString(port);
      name_->SetAction( bind(&ztk::FocusManager<ActionBase>::Next,&fm_) );
      port_->SetAction( bind(&ztk::FocusManager<ActionBase>::Next,&fm_) );
      name_->SetMax(13);
      name_->onUnfocused_ = bind(&NewGameWindow::MustCheck,this,Name);
      port_->onUnfocused_ = bind(&NewGameWindow::MustCheck,this,Port);
      fm_.Add( name_ );
      fm_.Add( port_ );
      fm_.Add( bok_ );
      fm_.Add( bcan_ );
      onFocused_ = bind(&NewGameWindow::OnFocus,this);
      onUnfocused_ = bind(&NewGameWindow::SaveData,this);
      errname_->SetOrigin(pos_cast<Pos>(errname_->GetWH())/2);
      errport_->SetOrigin(pos_cast<Pos>(errport_->GetWH())/2);
      errbind_->SetOrigin(pos_cast<Pos>(errbind_->GetWH())/2);
    }

    void GetString( string& name, string& port )
    {
      name = name_->GetString();
      port = port_->GetString();
    }

    void Update()
    {
      if ( !HaveFocus() )
	return;

      CheckInput();

      SmartSurface suf = GetVideoSurface();
      suf.Draw(bg_, spos);
      suf.Draw(name_,spos+Pos(90,38));
      suf.Draw(port_,spos+Pos(138,65));
      suf.Draw(bok_->GetGraphic(), spos+backpos);
      suf.Draw(bcan_->GetGraphic(), spos+nextpos);
      if (err_) {
	suf.Draw(err_, spos+errpos);
      }
    }

    sp_LobbyIO MakeLobby()
    {
      SaveData();
      try {
	return local::LobbyIO::Create(StrToInt(port_->GetString()),name_->GetString());
      } catch (exception& e) {
	err_ = errbind_;
	while ( !port_->HaveFocus() ) {
	  fm_.Next();
	}
	return sp_LobbyIO();
      }
    }

    void SetEvent( LobbyEvent::t eve, Uint16 key )
    {
      switch (eve) {
      case LobbyEvent::Quit:
	cb_Cancel();
	break;
      case LobbyEvent::OK:
	fm_.GetFocus()->Action();
	break;
      case LobbyEvent::Up:
      case LobbyEvent::BackChange:
	fm_.Prev();
	break;
      case LobbyEvent::Down:
      case LobbyEvent::Change:
	fm_.Next();
	break;
      case LobbyEvent::Key:
	if ( fi_ && name_->HaveFocus() ) {
	  fi_ = false;
	  name_->SetString("");
	}
	fm_.GetFocus()->SetEvent( key );
	break;
      default:
	break;
      }
    }

    static Pos spos;
  private:
    void OnFocus()
    {
      while ( !name_->HaveFocus() ) {
	fm_.Next();
      }
      err_.reset();
      StoreData();
    }

    void StoreData()
    {
      if ( !dname_->empty() )
	name_->SetString(*dname_);
      if ( !dname_->empty() )
	port_->SetString(*dport_);
    }

    void SaveData()
    {
      *dname_ = name_->GetString();
      *dport_ = port_->GetString();
    }

    void CheckInput()
    {
      switch (must_check_) {
      case None:
	break;

      case Name:
	if ( name_->GetString() == "" ) {
	  name_->SetString(DefName());
	  err_ = errname_;
	  while ( !name_->HaveFocus() ) {
	    fm_.Next();
	  }
	} else if ( err_ != errbind_ ) {
	  err_.reset();
	}
	break;

      case Port:
	try {
	  port_->SetString(IntToStr(StrToInt(port_->GetString())));
	  if ( err_ != errbind_ ) {
	    err_.reset();
	  }
	}
	catch (exception& e) {
	  port_->SetString("");
	  err_ = errport_;
	  while ( !port_->HaveFocus() ) {
	    fm_.Next();
	  }
	}
	break;
      }
      must_check_ = None;
    }

    enum Check {
      Name,
      Port,
      None,
    };

    void MustCheck( Check c ) { must_check_ = c; }

    sp_Font font_;
    sp_Graphic bg_, err_, errname_, errport_, errbind_;
    sp_TextInput name_, port_;
    sp_Button bcan_, bok_;
    ztk::FocusManager<ActionBase> fm_;
    function<void()> cb_Cancel;
    Check must_check_;
    bool fi_; // first input switch
    string *dname_, *dport_;
  };
  Pos NewGameWindow::spos(181, 95);

  //----------------------------------------------------------------------
  class ButtonWindow : public Window
  {
  public:
    ButtonWindow( const function<void()>& cb_con,
		  const function<void()>& cb_new,
		  const function<void()>& cb_exit )
      : bcon_(Button::Create(GetGraphic(InitConnectFocused), GetGraphic(InitConnectUnfocused), cb_con))
      , bnew_(Button::Create(GetGraphic(InitNewGameFocused), GetGraphic(InitNewGameUnfocused), cb_new))
      , bexit_(Button::Create(GetGraphic(InitExitFocused), GetGraphic(InitExitUnfocused), cb_exit))
    {
      bm_.Add( bcon_ );
      bm_.Add( bnew_ );
      bm_.Add( bexit_ );
    }

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      scr.Draw(bcon_->GetGraphic(), Pos(380, 290));
      scr.Draw(bnew_->GetGraphic(), Pos(415, 330));
      scr.Draw(bexit_->GetGraphic(), Pos(450, 370));
    }

    void SetEvent( LobbyEvent::t eve, Uint16 )
    {
      switch (eve) {
      case LobbyEvent::OK:
	bm_.GetFocus()->Action();
	break;
      case LobbyEvent::Down:
      case LobbyEvent::Change:
	bm_.Next();
	break;
      case LobbyEvent::Up:
      case LobbyEvent::BackChange:
	bm_.Prev();
	break;
      case LobbyEvent::Quit:
	bexit_->Action();
	break;
      default:
	break;
      }
    }

    sp_LobbyIO MakeLobby() { return sp_LobbyIO(); } // これが呼ばれることは実際には無い
  private:
    sp_Button bcon_, bnew_, bexit_;
    ztk::FocusManager<Button> bm_;
  };

  //----------------------------------------------------------------------
  // 初期画面GUI版
  class GUIInit
  {
  public:
    GUIInit( string& name, string& host, string& port )
      : bg_( GetGraphic(InitBackground) )
      , logo_( GetGraphic(InitLogo) )
      , bw_(new ButtonWindow(bind(&GUIInit::Connect,this),
 			     bind(&GUIInit::NewGame,this),
 			     bind(&GUIInit::Exit,this)))
      , cw_(new ConnectWindow(bind(&GUIInit::Back,this),
			      bind(&GUIInit::MakeLobby,this),
			      name,host,port))
      , nw_(new NewGameWindow(bind(&GUIInit::Back,this),
			      bind(&GUIInit::MakeLobby,this),
			      name,port))
      , fc_( bw_ )
      , ret_( false )
      , exit_( false )
    {}

    ~GUIInit()
    {}

    void Update()
    {
      SmartSurface scr = GetVideoSurface();
      scr.Draw(bg_);
      scr.Draw(logo_, Pos(80, 80));
      fc_->Update();
    }

    bool Return()
    {
      return ret_;
    }

    sp_LobbyIO GetLobby()
    {
      return lobby_;
    }

    void SetEvent( LobbyEvent::t eve, Uint16 key )
    {
      switch (eve) {
      case LobbyEvent::Quit:
	Exit();
	break;
      default:
	fc_->SetEvent( eve, key );
	break;
      }
    }

    bool IsExit()
    {
      return exit_;
    }

  private:
    void Exit() { exit_ = true; }
    void Back() { ChangeFocus(bw_); }
    void Connect() { ChangeFocus(cw_); }
    void NewGame() { ChangeFocus(nw_); }
    void MakeLobby()
    {
      if ( lobby_ = fc_->MakeLobby() )
	ret_ = exit_ = true;
    }
    void ChangeFocus( const sp_Window& f )
    {
      fc_->ResetFocus();
      fc_ = f;
      fc_->SetFocus();
    }

    sp_LobbyIO lobby_;
    sp_Graphic bg_, logo_;
    sp_Window bw_, cw_, nw_, fc_;
    bool ret_, exit_;
    int uni_;
  };
}

bool
ClientApp::InitGraphic()
{
  lobbyio_.reset();
  eio_.reset();
  GUIInit ui( name_, host_, port_ );
  SDLTimer timer;
  timer.SetFPS(20);
  int count = 0;
  sp_Graphic err = NullGraphic::Create();
  if ( !errmsg_.empty() ) {
    err = GetFont(FontType::Error)->RenderBlended(errmsg_,Color(0xee,0x22,0x33)).GetGraphic();
  }

  while (true) {
    timer.Start();
    ui.Update();
    SmartSurface suf = GetVideoSurface();
    if ( count < 20*10 ) {
      count++;
      Uint32 w, h;
      err->GetWH(w,h);
      Pos pos(20,WindowHeight - h - 20);
      suf.Draw(err,pos);
    } else {
      errmsg_.erase();
    }
    suf.Update();
    LobbyEvent::t eve;
    Uint16 key;
    while (EventManager::GetInstance()->GetEvent(eve,key)) {
      ui.SetEvent(eve,key);
    }
    if ( ui.IsExit() )
      break;
    timer.Wait();
  }

  if ( ui.Return() ) {
    lobbyio_ = ui.GetLobby();
    return true;
  } else
    return false;
}
