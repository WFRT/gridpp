#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <vector>
#include <set>

typedef std::vector<std::vector<float> > vec2; // Lat, Lon

//! Contains helper functions
class Util {
   public:
      //! Aborts the program with an error message to std::cout
      static void error(std::string iMessage);

      //! Issues a warning message to std::cout
      static void warning(std::string iMessage);

      //! Issues a status message to std::cout
      static void status(std::string iMessage);

      //! True will force calls to Util::error to show error messages.
      //! Calls to errors will cause abort regardless of this setting.
      static void setShowError(bool flag);
      
      //! True will force calls to Util::warning to show warning messages
      static void setShowWarning(bool flag);
      
      //! True will force calls to Util::status to show status messages
      static void setShowStatus(bool flag);
      
      //! Missing value indicator
      static float MV;

      //! \brief Checks if a value is valid
      //! @return false if iValue equals Util::MV or is +-inf or nan. Returns true otherwise.
      static bool isValid(float iValue);

      //! Checks if the file exists
      static bool exists(const std::string& iFilename);

      //! \brief Computes the geographical distance between two locations
      //! @param lat1 Latitude in degrees of point 1
      //! @param lon1 Longitude in degrees of point 1
      //! @param lat2 Latitude in degrees of point 2
      //! @param lon2 Longitude in degrees of point 2
      //! @return Distance in meters
      static float getDistance(float lat1, float lon1, float lat2, float lon2);

      //! Returns the current unix time in seconds
      static double clock();
      
      //! Convert degrees to radians
      static float deg2rad(float deg);
      
      //! Convert radians to degrees
      static float rad2deg(float rad);

      static float pi;
      
      //! Radius of the earth in meters
      static double radiusEarth;
      
      //! \brief Convert unix timestamp to YYYYMMDD format
      static int getDate(time_t iUnixTime);
      
      //! \brief Get the current date in YYYYMMDD format
      static int getCurrentDate();

      //! \brief Get the current date in YYYY-MM-DD HH:MM:SS format
      static std::string getCurrentTimeStamp();
      
      //! \brief Calculates new date by adding hours to an existing date
      //! @param iDate Date in format YYYYMMDD
      //! @param iAddHours Number of hours to add
      //! @return New date in format YYYYMMDD
      static int calcDate(int iDate, float iAddHours);
      
      //! \brief Creates a vector of strings by splitting string on each space
      //! Multiple consecutive spaces are treated as one space
      static std::vector<std::string> split(std::string iString);

      //! \brief Computes the logit of p
      //! @param p Must lie in the interval (0,1)
      static float logit(float p);

      //! \brief Computes the inverse logit of x
      //! @return x A value on the interval (0,1)
      static float invLogit(float x);

      template <class T> static std::vector<T> combine(const std::vector<T>& i1, const std::vector<T>& i2) {
         std::set<T> allValues(i1.begin(), i1.end());
         for(int i = 0; i < i2.size(); i++) {
            allValues.insert(i2[i]);
         }
         return std::vector<T>(allValues.begin(), allValues.end());
      };
      
      //! \brief Comparator class for sorting pairs using the first entry.
      //! Sorts from smallest to largest
      template<class T1, class T2> struct sort_pair_first {
         bool operator()(const std::pair<T1,T2>&left, const std::pair<T1,T2>&right) {
            return left.first < right.first;
         }
      };

      //! Copy the file with filename iFrom to filename iTo
      static bool copy(std::string iFrom, std::string iTo);
     
      //! \brief Comparator class for sorting pairs using the second entry.
      //! Sorts from smallest to largest
      template<class T1, class T2> struct sort_pair_second {
         bool operator()(const std::pair<T1,T2>&left, const std::pair<T1,T2>&right) {
            return left.second < right.second;
         }
      };
   private:
      static bool mShowError;
      static bool mShowWarning;
      static bool mShowStatus;
};
#endif
