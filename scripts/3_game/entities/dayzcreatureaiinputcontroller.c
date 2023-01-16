class DayZCreatureAIInputController
{
	proto native void OverrideMovementSpeed(bool state, float movementSpeed);
	proto native float GetMovementSpeed();

    proto native void OverrideTurnSpeed(bool state, float turnSpeed);
    proto native float GetTurnSpeed();

    proto native void OverrideHeading(bool state, float heading);
    proto native float GetHeading();

    proto native void OverrideJump(bool state, int jumpType, float jumpHeight = 0);
    proto native bool IsJump();
    proto native int GetJumpType();
    proto native float GetJumpHeight();

    proto native void OverrideLookAt(bool state, vector direction);
	proto native bool IsLookAtEnabled();
	proto native vector GetLookAtDirectionWS();

    proto native void OverrideAlertLevel(bool state, bool alerted, int level, float inLevel);
	proto native int GetAlertLevel();
	proto native float GetAlertInLevel();
	proto native bool IsAlerted();

    proto native void OverrideBehaviourSlot(bool state, int slot);
	proto native int GetBehaviourSlot();
	
	//--------------------------------------------------------------
	//! never created by script
	private void DayZCreatureAIInputController()
	{
	}

	//! never destroyed by script
	private void ~DayZCreatureAIInputController()
	{
	}
};
