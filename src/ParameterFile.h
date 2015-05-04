#ifndef PARAMETERFILE_H
#define PARAMETERFILE_H
#include <iostream>
#include <map>
#include "Parameters.h"
#include "Location.h"

//! Represents a collection of parameters, one set for each forecast time
//! Parameters are read from a specified text file with the following format:
//! 0 3.4 2.1 5.2 12 41
//! 1 3.4 2.1 5.2 12 41
//! 2 3.4 2.1 5.2 12 41
//! Each line represents one forecast time. The first column is the forecast timestep (an index
//! not the number of hours), starting at 0. The remaning columns are parameters that can be used
//! in post-processing methods. The number of columns most be constant. If a file has only one
//! line, then the parameters are used for all forecast hours.
class ParameterFile {
   public:
      //! Read parameters from this file
      ParameterFile(std::string iFilename);

      //! Get the parameter valid for specified forecast timestep. This is an index, not an hour.
      Parameters getParameters(int iTime) const;

      //! Set the parameter valid for specified time
      void setParameters(Parameters iParameters, int iTime);

      //! Returns the number of parameter sets available (i.e number of forecast hours)
      int getSize() const;

      //! Returns the number of parameters in one parameter set
      int getNumParameters() const;

      //! Returns the filename where parameters are retrieved from
      std::string getFilename() const;
   private:
      std::map<int, Parameters> mParameters; // Offset, Parameters
      std::string mFilename;
      int mNumParameters;
};
//! Represents parameters for different spatial coordinates.
//! Parameters are read from a specified text file with the following format:
//! <time> <lat> <lon> <elevation> <param1> <param2> ... <paramN>
//! 0 60 10 200 3.4 2.1 5.2
//! 1 60 10 200 1.4 4.1 2.2
//! 1 58 8 150 9.2 12.2 -2.1
//! A location does not need to have parameters for all times
class ParameterFileSpatial {
   public:
      ParameterFileSpatial(std::string iFilename);
      //! Get number of unique locations in parameter set
      int getNumLocations() const;

      //! Get parameters for a specific time and location index
      //! @param iLocation Must be >= 0 and < getNumLocations()
      //! @return Parameters valid for time and location. If no parameters are available
      //! returns an empty parameter set
      Parameters getParameters(int iTime, int iLocation) const;

      // Get unique locations in parameter set
      std::vector<Location> getLocations() const;

      //! Returns the number of parameters in one parameter set
      int getNumParameters() const;

      //! Returns the filename where parameters are retrieved from
      std::string getFilename() const;
   private:
      std::map<int, std::map<int, Parameters> > mParameters; // Offset, Location, Parameters
      std::vector<Location> mLocations;
      std::string mFilename;
      int mNumParameters;
};
/*
class ParameterFileRegion {
   public:
      ParameterFileRegion(std::string iFilename) {};
      Parameters getParameters(float iLat, float iLon, int iTime) const {return Parameters();};
   private:
      std::map<int, Parameters> mParameters; // Offset, Parameters
      std::string mFilename;
};
*/
#endif
