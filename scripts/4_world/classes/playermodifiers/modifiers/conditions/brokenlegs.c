class BrokenLegsMdfr: ModifierBase
{
	private const 	float	HEALTHY_LEG = 100; //Health at which legs are no longer considered broken
	private 		int 	currentState = -1; //Current broken leg state used upon reconnect
	private const 	float	TIME_TO_UPDATE = 0.5;
	private			float	elapsedTime = TIME_TO_UPDATE + 1;
	
	#ifdef DEVELOPER
	ref static Timer timer;
	#endif

	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_BROKEN_LEGS;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 0.5;
		m_ActivationType 		= EActivationType.TRIGGER_EVENT_ON_ACTIVATION;
		m_IsPersistent			= true;
	}
	
	override void OnActivate(PlayerBase player)
	{
		//Print("------------> OnActivate - BrokenLegsMdfr");
		player.SetBrokenLegs(-eBrokenLegs.BROKEN_LEGS);//note the negative sign
		player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_FRACTURE);
	}
	
	override void OnReconnect(PlayerBase player)
	{
		//Print("------------> OnReconnect - BrokenLegsMdfr");
		player.SetBrokenLegs(player.GetBrokenLegs());
		player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_FRACTURE);
	}
	
	
	override void OnDeactivate(PlayerBase player)
	{
		player.UpdateBrokenLegs(eBrokenLegs.NO_BROKEN_LEGS);
		if ( player.IsWearingSplint() )
		{
			MiscGameplayFunctions.RemoveSplint(player); //Remove splint when leg is healed
		}
		
		player.SetBrokenLegs(eBrokenLegs.NO_BROKEN_LEGS);
		player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_FRACTURE);
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		if ( player.GetHealth("RightLeg", "Health") >= HEALTHY_LEG && player.GetHealth("LeftLeg", "Health") >= HEALTHY_LEG )
		{
			
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.UpdateBrokenLegs(/*legacy param - value not used*/0);
	}
	

	#ifdef DEVELOPER
	static void DelayedRequest(PlayerBase player, bool state)
	{
		timer = new Timer( CALL_CATEGORY_SYSTEM );
		if(state)
			timer.Run( 10 , player.GetModifiersManager().GetModifier(eModifiers.MDF_BROKEN_LEGS), "Delayed", new Param1<bool>(state));
		else
			BrokenLegsMdfr.Cast(player.GetModifiersManager().GetModifier(eModifiers.MDF_BROKEN_LEGS)).Delayed(false);
	}
	
	void Delayed(bool state)
	{
		m_Player.GetModifiersManager().ActivateModifier(eModifiers.MDF_BROKEN_LEGS);
	}
	#endif
};
