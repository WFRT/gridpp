#include "Bypass.h"
#include "../File/File.h"
#include "../Util.h"
#include <math.h>

DownscalerBypass::DownscalerBypass(Variable::Type iVariable) :
      Downscaler(iVariable) {
}

void DownscalerBypass::downscaleCore(const File& iInput, File& iOutput) const {
}

std::string DownscalerBypass::description() {
   std::stringstream ss;
   ss << Util::formatDescription("-d bypass", "No downscaling is done, but useful if the variable is derived by a calibrator.") << std::endl;
   return ss.str();
}
