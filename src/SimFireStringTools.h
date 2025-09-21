//****************************************************************************************************
//! \file SimFireStringTools.h                                                                      *
//! Module contains convenient function for working with c-strings and std::string objects          *
//****************************************************************************************************
//*                                                                                                  *
//****************************************************************************************************
//* 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     *
//****************************************************************************************************

#ifndef H_SimFireStringTools
#define H_SimFireStringTools

#include <iostream>

#include <SimFireGlobals.h>

namespace SimFire
{

  extern const std::string gDefaultTrimChars;

  extern const char * gEmptyCharPtr;

  extern const char * gTrueName;

  extern const char * gFalseName;

  inline bool IsNumberChar( unsigned char ch ) { return ( 48 <= ch && ch <= 57 ); }

  inline bool IsStandardChar( unsigned char ch )
  { return ( 64 <= ch && ch <= 90 ) || ( 97 <= ch && ch <= 122 ) || ch == '_'; }

  //! Vrátí <b>true</b> je-li vstupní znak normální znak nebo číslice (0-9, A-Z, a-z, _).
  inline bool IsNormalChar( unsigned char ch ) { return IsNumberChar( ch ) || IsStandardChar( ch ); }

  inline bool IsEmpty( const char * str ) { return ( nullptr == str || 0 == *str ); }
  //!< Vrátí true je-li řetězec prázdný (nullptr nebo začíná terminátorem)

  inline const char * NullEmptyStr( const char * val ) { return( nullptr == val ? gEmptyCharPtr : val ); }
  //!< Returns constant c-string pointer \e val or "" if \e val is nullptr

  NumberType_t IsNumeric( const char * str );
  //!< Vrátí číselné možnosti výčtu TokenType_t, tj. Index, Hexa, Integer, Float nebo Complex.
  //!  Pro rychlost nepoužívá regulární výrazy, ale pouze typy znaků v řetězci, tj. například
  //!  řetězce "-10", "1-0" i "10-" budou vyhodnoceny jako Integer. Pro důkladnější kontrolu
  //!  (např při implementaci validátorů na vstupu od uživatele) je třeba použít nějakou lepší
  //!  metodu. Viz TokenType_t v ServiceEnumsFlags.h

  int IsSeparator( char character, const char * separators );
  //!< Zjistí, zda daný znak je obsažen v daném řetězci. Vrátí -1 pokud ho nenašel, jinak jeho index

  bool IEquals( const std::string & a, const std::string & b );

  void Trim( 
    const char * strin, 
    size_t & first, 
    size_t & last, 
    size_t len = 0,
    const std::string & chars = gDefaultTrimChars );
  //!< Zjistí začátek a konec řetězce, ignorují-li se zadané znaky

  std::string & Trim( std::string & s, const std::string & chars = gDefaultTrimChars );

  bool StartsWith( std::string const & haystack, std::string const & needle );
  //!< Vrátí <b>true</b> pokud c++ řetězec <i>haystack</i> začíná na C++ řetězec <i>needle</i>.

  bool EndsWith( std::string const & haystack, std::string const & needle );
  //!< Vrátí <b>true</b> pokud c++ řetězec <i>haystack</i> končí na C++ řetězec <i>needle</i>.

  size_t Unescape( 
    std::string & data, 
    bool nlq = true, 
    std::string * externalBuffer = nullptr );
  /*!< \brief Looks through input string for C/C++ escaped metacharacters (as \\t or \\n) and unescapes
       them (to \t or \n)

       \param[in] data  String to be altered
       \param[in] nlq   Quotes and newlines are ignored if false
       \returns         Celkový počet nahrazených znaků */

  void SplitLine( 
    StrVect_t & output, 
    const char * line, 
    const char * separators,
    const char * glues = nullptr, 
    bool leaveBlanks = false, 
    bool trim = false );
  /*!< Splits a string into its components based on the specified delimiters and returns it as
       a vector of std::string strings.

  \param[out] output      Resulting array
  \param[in] line         Original text
  \param[in] separators   List of delimiters
  \param[in] glues        List of characters preceding non-separable strings
  \param[in] leaveBlanks  If <b>true</b>, leaves blank items in the output array
  \param[in] trim         If <b>true</b>, trims all items by whitespace before storing them in the output */

  std::string JoinStrings( const StrVect_t & elements, const char * glue );
  /*!< \brief Spojí řetězecový vektor do jednoho řetězce. Pro pospojování použije zadané znaky. */

} // namespace SimFire

#endif
