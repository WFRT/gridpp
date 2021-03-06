#ifndef OPTIONS_H
#define OPTIONS_H
#include <iostream>
#include <vector>
#include <sstream>


//! Container class for key-value pairs.
//! Empty (i.e. "") keys or values are not allowed
class Options {
   public:
      //! Creates container
      //! @param iOptionString options with format: "key1=value1 key2=value2..."
      Options(std::string iOptionString="");

      //! \brief Adds key and value to container
      //! @param iKey cannot be ""
      //! @param iValue cannot be ""
      void addOption(std::string iKey, std::string iValue);

      //! \brief Parses options and adds to container
      //! @param iOptionString with format: "key1=value1 key2=value2...". key= ignored.
      void addOptions(std::string iOptionString);

      //! \brief Converts iValue to string and adds to options
      //! @param iKey cannot be ""
      template <class T> void addOption(std::string iKey, T iValue) {
         std::stringstream ss;
         ss << iValue;
         std::string value = ss.str();
         addOption(iKey, value);
      }

      //! Remove all key-value pairs from container
      void clear();

      //! String representation of all options in with format: "key1=value1 key2=value2...".
      //! Can be used to reconstruct the object by passing into the constructor.
      //! Order of options is not specified.
      std::string toString() const;

      //! \brief Find value corresponding to key
      //! @param iKey find this key
      //! @param iValue places the value in this variable. If key does not exist, this is unchanged.
      //! @return true if key is found, false otherwise
      template <class T> bool getValue(std::string iKey, T& iValue) const {
         for(int i = 0; i < mPairs.size(); i++) {
            std::string key = mPairs[i].first;
            std::string value = mPairs[i].second;
            if(key == iKey) {
               std::stringstream ss(value);
               ss >> iValue;
               return true;
            }
         }
         return false;
      };

      //! \brief Find vector values corresponding to key
      //! @param iKey find this key
      //! @param iValues places the values in this variable (variable cleared first). If key does
      //! not exist, the vector is empty.
      //! @return true if key is found, false otherwise
      template <class T> bool getValues(std::string iKey, std::vector<T>& iValues) const {
         iValues.clear();
         for(int i = 0; i < mPairs.size(); i++) {
            // Attributes are organized as follows:
            // option=value1,value2,value3...
            std::string key   = mPairs[i].first;
            if(key == iKey) {
               std::string values = mPairs[i].second;
               std::stringstream ss(values);
               // Parse collection of attributes separated by commas
               while(ss) {
                  std::string betweenComma;
                  if(!getline(ss, betweenComma, ','))
                     break; // We're at the end of the line
                  std::stringstream ss2(betweenComma);
                  T value;
                  ss2 >> value;
                  iValues.push_back(value);
               }
               return true;
            }
         }
         return false;
      };
   private:
      //! Parse a string with a single option "key=value"
      void addOption(std::string iOptionString);

      typedef std::pair<std::string,std::string> KeyValue;  // key, value
      std::vector<KeyValue> mPairs;
};
#endif
