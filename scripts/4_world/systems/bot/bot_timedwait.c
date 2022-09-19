
class BotEventWaitTimeout : BotEventBase { };

class BotTimedWait extends BotStateBase
{
	protected float m_dtAccumulator = 0.0;
	protected float m_Timeout = 3.0;
	protected bool m_Periodic = true;

	void BotTimedWait (Bot bot = NULL, BotStateBase parent = NULL, float timeout = 3.0)
	{
		m_Timeout = timeout;
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
		
		float rescanTime = m_Timeout;
		if (m_dtAccumulator >= rescanTime)
		{
			OnTimeout();

			if (m_Periodic)
				m_dtAccumulator = 0.0;
			else
				m_dtAccumulator = -1.0;
		}
	}

	void OnTimeout ()
	{
		botDebugSpam("[bot] + " + m_Owner + " BotTimedWait::OnTimeout");
		m_Bot.ProcessEvent(new BotEventWaitTimeout(m_Owner));
	}
};

