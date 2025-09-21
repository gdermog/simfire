//****************************************************************************************************
//! \file SimFireGlobals.h                                                                          
//! Module contains ...                                                                             
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_SimFireGlobals
#define H_SimFireGlobals

#include <vector>
#include <set>
#include <map>

namespace SimFire
{

  using StrVect_t = std::vector<std::string>;
  //!< Vector of strings

  using WStrVect_t = std::vector<std::wstring>;
  //!< Vector of wide strings

  using StrSet_t = std::set<std::string>;
  //!< Set of strings

  using WStrSet_t = std::set<std::wstring>;
  //!< Set of wide strings

  template<typename T>
  using StrMap_t = std::map<std::string, T>;
  //!< Map indexed by string

  //! Typy tokenů (např. při čtení z řetězce)
  enum class NumberType_t: unsigned short
  {
    kNothing            = 0x0000,
    kWhitespace         = 0x0001,
    kIndexNumeric       = 0x0002,
    kIntegerNumeric     = 0x0004,
    kHexaNumeric        = 0x0008,
    kFloatNumeric       = 0x0010,
    kScientificNumeric  = 0x0020,
    kComplexNumeric     = 0x0040,
  };

  inline bool IsZero( double_t val, double_t tol = 1e-12 ) { return ( ( -tol < val ) && ( val < tol ) ); }

  constexpr unsigned gPrintoutIdWidth = 20;
  //!< 

  constexpr unsigned gHelpMarginWidth = 5;

  constexpr unsigned gHelpItemWidth = 20;

} // namespace SimFire

#endif
