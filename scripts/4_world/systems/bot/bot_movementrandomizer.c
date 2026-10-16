#ifdef ROBOCLIENT
class BotMovementRandomizer extends BotStateBase
{
	ref HumanMovementState m_State = new HumanMovementState();

	float m_dtAccumulator = 0.0;

	void BotMovementRandomizer(Bot bot = NULL, BotStateBase parent = NULL)
	{
	}

	override void OnEntry (BotEventBase e)
	{
		m_dtAccumulator = 0.0;

		super.OnEntry(e);
	}

	override void OnExit (BotEventBase e)
	{
		m_dtAccumulator = 0.0;

		super.OnExit(e);
	}

	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);

		m_dtAccumulator += dt;
		
		GetPlayerOwner().GetMovementState(m_State);

		HumanCommandMove cm = GetPlayerOwner().GetCommand_Move();
		
		HumanInputController hic = GetPlayerOwner().GetInputController();
		
		hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ONE_FRAME, 3.0);
		hic.OverrideMovementAngle(HumanInputControllerOverrideType.ONE_FRAME, 0.0);
	}
};
#endif