#include "Regression.h"
#include <cmath>
#include "../Util.h"
#include "../File/File.h"
#include "../ParameterFile.h"
#include "../Downscaler/Pressure.h"
CalibratorRegression::CalibratorRegression(const ParameterFile* iParameterFile, Variable::Type iVariable) :
      Calibrator(),
      mVariable(iVariable),
      mParameterFile(iParameterFile) {
   if(iParameterFile->getNumParameters() == 0) {
      Util::error("Parameter file '" + iParameterFile->getFilename() + "' must have at least one datacolumns");
   }
}
bool CalibratorRegression::calibrateCore(File& iFile) const {
   int nLat = iFile.getNumLat();
   int nLon = iFile.getNumLon();
   int nEns = iFile.getNumEns();
   int nTime = iFile.getNumTime();
   vec2 elevs = iFile.getElevs();

   // Loop over offsets
   for(int t = 0; t < nTime; t++) {
      const Parameters& par = mParameterFile->getParameters(t);
      const FieldPtr field = iFile.getField(mVariable, t);

      #pragma omp parallel for
      for(int i = 0; i < nLat; i++) {
         for(int j = 0; j < nLon; j++) {
            for(int e = 0; e < nEns; e++) {
               if(Util::isValid((*field)(i,j,e))) {
                  float total = 0;
                  // Accumulate a + b * fcst + c * fcst^2 ...
                  for(int p = 0; p < par.size(); p++) {
                     float coeff = par[p];
                     if(!Util::isValid(coeff)) {
                        total = Util::MV;
                        break;
                     }
                     total += coeff*pow((*field)(i,j,e), p);
                  }
                  (*field)(i,j,e)  = total;
               }
               else {
                  (*field)(i,j,e)  = Util::MV;
               }
            }
         }
      }
   }
   return true;
}

std::string CalibratorRegression::description() {
   std::stringstream ss;
   ss << Util::formatDescription("-c regression", "Applies polynomial regression equation to forecasts: newForecast = a + b * forecast + c * forecast^2 ... ") << std::endl;
   ss << Util::formatDescription("   parameters=required", "Read parameters from this text file. The file format is:") << std::endl;
   ss << Util::formatDescription("", "offset0 a b c ...") << std::endl;
   ss << Util::formatDescription("", "...") << std::endl;
   ss << Util::formatDescription("", "offsetN a b c ...") << std::endl;
   ss << Util::formatDescription("", "If the file only has a single line, then the same set of parameters are used for all offsets.") << std::endl;
   return ss.str();
}
