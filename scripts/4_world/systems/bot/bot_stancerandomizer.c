class BotStanceRandomizer extends BotStateBase
{
	ref HumanMovementState m_State = new HumanMovementState();

	float m_dtAccumulator = 0.0;

	void BotStanceRandomizer(Bot bot = NULL, BotStateBase parent = NULL)
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
		if (cm && !cm.IsChangingStance())
		{
			if (m_State.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE)
			{
				cm.ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
			}
			else
			{
				cm.ForceStance(DayZPlayerConstants.STANCEIDX_PRONE);
			}
		}
	}
};
