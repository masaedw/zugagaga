#ifndef ZUGAGAGA_VERBOSE_HPP
#define ZUGAGAGA_VERBOSE_HPP

#include <iostream>

namespace zu3ga {
  /**
     ログ等出力用関数群
  */
  namespace verbose {
    /** うるささレベル取得

    大きい方がよりうるさい
    */
    unsigned int getVerboseLevel();
    /** うるささレベル設定
	@return それまでのうるささレベル
    大きい方がよりうるさい
    */
    unsigned int setVerboseLevel( unsigned int );

    /** うるささ調整出力
	@param i うるささレベル これより大きいうるささレベルの時に出力
    */
    inline void puts(unsigned int i) {
      if ( i <= getVerboseLevel() ) std::cout << std::endl;
    }

    /** うるささ調整出力 */
    template <class T>
    void puts(unsigned int i, const T& a) {
      if ( i <= getVerboseLevel() ) std::cout << a << std::endl;
    }

    /** うるささ調整出力 */
    template <class T, class U>
    void puts(unsigned int i, const T& a, const U& b) {
      if ( i <= getVerboseLevel() ) std::cout << a << ": " << b << std::endl;
    }

    /** うるささ調整出力 */
    template <class T, class U, class V>
    void puts(unsigned int i, const T& a, const U& b, const V& c) {
      if ( i <= getVerboseLevel() ) std::cout << a << ": " << b << ": " << c << std::endl;
    }

    /** うるささ調整エラー出力 */
    template <class T>
    void errPuts(unsigned int i, const T& a) {
      if ( i <= getVerboseLevel() ) std::cerr << a << std::endl;
    }

    /** うるささ調整エラー出力 */
    template <class T, class U>
    void errPuts(unsigned int i, const T& a, const U& b) {
      if ( i <= getVerboseLevel() ) std::cerr << a << ": " << b << std::endl;
    }

    /** うるささ調整エラー出力 */
    template <class T, class U, class V>
    void errPuts(unsigned int i, const T& a, const U& b, const V& c) {
      if ( i <= getVerboseLevel() ) std::cerr << a << ": " << b << ": " << c << std::endl;
    }

    /** うるささ調整出力 */
    template <class T>
    void print(unsigned int i, const T& a) {
      if ( i <= getVerboseLevel() ) std::cout << a << std::flush;
    }

    /** うるささ調整出力 */
    template <class T, class U>
    void print(unsigned int i, const T& a, const U& b) {
      if ( i <= getVerboseLevel() ) std::cout << a << ": " << b << std::flush;
    }

    /** うるささ調整出力 */
    template <class T, class U, class V>
    void print(unsigned int i, const T& a, const U& b, const V& c) {
      if ( i <= getVerboseLevel() ) std::cout << a << ": " << b << ": " << c << std::flush;
    }

    /** うるささ調整エラー出力 */
    template <class T>
    void errPrint(unsigned int i, const T& a) {
      if ( i <= getVerboseLevel() ) std::cerr << a << std::flush;
    }

    /** うるささ調整エラー出力 */
    template <class T, class U>
    void errPrint(unsigned int i, const T& a, const U& b) {
      if ( i <= getVerboseLevel() ) std::cerr << a << ": " << b << std::flush;
    }

    /** うるささ調整エラー出力 */
    template <class T, class U, class V>
    void errPrint(unsigned int i, const T& a, const U& b, const V& c) {
      if ( i <= getVerboseLevel() ) std::cerr << a << ": " << b << ": " << c << std::flush;
    }

  }
}

#endif // ZUGAGAGA_VERBOSE_HPP
