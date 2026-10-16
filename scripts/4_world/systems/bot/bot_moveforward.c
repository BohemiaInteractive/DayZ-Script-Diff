#ifdef ROBOCLIENT
class BotMoveForward extends BotStateBase
{	
	private bool m_FirstTime = true;
	private bool m_PrevIsClimbing = false;
	private float m_PrevMaxClimbDist = 0;
	
	void BotMoveForward(Bot bot = NULL, BotStateBase parent = NULL)
	{
	}
	
	//copied from DayZPlayerImplementJumpClimb
	private int GetClimbType(float pHeight)
	{
		int climbType = -1;
		if (pHeight < 1.1)
			climbType = 0;
		else if (pHeight >= 1.1 && pHeight < 1.7)
			climbType = 1;
		else if (pHeight >= 1.7 && pHeight < 2.75)
			climbType = 2;    
        
  		return climbType;
	}
	
	override void OnEntry (BotEventBase e)
	{
		super.OnEntry(e);
		m_PrevMaxClimbDist = m_Owner.GetDayZPlayerType().CommandClimbSettingsW().m_fFwMaxDistance;
		GetUApi().GetInputByID(UAMoveForward).ForceEnable(true);
	}
	
	override void OnExit (BotEventBase e)
	{
		super.OnExit(e);
		
		GetUApi().GetInputByID(UAMoveForward).ForceEnable(false);
	}
	
	override void OnUpdate (float dt)
	{
		bool isClimbing = m_Bot.m_Owner.IsClimbing();
		if (!isClimbing)
		{
			DayZPlayer player = g_Game.GetPlayer();
			vector m_OwnerPosition = m_Owner.GetPosition();
			vector m_WaterPositionCheck = m_OwnerPosition + m_Owner.GetDirection() * 3;
			if (m_PrevIsClimbing)
			{
				m_Owner.GetDayZPlayerType().CommandClimbSettingsW().m_fFwMaxDistance = m_PrevMaxClimbDist;
			}
			else if (GetGame().SurfaceIsSea(m_WaterPositionCheck[0], m_WaterPositionCheck[2]))
			{
				
				m_Owner.GetDayZPlayerType().CommandClimbSettingsW().m_fFwMaxDistance = m_PrevMaxClimbDist;
				m_Bot.ProcessEvent(m_Bot.__Obst__);
			}
			
			const vector raycastHeight = "0 0.75 0"; //raycast from a higher point
			const float startPosOffset = 0.1; //offset bc issues with climbing
			const float raycastDist = 0.8;
			const float raycastSphereRadius = 0.3;
			vector startPos = m_OwnerPosition + raycastHeight + startPosOffset * m_Owner.GetDirection();
			vector endPos = m_OwnerPosition + raycastHeight + raycastDist * m_Owner.GetDirection();
			vector hitPosition, hitNormal;
			float hitFraction;
			Object hitObject = null;
				
			PhxInteractionLayers collisionMask = PhxInteractionLayers.DEFAULT | PhxInteractionLayers.BUILDING | PhxInteractionLayers.CHARACTER | PhxInteractionLayers.DOOR | PhxInteractionLayers.DYNAMICITEM | PhxInteractionLayers.FENCE | PhxInteractionLayers.WORLDBOUNDS | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.WATERLAYER;
			bool hit = DayZPhysics.SphereCastBullet(startPos, endPos, raycastSphereRadius, collisionMask, m_Owner, hitObject, hitPosition, hitNormal, hitFraction);
			if (hit)
			{
				m_PrevMaxClimbDist = m_Owner.GetDayZPlayerType().CommandClimbSettingsW().m_fFwMaxDistance;
				m_Owner.GetDayZPlayerType().CommandClimbSettingsW().m_fFwMaxDistance = 2.5;
				SHumanCommandClimbResult climbRes = new SHumanCommandClimbResult();
				HumanCommandClimb.DoPerformClimbTest(player, climbRes, 0);
				int climbType = GetClimbType(climbRes.m_fClimbHeight);

				if ((climbRes.m_bIsClimb || climbRes.m_bIsClimbOver) && m_Owner.CanClimb(climbType, climbRes))
				{
#ifdef INPUT_OVERRIDE
					player.GetInputInterface().SyncedOverride_ID(UAGetOver, 1, OverrideInputType.PRESS, true);
#endif
				}		
				else
				{
					m_Owner.GetDayZPlayerType().CommandClimbSettingsW().m_fFwMaxDistance = m_PrevMaxClimbDist;
					m_Bot.ProcessEvent(m_Bot.__Obst__);
				}
			}
		}
		m_PrevIsClimbing = isClimbing;
		super.OnUpdate(dt);
	}
}
#endif