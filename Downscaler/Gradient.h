#ifndef DOWNSCALER_GRADIENT_H
#define DOWNSCALER_GRADIENT_H
#include "Downscaler.h"
#include "../Variable.h"
#include "../Util.h"
typedef std::vector<std::vector<int> > vec2Int;
class DownscalerGradient : public Downscaler {
   public:
      //! Downscale the specified variable
      DownscalerGradient(Variable::Type iVariable);
      //! Do not compute the gradient but set it to a fixed amount. Positive rate means
      //! increasing with elevation. The units are per meters.
      void setConstantGradient(float iGradient);
      //! Calculate gradient in a neighbour hood of points within +- iNumPoints
      //! in each direction.
      void setNeighbourhoodRadius(int iNumPoints);
   private:
      void downscaleCore(const File& iInput, File& iOutput) const;
      Variable::Type mVariable;
      int mRadius;
      float mConstGradient;
};
#endif