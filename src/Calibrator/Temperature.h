#ifndef CALIBRATOR_TEMPERATURE_H
#define CALIBRATOR_TEMPERATURE_H
#include "Calibrator.h"
class ParameterFile;

class CalibratorTemperature : public Calibrator {
   public:
      CalibratorTemperature(const ParameterFile& iParameterFile);
      static std::string description();
      std::string name() const {return "temperature";};
   private:
      bool calibrateCore(const File& iFile) const;
};
#endif
