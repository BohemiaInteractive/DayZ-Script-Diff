#ifdef ROBOCLIENT
class BotAttack extends BotStateBase
{	
	private const int m_InputsDelayLoop = 200;
	
	private bool m_AimOverrideVertical = false;
	private bool m_AimOverrideHorizontal = false;
	
	private const float m_HorizontalTurnRate = 0.5;
	private const float m_VerticalTurnRate = 0.25;
	private const float m_InitialCheckTime = 1;
	private float m_InitialCheckTimeElapsed = 0;
	
	void BotAttack(Bot bot = NULL, BotStateBase parent = NULL)
	{
	}
	
	private void ActivateInputs()
	{
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedOverride_ID(UAFire, 1, OverrideInputType.HOLD, false);
		g_Game.GetPlayer().GetInputInterface().SyncedOverride_ID(UADefaultAction, 1, OverrideInputType.HOLD, false);
#endif
	}
	
	private void DeactivateInputs()
	{
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAFire);
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UADefaultAction);
#endif
	}
	
	private void CallDeactivateInputs()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeactivateInputs, m_InputsDelayLoop, true);
	}
	
	private void RemoveHorizontalAim(bool Force = false)
	{
		if (!m_AimOverrideHorizontal && !Force)
			return;

#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimLeft);
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimRight);
#endif
		m_AimOverrideHorizontal = false;
	}
	
	private void RemoveVerticalAim(bool Force = false)
	{
		if (!m_AimOverrideVertical && !Force)
			return;
		
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimUp);
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UAAimDown);
#endif
		m_AimOverrideVertical = false;
	}
	
	private void HandleHorizontalAim()
	{
		bool hasTarget = m_Bot.m_Target && m_Bot.m_Target.IsAlive();
		if (!hasTarget)
		{
			RemoveHorizontalAim();
			return;
		}
		
		vector direction = vector.Direction(m_Owner.GetPosition(), m_Bot.m_Target.GetPosition());
		direction[1] = 0;
		direction.Normalize();
		float angle = Math.RAD2DEG * Math.Atan2(m_Owner.GetDirection()[0] * direction[2] - m_Owner.GetDirection()[2] * direction[0], vector.Dot(m_Owner.GetDirection(), direction));
		if (Math.AbsFloat(angle) < 3)
		{
			RemoveHorizontalAim();
			return;
		}
		
		int turnDirection = 1;
		if (angle > 0)
			turnDirection = -1;

		float slowDown = 1;
		if (Math.AbsFloat(angle) < 15)
			slowDown = Math.AbsFloat(angle) / 15;
		slowDown = Math.Clamp(slowDown, 0.2, 1);
		
		int overrideInput = UAAimRight;
		int removeOverrideInput = UAAimLeft;
		if (turnDirection < 0)
		{
			overrideInput = UAAimLeft;
			removeOverrideInput = UAAimRight;
		}
		
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(removeOverrideInput);
		g_Game.GetPlayer().GetInputInterface().SyncedOverride_ID(overrideInput, m_HorizontalTurnRate * slowDown, OverrideInputType.HOLD, false);
#endif
		m_AimOverrideHorizontal = true;
	}

	private void HandleVerticalAim()
	{
		if (!m_Bot.m_Target || !m_Bot.m_Target.IsAlive())
			return;

		vector cameraPos, cameraDir, cameraRot;
		g_Game.GetPlayer().GetCurrentCameraTransform(cameraPos, cameraDir, cameraRot);
		
		vector cameraPos2D = cameraPos;
		cameraPos2D[1] = 0;
		
		vector targetPos2D = m_Bot.m_Target.GetPosition();
		targetPos2D[1] = 0;
		
		float distToTarget = vector.DistanceSq(cameraPos2D, targetPos2D);

		//Target is too far/ran away
		if (distToTarget > 4)
		{
			RemoveVerticalAim();
			return;
		}
		
		vector minTarget, maxTarget;
		m_Bot.m_Target.GetPhysics().GetGeomWorldBounds(0, minTarget, maxTarget);	
		
		//Distance from camera aim to target x/z
		float distAimToTarget = distToTarget / Math.Cos(g_Game.GetPlayer().GetCurrentCamera().GetCurrentPitch() * Math.DEG2RAD);

		vector cameraAimTargetPoint = distAimToTarget * cameraDir + cameraPos;
		
		//difference between camera aim y and target head
		float heightDiff = cameraAimTargetPoint[1] - maxTarget[1];
		
		if (0 < heightDiff && heightDiff < 0.15)
		{
			RemoveVerticalAim();
			return;
		}
		
		int overrideInput = UAAimUp;
		int removeOverrideInput = UAAimDown;
		float turnDirection = 1;
		
		if (heightDiff >= 0.15)
		{
			overrideInput = UAAimDown;
			removeOverrideInput = UAAimUp;
			turnDirection = -1;
		}
		
#ifdef INPUT_OVERRIDE
 		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(removeOverrideInput);
		g_Game.GetPlayer().GetInputInterface().SyncedOverride_ID(overrideInput, m_VerticalTurnRate, OverrideInputType.HOLD, false);
#endif
		m_AimOverrideVertical = true;
	}
	
	private void HandleAim()
	{
		HandleVerticalAim();
		HandleHorizontalAim();
		return;
	}
	
	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
			
		m_InitialCheckTimeElapsed = 0;
		m_AimOverrideVertical = false;
		m_AimOverrideHorizontal = false;
					
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedOverride_ID(UATempRaiseWeapon, 1, OverrideInputType.CLICK, false);
#endif
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ActivateInputs, m_InputsDelayLoop, true);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CallDeactivateInputs, m_InputsDelayLoop - 100, false);
	}
	
	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
		
		if (m_Bot.m_Target && !m_Bot.m_Target.IsAlive())
			m_Bot.m_Target = null;
		
#ifdef INPUT_OVERRIDE
		g_Game.GetPlayer().GetInputInterface().SyncedRemoveOverride_ID(UATempRaiseWeapon);
#endif
		
		RemoveHorizontalAim(true);
		RemoveVerticalAim(true);
		DeactivateInputs();
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ActivateInputs);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(DeactivateInputs);
	}
	
	override void OnUpdate (float dt)
	{
		super.OnUpdate(dt);
		
		HandleAim();
		m_InitialCheckTimeElapsed += dt;
		
		if (!m_Owner.GetMeleeCombat().GetTargetEntity() && m_InitialCheckTimeElapsed > m_InitialCheckTime)
		{	
			m_InitialCheckTimeElapsed = 0;
			m_Bot.CheckForEnemies(true);
		}
	}
}
#endif