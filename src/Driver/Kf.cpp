#include <iostream>
#include <string>
#include <string.h>
#include "../File/File.h"
#include "../ParameterFile.h"
#include "../Calibrator/Calibrator.h"
#include "../Downscaler/Downscaler.h"
#include "../Util.h"
#include "../Options.h"
#include "../Setup.h"
#include "../KalmanFilter.h"
void writeUsage() {
   std::cout << "Kalman filter program to create a bias file to be used by krieging in gridpp" << std::endl;
   std::cout << std::endl;
   std::cout << "usage:  kalmanFilter input [currCoeffs] newCoeffs" << std::endl;
   std::cout << std::endl;
   std::cout << "Arguments:" << std::endl;
   std::cout << "   input         Input text file with obs/forecast data. Must be in the following format:" << std::endl;
   std::cout << "                 <time> <lat> <lon> <elev> <obs> <fcst>" << std::endl;
   std::cout << "   currCoeffs    Current Kalman coefficients. If unspecified, initialize. Must have the format:" << std::endl;
   std::cout << "                 <time> <lat> <lon> <elev> <pVarV> <kalmanGainVar> <varV> <p> <kalmanGain> <error> <lastError> <biasEstimate>" << std::endl;
   std::cout << "   newCoeffs     New coeffcients stored in this file." << std::endl;
   std::cout << std::endl;
   std::cout << "Notes:" << std::endl;
   std::cout << "   - If 'input' contains stations that don't exist in currCoeffs, new parameters" << std::endl;
   std::cout << "     initialized." << std::endl;

   std::cout << std::endl;
}
int main(int argc, const char *argv[]) {
   if(argc <= 2) {
      writeUsage();
      return 0;
   }

   std::string dataFilename;
   std::string currFilename;
   std::string newFilename;
   if(argc == 3) {
      dataFilename = argv[1];
      currFilename = "";
      newFilename  = argv[2];
   }
   else {
      dataFilename = argv[1];
      currFilename = argv[2];
      newFilename  = argv[3];
   }

   KalmanFilter kf(Variable::T, 0.1);
   ParameterFileSpatial dataFile   = ParameterFileSpatial(dataFilename);
   ParameterFileSpatial currFile   = ParameterFileSpatial(currFilename);
   ParameterFileSpatial newFile    = ParameterFileSpatial(newFilename);

   std::vector<int>      times     = Util::combine(dataFile.getTimes(), currFile.getTimes());
   std::vector<Location> locations = Util::combine(dataFile.getLocations(), currFile.getLocations());
   for(int t = 0; t < times.size(); t++) {
      int time = times[t];
      for(int i = 0; i < locations.size(); i++) {
         const Location& location = locations[i];

         Parameters currData = dataFile.getParameters(time, location);
         // KF must still be run when obs/fcst are missing because it needs to update certain
         // coefficients.
         float obs      = Util::MV;
         float forecast = Util::MV;
         if(currData.size() == 2) {
            obs      = currData[0];
            forecast = currData[1];
         }

         std::vector<float> values;
         Parameters currKF = currFile.getParameters(time, location);
         Parameters newKF  = kf.update(obs, forecast, currKF);
         newFile.setParameters(newKF, time, location);
      }
   }

   newFile.write();
}
