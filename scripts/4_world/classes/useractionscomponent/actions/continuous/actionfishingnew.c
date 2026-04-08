class FishingActionReceiveData : ActionReciveData
{
	bool m_IsSurfaceSea;
} 

class FishingActionData : ActionData
{
	bool 	m_IsSurfaceSea;
	int 	m_FishingResult = -1; //use for hashed name?
	bool 	m_IsSignalActive = false;
	
	int 	m_SplashUniqueID = -1;
	
	ref CatchingContextFishingRodAction m_ContextData;
	
	void RollNextResultChance()
	{
		CAContinuousRepeatFishing component;
		Class.CastTo(component,m_ActionComponent);
		
		m_IsSignalActive = false; //redundant?
		if (m_ContextData.RollCatch())
		{
			RandomizeSignalValues();
		}
		else
		{
			component.SetNewSignalData(-1,-1);
		}
	}
	
	//signal methods
	protected void RandomizeSignalValues()
	{
		CAContinuousRepeatFishing component;
		Class.CastTo(component,m_ActionComponent);
		
		component.SetNewSignalData(m_ContextData.RandomizeSignalStartTime(),m_ContextData.RandomizeSignalDuration());
	}
	
	void AnimateRod(float phase, bool instant = false)
	{
		FishingRod_Base_New rod = FishingRod_Base_New.Cast(m_MainItem);
		rod.AnimateFishingRodEx(phase,instant);
	}
	
	//------------------------------------------------------------------
	//DEPRECATED
	//------------------------------------------------------------------
	const float FISHING_SUCCESS 		= 0.2;
	const float FISHING_BAIT_LOSS 		= 0.02;
	const float FISHING_HOOK_LOSS 		= 0.015;
	const float FISHING_DAMAGE 			= 1.5;
	const float FISHING_GARBAGE_CHANCE 	= 0.2;
	bool 		m_IsBaitAnEmptyHook;
	float 		m_RodQualityModifier = 0;
	ItemBase 	m_Bait;
	void InitBait(ItemBase item)
	{
		m_Bait = item;
		m_IsBaitAnEmptyHook = !m_Bait.ConfigIsExisting("hookType");
	}
	bool IsBaitEmptyHook()
	{
		return m_IsBaitAnEmptyHook;
	}
}

class ActionFishingNewCB : ActionContinuousBaseCB
{
	FishingActionData 	m_ActionDataFishing;
	
	override void CreateActionComponent()
	{
		EnableStateChangeCallback();
		float cycleTime = UAFishingConstants.CYCLE_LENGTH_BASE; //redundant?
		if (Class.CastTo(m_ActionDataFishing,m_ActionData))
		{
			cycleTime = m_ActionDataFishing.m_ContextData.GetActionCycleTime();
		}
		m_ActionData.m_ActionComponent = new CAContinuousRepeatFishing(cycleTime);
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		
		RegisterAnimationEvent("ParticleSplash", UAFishingConstants.EVENT_SPLASH_SIGNAL);
		RegisterAnimationEvent("EventFishPlacement", UAFishingConstants.EVENT_ANIMATE_ROD_CLOSE);
	}
	
	override void EndActionComponent()
	{
		bool animateRod = false;
		if (m_ActionDataFishing.m_State == UA_CANCEL)
			m_Canceled = true;
		
		if (m_ActionDataFishing.m_State == UA_FINISHED)
		{
			if (m_ActionDataFishing.m_FishingResult == 1)
			{
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
			else
			{
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
				animateRod = true;
			}
		}
		else
		{
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
			animateRod = true;
		}
		
		animateRod |= m_Canceled;
		if (animateRod)
		{
			m_ActionDataFishing.AnimateRod(false,m_Canceled);
		}
		
		DestroySplashEffectSynced();
	}
	
	override void OnStateChange(int pOldState, int pCurrentState)
	{
		super.OnStateChange(pOldState,pCurrentState);
		
		if (pOldState == STATE_NONE && pCurrentState == STATE_LOOP_IN)
		{
			m_ActionDataFishing.AnimateRod(true);
		}
	}
	
	void ToggleFishBiting()
	{
		if (GetState() == STATE_LOOP_LOOP || GetState() == STATE_LOOP_LOOP2)
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTIONLOOP);
	}
	
	//! Destroys the effecter, but lets the rest of the particle play out
	void DestroySplashEffectSynced()
	{
		if (g_Game.IsServer() && m_ActionDataFishing.m_SplashUniqueID >= 0)
		{
			SEffectManager.DestroyEffecterParticleServer(m_ActionDataFishing.m_SplashUniqueID);
			m_ActionDataFishing.m_SplashUniqueID = -1;
		}
	}
	
	//DEPRECATED
	ref array<string> 	m_JunkTypes = {};
	void HandleFishingResultSuccess();
	void HandleFishingResultFailure();
};

class ActionFishingNew: ActionContinuousBase
{
	private const string ALLOWED_WATER_SURFACES = string.Format("%1|%2", UAWaterType.SEA, UAWaterType.FRESH);

	void ActionFishingNew()
	{
		m_CallbackClass 	= ActionFishingNewCB;
		m_SpecialtyWeight 	= UASoftSkillsWeight.PRECISE_MEDIUM;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_FISHING;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text 				= "#start_fishing";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTWaterSurfaceEx(30, LIQUID_SALTWATER|LIQUID_FRESHWATER);
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}
	
	override bool CanBeUsedInFreelook()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase hook;
		hook = ItemBase.Cast(item.FindAttachmentBySlotName("Hook"));
		
		return hook && !hook.IsRuined() && !hook.IsPendingDeletion();
	}
	
	override ActionData CreateActionData()
	{
		FishingActionData actionData = new FishingActionData();
		return actionData;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = null)
	{
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			FishingActionData data;
			if (Class.CastTo(data,action_data))
			{
				ComposeLocalContextData(data);
				return true;
			}
		}
		return false;
	}
	
	protected void ComposeLocalContextData(FishingActionData data)
	{
		#ifndef SERVER
		data.m_IsSurfaceSea = CheckForSea(data); //already synced from client by this point
		#endif
		Param2<EntityAI,bool> par = new Param2<EntityAI,bool>(data.m_MainItem,data.m_IsSurfaceSea);
		data.m_ContextData = new CatchingContextFishingRodAction(par);
		
		data.m_ContextData.GenerateResult(); //call only once
		
		#ifdef DEVELOPER
		data.m_ContextData.SetResultChanceOverride(data.m_Player.IsQuickFishing(),1.0); //active override sets chance to 100% for the action duration
		#endif
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		FishingActionData data
		if (Class.CastTo(data,action_data))
		{
			ctx.Write(data.m_IsSurfaceSea);
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if (!action_recive_data)
		{
			action_recive_data = new FishingActionReceiveData;
		}
		
		super.ReadFromContext(ctx, action_recive_data);
		
		FishingActionReceiveData receiveDataFishing;
		if (Class.CastTo(receiveDataFishing,action_recive_data))
		{
			if (!ctx.Read(receiveDataFishing.m_IsSurfaceSea))
				return false;
			
			return true;
		}
		
		return false;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		FishingActionReceiveData receiveDataFishing = FishingActionReceiveData.Cast(action_recive_data);
		FishingActionData fishingData = FishingActionData.Cast(action_data);
		
		fishingData.m_IsSurfaceSea = receiveDataFishing.m_IsSurfaceSea;
	}
	
	override protected void CreateAndSetupActionCallback(ActionData action_data)
	{
		super.CreateAndSetupActionCallback(action_data);
		
		FishingActionData data;
		if (Class.CastTo(data,action_data))
		{
			data.RollNextResultChance(); //performed here so that component exists
		}
	}
	
	override void OnAnimationEvent( ActionData action_data )
	{
		super.OnAnimationEvent(action_data);
		FishingActionData actionDataFishing = FishingActionData.Cast(action_data);
		
		switch (action_data.m_DelayedAnimationEventID)
		{
			case UAFishingConstants.EVENT_SPLASH_SIGNAL: 
				PlaySplashEffectSynced(actionDataFishing);
			break;
			
			case UAFishingConstants.EVENT_ANIMATE_ROD_CLOSE:
				actionDataFishing.AnimateRod(false,false);
			break;
		}
	}
	
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);
		
		vector dir = vector.Direction(action_data.m_Player.GetPosition(), action_data.m_Target.GetCursorHitPos());
		dir[1] = 0;
		dir.Normalize();
		action_data.m_Player.AlignDirectionWS(dir);
	}
		
	void PlaySplashEffectSynced(FishingActionData actionDataFishing)
	{
		if (g_Game.IsServer())
		{
			int particleId = actionDataFishing.m_ContextData.GetResultParticleId();
			if (actionDataFishing.m_SplashUniqueID < 0)
			{
				actionDataFishing.m_SplashUniqueID = SEffectManager.CreateParticleServer(actionDataFishing.m_Target.GetCursorHitPos(), new ParticleEffecterParameters("ParticleEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, particleId));
			}
			else
			{
				SEffectManager.ReinitParticleServer(actionDataFishing.m_SplashUniqueID, new ParticleEffecterParameters("ParticleEffecter", EffecterBase.NOT_DEFINED_LIFESPAN, particleId)); //reinit here, since particleId might differ
				SEffectManager.ReactivateParticleServer(actionDataFishing.m_SplashUniqueID);
			}
		}
	}
	
	protected bool CheckForSea(ActionData action_data)
	{
		bool ret = false;
		vector cursorPosition = action_data.m_Target.GetCursorHitPos();
		
		if (g_Game.SurfaceIsSea(cursorPosition[0], cursorPosition[2]))
		{
			ret = true;
		}
		
		return ret;
	}
	
	//------------------------------------------------------
	
	override void OnFinishProgress(ActionData action_data)
	{
		super.OnFinishProgress(action_data);
		
		FishingActionData data;
		if (Class.CastTo(data,action_data))
		{
			data.RollNextResultChance();
		}
	}
	
	override void OnEndInput(ActionData action_data)
	{
		ActionContinuousBaseCB callback;
		if (Class.CastTo(callback, action_data.m_Callback))
		{
			switch (callback.GetState())
			{
				case HumanCommandActionCallback.STATE_LOOP_ACTION:
				case HumanCommandActionCallback.STATE_LOOP_LOOP:
				case HumanCommandActionCallback.STATE_LOOP_LOOP2:
					FishingActionData data;
					if (Class.CastTo(data,action_data))
					{
						data.m_FishingResult = EvaluateFishingResult(action_data);
						if (data.m_FishingResult != -1)
							data.m_State = UA_FINISHED;
					}
					break;
				
				default:
					super.OnEndInput(action_data);
					callback.EndActionComponent(); //think about putting this everywhere, interrupts HumanCommandActionCallback.STATE_LOOP_IN neatly
					break;
			}
		}
	}
	
	override void OnEnd(ActionData action_data)
	{
		super.OnEnd(action_data);
		
		FishingActionData fad;
		if (Class.CastTo(fad,action_data))
			fad.AnimateRod(false,true); //just in case
	}
	
	//! Player input reaction evaluation
	int EvaluateFishingResult(ActionData action_data)
	{
		FishingActionData fad;
		if (Class.CastTo(fad,action_data))
		{
			//legacy stuff
			ActionFishingNewCB legacyCB = ActionFishingNewCB.Cast(fad.m_Callback);
			if (fad.m_IsSignalActive)//catch
			{
				fad.m_ContextData.OnBeforeSpawnSignalHit();
				
				if (!g_Game.IsMultiplayer() || g_Game.IsDedicatedServer())
					TrySpawnCatch(fad);
				
				fad.m_ContextData.OnAfterSpawnSignalHit();
				if (legacyCB)
					legacyCB.HandleFishingResultSuccess();
				
				EntityAI hookCheck = action_data.m_MainItem.FindAttachmentBySlotName("Hook");
				return hookCheck != null && !hookCheck.IsPendingDeletion(); //success animation only if hook loss didn't prevent catch (local)
			}
			else//fail
			{
				fad.m_ContextData.OnSignalMiss();
				if (legacyCB)
					legacyCB.HandleFishingResultFailure();
				
				return false;
			}
		}
		
		return -1;
	}
	
	//------------------------------------------------------
	void OnSignalStart(FishingActionData action_data)
	{
		action_data.m_IsSignalActive = true;
		ActionFishingNewCB fishingCB;
		if (Class.CastTo(fishingCB,action_data.m_Callback))
			fishingCB.ToggleFishBiting();
	}
	
	void OnSignalEnd(FishingActionData action_data)
	{
		action_data.m_IsSignalActive = false;
		ActionFishingNewCB fishingCB;
		if (Class.CastTo(fishingCB,action_data.m_Callback))
			fishingCB.ToggleFishBiting();
		
		action_data.m_ContextData.OnSignalPass();
	}
	
	protected EntityAI TrySpawnCatch(FishingActionData action_data)
	{
		vector spawnPos = vector.Direction(action_data.m_Player.GetPosition(),action_data.m_Target.GetCursorHitPos()); //direction first
		spawnPos = MiscGameplayFunctions.GetRandomizedPositionVerified(action_data.m_Player.GetPosition(),action_data.m_Player.GetPosition() + spawnPos * 0.1,0.15,action_data.m_Player);
		
		int idx;
		return action_data.m_ContextData.SpawnAndSetupCatch(idx,spawnPos);
	}
}
