//****************************************************************************************************
//! \file CSimFireConfig.h                                                                          *
//! Module contains declaration of CSimFireConfig class, which ...                                  *
//****************************************************************************************************
//*                                                                                                  *
//****************************************************************************************************
//* 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     *
//****************************************************************************************************

#ifndef H_CSimFireConfig
#define H_CSimFireConfig

#include <fstream>
#include <regex>

#include <SimFireGlobals.h>

namespace SimFire
{

  //***** CSimFireConfig *****************************************************************************

  /*!  \brief Brief description of the class

  Detailed description of the class \n\n

  <b>Remark.:</b> ...

  */
  class CSimFireConfig
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CSimFireConfig();

    virtual ~CSimFireConfig();

   //@}-----------------------------------------------------------------------------------------------
   //! @name Public methods 
   //@{-----------------------------------------------------------------------------------------------

    void Preprint( std::ostream & out = std::cout );

    bool ParseCommandLine( int argc, char * const argv[] );

    bool ParseINIFile( std::ifstream & in, size_t &lastLineRead );

    std::string GetValueStr(
      const std::string & inSect,
      const std::string & inItem,
      const std::string & inDefault = {} ) const;

    bool GetValueBool(
      const std::string & inSect,
      const std::string & inItem,
      bool  inDefault = false ) const;

    double_t GetValueDouble(
      const std::string & inSect,
      const std::string & inItem,
      double_t  inDefault = 0.0 ) const;

    int64_t GetValueInteger(
      const std::string & inSect,
      const std::string & inItem,
      int64_t  inDefault = 0 ) const;

    uint64_t GetValueUnsigned(
      const std::string & inSect,
      const std::string & inItem,
      uint64_t  inDefault = 0 ) const;

  protected:

    //@{}---------------------------------------------------------------------------------------------
    //! @name Protected data                                                                         
    //@{----------------------------------------------------------------------------------------------

    using CfgItem_t = struct
    {
      StrVect_t Value;
    };

    using CfgItemList_t = StrMap_t<CfgItem_t>;

    StrMap_t<CfgItemList_t> mCfgContent;

    const char * mSeparators;
    const char * mSeparatorsSpace;
    const char * mGlues;

    std::regex mRegexINILineEmptyCommented;
    //!< INI file content: regex for empty line with comment

    std::regex mRegexLineEmpty;
    //!< regex for empty variable value or ini file line without comment

    std::regex mRegexINISectionCommented;
    //!< INI file content: regex for section definition with comment

    std::regex mRegexINISection;
    //!< INI file content: regex for section definition without comment

    //@{}---------------------------------------------------------------------------------------------
    //! @name Protected methods
    //@{----------------------------------------------------------------------------------------------

    bool ParseINIKeyValuePair(
      const std::string & inLine, const std::string & actSection );

    bool InsertItem(
      const std::string & in_ai8Sect,
      const std::string & in_ai8Item,
      const StrVect_t & in_ai8ItemVal );
 
    //@}

  }; // CSimFireConfig

} // namespace SimFire

#endif
