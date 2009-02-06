// last modified <2004/02/02 14:44:13 2004 JST>
#ifndef ZUGAGAGA_ZTK_HPP
#define ZUGAGAGA_ZTK_HPP

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include "edw/basic_structs.hpp"

namespace zu3ga {
  namespace ztk {
    class ClickableBase
    {
    public:
      virtual Rect GetRect() = 0;
      virtual void Click() = 0;
      virtual ~ClickableBase() {}
    };
    typedef boost::shared_ptr<ClickableBase> sp_Clickable;
    typedef boost::weak_ptr<ClickableBase> wp_Clickable;

    class ClickableManager
    {
    public:
      void AddClickable( const sp_Clickable& c ) { cset_.insert(c); }
      void Click( const edw::basic_structs::Pos& p )
      {
	for (std::set<wp_Clickable>::iterator i = cset_.begin(); i != cset_.end(); ) {
	  if ( sp_Clickable t = (*i).lock() ) {
	    if ( t->GetRect().Include(p) ) {
	      t->Click();
	      break;
	    }
	    ++i;
	  } else {
	    cset_.erase(i++);
	  }
	}
      }
    private:
      std::set<wp_Clickable> cset_;
    };
    typedef boost::shared_ptr<ClickableManager> sp_ClickableManager;

    class FocusPart
    {
    public:
      virtual void SetFocus()
      {
	if ( onFocused_ )
	  onFocused_();
	focus_ = true;
      }
      virtual void ResetFocus()
      {
	if ( onUnfocused_ )
	  onUnfocused_();
	focus_ = false;
      }
      virtual bool HaveFocus() const { return focus_; }

      FocusPart() : focus_( false ) {}
      virtual ~FocusPart() {}

    protected:
      boost::function<void()> onFocused_;
      boost::function<void()> onUnfocused_;

    private:
      bool focus_;
    };
    typedef boost::shared_ptr<FocusPart> sp_FocusPart;

    //----------------------------------------------------------------------
    // フォーカス切り替え器
    template<class T> class FocusManager : public FocusPart
    {
    public:
      FocusManager() : in_(0){}
      void Add( const boost::shared_ptr<T>& prt )
      {
	if ( prt_.empty() )
	  prt->SetFocus();
	prt_.push_back(prt);
      }

      void Next()
      {
	int tmp = in_;
	++in_ %= prt_.size();
	prt_[tmp]->ResetFocus();
	prt_[in_]->SetFocus();
      }

      void Prev()
      {
	int tmp = in_;
	if ( --in_ == -1 )
	  in_ = prt_.size() - 1;
	prt_[tmp]->ResetFocus();
	prt_[in_]->SetFocus();
      }

      boost::shared_ptr<T> GetFocus()
      {
	return prt_[in_];
      }

    private:
      std::vector<boost::shared_ptr<T> > prt_;
      int in_;
    };
  }
}

#endif // ZUGAGAGA_ZTK_HPP
