//****************************************************************************************************
//! \file CSimFireConfig.cpp                                                                        *
//! Module contains definitions of CSimFireConfig class, declared in CSimFireConfig.h, which ...    *
//****************************************************************************************************
//*                                                                                                  *
//****************************************************************************************************
//* 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     *                   
//****************************************************************************************************

#include <iomanip>
#include <string>
#include <regex>

#include <SimFireStringTools.h>
#include <CSimFireConfig.h>

namespace SimFire
{

#define INI_KEY_CHARS R"([a-zA-Z0-9_@$.\|\\/])"
#define INI_COMMENT R"([#;])"

   //****** CCSimFireConfig **************************************************************************

   CSimFireConfig::CSimFireConfig():
     mCfgContent(),
     mSeparators( ",|;" ),
     mSeparatorsSpace( " ,|;" ),
     mGlues( "\"\'" ),
     mRegexINILineEmptyCommented{ R"(^\s*)" INI_COMMENT R"((.*)$)" },
     mRegexLineEmpty{ R"(^\s*$)" },
     mRegexINISectionCommented{ R"(^\s*\[\s*()" INI_KEY_CHARS R"(*)\s*\]\s*)" INI_COMMENT R"((.*)$)" },
     mRegexINISection{ R"(^\s*\[\s*()" INI_KEY_CHARS R"(*)\s*\]\s*$)" }
   {

   } /* CSimFireConfig::CSimFireConfig */


   //-------------------------------------------------------------------------------------------------

   CSimFireConfig::~CSimFireConfig() = default;

   //-------------------------------------------------------------------------------------------------

   std::ostream & CSimFireConfig::Preprint( std::ostream & out  )
   { 

     for( auto &section: mCfgContent )
     {
        if( !section.first.empty() )
          out << "[" << section.first << "]" << std::endl;
        for( auto & item : section.second )
        {
          out << std::setw( gPrintoutIdWidth ) << std::left << item.first + ":"
              << JoinStrings( item.second.Value, ", " ) << std::endl;
        } // for
        out << std::endl;
     } // for

     return out;

   } // CSimFireConfig::Preprint


   //-------------------------------------------------------------------------------------------------

   bool CSimFireConfig::ParseCommandLine( int argc, char * const argv[] )
   {

     bool firstTokenFound = false;

     std::string lastKey;
     std::vector<std::string> lastVal;
     std::string lastValJoined;
     //std::string actToken;

     StrVect_t splittedLine;

     try
     {

       for( int32_t loop = 0; loop < argc; ++loop )
       {

         SplitLine( splittedLine, argv[loop], mSeparators, mGlues, false, true);
         // Tokenizer dismantles strings returned by OS. On the commandline there is
         // usually only space character designated as delimiter, but we need also
         // ';', ',' and '|' delimiters.

         for( auto & tIter : splittedLine )
         {

           Unescape( tIter );
           // Any metacharacters should be transformed to apropriate special chars

           bool dblM = StartsWith( tIter, "--" );
           bool snglM = StartsWith( tIter, "-" );
           if( dblM || snglM )
           {            // This token is new key - previous value have to be stored
             if( firstTokenFound )
             {
               if( lastVal.empty() )
               {        // check if its not just an argument (true false value for --help)
                 lastVal.emplace_back( gTrueName );
                 if( !InsertItem( {}, lastKey, { gTrueName } ) )
                   return false;
               } // if
               else
               {
                 if( !InsertItem( {}, lastKey, lastVal ) ) return false;
               } // else
             } // if

             if( dblM )
               lastKey = tIter.substr( 2 );
             else
               lastKey = tIter.substr( 1 );

             lastVal.clear();
             firstTokenFound = true;

           } // if
           else
           {            // This token is part of actual value   

             if( ( StartsWith( tIter, "\"" ) && EndsWith( tIter, "\"" ) ) ||
               ( StartsWith( tIter, "\'" ) && EndsWith( tIter, "\'" ) ) )
               tIter = tIter.substr( 1, tIter.size() - 2 );
                        // Quotes around whole item should be removed now

             if( !tIter.empty() ) lastVal.push_back( tIter );

           } /*else*/

         } /*for(tIter)*/

       } /*for*/

       if( !lastKey.empty() )
       {
         if( lastVal.empty() )
         {             // check if its not just an argument (true false value for --help)
           lastVal.emplace_back( gTrueName );
           if( !InsertItem( {}, lastKey, { gTrueName } ) )
             return false;
         } // if
         else
         {
           if( !InsertItem( {}, lastKey, lastVal ) ) 
             return false;
         } // else
       } // if
                        // Last key-value pair have to be stored as well

     } /*try*/
     catch( std::exception & e )
     {
       std::cerr << "Error reading command line parameters, quitting:" << std::endl;
       std::cerr << "  -> " << e.what() << std::endl;
       exit(-1);
     } // catch
     catch( ... )
     {
       std::cerr << "Error reading command line parameters, quitting" << std::endl;
       exit( -1 );
     } // catch

     return true;
   }

   //-------------------------------------------------------------------------------------------------

   bool CSimFireConfig::ParseINIFile( std::ifstream & in_pFile, size_t & lastLineRead )
   {

     if( !in_pFile.is_open() )
       return false;

     lastLineRead = 0;

     std::string actSection;
     std::string inLine;
     std::smatch what;

     while( !in_pFile.eof() )
     {
       ++lastLineRead;
       std::getline( in_pFile, inLine );

       if( inLine.empty() )
         continue;      // Empty line, skipping

       if( std::regex_match( inLine, mRegexINILineEmptyCommented) )
         continue;      // Comment line, skipping

       if( std::regex_match( inLine, what, mRegexINISectionCommented ) )
       {                // Section headers (with comment)
         actSection = what[1];
         continue;
       } // if

       if( std::regex_match( inLine, what, mRegexINISection ) )
       {                // Section headers (without comment)
         actSection = what[1];
         continue;
       } /*if*/

       if( !ParseINIKeyValuePair( inLine, actSection ) )
         return false;

     } // while

     return true;

   } // CSimFireConfig::ParseINIFile

   //-------------------------------------------------------------------------------------------------

   std::string CSimFireConfig::GetValueStr( 
     const std::string & inSect, 
     const std::string & inItem, 
     const std::string & inDefault ) const
   {
     auto sectIt = mCfgContent.find( inSect );
     if( sectIt == mCfgContent.end() ) 
       return { inDefault };

     auto itemIt = sectIt->second.find( inItem );
     if( itemIt == sectIt->second.end() )
       return { inDefault };

     return JoinStrings( itemIt->second.Value, ", ");

   } // CSimFireConfig::GetValueStr

   //-------------------------------------------------------------------------------------------------

   bool CSimFireConfig::GetValueBool( 
     const std::string & inSect, 
     const std::string & inItem, 
     bool inDefault ) const
   {
     auto strVal = GetValueStr( inSect, inItem, {} );

     if( strVal.empty() ) 
       return inDefault;

     auto strValType = IsNumeric( strVal.c_str() );

     if( NumberType_t::kIndexNumeric == strValType   ||
         NumberType_t::kIntegerNumeric == strValType ||
         NumberType_t::kHexaNumeric == strValType )
       return ( std::stoll( strVal, nullptr, 0 ) != 0 );
     
     return IEquals( strVal, gTrueName );

   } // CSimFireConfig::GetValueBool

   //-------------------------------------------------------------------------------------------------

   double_t CSimFireConfig::GetValueDouble( 
     const std::string & inSect, 
     const std::string & inItem, 
     double_t inDefault ) const
   {
     auto strVal = GetValueStr( inSect, inItem, {} );

     if( strVal.empty() )
       return inDefault;

     auto strValType = IsNumeric( strVal.c_str() );

     if( NumberType_t::kIndexNumeric == strValType ||
         NumberType_t::kIntegerNumeric == strValType ||
         NumberType_t::kHexaNumeric == strValType )
       return (double_t)std::stoll( strVal, nullptr, 0 );

     if( NumberType_t::kFloatNumeric == strValType ||
         NumberType_t::kScientificNumeric == strValType )
       return std::stod( strVal );

     return inDefault;
   } // CSimFireConfig::GetValueDouble

   //-------------------------------------------------------------------------------------------------

   int64_t CSimFireConfig::GetValueInteger(
     const std::string & inSect, 
     const std::string & inItem, 
     int64_t inDefault ) const
   {
     auto strVal = GetValueStr( inSect, inItem, {} );

     if( strVal.empty() )
       return inDefault;

     auto strValType = IsNumeric( strVal.c_str() );

     if( NumberType_t::kIndexNumeric == strValType ||
         NumberType_t::kIntegerNumeric == strValType ||
         NumberType_t::kHexaNumeric == strValType )
       return std::stoll( strVal, nullptr, 0 );

     return inDefault;
   } // CSimFireConfig::GetValueInteger

   //-------------------------------------------------------------------------------------------------


   uint64_t CSimFireConfig::GetValueUnsigned( 
     const std::string & inSect,
     const std::string & inItem, 
     uint64_t inDefault ) const
   {
     auto strVal = GetValueStr( inSect, inItem, {} );

     if( strVal.empty() )
       return inDefault;

     auto strValType = IsNumeric( strVal.c_str() );

     if( NumberType_t::kIndexNumeric == strValType ||
         NumberType_t::kIntegerNumeric == strValType ||
         NumberType_t::kHexaNumeric == strValType )
       return std::stoull( strVal, nullptr, 0 );

     return inDefault;
   } // CSimFireConfig::GetValueUnsigned

   //-------------------------------------------------------------------------------------------------

   bool CSimFireConfig::ParseINIKeyValuePair( 
     const std::string & inLine, const std::string & actSection )
   {
     bool readingKey = true;
     char readingQuotes = 0;

     std::string readedKey;
     StrVect_t readedVal;
     size_t startOfLastValSect = SIZE_MAX;

     size_t chIdx = 0;
     for( ; chIdx < inLine.size(); ++chIdx )
     {
       auto actChar = inLine[chIdx];
       if( actChar == 0 ) return false;

       if( actChar == readingQuotes )
       {
         if( inLine[chIdx - 1] != '\\' )
           readingQuotes = 0;
         continue;
       } // if

       if( readingQuotes != 0 )
         continue;

       if( actChar == '\"' || actChar == '\'' )
       {
         readingQuotes = actChar;
         continue;
       } // if

       if( actChar == '#' || actChar == ';' )
         break;         // Remark in INI file (rest of the line is ignored)

       if( ( actChar == '=' ) && readingKey )
       {                // First assign operator found ends key section and start values section
         readedKey = inLine.substr( 0, chIdx );
         startOfLastValSect = chIdx + 1;
         readingKey = false;
         continue;
       } // if

       if( ( actChar == ',' || actChar == '|' ) && !readingKey )
       {
         readedVal.push_back( inLine.substr( startOfLastValSect, chIdx - startOfLastValSect ) );
         startOfLastValSect = chIdx + 1;
       } // if

     } // for

     if( readingKey ) readedKey = inLine;
     else readedVal.push_back( inLine.substr( startOfLastValSect, chIdx - startOfLastValSect ) );

     Trim( readedKey );
     if( ( StartsWith( readedKey, "\"" ) && EndsWith( readedKey, "\"" ) ) ||
       ( StartsWith( readedKey, "\'" ) && EndsWith( readedKey, "\'" ) ) )
     {
       Unescape( readedKey );
       readedKey = readedKey.substr( 1, readedKey.size() - 2 );
     }

     if( !readedKey.empty() )
     {
       InsertItem( actSection, readedKey, readedVal );
     }

     return true;

   } // CSimFireConfig::ParseINIKeyValuePair

   //-------------------------------------------------------------------------------------------------

   bool CSimFireConfig::InsertItem(
     const std::string & in_ai8Sect, 
     const std::string & in_ai8Item, 
     const StrVect_t & in_ai8ItemVal )
   {
     if( in_ai8Item.empty() )
       return false;

     auto & item = mCfgContent[ in_ai8Sect][in_ai8Item];

     item.Value = in_ai8ItemVal;
     for( auto &item: item.Value )
       Trim( item );
     return true;

   } // CSimFireConfig::InsertItem

} // namespace SimFire
