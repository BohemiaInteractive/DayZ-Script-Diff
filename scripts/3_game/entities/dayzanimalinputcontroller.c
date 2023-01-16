//! defined in C++
enum DayZAnimalBehaviourSlot
{
	NONE,
    CALM,
    CALM_RESTING,
    CALM_GRAZING,
    CALM_TRAVELLING,
    DRINKING,
    NON_SPECIFIC_THREAT,
    SPECIFIC_THREAT,
    ALERTED,
    ATTRACTED,
    SCARED,
    HUNTING,
    EATING,
    SIEGE,
    FIREPLACE,
    ENRAGED,
    ENRAGED_TARGETLOST,
    PREATTRACTED,
    INTIMIDATE,
};

//! defined in C++
enum DayZAnimalBehaviourAction
{
    NONE_INPUT,
    SAFETY_INPUT,
    GRAZE_WALKING_INPUT,
    GRAZE_ON_SPOT_INPUT,
    RESTING_INPUT,
    TRAVELING_INPUT,
    EATING_INPUT,
    DRINKING_INPUT,
    CHARGING,
    APPROACHING_INPUT,
    REACH_INPUT,
    WALKING_INPUT,
    IDLE1_INPUT,
    IDLE2_INPUT,
    IDLE3_INPUT,
    THREAT_WALK_AWAY,
    THREAT_WALK_TO,
    THREAT_STAY_LOOKAT,
    THREAT_STAY,
};

class DayZAnimalInputController : DayZCreatureAIInputController
{
	proto native bool IsDead();
	proto native bool IsAttack();

    proto native void OverrideBehaviourAction(bool state, int action);
	proto native int GetBehaviourAction();
	
	//--------------------------------------------------------------

	//! never created by script
	private void DayZAnimalInputController()
	{
	}

	//! never destroyed by script
	private void ~DayZAnimalInputController()
	{
	}	
};
