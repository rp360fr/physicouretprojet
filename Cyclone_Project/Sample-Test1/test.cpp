#include "pch.h"
#include "../Cyclone_Library/core.h"
#include "../Cyclone_Library/particle.h"
TEST(TestCaseName2, TestName2)
{

    
    cyclone::Vector3 a = {3,4,12};
    auto b = a.magnitude();
    // EXPECT_EQ(1, 1);
    // EXPECT_TRUE(true);
    EXPECT_NEAR(b,13,0.001);
 

}
TEST(TestCaseName, TestName) {

    
  cyclone::Vector3 a = {0,3,4};
 a.normalise();
  // EXPECT_EQ(1, 1);
  // EXPECT_TRUE(true);
  EXPECT_NEAR(a.x, 0, 0.001);
  EXPECT_NEAR(a.y, 0.6, 0.001);
  EXPECT_NEAR(a.z, 0.8, 0.02);
    

}

TEST(TestCaseName1, TestName1)
{
    cyclone::Vector3 a = {1,2,3};
    cyclone::Vector3 b = {-2,5,4};
    auto c = a.scalarProduct(b);
    EXPECT_NEAR(c,20,0.001);
   
}

TEST(TestCaseName3, TestPaticule)
{
cyclone::Particle particule;
    particule.setPosition(0,0,0);
    particule.setVelocity(2,0,0);
    particule.setAcceleration(0,0,0);
    particule.setDamping(1);
    particule.setMass(1);
    particule.integrate(0.5);
    
    EXPECT_NEAR(particule.getPosition().x, 1, 0.001);
    
    
    
}