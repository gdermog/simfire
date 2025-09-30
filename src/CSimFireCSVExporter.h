//****************************************************************************************************
//! \file CSimFireSCVExporter.h                                                                        
//! Module contains declaration of CSimFireSCVExporter class, which exports state of single simulation
//! run into CSV file.                                
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#ifndef H_CSimFireCSVExporter
#define H_CSimFireCSVExporter

#include <CSimFireSettings.h>
#include <CSimFireSingleRunParams.h>
#include <CSimFireSingleRun.h>

namespace SimFire
{

  /*! \brief This class  exports state of single simulation run into CSV file. Exported data can 
      be further visualized in spreadsheet applications or by gnuplot. */
  class CSimFireCSVExporter
  {

  public:

    //------------------------------------------------------------------------------------------------
    //! @name Constructors, destructor, clonning, assign operators                                   
    //@{----------------------------------------------------------------------------------------------

    CSimFireCSVExporter(
      const CSimFireSettings& settings);

    virtual ~CSimFireCSVExporter();

		bool NewFile( const std::string& filename );

    void DoExportState(
      double_t x, double_t y, double_t z,
      double_t vX, double_t vY, double_t vZ,
      double_t dist, double_t t,
      bool raising, bool below, bool nearHalfPlane);


    //@}---------------------------------------------------------------------------------------------
    //! @name Public setup                                                                            
    //@{----------------------------------------------------------------------------------------------

		bool mShowX;          //!< If true, X coordinate is exported
		bool mShowY;          //!< If true, Y coordinate is exported
		bool mSHowXY;         //!< If true, horizontal distance XY is exported
		bool mShowZ;          //!< If true, Z coordinate is exported
		bool mShowvX;         //!< If true, velocity component vX is exported
		bool mShowvY;         //!< If true, velocity component vY is exported
		bool mSHowvXY;        //!< If true, horizontal velocity vXY is exported
		bool mShowvZ;         //!< If true, velocity component vZ is exported
		bool mShowDistance;   //!< If true, distance from target is exported
		bool mShowTime;       //!< If true, time from shot is exported
		bool mShowFlags;      //!< If true, flags (raising, below, nearHalfPlane) are exported


    //@}---------------------------------------------------------------------------------------------
    //! @name Protected data                                                                            
    //@{----------------------------------------------------------------------------------------------

  protected:

    const CSimFireSettings& mSettings;
    //!< Reference to settings object, which contains all parameters entered by user on startup

    std::ofstream mCSVFile;
		//!< Output CSV file stream

    //@}

  };

} // namespace SimFire

#endif