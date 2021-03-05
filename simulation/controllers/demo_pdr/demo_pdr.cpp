/* Include the controller definition */
#include "demo_pdr.h"
/* Function definitions for XML parsing */
#include <argos3/core/utility/configuration/argos_configuration.h>
/* 2D vector definition */
#include <argos3/core/utility/math/vector2.h>
/* Logging */
#include <argos3/core/utility/logging/argos_log.h>
#define CRITICAL_VALUE 100.0f
/****************************************/
/****************************************/

CRadians* decideTurn(float left, float right)
{
   if (right < 0.0 || right > left)
   {
      return new CRadians(-CRadians::PI_OVER_FOUR);
   }
   return new CRadians(CRadians::PI_OVER_FOUR);
}
CDemoPdr::CDemoPdr() : m_pcDistance(NULL),
                       m_pcPropellers(NULL),
                       m_pcPos(NULL),
                       m_uiCurrentStep(0) {}

/****************************************/
/****************************************/

void CDemoPdr::Init(TConfigurationNode &t_node)
{
   m_pcDistance = GetSensor<CCI_CrazyflieDistanceScannerSensor>("crazyflie_distance_scanner");
   m_pcPropellers = GetActuator<CCI_QuadRotorPositionActuator>("quadrotor_position");
   try
   {
      m_pcPos = GetSensor<CCI_PositioningSensor>("positioning");
   }
   catch (CARGoSException &ex)
   {
   }
   /*
    * Initialize other stuff
    */
   /* Create a random number generator. We use the 'argos' category so
      that creation, reset, seeding and cleanup are managed by ARGoS. */
   m_pcRNG = CRandom::CreateRNG("argos");

   count = 0;
   m_uiCurrentStep = 0;
   Reset();
   lockAngle = *(new CRadians(0.1f));
   CRadians *useless = new CRadians(0.1f);
   m_pcPos->GetReading().Orientation.ToEulerAngles(lockAngle, *useless, *useless);
}

/****************************************/
/****************************************/

void CDemoPdr::ControlStep()
{
   CVector3 cPos = m_pcPos->GetReading().Position;
   //Real angle = m_pcPos->GetReading().Orientation.GetZ();
   CCI_CrazyflieDistanceScannerSensor::TReadingsMap sDistRead = m_pcDistance->GetReadingsMap();
   auto iterDistRead = sDistRead.begin();
   float rightDist = (iterDistRead++)->second;
   float frontDist = (iterDistRead++)->second;
   float leftDist = (iterDistRead++)->second;
   float backDist = (iterDistRead++)->second;
   if (sDistRead.size() == 4)
   {
      LOG << "Front dist: " << frontDist << std::endl;
      LOG << "Left dist: " << leftDist << std::endl;
      LOG << "Back dist: " << backDist << std::endl;
      LOG << "Right dist: " << rightDist << std::endl;
   }
   //LOG << m_pcPos->GetReading().Orientation << std::endl;

   count--;
   if (m_uiCurrentStep < 20) // decolage
   {
      cPos.SetZ(cPos.GetZ() + 0.25f);
      m_pcPropellers->SetAbsolutePosition(cPos);
   }
   else
   {
      if (frontDist < CRITICAL_VALUE && frontDist >= 0 && count<=0)
      {
         if (turnAngle == nullptr)
         {
            turnAngle = decideTurn(leftDist, rightDist);
         }
         lockAngle += *turnAngle;
         m_pcPropellers->SetAbsoluteYaw(lockAngle);
	      count = 40;
      }
      else if (rightDist < 50.0 && rightDist >= 0)
      {
	      newCVector = new CVector3(
             (cos(lockAngle.GetValue() + 1.56) * 0.1 + cPos.GetX()) * 1,
             (sin(lockAngle.GetValue() + 1.56) * 0.1 + cPos.GetY()) * 1,
             cPos.GetZ());
         m_pcPropellers->SetAbsolutePosition(*newCVector);
      }
      else if (leftDist < 50.0 && leftDist >= 0)
      {
         newCVector = new CVector3(
             (cos(lockAngle.GetValue() - 0.8) * 0.1) * 1,
             (sin(lockAngle.GetValue() - 0.8) * 0.1) * 1,
             cPos.GetZ());
         m_pcPropellers->SetRelativePosition(*newCVector);

      }
      else if (backDist < 50.0 && backDist >= 0)
      {
         newCVector = new CVector3(
             (cos(lockAngle.GetValue() - 0.8) * 0.1 + cPos.GetX()) * 1,
             (sin(lockAngle.GetValue() - 0.8) * 0.1 + cPos.GetY()) * 1,
             cPos.GetZ());
         m_pcPropellers->SetAbsolutePosition(*newCVector);

      }
      else if ((frontDist > CRITICAL_VALUE || frontDist == -2) && count<=0) // front
      {
         turnAngle = nullptr;
         newCVector = new CVector3(
             (cos(lockAngle.GetValue()) * 0.4 + cPos.GetX()) * 1,
             (sin(lockAngle.GetValue()) * 0.4 + cPos.GetY()) * 1,
             cPos.GetZ());
         m_pcPropellers->SetAbsolutePosition(*newCVector);
      }
   }
   m_uiCurrentStep++;
}



/****************************************/
/****************************************/

void CDemoPdr::Reset()
{
   m_uiCurrentStep = 0;
}

/****************************************/
/****************************************/

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the XML configuration file to refer to
 * this controller.
 * When ARGoS reads that string in the XML file, it knows which controller
 * class to instantiate.
 * See also the XML configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CDemoPdr, "demo_pdr_controller")

