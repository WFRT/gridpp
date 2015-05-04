#include "ParameterFile.h"
#include <fstream>
#include <sstream>
#include "Util.h"
#include <assert.h>
#include <set>
#include <fstream>

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
      mFilename(iFilename) {
   std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
   if(!ifs.good()) {
      return;
   }
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
         Location location(lat,lon,elev);
         Parameters parameters(values);
         std::pair<int,Location> timeLoc(time,location);
         allParameters.push_back(std::pair<std::pair<int,Location>,Parameters>(timeLoc,parameters));
      }
   }
   ifs.close();
   std::stringstream ss;
   ss << "Reading " << mFilename << ". Found " << allParameters.size() << " parameter sets.";
   Util::status(ss.str());

   // Rearrange parameters
   for(int i = 0; i < allParameters.size(); i++) {
      std::pair<int, Location> timeLoc = allParameters[i].first;
      int currTime = timeLoc.first;
      Location currLoc = timeLoc.second;
      Parameters currParameters = allParameters[i].second;
      mParameters[currTime][currLoc] = currParameters;
   }
}

Parameters ParameterFileSpatial::getParameters(int iTime, const Location& iLocation) const {
   std::map<int, std::map<Location, Parameters> >::const_iterator it = mParameters.find(iTime);
   if(it == mParameters.end())
      return Parameters();
   std::map<Location, Parameters>::const_iterator it2 = it->second.find(iLocation);
   if(it2 == it->second.end()) {
      return Parameters();
   }
   else {
      return it2->second;
   }
}

std::vector<Location> ParameterFileSpatial::getLocations() const {
   std::set<Location> locationsSet;
   std::map<int, std::map<Location, Parameters> >::const_iterator it;
   for(it = mParameters.begin(); it != mParameters.end(); it++) {
      std::map<Location, Parameters>::const_iterator it2;
      for(it2 = it->second.begin(); it2 != it->second.end(); it2++) {
         locationsSet.insert(it2->first);
      }
   }
   std::vector<Location> locations(locationsSet.begin(), locationsSet.end());
   return locations;
}

std::vector<int> ParameterFileSpatial::getTimes() const {
   std::vector<int> times;
   std::map<int, std::map<Location, Parameters> >::const_iterator it;
   for(it = mParameters.begin(); it != mParameters.end(); it++) {
      times.push_back(it->first);
   }
   return times;
}

std::string ParameterFileSpatial::getFilename() const {
   return mFilename;
}

void ParameterFileSpatial::setParameters(Parameters iParameters, int iTime, const Location& iLocation) {
   mParameters[iTime][iLocation] = iParameters;
}

void ParameterFileSpatial::write(const std::string& iFilename) const {
   std::string filename = iFilename;
   if(iFilename == "") {
      filename = mFilename;
   }
   std::ofstream ofs(filename.c_str(), std::ios_base::out);
   if(!ofs.good()) {
      Util::error("Cannot write spatial parameters to " + filename);
   }

   std::map<int, std::map<Location, Parameters> >::const_iterator it;
   // Loop over times
   ofs << "# time lat lon elev parameters" << std::endl;
   for(it = mParameters.begin(); it != mParameters.end(); it++) {
      std::map<Location, Parameters>::const_iterator it2;
      // Loop over locations
      int time = it->first;
      for(it2 = it->second.begin(); it2 != it->second.end(); it2++) {
         const Location& location = it2->first;
         const Parameters& parameters = it2->second;
         ofs << time << " " << location.lat() << " " << location.lon() << " " << location.elev();
         // Loop over parameter values
         for(int i = 0; i < parameters.size(); i++) {
            ofs << " " << parameters[i];
         }
         ofs << std::endl;
      }
   }
   ofs.close();
}
