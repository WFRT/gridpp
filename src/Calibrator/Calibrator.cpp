#include "Calibrator.h"
#include <algorithm>
#include <math.h>
#include <boost/math/distributions/gamma.hpp>
#include "../Util.h"
#include "../Options.h"
#include "../ParameterFile.h"

Calibrator::Calibrator() {

}
Calibrator* Calibrator::getScheme(std::string iName, const Options& iOptions) {

   if(iName == "zaga") {
      std::string parFilename;
      if(!iOptions.getValue("parameters", parFilename)) {
         Util::error("Calibrator 'zaga' needs parameters");
      }

      ParameterFile* parFile = new ParameterFile(parFilename);
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'zaga' needs variable");
      }
      CalibratorZaga* c = new CalibratorZaga(parFile, Variable::getType(variable));

      // Optional settings
      float fracThreshold;
      if(iOptions.getValue("fracThreshold", fracThreshold)) {
         c->setFracThreshold(fracThreshold);
      }
      return c;
   }
   else if(iName == "cloud") {
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'cloud' needs variable");
      }
      CalibratorCloud* c = new CalibratorCloud(Variable::Precip, Variable::getType(variable));
      return c;
   }
   else if(iName == "accumulate") {
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'accumulate' needs variable");
      }
      CalibratorAccumulate* c = new CalibratorAccumulate(Variable::getType(variable));
      return c;
   }
   else if(iName == "neighbourhood") {
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'neighbourhood' needs variable");
      }
      CalibratorNeighbourhood* c = new CalibratorNeighbourhood(Variable::getType(variable), iOptions);
      return c;
   }
   else if(iName == "phase") {
      std::string parFilename;
      if(!iOptions.getValue("parameters", parFilename)) {
         Util::error("Calibrator 'phase' needs parameters");
      }
      ParameterFile* parFile = new ParameterFile(parFilename);
      CalibratorPhase* c = new CalibratorPhase(parFile);
      float minPrecip;
      if(iOptions.getValue("minPrecip", minPrecip)) {
         c->setMinPrecip(minPrecip);
      }
      bool useWetbulb;
      if(iOptions.getValue("useWetbulb", useWetbulb)) {
         c->setUseWetbulb(useWetbulb);
      }

      return c;
   }
   else if(iName == "windDirection") {
      std::string parFilename;
      if(!iOptions.getValue("parameters", parFilename)) {
         Util::error("Calibrator 'zaga' needs parameters");
      }

      ParameterFile* parFile = new ParameterFile(parFilename);
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'zaga' needs variable");
      }
      CalibratorWindDirection* c = new CalibratorWindDirection(parFile, Variable::getType(variable));

      return c;
   }
   else if(iName == "qc") {
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'qc' needs variable");
      }
      CalibratorQc* c = new CalibratorQc(Variable::getType(variable), iOptions);

      return c;
   }
   else if(iName == "qnh") {
      CalibratorQnh* c = new CalibratorQnh();

      return c;
   }
   else if(iName == "regression") {
      std::string parFilename;
      if(!iOptions.getValue("parameters", parFilename)) {
         Util::error("Calibrator 'regression' needs parameters");
      }

      ParameterFile* parFile = new ParameterFile(parFilename);
      std::string variable;
      if(!iOptions.getValue("variable", variable)) {
         Util::error("Calibrator 'regression' needs variable");
      }
      CalibratorRegression* c = new CalibratorRegression(parFile, Variable::getType(variable));

      return c;
   }
   else {
      Util::error("Could not instantiate calibrator with name '" + iName + "'");
      return NULL;
   }
}
bool Calibrator::calibrate(File& iFile) const {
   return calibrateCore(iFile);
}

void Calibrator::shuffle(const std::vector<float>& iBefore, std::vector<float>& iAfter) {
   if(iBefore.size() != iAfter.size()) {
      return;
   }
   if(iBefore.size() == 0)
      return;

   int N = iBefore.size();
   std::vector<std::pair<float,int> > pairs(N);
   for(int e = 0; e < N; e++) {
      if(!Util::isValid(iBefore[e]) || !Util::isValid(iAfter[e])) {
         return;
      }
      pairs[e].first = iBefore[e];
      pairs[e].second = e;
   }
   std::vector<float> afterCopy = iAfter;
   // Sort values so that the rank of a member is the same before and after calibration
   std::sort(pairs.begin(), pairs.end(), Util::sort_pair_first<float,int>());
   std::sort(afterCopy.begin(), afterCopy.end());
   for(int e = 0; e < N; e++) {
      int ei = pairs[e].second;
      float valueCal = afterCopy[e];
      iAfter[ei] = valueCal;
   }
}
