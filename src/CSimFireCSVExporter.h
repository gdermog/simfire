//****************************************************************************************************
//! \file CSimFireSCVExporter.h                                                                        
//! Module contains declaration of CSimFireSCVExporter class, which exports progress of single simulation
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

  /*! \brief This class  exports progress of single simulation run into CSV file. Exported data can 
      be further visualized in spreadsheet applications or by gnuplot (see showshoot.gpl script). */
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
    /*!< \brief Opens new CSV file and writes header line into it
    * 
         \param[in] filename Name of the CSV file to be created
				 \return true if file was successfully */

    void DoExportState(
      double_t x, double_t y, double_t z,
      double_t vX, double_t vY, double_t vZ,
      double_t dist, double_t t,
      bool raising, bool below, bool nearHalfPlane);
		/*!< \brief Exports current state of the simulation into CSV file
    
         \param[in] x             X coordinate of the projectile [m]
         \param[in] y             Y coordinate of the projectile [m]
         \param[in] z             Z coordinate of the projectile [m]
         \param[in] vX            X component of the projectile velocity [m/s]
         \param[in] vY            Y component of the projectile velocity [m/s]
         \param[in] vZ            Z component of the projectile velocity [m/s]
         \param[in] dist          Distance of the projectile from the target [m]
         \param[in] t             Time from shot [s]
         \param[in] raising       True if the projectile is rising (vZ > 0)
         \param[in] below         True if the projectile is below the target (z < targetZ)
				 \param[in] nearHalfPlane True if the projectile is between shooter and target */


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