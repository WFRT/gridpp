#ifndef FILE_NETCDF_H
#define FILE_NETCDF_H
#include <netcdf.hh>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include "File.h"
#include "../Variable.h"

//! Represents a Netcdf data file
class FileNetcdf : public File {
   public:
      FileNetcdf(std::string iFilename, bool iReadOnly=false);
      ~FileNetcdf();

      virtual std::string getVariableName(Variable::Type iVariable) const = 0;
      //! Add attribute to a variable (overwrite if existing)
      void setAttribute(NcVar* iVar, std::string iName, std::string iValue);
      //! Add global attribute to file (overwrite if existing)
      void setGlobalAttribute(std::string iName, std::string iValue);
      //! Add global attribute to file (append to attribute if existing)
      void appendGlobalAttribute(std::string iName, std::string iValue);
      //! Add global attribute to file (prepend to attribute if existing)
      void prependGlobalAttribute(std::string iName, std::string iValue);
      //! Get global string attribute. Returns "" if non-existant.
      std::string getGlobalAttribute(std::string iName);
   protected:
      float getScale(NcVar* iVar) const;
      float getOffset(NcVar* iVar) const;
      NcFile mFile;

      // Does this file contain the variable?
      bool hasVariableCore(Variable::Type iVariable) const;
      bool hasVariableCore(std::string iVariable) const;

      NcDim* getDim(std::string iDim) const;
      NcVar* getVar(std::string iVar) const;
      bool   hasDim(std::string iDim) const;
      bool   hasVar(std::string iVar) const;
      static bool   hasDim(const NcFile& iFile, std::string iDim);
      static bool   hasVar(const NcFile& iFile, std::string iVar);
      static float getMissingValue(const NcVar* iVar);
      static void  setMissingValue(NcVar* iVar, float iValue);
      void writeTimes();
      void writeReferenceTime();
      void writeGlobalAttributes();
};
#include "Ec.h"
#include "Arome.h"
#endif
