//****************************************************************************************************
//! \file SimFireGlobals.h                                                                          
//! Module contains some basics types and global constants used in other modules.                                                                            
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_SimFireGlobals
#define H_SimFireGlobals

#include <cmath> 
#include <cstdint> 
#include <string>
#include <memory>
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

  //! Number types
  enum class NumberType_t: unsigned short
  {
    kNothing            = 0,
    kIndexNumeric       = 1,
    kIntegerNumeric     = 2,
    kHexaNumeric        = 3,
    kFloatNumeric       = 4,
    kScientificNumeric  = 5,
    kComplexNumeric     = 6,
  };

  constexpr double_t gAlmostZero = 1E-12;
  //!< Specifies the size of a number that is already considered zero

  inline bool IsZero( double_t val, double_t tol = gAlmostZero ) { return ( ( -tol <= val ) && ( val <= tol ) ); }
	//!< Returns true if the value is zero within given tolerance

  inline bool IsPositive(double_t val, double_t tol = gAlmostZero ) { return ( tol < val ); }
	//!< Returns true if the value is positive within given tolerance

  inline bool IsNegative(double_t val, double_t tol = gAlmostZero) { return ( val < -tol ); }
  //!< Returns true if the value is negative within given tolerance

  constexpr unsigned gPrintoutIdWidth = 20;
	//!< Standard width for printout identifiers

  constexpr unsigned gHelpMarginWidth = 5;
	//!< Standard left margin for commandline help printout

  constexpr unsigned gHelpItemWidth = 20;
	//!< Standard width for commandline help items

  constexpr double_t gPI = 3.141592653589793;
  //!< Value of pi constant - 3.1415926535897932384626433832795 ...

} // namespace SimFire

#endif
