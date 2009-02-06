/** @file
 * SDLのオブジェクト指向ラッパーのファイル
 */
// last modified <2004/02/15 05:37:24 2004 JST>
// いろはにほへと
#ifndef EDW_SDL_HPP
#define EDW_SDL_HPP

#include <string>
#include <stdexcept>
#include <SDL.h>
#include <SDL_ttf.h>
#include "boost/shared_ptr.hpp"
#include "timer.hpp"
#include "basic_structs.hpp"

namespace edw {
  /**
   * SDLのオブジェクト指向ラッパー
   */
  namespace sdl {
    using namespace basic_structs;

    class SmartSurface;
    class Font;
    class GraphicBase;
    class SurfaceGraphic;
    class RectGraphic;
    typedef boost::shared_ptr<Font> sp_Font;
    typedef boost::shared_ptr<GraphicBase> sp_Graphic;
    typedef boost::shared_ptr<SurfaceGraphic> sp_SurfaceGraphic;
    typedef boost::shared_ptr<RectGraphic> sp_RectGraphic;

    inline SDL_Rect mkRect( Sint16 x, Sint16 y, Uint16 w, Uint16 h )
    {
      SDL_Rect rect = { x, y, w, h };
      return rect;
    }

    /**
     * SDL中の例外送出用クラス
     */
    class Error : public std::runtime_error
    {
    public:
      Error() : runtime_error( SDL_GetError() ) {}
      Error( const std::string& msg )
	: runtime_error( msg + ": " + SDL_GetError() ) {}
    };

    /**
     * ディスプレイフォーマット変換
     */
    inline bool
    SwapImageFormat( SDL_Surface*& suf )
    {
      if ( !suf )
	throw Error("SwapImageFormat: called to null surface.");
      SDL_Surface* temp;
      temp = SDL_DisplayFormat( suf );
      if ( !temp )
	return false;

      SDL_FreeSurface( suf );
      suf = temp;

      return true;
    }

    /**
     * ディスプレイフォーマット変換���つき
     */
    inline bool
    SwapImageFormatAlpha( SDL_Surface*& suf )
    {
      if ( !suf )
	throw Error("SwapImageFormatAlpha: called to null surface.");

      SDL_Surface* temp;
      temp = SDL_DisplayFormatAlpha( suf );
      if ( !temp )
	return false;

      SDL_FreeSurface( suf );
      suf = temp;

      return true;
    }

    /**
     * 色クラス
     */
    struct Color
    {
      Uint8 r, g, b, a;
      Color() : r(0), g(0), b(0), a(0) {}
      Color( Uint8 r, Uint8 g, Uint8 b, Uint8 a = 0xff )
	: r(r), g(g), b(b), a(a) {}
      /**
       * SDL_MapRGB(A)
       * @param suf このサーフェスの format に合わせる
       */
      Uint32 Map( const SmartSurface& suf ) const;
      operator SDL_Color () const;
    };

    /**
     * タイマーの実体化に必要なポリシークラス
     */
    struct TimerPolicy
    {
      typedef Uint32 time_t;
      time_t GetTicks() { return SDL_GetTicks(); }
      void Delay( time_t t ) { SDL_Delay( t ); }
    };
    /**
     * SDL の関数を用いたタイマー
     */
    typedef Timer<TimerPolicy> SDLTimer;

    /**
     * 頭良いサーフェス
     */
    class SmartSurface
    {
    public:
      SmartSurface();
      /**
       * @param suf サーフェス
       * @param owner 所有権あり(解放しなければならない)なら true
       */
      SmartSurface( SDL_Surface* const suf, bool owner = true );
      SmartSurface( const SmartSurface& other );
      ~SmartSurface();
      SmartSurface& operator = ( const SmartSurface& );

      bool operator == ( const SmartSurface& other ) const;
      bool operator != ( const SmartSurface& o ) const { return !(*this == o); }
      operator bool () const { return data_->suf; }
      const SDL_Surface* operator -> () const { return data_->suf; }
      SDL_Surface* Get() const { return data_->suf; }

      bool SwapFormat() { return SwapImageFormat( data_->suf ); }
      bool SwapFormatAlpha() { return SwapImageFormatAlpha( data_->suf ); }
      ///< 色をサーフェスのフォーマットに合わせて作る
      Uint32 Map( const Color& c ) const { return c.Map( *this ); }
      ///< 画面消去
      void Clear();
      ///< 色指定で画面消去
      void Clear( const Color& c );
      ///< SDL_Flip
      void Update();
      ///< SDL_UpdateRect
      void Update( const SDL_Rect& r );

      ///< { 0, 0, w, h } な矩形取得
      void GetRect( SDL_Rect& r ) const;
      ///< { 0, 0, w, h } な矩形を返す
      SDL_Rect GetRect() const;

      ///< Surface 描画オブジェクトを返す
      sp_SurfaceGraphic GetGraphic() const;

      ///< GraphicBase を描画する 位置指定つき
      void Draw( const sp_Graphic& g, const Pos& p );
      ///< GraphicBase を描画する はしから
      void Draw( const sp_Graphic& g );

      ///< SDL_LockSurface
      void Lock();
      ///< SDL_UnlockSurface
      void Unlock();

      ///< SDL_SetAlpha
      bool SetAlpha( Uint32 flag, Uint8 alpha );
      ///< SDL_SetColorKey
      bool SetColorKey( Uint32 flag, Uint32 key );

      ///< 複製を作って返す
      SmartSurface Duplicate( Uint32 flags = SDL_HWSURFACE );
      ///< 新規にサーフェスを確保する
      ///< 属性は VideoMode に同じ
      static SmartSurface New( Uint32 flags, Uint32 w, Uint32 h );
      static SmartSurface New( Uint32 w, Uint32 h );

    private:
      struct inner
      {
	SDL_Surface* suf;
	Uint32 count;
	const bool owner;
	inner( SDL_Surface* suf, bool owner )
	  : suf( suf ), count( 1 ), owner( owner ) {}
      };
      inner* data_;
      void inc_ref();
      void dec_ref();
    };

    /**
     * 画面サーフェス取得
     */
    inline SmartSurface GetVideoSurface()
    {
      SDL_Surface* screen = SDL_GetVideoSurface();
      if ( !screen )
	throw Error( "GetVideoSurface: video is not initialized yet." );
      return SmartSurface( screen, false );
    }

    /**
     * SDL_ttf の簡単なラッパー
     */
    class Font
    {
    public:
      static bool Init();
      static bool WasInit();
      static void Quit();

      Font( const std::string& name, int size );
      ~Font();

      static sp_Font Create( const std::string& name, int size );

      SmartSurface RenderSolid( const std::string& str, const Color& fg );
      SmartSurface RenderShaded( const std::string& str, const Color& fg, const Color& bg );
      SmartSurface RenderBlended( const std::string& str, const Color& fg );

      SmartSurface RenderSolidV( const std::string& str, const Color& fg );
      SmartSurface RenderShadedV( const std::string& str, const Color& fg, const Color& bg );
      SmartSurface RenderBlendedV( const std::string& str, const Color& fg );

      bool Size( const std::string& str, int& w, int& h );

      int Height();

      TTF_Font* Get() { return font_; }
      TTF_Font* get() { return font_; }

    private:
      TTF_Font* font_;
    };

    /**
     * 画像基底クラス
     */
    class GraphicBase
    {
    public:
      virtual ~GraphicBase() {}
      virtual void Draw( const SDL_Rect& r, const SmartSurface& suf, const Pos& ) const = 0;
      virtual void Draw( const SmartSurface& suf, const Pos& ) const = 0;

      virtual BasicPos<Uint32> GetWH() const;
      virtual Pos GetOrigin() const;

      virtual void GetWH( Uint32& w, Uint32& h ) const = 0;
      virtual void GetOrigin( Pos& ) const {}

      virtual void SetOrigin( const Pos& ) {}
    };
    /* Graphic継承ガイド
      void Draw( const SDL_Rect& r, const SmartSurface& s, const Pos& p ) const;
      void Draw( const SmartSurface& s, const Pos& p ) const;
      using GraphicBase::GetWH;		// BasicPos<Uint32> GetWH() const;
      using GraphicBase::GetOrigin;	// Pos GetOrigin() const;
      void GetWH( Uint32& w, Uint32& h ) const;
      //void GetOrigin( Pos& p ) const;	// null function {}
      //void SetOrigin( const Pos& p );	// null function {}
    */

    /**
     * サーフェス画像
     */
    class SurfaceGraphic : public GraphicBase
    {
    public:
      SurfaceGraphic() {}
      SurfaceGraphic( const SmartSurface& suf );
      SurfaceGraphic( const SmartSurface& suf, const SDL_Rect& rect );
      SurfaceGraphic( const SmartSurface& suf, const SDL_Rect& rect, const Pos& origin );
      ~SurfaceGraphic() {}

      static sp_SurfaceGraphic Create();
      static sp_SurfaceGraphic Create( const SmartSurface& suf );
      static sp_SurfaceGraphic Create( const SmartSurface& suf, const SDL_Rect& rect );
      static sp_SurfaceGraphic Create( const SmartSurface& suf, const SDL_Rect& rect, const Pos& origin );

      void Draw( const SDL_Rect& r, const SmartSurface& suf, const Pos& ) const;
      void Draw( const SmartSurface& suf, const Pos& ) const;

      BasicPos<Uint32> GetWH() const;
      void GetWH( Uint32& w, Uint32& h ) const;
      Pos GetOrigin() const;
      void GetOrigin( Pos& p ) const;
      void GetSurface( SmartSurface& suf ) const;
      void GetRect( SDL_Rect& rect ) const;

      void SetOrigin( const Pos& p );
      void SetSurface( const SmartSurface& suf );
      SmartSurface GetSurface();
      void SetRect( const SDL_Rect& rect );

      sp_SurfaceGraphic Duplicate();

    private:
      SmartSurface src_;
      SDL_Rect srcrect_;
      Pos origin_;
    };

    /**
     * 矩形
     */
    class RectGraphic : public GraphicBase
    {
    public:
      RectGraphic() {}

      /**
       * @param w 幅
       * @param h 高さ
       * @param l 線の太さ
       * @param color 色
       * @param fill 中を塗りつぶすなら true 塗りつぶさないなら false
       */
      RectGraphic( Uint32 w, Uint32 h, Uint32 l, Color color, bool fill = true )
	: width_( w ), height_( h ), line_( l ), color_( color ), fill_( fill )
      {}

      /**
       * @param w 幅
       * @param h 高さ
       * @param l 線の太さ
       * @param fill 中を塗りつぶすなら true 塗りつぶさないなら false
       */
      RectGraphic( Uint8 w, Uint8 h, Uint8 l, bool fill = true )
	: width_( w ), height_( h ), line_( l ), color_( Color( 0, 0, 0 ) ), fill_( fill )
      {}

      static sp_RectGraphic Create();
      static sp_RectGraphic Create( Uint32 w, Uint32 h, Uint32 l, Color color, bool fill = true );
      static sp_RectGraphic Create( Uint8 w, Uint8 h, Uint8 l, bool fill = true );

      void Draw( const SDL_Rect& r, const SmartSurface& suf, const Pos& ) const;
      void Draw( const SmartSurface& suf, const Pos& ) const;

      BasicPos<Uint32> GetWH() const;
      void GetWH( Uint32& w, Uint32& h ) const;
      Pos GetOrigin() const;
      void GetOrigin( Pos& p ) const;
      void GetColor( Color& c ) const;

      void SetOrigin( const Pos& p );
      void SetWH( Uint32 w, Uint32 h );
      void SetColor( const Color& c );

    private:
      Pos origin_;
      Uint32 width_, height_, line_;
      Color color_;
      bool fill_;
    };

    //======================================================================
    inline Uint32
    Color::Map( const SmartSurface& suf ) const
    {
      if ( suf->flags & SDL_SRCALPHA ) {
	return SDL_MapRGBA( suf->format, r, g, b, a );
      } else {
	return SDL_MapRGB( suf->format, r, g, b );
      }
    }

    inline Color::operator SDL_Color () const
    {
      SDL_Color temp = {  r, g, b, a };
      return temp;
    }

    //======================================================================
    // 以下 頭良いサーフェスの実装
    inline SmartSurface::SmartSurface()
      : data_( 0 )
    {}

    inline SmartSurface::SmartSurface( SDL_Surface* const suf, bool owner )
      : data_( new inner( suf, owner ) )
    {}

    inline SmartSurface::SmartSurface( const SmartSurface& o )
      : data_( o.data_ )
    {
      inc_ref();
    }

    inline SmartSurface&
    SmartSurface::operator = ( const SmartSurface& o )
    {
      if ( data_ != o.data_ ) {
	dec_ref();
	data_ = o.data_;
	inc_ref();
      }
      return *this;
    }

    inline bool
    SmartSurface::operator == ( const SmartSurface& o ) const
    {
      if ( !data_ || data_->owner )
	return data_ == o.data_;
      else if ( o.data_ )
	return data_->suf == o.data_->suf;
      else
	return false;
    }

    inline sp_SurfaceGraphic
    SmartSurface::GetGraphic() const
    {
      return sp_SurfaceGraphic( new SurfaceGraphic( *this ) );
    }

    inline void
    SmartSurface::GetRect( SDL_Rect& rect ) const
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::GetRect(): called to null pointer." );
      rect.x = 0;
      rect.y = 0;
      rect.w = data_->suf->w;
      rect.h = data_->suf->h;
    }

    inline SDL_Rect
    SmartSurface::GetRect() const
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::GetRect(): called to null pointer." );
      SDL_Rect rect;
      rect.x = 0;
      rect.y = 0;
      rect.w = data_->suf->w;
      rect.h = data_->suf->h;
      return rect;
    }

    inline void
    SmartSurface::Clear()
    {
      SDL_FillRect( data_->suf, 0, Map( Color( 0, 0, 0 ) ) );
    }

    inline void
    SmartSurface::Clear( const Color& c )
    {
      SDL_FillRect( data_->suf, 0, Map( c ) );
    }

    inline void
    SmartSurface::Update()
    {
      SDL_Flip( data_->suf );
    }

    inline void
    SmartSurface::Update( const SDL_Rect& r )
    {
      SDL_UpdateRect( data_->suf, r.x, r.y, r.w, r.h );
    }

    inline void
    SmartSurface::Draw( const sp_Graphic& g, const Pos& p )
    {
      g->Draw( *this, p );
    }

    inline void
    SmartSurface::Draw( const sp_Graphic& g )
    {
      g->Draw( *this, Pos(0, 0) );
    }

    inline void
    SmartSurface::Lock()
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::Lock(): called to null pointer." );
      SDL_LockSurface(data_->suf);
    }

    inline void
    SmartSurface::Unlock()
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::Unlock(): called to null pointer." );
      SDL_UnlockSurface(data_->suf);
    }

    inline bool
    SmartSurface::SetAlpha( Uint32 flag, Uint8 alpha )
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::SetAlpha(): called to null pointer." );
      return SDL_SetAlpha( data_->suf, flag, alpha ) == 0;
    }

    inline bool
    SmartSurface::SetColorKey( Uint32 flag, Uint32 key )
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::SetColorKey(): called to null pointer." );
      return SDL_SetColorKey( data_->suf, flag, key ) == 0;
    }

    inline SmartSurface
    SmartSurface::Duplicate( Uint32 flags )
    {
      if ( !data_->suf )
	throw Error( "SmartSurface::Duplicate(): called to null pointer." );
      Lock();
      SDL_PixelFormat* format = data_->suf->format;
      SmartSurface temp( SDL_ConvertSurface(data_->suf,format,flags ) );
      Unlock();
      return temp;
    }

    inline SmartSurface
    SmartSurface::New( Uint32 w, Uint32 h )
    {
      return New( SDL_HWSURFACE, w, h );
    }

    inline SmartSurface
    SmartSurface::New( Uint32 flags, Uint32 w, Uint32 h )
    {
      GetVideoSurface();
      Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
      rmask = 0xff000000;
      gmask = 0x00ff0000;
      bmask = 0x0000ff00;
      amask = 0x000000ff;
#else
      rmask = 0x000000ff;
      gmask = 0x0000ff00;
      bmask = 0x00ff0000;
      amask = 0xff000000;
#endif

      SDL_Surface* temp = SDL_CreateRGBSurface(flags, w, h, 32, rmask, gmask, bmask, amask);
      if (!temp)
	throw Error();
      return SmartSurface( temp );
    }

    inline void
    SmartSurface::dec_ref()
    {
      if ( data_ ) {
	if ( --data_->count == 0 ) {
	  if ( data_->owner )
	    SDL_FreeSurface( data_->suf );
	  delete data_;
	  data_ = 0;
	}
      }
    }

    inline void
    SmartSurface::inc_ref()
    {
      if ( data_ ) {
	data_->count++;
      }
    }

    inline SmartSurface::~SmartSurface()
    {
      dec_ref();
    }

    //======================================================================
    // 以下 Font の実装
    inline bool
    Font::Init()
    {
      return TTF_Init() == 0;
    }

    inline bool
    Font::WasInit()
    {
      return TTF_WasInit();
    }

    inline void
    Font::Quit()
    {
      TTF_Quit();
    }

    inline Font::Font( const std::string& name, int size )
      : font_(TTF_OpenFont(name.c_str(), size))
    {
      if ( !font_ )
	throw Error("Font::Font");
    }

    inline sp_Font
    Font::Create( const std::string& name, int size )
    {
      return sp_Font( new Font(name, size) );
    }

    inline SmartSurface
    Font::RenderSolid( const std::string& str, const Color& fg )
    {
      return SmartSurface( TTF_RenderUTF8_Solid(font_, str.c_str(), fg) );
    }

    inline SmartSurface
    Font::RenderShaded( const std::string& str, const Color& fg, const Color& bg )
    {
      return SmartSurface( TTF_RenderUTF8_Shaded(font_, str.c_str(), fg, bg) );
    }

    inline SmartSurface
    Font::RenderBlended( const std::string& str, const Color& fg )
    {
      return SmartSurface( TTF_RenderUTF8_Blended(font_, str.c_str(), fg) );
    }

    inline SmartSurface
    Font::RenderSolidV( const std::string& str, const Color& fg )
    {
      return SmartSurface( TTF_RenderUTF8_Solid(font_, str.c_str(), fg) );
    }

    inline SmartSurface
    Font::RenderShadedV( const std::string& str, const Color& fg, const Color& bg )
    {
      return SmartSurface( TTF_RenderUTF8_Shaded(font_, str.c_str(), fg, bg) );
    }

    inline SmartSurface
    Font::RenderBlendedV( const std::string& str, const Color& fg )
    {
      return SmartSurface( TTF_RenderUTF8_Blended(font_, str.c_str(), fg) );
    }

    inline bool
    Font::Size( const std::string& str, int& w, int& h )
    {
      return TTF_SizeUTF8(font_, str.c_str(), &w, &h );
    }

    inline int
    Font::Height()
    {
      return TTF_FontHeight(font_);
    }

    inline Font::~Font()
    {
      TTF_CloseFont(font_);
    }

    //======================================================================
    // 以下 GraphicBase の実装
    inline BasicPos<Uint32>
    GraphicBase::GetWH() const
    {
      BasicPos<Uint32> temp;
      GetWH( temp.x, temp.y );
      return temp;
    }

    inline Pos
    GraphicBase::GetOrigin() const
    {
      Pos temp;
      GetOrigin( temp );
      return temp;
    }

    //======================================================================
    // 以下 図形の実装
    inline sp_RectGraphic
    RectGraphic::Create()
    {
      return sp_RectGraphic( new RectGraphic );
    }

    inline sp_RectGraphic
    RectGraphic::Create( Uint32 w, Uint32 h, Uint32 l, Color color, bool fill )
    {
      return sp_RectGraphic( new RectGraphic( w, h, l, color, fill ) );
    }

    inline sp_RectGraphic
    RectGraphic::Create( Uint8 w, Uint8 h, Uint8 l, bool fill )
    {
      return sp_RectGraphic( new RectGraphic( w, h, l, fill ) );
    }

    inline void
    RectGraphic::Draw( const SDL_Rect&, const SmartSurface&, const Pos& ) const
    {
      //未実装
      throw Error( "RectGraphic::Draw(const SDL_Rect&, const SmartSurface&, const Pos&) is not implemented yet." );
    }

    inline void
    RectGraphic::Draw( const SmartSurface& suf, const Pos& p ) const
    {
      Uint32 hline = line_ / 2;
      Uint32 mod = line_ % 2;
      Uint32 color = suf.Map( color_ );
      if ( fill_ ) {
	SDL_Rect re;
	re.x = - origin_.x + p.x - hline;
	re.y = - origin_.y + p.y - hline;
	re.w = width_ + line_ - 1;
	re.h = height_ + line_ - 1;
	SDL_FillRect( suf.Get(), &re, color );
      } else {
	SDL_Rect re;
	// 左
	re.x = - origin_.x + p.x - hline;
	re.y = - origin_.y + p.y - hline;
	re.w = line_;
	re.h = height_ + line_ - 1;
	SDL_FillRect( suf.Get(), &re, color );
	// 上
	re.x += line_;
	re.w = width_ - 1;
	re.h = line_;
	SDL_FillRect( suf.Get(), &re, color );
	// 下
	re.y += height_ - 1;
	SDL_FillRect( suf.Get(), &re, color );
	// 右
	re.x = - origin_.x + p.x + width_ - hline - 1;
	re.y = - origin_.x + p.y + hline + mod;
	re.w = line_;
	re.h = height_ - line_ - 1;
	SDL_FillRect( suf.Get(), &re, color );
      }
    }

    inline BasicPos<Uint32>
    RectGraphic::GetWH() const
    {
      return BasicPos<Uint32>( width_, height_ );
    }

    inline void
    RectGraphic::GetWH( Uint32& w, Uint32& h ) const
    {
      w = width_;
      h = height_;
    }

    inline Pos
    RectGraphic::GetOrigin() const
    {
      return origin_;
    }

    inline void
    RectGraphic::GetOrigin( Pos& p ) const
    {
      p = origin_;
    }

    inline void
    RectGraphic::GetColor( Color& c ) const
    {
      c = color_;
    }

    inline void
    RectGraphic::SetWH( Uint32 w, Uint32 h )
    {
      width_ = w;
      height_ = h;
    }

    inline void
    RectGraphic::SetOrigin( const Pos& p )
    {
      origin_ = p;
    }

    inline void
    RectGraphic::SetColor( const Color& c )
    {
      color_ = c;
    }

    //======================================================================
    // 以下 サーフェス画像の実装
    inline SurfaceGraphic::SurfaceGraphic( const SmartSurface& suf )
      : src_( suf ), srcrect_( suf.GetRect() ) {}

    inline SurfaceGraphic::SurfaceGraphic( const SmartSurface& suf, const SDL_Rect& rect )
      : src_( suf ), srcrect_( rect & suf.GetRect() ) {}

    inline SurfaceGraphic::SurfaceGraphic( const SmartSurface& suf, const SDL_Rect& rect,
					   const Pos& origin )
      : src_( suf ), srcrect_( rect & suf.GetRect() ), origin_( origin ) {}

    inline sp_SurfaceGraphic
    SurfaceGraphic::Create()
    {
      return sp_SurfaceGraphic( new SurfaceGraphic );
    }

    inline sp_SurfaceGraphic
    SurfaceGraphic::Create( const SmartSurface& suf )
    {
      return sp_SurfaceGraphic( new SurfaceGraphic( suf ) );
    }

    inline sp_SurfaceGraphic
    SurfaceGraphic::Create( const SmartSurface& suf, const SDL_Rect& rect )
    {
      return sp_SurfaceGraphic( new SurfaceGraphic( suf, rect ) );
    }

    inline sp_SurfaceGraphic
    SurfaceGraphic::Create( const SmartSurface& suf, const SDL_Rect& rect, const Pos& origin )
    {
      return sp_SurfaceGraphic( new SurfaceGraphic( suf, rect, origin ) );
    }

    inline void
    SurfaceGraphic::Draw( const SDL_Rect& rect, const SmartSurface& dst, const Pos& pos ) const
    {
      if ( !src_ )
	throw Error( "SurfaceGraphic::Draw: Surface is not set." );

      if ( !Overlap(rect, srcrect_) )
	return;

      SDL_Rect srcrect, dstrect;
      srcrect = rect & srcrect_;
      dstrect.x = pos.x - origin_.x;
      dstrect.y = pos.y - origin_.y;
      if ( SDL_BlitSurface( src_.Get(), &srcrect, dst.Get(), &dstrect ) == -1 )
	throw Error( "SurfaceGraphic::Draw: SDL_BlitSurface failed." );
    }

    inline void
    SurfaceGraphic::Draw( const SmartSurface& dst, const Pos& pos ) const
    {
      if ( !src_ )
	throw Error( "SurfaceGraphic::Draw: Surface is not set." );

      SDL_Rect srcrect, dstrect;
      srcrect = srcrect_;
      dstrect.x = pos.x - origin_.x;
      dstrect.y = pos.y - origin_.y;
      if ( SDL_BlitSurface( src_.Get(), &srcrect, dst.Get(), &dstrect ) == -1 )
	throw Error( "SurfaceGraphic::Draw: SDL_BlitSurface failed." );
    }

    inline BasicPos<Uint32>
    SurfaceGraphic::GetWH() const
    {
      return BasicPos<Uint32>(srcrect_.w,srcrect_.h);
    }

    inline void
    SurfaceGraphic::GetWH( Uint32& w, Uint32& h ) const
    {
      w = srcrect_.w;
      h = srcrect_.h;
    }

    inline Pos
    SurfaceGraphic::GetOrigin() const
    {
      return origin_;
    }

    inline void
    SurfaceGraphic::GetOrigin( Pos& p ) const
    {
      p = origin_;
    }

    inline void
    SurfaceGraphic::GetSurface( SmartSurface& suf ) const
    {
      suf = src_;
    }

    inline void
    SurfaceGraphic::SetRect( const SDL_Rect& rect )
    {
      srcrect_ = src_.GetRect() & rect;
    }

    inline void
    SurfaceGraphic::SetOrigin( const Pos& p )
    {
      origin_ = p;
    }

    inline void
    SurfaceGraphic::SetSurface( const SmartSurface& suf )
    {
      src_ = suf;
      srcrect_ = suf.GetRect();
    }

    inline SmartSurface
    SurfaceGraphic::GetSurface()
    {
      return src_;
    }

    inline void
    SurfaceGraphic::GetRect( SDL_Rect& rect ) const
    {
      rect = srcrect_;
    }

  }
}

#endif // EDW_SDL_HPP
