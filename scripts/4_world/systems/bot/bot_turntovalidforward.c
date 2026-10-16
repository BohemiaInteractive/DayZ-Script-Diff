#ifdef ROBOCLIENT
class BotTurnToValidForward extends BotStateBase
{		
	private int m_AngleCheck = 5; //5 degree incremental checks
	private float m_TurnRate = 2;
	
	private bool m_FoundDirection = false;
	private vector m_ValidDirection = vector.Zero;
	private float m_TurnDirection = 1;
	private float m_MinAngle = 500;

	void BotTurnToValidForward(Bot bot = NULL, BotStateBase parent = NULL)
	{
	}
	
	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		m_ValidDirection = m_Owner.GetDirection();
		m_FoundDirection = false;
		m_MinAngle = 500;
		m_TurnRate = 2;
			
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimLeft);
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimRight);
#endif
	}
	
	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
		
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimLeft);
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimRight);
#endif
	}
	
	BotEventBase GetNextEvent()
	{
		if (m_Bot.m_Target && m_Bot.m_Target.IsAlive())
			return m_Bot.__Atkd__;
		else
			return m_Bot.__VaFw__;
	}
	
	override void OnUpdate (float dt)
	{
		if (!m_FoundDirection)
		{
			vector m_OwnerPosition = m_Owner.GetPosition();
			vector m_WaterPositionCheck = m_OwnerPosition + m_Owner.GetDirection() * 3;
			if (GetGame().SurfaceIsSea(m_WaterPositionCheck[0], m_WaterPositionCheck[2]))
			{
				m_TurnDirection = 1;
				m_ValidDirection = -m_Owner.GetDirection();
				m_FoundDirection = true;
			}
			else
			{
				const vector raycastHeight = "0 0.75 0"; //raycast from a higher point than the feet
				const float startPosOffset = 0.1; //offset for some issues with vaulting
				const float raycastDist = 0.8;
				const float raycastSphereRadius = 0.3;
				vector startPos = m_OwnerPosition + raycastHeight + startPosOffset * m_Owner.GetDirection();
				vector endPos = m_OwnerPosition + raycastHeight + raycastDist * m_Owner.GetDirection();
				vector hitPosition, hitNormal;
				float hitFraction;
				Object hitObject = null;
				PhxInteractionLayers collisionMask = PhxInteractionLayers.DEFAULT | PhxInteractionLayers.BUILDING | PhxInteractionLayers.CHARACTER | PhxInteractionLayers.DOOR | PhxInteractionLayers.DYNAMICITEM | PhxInteractionLayers.FENCE | PhxInteractionLayers.WORLDBOUNDS | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.WATERLAYER;
					
				int leftStreak = 0;
				int rightStreak = 0;
				int rotationsAvailable = (360 / m_AngleCheck) / 2 + 1;
				float lastResortTurnDirection = 0;
				vector lastResortDirection = vector.Zero;
				
				for (int i = 1; i < rotationsAvailable; i++)
				{
					vector newLeftDirection = vector.RotateAroundZeroDeg(m_Owner.GetDirection(), vector.Up, -m_AngleCheck * i);
					vector newRightDirection = vector.RotateAroundZeroDeg(m_Owner.GetDirection(), vector.Up, m_AngleCheck * i);
						
					vector leftEndPos = startPos + newLeftDirection * 3;
					vector rightEndPos = startPos + newRightDirection * 3;
						
					//looking for 2 consecutive valid raycast checks
					bool resLeft = DayZPhysics.SphereCastBullet(startPos, leftEndPos, raycastSphereRadius, collisionMask, m_Owner, hitObject, hitPosition, hitNormal, hitFraction);
					if (!resLeft)
					{
						leftStreak++;
						lastResortTurnDirection = -1;
						lastResortDirection = newLeftDirection;
						
						if (leftStreak == 2)
						{
							m_ValidDirection = vector.RotateAroundZeroDeg(m_Owner.GetDirection(), vector.Up, -(m_AngleCheck * (i - 1) + m_AngleCheck / 2));
							m_TurnDirection = -1;
							m_FoundDirection = true;
							break;
						}
					}
					else
						leftStreak = 0;
					
					bool resRight = DayZPhysics.SphereCastBullet(startPos, rightEndPos, raycastSphereRadius, collisionMask, m_Owner, hitObject, hitPosition, hitNormal, hitFraction);			
					if (!resRight)
					{
						rightStreak++;
						lastResortTurnDirection = 1;
						lastResortDirection = newRightDirection;
						if (rightStreak == 2)
						{
							m_ValidDirection = vector.RotateAroundZeroDeg(m_Owner.GetDirection(), vector.Up, m_AngleCheck * (i - 1) + m_AngleCheck / 2);
							m_TurnDirection = 1;
							m_FoundDirection = true;
							break;
						}
					}
					else
						rightStreak = 0;
					
					if (!m_FoundDirection)
					{
						m_TurnDirection = lastResortTurnDirection;
						m_ValidDirection = lastResortDirection;
						m_FoundDirection = true;
					}
				}
			}
		}
				
		if (!m_FoundDirection)
		{
			m_Bot.ProcessEvent(GetNextEvent());
			return;
		}
		
		float prod = m_Owner.GetDirection().Length() * m_ValidDirection.Length();
		if (prod == 0)
		{
			m_Bot.ProcessEvent(GetNextEvent());
			return;
		}

		float dot = vector.Dot(m_Owner.GetDirection(), m_ValidDirection);
		float angle = Math.Acos(dot / prod);
		angle = Math.RAD2DEG * angle;
		if (angle < 3 || angle > m_MinAngle + 0.5)
		{
			m_Bot.ProcessEvent(GetNextEvent());
			return;
		}
		
		m_MinAngle = Math.Min(angle, m_MinAngle);
		
		float slowDown = 1;
		if (angle < 30)
			slowDown = angle / 30;
		
		slowDown = Math.Clamp(slowDown, 0.2, 1);
		
		int inputID = UAAimLeft;
		if (m_TurnDirection > 0)
			inputID = UAAimRight;
			
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedOverride_ID(inputID, m_TurnDirection * m_TurnRate * slowDown, OverrideInputType.HOLD, false);
#endif
			
		super.OnUpdate(dt);
	}
}
#endif