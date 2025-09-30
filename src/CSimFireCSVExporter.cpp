//****************************************************************************************************
//! \file CSimFireSCVExporter.cpp                                                                       
//! Module contains declaration of CSimFireSCVExporter class, which exports progress of single simulation
//! run into CSV file.                                
//****************************************************************************************************
//                                                                                                  
//****************************************************************************************************
// 19. 11. 2025, V. Pospíšil, gdermog@seznam.cz                                                     
//****************************************************************************************************

#include "CSimFireCSVExporter.h"

namespace SimFire
{
	CSimFireCSVExporter::CSimFireCSVExporter(const CSimFireSettings& settings):
		mSettings( settings ),
		mCSVFile(),
		mShowX( true ),
	  mShowY( true ),
	  mSHowXY( true ),
	  mShowZ( true ),
	  mShowvX( true ),
	  mShowvY( true ),
	  mSHowvXY( true ),
	  mShowvZ( true ),
	  mShowDistance( true ),
	  mShowTime( true ),
	  mShowFlags( true )
	{}

	//-------------------------------------------------------------------------------------------------

	CSimFireCSVExporter::~CSimFireCSVExporter()
	{
		if( mCSVFile.is_open() )
			mCSVFile.close();
	} // CSimFireSCVExporter::~CSimFireSCVExporter

	//-------------------------------------------------------------------------------------------------

	bool CSimFireCSVExporter::NewFile(const std::string& filename)
	{

		if( mCSVFile.is_open() )
			mCSVFile.close();

		mCSVFile.open(filename, std::ofstream::out);
		if( !mCSVFile.is_open() )
		{
			std::cout << "Cannot open output file '" << filename << "'." << std::endl;
			return false;
		} // if

		if( mShowX ) mCSVFile << "X [m];";
		if( mShowY ) mCSVFile << "Y [m];";
		if( mSHowXY ) mCSVFile << "XY [m];";
		if( mShowZ ) mCSVFile << "Z [m];";
		if( mShowvX ) mCSVFile << "vX [m/s];";
		if( mShowvY ) mCSVFile << "vY [m/s];";
		if( mSHowvXY ) mCSVFile << "vXY [m/s];";
		if( mShowvZ ) mCSVFile << "vZ [m/s];";
		if( mShowDistance ) mCSVFile << "Distance [m];";
		if( mShowTime ) mCSVFile << "Time [s];";
		if( mShowFlags ) mCSVFile << "Flags;";
		mCSVFile << std::endl;

		std::cout << "Exporting simulation into file '" << filename << "'." << std::endl;
		return true;

	} // CSimFireSCVExporter::NewFile

	//-------------------------------------------------------------------------------------------------
	
	void CSimFireCSVExporter::DoExportState(
		double_t x, double_t y, double_t z, 
		double_t vX, double_t vY, double_t vZ, 
		double_t dist, double_t t, 
		bool raising, bool below, bool nearHalfPlane )
	{
		if (!mCSVFile.is_open())
			return;

		if (mShowX) mCSVFile << x << ";";
		if (mShowY) mCSVFile << y << ";";
		if (mSHowXY) mCSVFile << std::sqrt(x * x + y * y) << ";";
		if (mShowZ) mCSVFile << z << ";";
		if (mShowvX) mCSVFile << vX << ";";
		if (mShowvY) mCSVFile << vY << ";";
		if (mSHowvXY) mCSVFile << std::sqrt(vX * vX + vY * vY) << ";";
		if (mShowvZ) mCSVFile << vZ << ";";
		if (mShowDistance) mCSVFile << dist << ";";
		if (mShowTime) mCSVFile << t << ";";
		if (mShowFlags)
		{
			std::string flags;
			if (raising) flags += "R";
			else flags += "F";
			if (below) flags += "B";
			else flags += "A";
			if (nearHalfPlane) flags += "N";
			else flags += "F";
			mCSVFile << flags;
		}
		mCSVFile << std::endl;
	} // CSimFireSCVExporter::DoExportState

} // namespace SimFire