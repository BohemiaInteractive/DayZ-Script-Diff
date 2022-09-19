enum EStaminaMultiplierTypes
{
	MASK = 1,
	FATIGUE,
	EPINEPHRINE,
	DROWNING,
}


/**@class	Stamina Consumer
 * @brief	Holds information about Stamina Consumer
 *
 * @param[in]	threshold	value needed to allow consume stamina
 * @param[in]	state		keeps state of the consumer non-depleted/depleted
 */
class StaminaConsumer
{
	protected float m_ActivationThreshold;
	protected float m_DrainThreshold
	protected bool m_State;
	
	void StaminaConsumer(float threshold, float threshold2, bool state)
	{
		m_ActivationThreshold = threshold; //can be activated if above this threshold
		m_DrainThreshold = threshold2; //can continually drain until it reaches this threshold
		m_State = state;
	}
	
	bool GetState() { return m_State; }
	void SetState(bool state) { m_State = state; }
	
	float GetActivationThreshold() { return m_ActivationThreshold; }
	void SetActivationThreshold(float threshold) { m_ActivationThreshold = threshold; }
	
	float GetDrainThreshold() { return m_DrainThreshold; }
	void SetDrainThreshold(float threshold) { m_DrainThreshold = threshold; }	
}

class StaminaConsumers
{
	protected ref map<EStaminaConsumers, ref StaminaConsumer> m_StaminaConsumers;

	void StaminaConsumers()
	{
		m_StaminaConsumers = new map<EStaminaConsumers, ref StaminaConsumer>;
	}

	void RegisterConsumer(EStaminaConsumers consumer, float threshold, float depletion_threshold = -1)
	{
		if (depletion_threshold == -1)
		{
			depletion_threshold = threshold;
		}
		
		if ( !m_StaminaConsumers.Contains(consumer) )
		{
			//! init of StaminaConsumer - threshold, state
			StaminaConsumer sc = new StaminaConsumer(threshold, depletion_threshold, true);
			m_StaminaConsumers.Set(consumer, sc);
		}
	}

	bool HasEnoughStaminaFor(EStaminaConsumers consumer, float curStamina, bool isDepleted, float cap)
	{
		if ( m_StaminaConsumers && m_StaminaConsumers.Contains(consumer) )
		{
			StaminaConsumer sc = m_StaminaConsumers.Get(consumer);
			
			if ( consumer != EStaminaConsumers.SPRINT )
			{
				if ( (isDepleted || (curStamina < sc.GetDrainThreshold()/* && curStamina < cap*/)) )
				{
					sc.SetState(false);
					return false;
				}
			}
			else
			{
				if( !isDepleted )
				{
					if ( sc.GetState() ) 
					{
						sc.SetState(true);
						return true;
					}
				}
				else
				{
					sc.SetState(false);
					return false;
				}
			}

			if ( curStamina > sc.GetDrainThreshold() || curStamina == cap ) //Sometimes player can't go up to drain threshold
			{
				sc.SetState(true);
				return true;
			}
		}

		return false;
	}
	
	bool HasEnoughStaminaToStart(EStaminaConsumers consumer, float curStamina, bool isDepleted, float cap)
	{
		if ( m_StaminaConsumers && m_StaminaConsumers.Contains(consumer) )
		{
			StaminaConsumer sc = m_StaminaConsumers.Get(consumer);
			
			if( (isDepleted || (curStamina < sc.GetActivationThreshold() && curStamina < cap)) )
			{
				sc.SetState(false);
				return false;
			}
			else
			{
				sc.SetState(true);
				return true;
			}
		}
		
		return false;
	}
}


/**@class	Stamina Modifier
 * @brief	Holds information about Stamina Modifier
 *
 * @param[in]	type	 	calculation method
 * @param[in]	minValue	min value substracted from stamina (if type > 0)
 * @param[in]	maxValue	max value substracted from stamina
 * @param[in]	cooldown	how many seconds will not be the stamina replenishing after depleting
 */
class StaminaModifier
{
	bool m_InUse = false;
	int m_Type;
	float m_MinValue, m_MaxValue, m_Cooldown, m_StartTime, m_Duration, m_ProgressTime, m_Tick;

	void StaminaModifier(int type, float min, float max, float cooldown, float startTime = 0, float duration = 0)
	{
		m_Type = type;
		m_MinValue = min;
		m_MaxValue = max;
		m_Cooldown = cooldown;
		m_StartTime = startTime;
		m_Duration = duration;
		m_Tick = 1;
	}
	
	int GetType() { return m_Type; }
	
	float GetMinValue() { return m_MinValue; }
	void SetMinValue(float val) { m_MinValue = val; } 
	
	float GetMaxValue() { return m_MaxValue; }
	void SetMaxValue(float val) { m_MaxValue = val; }

	float GetCooldown() { return m_Cooldown; }
	void SetCooldown(float val) { m_Cooldown = val; }
	
	float GetStartTime() { return m_StartTime; }
	void SetStartTime(float val) { m_StartTime = val; }
	
	float GetDuration() { return m_Duration; }
	float GetDurationAdjusted() { return m_Duration / m_Tick; }
	
	bool IsInUse() { return m_InUse; }
	void SetInUse(bool val) { m_InUse = val; }
	
	float GetRunTime() { return m_ProgressTime; }
	void AddRunTime(float val) { m_ProgressTime += val; }
	void SetRunTimeTick(float val) { m_Tick = val; }
	void ResetRunTime() { m_ProgressTime = 0 }
}

class StaminaModifiers
{
	const int FIXED 		= 0;
	const int RANDOMIZED 	= 1;
	const int LINEAR 		= 2; //Useful ONLY for regular, over-time stamina drain
	const int EXPONENTIAL 	= 3; //Useful ONLY for regular, over-time stamina drain

	protected ref map<EStaminaModifiers, ref StaminaModifier> m_StaminaModifiers;

	void StaminaModifiers()
	{
		m_StaminaModifiers = new map<EStaminaModifiers, ref StaminaModifier>;
	}

	//! register single value modifier - depletes stamina for that value
	void RegisterFixed(EStaminaModifiers modifier, float value, float cooldown = GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION)
	{	
		if ( !m_StaminaModifiers.Contains(modifier) )
		{
			//! init of StaminaModifier - type and min, max values (min is not relevant for that type)
			StaminaModifier sm = new StaminaModifier(FIXED, -1, value, cooldown);
			m_StaminaModifiers.Set(modifier, sm);
		}
	}
	
	//! register randomized modifier - stamina will be depleted by value between min and max value;
	void RegisterRandomized(EStaminaModifiers modifier, float minValue, float maxValue, float cooldown = GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION)
	{
		if ( !m_StaminaModifiers.Contains(modifier) )
		{
			//! init of StaminaModifier - type, min, max values
			StaminaModifier sm = new StaminaModifier(RANDOMIZED, minValue, maxValue, cooldown);
			m_StaminaModifiers.Set(modifier, sm);
		}
	}
	
	//! register lerped modifier - depletes stamina for startValue, and, after a startTime, lerps to endValue over duration
	void RegisterLinear(EStaminaModifiers modifier, float startValue, float endValue, float startTime, float duration, float cooldown = GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION)
	{
		StaminaModifier sm = new StaminaModifier(LINEAR, startValue, endValue, cooldown, startTime, duration);
		m_StaminaModifiers.Set(modifier, sm);
	}
	
	//! register exponential modifier - depletes stamina for startValue, and, after a startTime, lerps from 0 to exponent over duration
	void RegisterExponential(EStaminaModifiers modifier, float startValue, float exponent, float startTime, float duration, float cooldown = GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION)
	{
		StaminaModifier sm = new StaminaModifier(EXPONENTIAL, startValue, exponent, cooldown, startTime, duration);
		m_StaminaModifiers.Set(modifier, sm);
	}
	
	StaminaModifier GetModifierData(EStaminaModifiers modifier)
	{
		return m_StaminaModifiers.Get(modifier);
	}
}


class StaminaHandler
{	
	protected float 						m_PlayerLoad;
	protected float 						m_StaminaDelta;
	protected float 						m_Stamina;
	protected float 						m_StaminaSynced; //guaranteed to be identical on server and client
	protected float 						m_StaminaCap;
	protected float							m_StaminaDepletion;
	protected float 						m_StaminaDepletionMultiplier; //controls depletion rate
	protected float 						m_StaminaRecoveryMultiplier; //controls recovery rate
	protected float 						m_Time;
	protected ref Param3<float,float,bool>	m_StaminaParams; 
	protected ref HumanMovementState		m_State;
	protected PlayerBase					m_Player;

	protected bool 							m_Debug; 						//! DEPRECATED
	protected bool							m_StaminaDepleted;

	//protected ref Timer					m_CooldownTimer;
	protected ref map<int,ref Timer> 		m_TimerMap;
	ref map<EStaminaMultiplierTypes, float>	m_RegisteredDepletionModifiers;
	ref set<EStaminaMultiplierTypes>		m_ActiveDepletionModifiers;
	
	ref map<EStaminaMultiplierTypes, float>	m_RegisteredRecoveryModifiers;
	ref set<EStaminaMultiplierTypes>		m_ActiveRecoveryModifiers;
	
	protected bool							m_IsInCooldown;
	
	protected ref StaminaConsumers			m_StaminaConsumers;
	protected ref StaminaModifiers			m_StaminaModifiers;
	
	#ifdef DEVELOPER
	protected bool 							m_StaminaEnabled = true;
	#endif
	
	void StaminaHandler(PlayerBase player)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_StaminaParams = new Param3<float,float,bool>(0,0, false);		
		}
		m_State 						= new HumanMovementState();
		m_Player 						= player;
		m_Stamina 						= CfgGameplayHandler.GetStaminaMax(); 
		m_StaminaCap 					= CfgGameplayHandler.GetStaminaMax();
		m_StaminaDepletion 				= 0;
		m_StaminaDepletionMultiplier 	= 1;
		m_StaminaRecoveryMultiplier 	= 1;
		m_Time 							= 0;
		m_StaminaDepleted				= false;
		//m_CooldownTimer				= new Timer(CALL_CATEGORY_SYSTEM);
		m_IsInCooldown					= false;

		RegisterStaminaConsumers();
		RegisterStaminaModifiers();
		
		m_TimerMap = new map<int,ref Timer>;
		
		//----------------- depletion --------------------
		m_RegisteredDepletionModifiers = new map<EStaminaMultiplierTypes, float>;
		m_ActiveDepletionModifiers = new set<EStaminaMultiplierTypes>;

		//----------------- recovery --------------------
		m_RegisteredRecoveryModifiers = new map<EStaminaMultiplierTypes, float>;
		m_ActiveRecoveryModifiers = new set<EStaminaMultiplierTypes>;
		
		Init();
	}
	
	
	void Init()
	{
		//----------------- depletion --------------------
		m_RegisteredDepletionModifiers.Insert(EStaminaMultiplierTypes.MASK, MaskMdfr.STAMINA_DEPLETION_MODIFIER);
		m_RegisteredDepletionModifiers.Insert(EStaminaMultiplierTypes.FATIGUE, FatigueMdfr.STAMINA_DEPLETION_MULTIPLIER);
		m_RegisteredDepletionModifiers.Insert(EStaminaMultiplierTypes.EPINEPHRINE, EpinephrineMdfr.STAMINA_DEPLETION_MULTIPLIER);
		
		//----------------- recovery --------------------
		m_RegisteredRecoveryModifiers.Insert(EStaminaMultiplierTypes.MASK, MaskMdfr.STAMINA_RECOVERY_MODIFIER);
		m_RegisteredRecoveryModifiers.Insert(EStaminaMultiplierTypes.FATIGUE, FatigueMdfr.STAMINA_RECOVERY_MULTIPLIER);
		m_RegisteredRecoveryModifiers.Insert(EStaminaMultiplierTypes.DROWNING, DrowningMdfr.STAMINA_RECOVERY_MULTIPLIER);
		
	}
	
	void ActivateDepletionModifier(EStaminaMultiplierTypes type)
	{
		if( m_RegisteredDepletionModifiers.Contains(type))
		{
			m_ActiveDepletionModifiers.Insert(type);
			RecalculateDepletionMultiplier();
		}
		else
		{
			Error("attempting to activate unregistered depletion modifier");
		}
	}	
	
	void DeactivateDepletionModifier(EStaminaMultiplierTypes type)
	{
		int index = m_ActiveDepletionModifiers.Find(type);
		if( index != -1 )
		{
			m_ActiveDepletionModifiers.Remove(index);
			RecalculateDepletionMultiplier();
		}
	}
	
	void RecalculateDepletionMultiplier()
	{
		float value = 1;
		
		foreach(int multiplier: m_ActiveDepletionModifiers)
		{
			value *= m_RegisteredDepletionModifiers.Get(multiplier);
		}
		if (value != m_StaminaDepletionMultiplier)
			SetDepletionMultiplier(value);
		//Print("m_StaminaDepletionMultiplier = " + m_StaminaDepletionMultiplier);
	}
	
	void ActivateRecoveryModifier(EStaminaMultiplierTypes type)
	{
		if( m_RegisteredRecoveryModifiers.Contains(type))
		{
			m_ActiveRecoveryModifiers.Insert(type);
			RecalculateRecoveryMultiplier();
		}
		else
		{
			Error("attempting to activate unregistered recovery modifier");
		}
	}

	
	void DeactivateRecoveryModifier(EStaminaMultiplierTypes type)
	{
		int index = m_ActiveRecoveryModifiers.Find(type);
		if( index != -1 )
		{
			m_ActiveRecoveryModifiers.Remove(index);
			RecalculateRecoveryMultiplier();
		}
	}
	
	void RecalculateRecoveryMultiplier()
	{
		float value = 1;
		
		foreach (int multiplier: m_ActiveRecoveryModifiers)
		{
			value *= m_RegisteredRecoveryModifiers.Get(multiplier);
		}

		if (value != m_StaminaRecoveryMultiplier)
		{
			SetRecoveryMultiplier(value);
		}
		//Print("m_StaminaRecoveryMultiplier = " + m_StaminaRecoveryMultiplier);
	}
	
	void Update(float deltaT, int pCurrentCommandID)
	{
		#ifdef DEVELOPER
		if (!m_StaminaEnabled || DiagMenu.GetBool(DiagMenuIDs.DM_CHEATS_STAMINA_DISABLE))
		{
			return;
		}
		#endif
		if (m_Player)
		{
			// Calculates actual max stamina based on player's load
			if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				//! gets stamina from PlayerStat
				m_Stamina = m_Player.GetStatStamina().Get();
				//! gets the actual players load
				m_PlayerLoad = m_Player.GetPlayerLoad();

				//! StaminaCap calculation starts when PlayerLoad exceeds STAMINA_WEIGHT_LIMIT_THRESHOLD
				if (m_PlayerLoad >= CfgGameplayHandler.GetStaminaWeightLimitThreshold())
				{
					m_StaminaCap =  Math.Max((CfgGameplayHandler.GetStaminaMax() - (((m_PlayerLoad - CfgGameplayHandler.GetStaminaWeightLimitThreshold())/GameConstants.STAMINA_KG_TO_GRAMS) * CfgGameplayHandler.GetStaminaKgToStaminaPercentPenalty())),CfgGameplayHandler.GetStaminaMinCap());
				}
				else
				{
					m_StaminaCap = CfgGameplayHandler.GetStaminaMax();
				}
			}
			
			// Calculates stamina gain/loss based on movement and load
			m_Player.GetMovementState(m_State);

			switch (pCurrentCommandID)
			{
			case DayZPlayerConstants.COMMANDID_MOVE:
				StaminaProcessor_Move(m_State);
				break;
			case DayZPlayerConstants.COMMANDID_LADDER:
				StaminaProcessor_Ladder(m_State);
				break;
			case DayZPlayerConstants.COMMANDID_SWIM:
				StaminaProcessor_Swimming(m_State);
				break;
			case DayZPlayerConstants.COMMANDID_FALL:	//! processed on event
			case DayZPlayerConstants.COMMANDID_MELEE2:  //! processed on event
			case DayZPlayerConstants.COMMANDID_CLIMB:	//! processed on event
				break;
			default:
				if (!m_IsInCooldown)
				{
					m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC;
				}
				break;
			}
			
			//Sets current stamina & stores + syncs data with client
			float temp = m_StaminaDelta * deltaT;
			if (temp < 0)
			{
				temp *= m_StaminaDepletionMultiplier;
			}
			else
			{
				temp *= m_StaminaRecoveryMultiplier;
			}
			
			m_Stamina = Math.Max(0, Math.Min((m_Stamina + temp), m_StaminaCap));
			m_Stamina = m_Stamina - m_StaminaDepletion;

			if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				m_Player.GetStatStamina().Set(m_Stamina);
				m_Time += deltaT;
				
				if (m_Time >= GameConstants.STAMINA_SYNC_RATE)
				{
					m_Time = 0;
					SetStamina(m_Stamina);
				}
			}

			#ifndef SERVER
			m_Player.SetStamina(m_Stamina, m_StaminaCap);
			#endif

			ApplyExhaustion();
			CheckStaminaState();

			m_StaminaDelta = 0;
			m_StaminaDepletion = 0; // resets depletion modifier
			
		}
	}
	
	//! deprecated use, StaminaHandler uses SyncJunctures now
	void OnRPC(float stamina, float stamina_cap, bool cooldown)
	{
	}
	
	//! called from PlayerBase - syncs stamina values on server with client AND sets the value to match on server and client both (m_StaminaSynced guarantees identical values)
	void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		switch ( pJunctureID )
		{
		case DayZPlayerSyncJunctures.SJ_STAMINA:
			float stamina;
			float stamina_cap;
			bool cooldown;
			
			if (!pCtx.Read(stamina) || !pCtx.Read(stamina_cap) || !pCtx.Read(cooldown))
			{
				return;
			}
			
			m_Stamina = stamina; //?
			m_StaminaSynced = stamina;
			
			if (m_Player.GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
			{
				return;
			}
			
			if ( stamina_cap != m_StaminaCap )
			{
				m_StaminaCap = stamina_cap;
			}
			
			m_IsInCooldown = cooldown;
			m_Player.SetStamina(m_Stamina, m_StaminaCap);
		break;
		
		case DayZPlayerSyncJunctures.SJ_STAMINA_MISC:
			ReadAdditionalStaminaInfo(pCtx);
		break;
		}
	}
	
	protected void StaminaProcessor_Move(HumanMovementState pHumanMovementState)
	{
		switch (pHumanMovementState.m_iMovement)
		{
		case DayZPlayerConstants.MOVEMENTIDX_SPRINT: //sprint
			if (pHumanMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT)
			{
				m_StaminaDelta = -GameConstants.STAMINA_DRAIN_STANDING_SPRINT_PER_SEC * CfgGameplayHandler.GetSprintStaminaModifierErc();
				SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
				break;
			}
			else if ( pHumanMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH)
			{
				m_StaminaDelta = -GameConstants.STAMINA_DRAIN_CROUCHED_SPRINT_PER_SEC * CfgGameplayHandler.GetSprintStaminaModifierCro();
				SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
				break;
			}
			m_StaminaDelta = GameConstants.STAMINA_GAIN_JOG_PER_SEC;
		break;
			
		case DayZPlayerConstants.MOVEMENTIDX_RUN: //jog
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_JOG_PER_SEC + CalcStaminaGainBonus());
			}
		break;
			
		case DayZPlayerConstants.MOVEMENTIDX_WALK: //walk
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_WALK_PER_SEC + CalcStaminaGainBonus());
			}
		break;
			
		case DayZPlayerConstants.MOVEMENTIDX_IDLE: //idle
			if (m_Player.IsRolling())
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_ROLL_PER_SEC;
				break;
			}
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_IDLE_PER_SEC + CalcStaminaGainBonus());
			}
		break;
			
		default:
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC;
			}
		break;
		}
	}
	
	protected void StaminaProcessor_Ladder(HumanMovementState pHumanMovementState)
	{
		switch (pHumanMovementState.m_iMovement)
		{
		case 2: //climb up (fast)
			m_StaminaDelta = -GameConstants.STAMINA_DRAIN_LADDER_FAST_PER_SEC;
			SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
			break;
		break;
			
		case 1: //climb up (slow)
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_LADDER_PER_SEC + CalcStaminaGainBonus());
			}
		break;
			
		default:
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC + CalcStaminaGainBonus();
			}
		break;
		}
	}

	protected void StaminaProcessor_Swimming(HumanMovementState pHumanMovementState)
	{
		switch (pHumanMovementState.m_iMovement)
		{
		case 3: //swim fast
			m_StaminaDelta = -GameConstants.STAMINA_DRAIN_SWIM_FAST_PER_SEC;
			SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION);
			break;
		break;
			
		case 2: //swim slow
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = (GameConstants.STAMINA_GAIN_SWIM_PER_SEC + CalcStaminaGainBonus());
			}
		break;
			
		default:
			if (!m_IsInCooldown)
			{
				m_StaminaDelta = GameConstants.STAMINA_GAIN_IDLE_PER_SEC + CalcStaminaGainBonus();
			}
		break;
		}
	}
	
	//! stamina sync - server part
	protected void SyncStamina(float stamina, float stamina_cap, bool cooldown)
	{
		m_Player.GetStatStamina().Set(m_Stamina);
		ScriptJunctureData pCtx = new ScriptJunctureData;
		pCtx.Write(m_Stamina);
		pCtx.Write(m_StaminaCap);
		pCtx.Write(m_IsInCooldown);
		m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_STAMINA,pCtx);
	}
	
	//! Method to sync more info for stamina manager. Template parameter means it is very extendable for further use
	protected void SyncAdditionalStaminaInfo(Param par)
	{
		Param2<float,float> p = Param2<float,float>.Cast(par);
		
		ScriptJunctureData pCtx = new ScriptJunctureData;
		pCtx.Write(p.param1);
		pCtx.Write(p.param2);
		m_Player.SendSyncJuncture(DayZPlayerSyncJunctures.SJ_STAMINA_MISC,pCtx);
	}
	
	//! Order of read parameters must match the order of writing above
	protected void ReadAdditionalStaminaInfo(ParamsReadContext pCtx)
	{
		float mult_dep;
		float mult_rec;
		if (!pCtx.Read(mult_dep) || !pCtx.Read(mult_rec))
		{
			return;
		}
		
		m_StaminaDepletionMultiplier = mult_dep;
		m_StaminaRecoveryMultiplier = mult_rec;
	}
	
	protected void RegisterStaminaConsumers()
	{
		//! stamina consumers registration
		m_StaminaConsumers = new StaminaConsumers;
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.HOLD_BREATH, GameConstants.STAMINA_HOLD_BREATH_THRESHOLD_ACTIVATE,GameConstants.STAMINA_HOLD_BREATH_THRESHOLD_DRAIN);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.SPRINT, CfgGameplayHandler.GetStaminaMinCap() + 15);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.JUMP, GameConstants.STAMINA_JUMP_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.VAULT, GameConstants.STAMINA_VAULT_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.CLIMB, GameConstants.STAMINA_CLIMB_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.MELEE_HEAVY, GameConstants.STAMINA_MELEE_HEAVY_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.MELEE_EVADE, GameConstants.STAMINA_MELEE_EVADE_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.ROLL, GameConstants.STAMINA_ROLL_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.DROWN,0);
	}

	protected void RegisterStaminaModifiers()
	{
		//! stamina modifiers registration
		m_StaminaModifiers = new StaminaModifiers;
		m_StaminaModifiers.RegisterExponential(EStaminaModifiers.HOLD_BREATH, GameConstants.STAMINA_DRAIN_HOLD_BREATH_START, GameConstants.STAMINA_DRAIN_HOLD_BREATH_EXPONENT,0,GameConstants.STAMINA_DRAIN_HOLD_BREATH_DURATION);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.DROWN, 10);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.JUMP, GameConstants.STAMINA_DRAIN_JUMP);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.VAULT, GameConstants.STAMINA_DRAIN_VAULT);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.CLIMB, GameConstants.STAMINA_DRAIN_CLIMB);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.MELEE_LIGHT, GameConstants.STAMINA_DRAIN_MELEE_LIGHT);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.MELEE_HEAVY, GameConstants.STAMINA_DRAIN_MELEE_HEAVY);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.OVERALL_DRAIN, CfgGameplayHandler.GetStaminaMax(), 5.0);
		m_StaminaModifiers.RegisterRandomized(EStaminaModifiers.MELEE_EVADE, 3, GameConstants.STAMINA_DRAIN_MELEE_EVADE);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.ROLL, GameConstants.STAMINA_DRAIN_ROLL);
	}

	//! Calulates stamina regain bonus coef based on current stamina cap and level
	protected float CalcStaminaGainBonus()
	{
		if (m_StaminaDepletion > 0)
			return 0;

		if (m_Stamina > 25)
			return Math.Min((m_Stamina/10),GameConstants.STAMINA_GAIN_BONUS_CAP); // exp version
		else
			return GameConstants.STAMINA_GAIN_BONUS_CAP; // linear version
	}

	protected void ApplyExhaustion()
	{
		//! sets exhaustion look of player based on stamina level
		HumanCommandAdditives ad = m_Player.GetCommandModifier_Additives();
		
		float exhaustion_value = 1;
		if (m_StaminaCap != 0)
		{
			exhaustion_value = 1 - ((m_Stamina / (m_StaminaCap * 0.01)) * 0.01);
		}
		
		exhaustion_value = Math.Min(1, exhaustion_value);
		if (ad)
		{
			// do not apply exhaustion on local client if player is in ADS/Optics (camera shakes)
			if (m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT && (m_Player.IsInOptics() || m_Player.IsInIronsights()))
			{
				ad.SetExhaustion(0, true);
			}
			else
			{
				ad.SetExhaustion(exhaustion_value, true);
			}
		}
	}
	
	//! check if the stamina is completely depleted
	protected void CheckStaminaState()
	{
		if (m_Stamina <= 0)
		{
			m_StaminaDepleted = true;
			//! in case of complete depletion - start a cooldown timer before the regeneration cycle start
			if (!m_IsInCooldown)
			{
				// set this only once
				SetCooldown(GameConstants.STAMINA_REGEN_COOLDOWN_EXHAUSTION); 
			}
		}
		else
		{
			m_StaminaDepleted = false;
		}
	}

	//! set cooldown timer between each consume of stamina	
	protected void SetCooldown(float time, int modifier = -1)
	{
		//StaminaModifier sm = m_StaminaModifiers.GetModifierData(modifier);
		if ( m_StaminaDepleted || m_Stamina <= 0.0 )
		{
			ResetCooldown(modifier);
			return;
		}

		m_IsInCooldown = true;
		/*if( m_CooldownTimer.IsRunning() )
			m_CooldownTimer.Stop();
		m_CooldownTimer.Run(time, this, "ResetCooldown",  new Param1<int>( modifier ));*/
		
		Timer timer;
		if (m_TimerMap.Find(modifier, timer) && timer.IsRunning())
		{
			timer.Stop();
		}
		else
		{
			timer = new ref Timer;
			m_TimerMap.Set(modifier,timer);
		}
		timer.Run(time, this, "ResetCooldown",  new Param1<int>( modifier ));
		//Print(m_TimerMap.Count());
	}
	
	protected void ResetCooldown(int modifier = -1)
	{
		StaminaModifier sm = m_StaminaModifiers.GetModifierData(modifier);
		if (sm)
		{
			//Print(modifier);
			//Error("Error: No StaminaModifier found! | StaminaHandler | ResetCooldown");
			sm.SetStartTime(-1);
			sm.ResetRunTime();
			sm.SetInUse(false);
		}
		m_IsInCooldown = false;
	}
	
	Timer GetCooldownTimer(int modifier)
	{
		
	}
	
	// ---------------------------------------------------
	bool HasEnoughStaminaFor(EStaminaConsumers consumer)
	{
		return m_StaminaConsumers.HasEnoughStaminaFor(consumer, m_Stamina, m_StaminaDepleted, m_StaminaCap);
	}
	
	bool HasEnoughStaminaToStart(EStaminaConsumers consumer)
	{
		return m_StaminaConsumers.HasEnoughStaminaToStart(consumer, m_Stamina, m_StaminaDepleted, m_StaminaCap);
	}

	void SetStamina(float stamina_value)
	{
		m_Stamina = Math.Clamp(stamina_value, 0, CfgGameplayHandler.GetStaminaMax());
		SyncStamina(m_Stamina, m_StaminaCap, m_IsInCooldown);
	}
	
	float GetStamina()
	{
		return m_Stamina;
	}

	float GetStaminaNormalized()
	{
		return m_Stamina / GetStaminaMax();
	}

	float GetSyncedStamina()
	{
		return m_StaminaSynced;
	}
	
	float GetSyncedStaminaNormalized()
	{
		return GetSyncedStamina() / GetStaminaMax();
	}
	
	float GetStaminaCap()
	{
		return m_StaminaCap;
	}
	
	float GetStaminaMax()
	{
		return CfgGameplayHandler.GetStaminaMax();
	}
	
	//obsolete, use ActivateDepletionModifier/DeactivateDepletionModifier instead
	void SetDepletionMultiplier(float val)
	{
		if (m_StaminaDepletionMultiplier < 0)
			m_StaminaDepletionMultiplier = 0;
		m_StaminaDepletionMultiplier = val;
		SyncAdditionalStaminaInfo(new Param2<float,float>(m_StaminaDepletionMultiplier,m_StaminaRecoveryMultiplier));
	}
	//obsolete, use ActivateRecoveryModifier/DeactivateRecoveryModifier instead
	void SetRecoveryMultiplier(float val)
	{
		if (m_StaminaRecoveryMultiplier < 0)
			m_StaminaRecoveryMultiplier = 0;
		m_StaminaRecoveryMultiplier = val;
		SyncAdditionalStaminaInfo(new Param2<float,float>(m_StaminaDepletionMultiplier,m_StaminaRecoveryMultiplier));
	}
	
	float GetDepletionMultiplier()
	{
		return m_StaminaDepletionMultiplier;
	}
	
	float GetRecoveryMultiplier()
	{
		return m_StaminaRecoveryMultiplier;
	}
	
	void DepleteStamina(EStaminaModifiers modifier, float dT = -1)
	{
		#ifdef DEVELOPER
		if (!m_StaminaEnabled || DiagMenu.GetBool(DiagMenuIDs.DM_CHEATS_STAMINA_DISABLE))
		{
			return;
		}
		#endif
		float val = 0.0;
		float current_time = m_Player.GetSimulationTimeStamp();
		float time;
		StaminaModifier sm = m_StaminaModifiers.GetModifierData(modifier);

		// select by modifier type and drain stamina
		switch (sm.GetType())
		{
			case m_StaminaModifiers.FIXED:
				if (dT == -1)
				{
					dT = 1;
				}
				m_StaminaDepletion = m_StaminaDepletion + sm.GetMaxValue() * dT;
			break;
			
			case m_StaminaModifiers.RANDOMIZED:
				val = Math.RandomFloat(sm.GetMinValue(), sm.GetMaxValue());
				m_StaminaDepletion = m_StaminaDepletion + val;
			break;
			
			case m_StaminaModifiers.LINEAR:
				if (!sm.IsInUse())
				{
					sm.SetStartTime(current_time + ( (PlayerSwayConstants.SWAY_TIME_IN + PlayerSwayConstants.SWAY_TIME_STABLE) / dT ) );
					sm.SetRunTimeTick(dT);
					sm.SetInUse(true);
				}
				time = Math.Clamp( ((current_time - sm.GetStartTime()) / sm.GetDurationAdjusted()), 0, 1 );
				val = Math.Lerp(sm.GetMinValue(), sm.GetMaxValue(), time);
				m_StaminaDepletion = m_StaminaDepletion + val;
			
			break;
			
			case m_StaminaModifiers.EXPONENTIAL:
				if (!sm.IsInUse())
				{
					sm.SetStartTime(current_time + ( (PlayerSwayConstants.SWAY_TIME_IN + PlayerSwayConstants.SWAY_TIME_STABLE) / dT ) );
					sm.SetRunTimeTick(dT);
					sm.SetInUse(true);
				}
				time = Math.Clamp( ((current_time - sm.GetStartTime()) / sm.GetDurationAdjusted()), 0, 1 );
				float exp;
				if (sm.GetMinValue() < 1)
				{
					exp = 1 - Math.Lerp(0, sm.GetMaxValue(), time);
				}
				else
				{
					exp = Math.Lerp(0, sm.GetMaxValue(), time);
				}
				val = Math.Pow(sm.GetMinValue(),exp);
				m_StaminaDepletion = m_StaminaDepletion + val;
			
			break;
		}

		//! run cooldown right after depletion
		SetCooldown(sm.GetCooldown(),modifier);
		m_StaminaDepletion = Math.Clamp(m_StaminaDepletion, 0, CfgGameplayHandler.GetStaminaMax());

		m_StaminaDepletion = m_StaminaDepletion * m_StaminaDepletionMultiplier;
	}
	
	#ifdef DEVELOPER
	void SetStaminaEnabled(bool value)
	{
		m_StaminaEnabled = value;
	}
	#endif
};