//****************************************************************************************************
//! \file main.cpp                                                                                  
//! Module contains entrypoint for ...                                                              
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include <iostream>
#include <iomanip>
#include <filesystem>

#include <SimFireGlobals.h>
#include <CSimFireConfig.h>
#include <CSimFireSettings.h>
#include <CSimFireCore.h>

//******* Command line help **************************************************************************

std::ostream & HlpLine()
{
  static const std::string hMargin( SimFire::gHelpMarginWidth, ' ' );
  return std::cout << hMargin << std::setw( SimFire::gHelpItemWidth ) << std::left;
} // HlpLine

void PrintCommandlineHelp()
{

  std::cout << std::endl << std::endl;
  std::cout << "Command line expected values: " << std::endl << std::endl;

  HlpLine() << "--help"              << "Print this help" << std::endl;
  HlpLine() << "--setup <File name>" << "Path to INI file containing setup" << std::endl;

  std::cout << std::endl << std::endl;
  std::cout << "INI file expected values: " << std::endl << std::endl;

  std::cout << std::endl << std::endl;

} // PrintCommandlineHelp

//******* Main function ******************************************************************************

int main( int argc, char * argv[] )
{
  
  const std::string asteriskLine( 40, '*' );

  //------ Import command line arguments -------------------------------------------------------------

  SimFire::CSimFireConfig cfg;
  cfg.ParseCommandLine( argc, argv );

  if( cfg.GetValueBool( {}, "help" ) || cfg.GetValueBool( {}, "h" ) )
  {
    PrintCommandlineHelp();
    return 0;
  } // if

  //------ Import settings from configuration file ---------------------------------------------------

  auto inFileName = cfg.GetValueStr( {}, "setup" );
  if( inFileName.empty() )
  {
    std::cerr << "No setup file specified, use --setup <file name>." << std::endl;
    PrintCommandlineHelp();
    return -1;
  } // if

  if( ! std::filesystem::exists( inFileName ) )
  {
    std::cerr << "Setup file '" << inFileName << "' does not exist." << std::endl;
    return -1;
  } // if

  std::ifstream inFile;
  inFile.open( inFileName, std::ifstream::in );
  if( !inFile.is_open() )
  {
    std::cerr << "Cannot open setup file '" << inFileName << "'." << std::endl;
    return -1;
  } // if

  size_t lastLineRead = 0;
  if( ! cfg.ParseINIFile( inFile, lastLineRead ) )
  {
    std::cerr << "Error reading setup file '" << inFileName 
              << "', problem on line ." << lastLineRead << std::endl;
    return -1;
  } // if

  inFile.close();

  //------ Create simulation settings object --------------------------------------------------------

  SimFire::CSimFireSettings settings;
  auto vErrors = settings.ImportSettings( cfg );
  if( !vErrors.empty() )
  {
    std::cerr << "Errors found in configuration, quitting:" << std::endl;
    for( auto & err : vErrors )
      std::cerr << "  -> " << err << std::endl;
    return -1;
  } // if

  std::cout << asteriskLine << std::endl;
  std::cout << "Simulation settings: " << std::endl;
  std::cout << asteriskLine << std::endl << std::endl;
  settings.Preprint( std::cout );
  std::cout << std::endl << std::endl;

  //------ Create and run the simulation -----------------------------------------------------------

  bool res = true;

  if (settings.GetDoTestRun())
  {
    SimFire::CSimFireSingleRun run(settings,
      [](const std::string& id, const std::string& mssg)
      {
         std::cout << "[" << id << "]   " << mssg << std::endl;
      }
    );

    SimFire::CSimFireSingleRunParams runPars;
    runPars.mRunIdentifier = "SIMPLE";
    runPars.mThreadIdentifier = "Main";

    std::cout << asteriskLine << std::endl;
    std::cout << "Test run: " << std::endl;
    std::cout << asteriskLine << std::endl << std::endl;
    runPars.Preprint(std::cout) << std::endl;

		double_t zAct = settings.GetAimZStart();
		double_t increment = 
      (settings.GetAimZEnd() - settings.GetAimZStart()) / settings.GetAimZSteps();

    std::vector<SimFire::CSimFireSingleRunParams> allHits;

    for (uint32_t step = 0; step < settings.GetAimZSteps(); ++step, zAct += increment )
    {
			runPars.Reset();

      runPars.mVelocityXCoef = settings.GetAimX();
      runPars.mVelocityYCoef = settings.GetAimY();
			runPars.mVelocityZCoef = zAct;

      run.Run( runPars );
      std::cout << "Test " << runPars.GetRunDesc() << " ended with code "
        << SimFire::CSimFireSingleRunParams::GetStrValue(runPars.mReturnCode) << std::endl;
      if (runPars.mReturnCode == SimFire::CSimFireSingleRunParams::SimResCode_t::kError)
        res = false;
      else if (runPars.mReturnCode == SimFire::CSimFireSingleRunParams::SimResCode_t::kEndedCollision)
				allHits.push_back(runPars);
    }

    std::cout << std::endl << asteriskLine << std::endl;
    std::cout << "Hits " << std::endl;
    std::cout << asteriskLine << std::endl << std::endl;

    for( auto & hit : allHits )
    {
      std::cout << hit.GetRunDesc() << std::endl;
		}

  }
  else
  {

    SimFire::CSimFireCore simCore(settings);

    res = simCore.Run();

    if (!res)
    {
      std::cerr << "Simulation ended in error(s), check log file '"
        << simCore.GetLogFilePath() << "'" << std::endl << std::endl;
    } // if

  }

  return res;

} // main 
