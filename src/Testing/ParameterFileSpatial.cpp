#include "../ParameterFile.h"
#include "../Util.h"
#include <gtest/gtest.h>

namespace {
   TEST(ParameterFileSpatial, default) {
      ParameterFileSpatial file("testing/files/parametersKrieging.txt");
      ASSERT_EQ(2, file.getNumLocations());
      std::vector<Location> locations = file.getLocations();
      ASSERT_EQ(2, locations.size());
      int I5 = 1; // Index for location 5,5,140
      int I0 = 0; // Index for location 0,0,150
      if(locations[0].lat() == 5) {
         I5 = 0;
         I0 = 1;
      }
      EXPECT_FLOAT_EQ(5, locations[I5].lat());
      EXPECT_FLOAT_EQ(5, locations[I5].lon());
      EXPECT_FLOAT_EQ(140, locations[I5].elev());
      EXPECT_FLOAT_EQ(0, locations[I0].lat());
      EXPECT_FLOAT_EQ(0, locations[I0].lon());
      EXPECT_FLOAT_EQ(150, locations[I0].elev());

      // Location 0
      Parameters par = file.getParameters(0, I5);
      ASSERT_EQ(1, par.size());
      EXPECT_FLOAT_EQ(4.2, par[0]);
      par = file.getParameters(1, I5);
      ASSERT_EQ(1, par.size());
      EXPECT_FLOAT_EQ(-5.4, par[0]);

      // Location 1
      par = file.getParameters(0, I0);
      EXPECT_EQ(0, par.size());
      par = file.getParameters(1, I0);
      ASSERT_EQ(1, par.size());
      EXPECT_FLOAT_EQ(4, par[0]);
   }
}
int main(int argc, char **argv) {
     ::testing::InitGoogleTest(&argc, argv);
       return RUN_ALL_TESTS();
}
