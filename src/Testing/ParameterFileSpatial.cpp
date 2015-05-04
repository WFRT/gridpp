#include "../ParameterFile.h"
#include "../Util.h"
#include <gtest/gtest.h>
#include <algorithm>

namespace {
   TEST(ParameterFileSpatial, default) {
      ParameterFileSpatial file("testing/files/parametersKrieging.txt");
      std::vector<Location> locations = file.getLocations();
      ASSERT_EQ(2, locations.size());
      Location loc5 = locations[1]; // location 5,5,140
      Location loc0 = locations[0]; // location 0,0,150
      if(locations[0].lat() == 5) {
         loc5 = locations[0];
         loc0 = locations[1];
      }
      EXPECT_FLOAT_EQ(5, loc5.lat());
      EXPECT_FLOAT_EQ(5, loc5.lon());
      EXPECT_FLOAT_EQ(140, loc5.elev());
      EXPECT_FLOAT_EQ(0, loc0.lat());
      EXPECT_FLOAT_EQ(0, loc0.lon());
      EXPECT_FLOAT_EQ(150, loc0.elev());

      // Location 0
      Parameters par = file.getParameters(0, loc5);
      ASSERT_EQ(1, par.size());
      EXPECT_FLOAT_EQ(4.2, par[0]);
      par = file.getParameters(1, loc5);
      ASSERT_EQ(1, par.size());
      EXPECT_FLOAT_EQ(-5.4, par[0]);

      // Location 1
      par = file.getParameters(0, loc0);
      EXPECT_EQ(0, par.size());
      par = file.getParameters(1, loc0);
      ASSERT_EQ(1, par.size());
      EXPECT_FLOAT_EQ(4, par[0]);
   }
   TEST(ParameterFileSpatial, write) {
      ParameterFileSpatial file("testing/files/parametersKrieging.txt");
      file.write("testing/files/parametersKrieging2.txt");
      ParameterFileSpatial file2("testing/files/parametersKrieging2.txt");
      // Should have the same locations
      std::vector<Location> locations1 = file.getLocations();
      std::vector<Location> locations2 = file.getLocations();
      std::sort(locations1.begin(), locations1.end());
      std::sort(locations2.begin(), locations2.end());
      EXPECT_EQ(locations1, locations2);
   }
   TEST(ParameterFileSpatial, emptyFile) {
      ParameterFileSpatial file("testing/files/89h9382he9823he92.txt");
      std::vector<Location> locations = file.getLocations();
      EXPECT_EQ(0, locations.size());
   }
   TEST(ParameterFileSpatial, writeError) {
      ::testing::FLAGS_gtest_death_test_style = "threadsafe";
      Util::setShowError(false);
      ParameterFileSpatial file("testing/files/parametersKrieging.txt");
      // Shouldn't be able to write to a directory
      EXPECT_DEATH(file.write("testing/files/"), ".*");
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
