#include "ParameterFile.h"
#include <fstream>
#include <sstream>
#include "Util.h"
#include <assert.h>
#include <set>

ParameterFile::ParameterFile(std::string iFilename) : 
      mFilename(iFilename),
      mNumParameters(Util::MV) {
   std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
   if(!ifs.good()) {
      Util::error("Parameter file '" + iFilename + "' does not exist");
   }
   mNumParameters = Util::MV;
   while(ifs.good()) {
      char line[10000];
      ifs.getline(line, 10000, '\n');
      if(ifs.good() && line[0] != '#') {
         std::stringstream ss(line);
         // Loop over each value
         std::vector<float> values;
         int time;
         bool status = ss >> time;
         if(!status) {
            Util::error("Could not read time from file '" + mFilename + "'");
         }
         while(ss.good()) {
            float value;
            bool status  = ss >> value;
            if(!status) {
               Util::error("Could not read value from file '" + mFilename + "'");
            }
            values.push_back(value);
         }
         if(mNumParameters == Util::MV)
            mNumParameters = values.size();
         else if(values.size() != mNumParameters) {
            std::stringstream ss;
            ss << "Parameter file '" + iFilename + "' is corrupt, because it does not have the same"
               << " number of columns on each line" << std::endl;
            Util::error(ss.str());
         }
         Parameters parameters(values);
         mParameters[time] = parameters;
      }
   }
   ifs.close();
   std::stringstream ss;
   ss << "Reading " << mFilename << ". Found " << getSize() << " parameter sets.";
   Util::status(ss.str());
   if(!Util::isValid(mNumParameters))
      mNumParameters = 0;

   Parameters par = getParameters(0);
}
Parameters ParameterFile::getParameters(int iTime) const {
   if(mParameters.size() == 1) {
      // Assume only one set of parameters for all hours
      std::map<int, Parameters>::const_iterator it = mParameters.begin();
      return it->second;
   }
   else {
      std::map<int, Parameters>::const_iterator it = mParameters.find(iTime);
      if(it == mParameters.end()) {
         std::stringstream ss;
         ss << "Parameter file '" << mFilename << "' does not have values for time " << iTime << ".";
         Util::error(ss.str());
      }
      return it->second;
   }
}

void ParameterFile::setParameters(Parameters iParameters, int iTime) {
   mParameters[iTime] = iParameters;
}

int ParameterFile::getSize() const {
   return mParameters.size();
}

int ParameterFile::getNumParameters() const {
   return mNumParameters;
}

std::string ParameterFile::getFilename() const {
   return mFilename;
}

ParameterFileSpatial::ParameterFileSpatial(std::string iFilename) :
      mFilename(iFilename),
      mNumParameters(Util::MV) {
   std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
   if(!ifs.good()) {
      Util::error("Parameter file '" + iFilename + "' does not exist");
   }
   mNumParameters = Util::MV;
   std::set<Location> allLocations;
   std::vector<std::pair<std::pair<int, Location>, Parameters> > allParameters; // Time, Location, Parameters
   while(ifs.good()) {
      char line[10000];
      ifs.getline(line, 10000, '\n');
      if(ifs.good() && line[0] != '#') {
         std::stringstream ss(line);
         // Loop over each value
         std::vector<float> values;
         int time;
         bool status = ss >> time;
         if(!status) {
            Util::error("Could not read time from file '" + mFilename + "'");
         }

         float lat;
         status = ss >> lat;
         if(!status) {
            Util::error("Could not read lat from file '" + mFilename + "'");
         }

         float lon;
         status = ss >> lon;
         if(!status) {
            Util::error("Could not read lon from file '" + mFilename + "'");
         }

         float elev;
         status = ss >> elev;
         if(!status) {
            Util::error("Could not read elev from file '" + mFilename + "'");
         }

         while(ss.good()) {
            float value;
            bool status  = ss >> value;
            if(!status) {
               Util::error("Could not read value from file '" + mFilename + "'");
            }
            values.push_back(value);
         }
         if(mNumParameters == Util::MV)
            mNumParameters = values.size();
         else if(values.size() != mNumParameters) {
            std::stringstream ss;
            ss << "Parameter file '" + iFilename + "' is corrupt, because it does not have the same"
               << " number of columns on each line" << std::endl;
            Util::error(ss.str());
         }
         Location location(lat,lon,elev);
         allLocations.insert(location);
         Parameters parameters(values);
         std::pair<int,Location> timeLoc(time,location);
         allParameters.push_back(std::pair<std::pair<int,Location>,Parameters>(timeLoc,parameters));
      }
   }
   ifs.close();
   std::stringstream ss;
   ss << "Reading " << mFilename << ". Found " << allParameters.size() << " parameter sets.";
   Util::status(ss.str());
   if(!Util::isValid(mNumParameters))
      mNumParameters = 0;

   mLocations = std::vector<Location>(allLocations.begin(), allLocations.end());
   std::map<Location, int> location2Index;
   for(int i = 0; i < mLocations.size(); i++) {
      location2Index[mLocations[i]] = i;
   }

   // Rearrange parameters
   for(int i = 0; i < allParameters.size(); i++) {
      std::pair<int, Location> timeLoc = allParameters[i].first;
      int currTime = timeLoc.first;
      Location currLoc = timeLoc.second;
      Parameters currParameters = allParameters[i].second;
      int currLocIndex = location2Index[currLoc];
      mParameters[currTime][currLocIndex] = currParameters;
   }

   Parameters par = getParameters(0, 0);
}

Parameters ParameterFileSpatial::getParameters(int iTime, int iLocation) const {
   // TODO: Check if missing
   std::map<int, std::map<int, Parameters> >::const_iterator it = mParameters.find(iTime);
   assert(it != mParameters.end());
   std::map<int, Parameters>::const_iterator it2 = it->second.find(iLocation);
   if(it2 == it->second.end()) {
      return Parameters();
   }
   else {
      return it2->second;
   }
}

int ParameterFileSpatial::getNumLocations() const {
   return mLocations.size();
}

std::vector<Location> ParameterFileSpatial::getLocations() const {
   return mLocations;
}

int ParameterFileSpatial::getNumParameters() const {
   return mNumParameters;
}

std::string ParameterFileSpatial::getFilename() const {
   return mFilename;
}

