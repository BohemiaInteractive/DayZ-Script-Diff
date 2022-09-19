class ZombieBase extends DayZInfected
{
	const float TARGET_CONE_ANGLE_CHASE = 20;
	const float TARGET_CONE_ANGLE_FIGHT = 30;
	const float ORIENTATION_SYNC_THRESHOLD = 30; //threshold for local heading/orientation sync
	
	const float	SHOCK_TO_STUN_MULTIPLIER = 2.82;

	//! server / singleplayer properties
	protected int m_StanceVariation = 0;
	protected int m_LastMindState = -1;
	protected float m_LastMovementSpeed = -1;
	
	protected bool m_KnuckleLand = false;
	protected float m_KnuckleOutTimer = 0;

	protected int m_MindState = -1;
	protected int m_OrientationLocal = -1; //local 'companion' value for sync checking
	protected int m_OrientationSynced = -1;
	protected float m_OrientationTimer;
	protected float m_MovementSpeed = -1;
	
	protected vector m_DefaultHitPosition;
	
	protected AbstractWave m_LastSoundVoiceAW;
	protected ref InfectedSoundEventHandler	m_InfectedSoundEventHandler;

	protected ref array<Object> m_AllTargetObjects;
	protected ref array<typename>m_TargetableObjects;
	
	//static ref map<int,ref array<string>> 	m_FinisherSelectionMap; //! which selections in the FireGeometry trigger which finisher on hit (when applicable)
	
	protected bool m_IsCrawling; //'DayZInfectedCommandCrawl' is transition to crawl only, 'DayZInfectedCommandMove' used after that, hence this VARIABLE_WET
	
	protected bool m_FinisherInProgress = false; //is this object being backstabbed?



	//-------------------------------------------------------------
	void ZombieBase()
	{
		Init();
	}
	
	void Init()
	{
		SetEventMask(EntityEvent.INIT);
		
		m_IsCrawling = false;
		
		RegisterNetSyncVariableInt("m_MindState", -1, 4);
		RegisterNetSyncVariableInt("m_OrientationSynced", 0, 359);
		RegisterNetSyncVariableFloat("m_MovementSpeed", -1, 3);
		RegisterNetSyncVariableBool("m_IsCrawling");
		
		//! sets default hit position and cache it here (mainly for impact particles)
		m_DefaultHitPosition = SetDefaultHitPosition(GetDayZInfectedType().GetDefaultHitPositionComponent());

		//! client only
		if ( !GetGame().IsDedicatedServer() )
		{
			m_LastSoundVoiceAW 			= null;
			m_InfectedSoundEventHandler = new InfectedSoundEventHandler(this);
		}

		m_AllTargetObjects		= new array<Object>;
		m_TargetableObjects 	= new array<typename>;
		m_TargetableObjects.Insert(PlayerBase);
		m_TargetableObjects.Insert(AnimalBase);
		
		m_OrientationTimer = 0;
	}
	
	//! synced variable(s) handler
	override void OnVariablesSynchronized()
	{
		DebugSound("[Infected @ " + this + "][OnVariablesSynchronized]");
		HandleSoundEvents();
		
		if ( m_OrientationLocal != m_OrientationSynced )
		{
			m_OrientationLocal = m_OrientationSynced;
		}
	}

	//-------------------------------------------------------------
	override void EOnInit(IEntity other, int extra)
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			m_StanceVariation = Math.RandomInt(0, 4);
	
			DayZInfectedCommandMove moveCommand = GetCommand_Move();
			moveCommand.SetStanceVariation(m_StanceVariation);
		}
	}
	
	override bool IsZombie()
	{
		return true;
	}
	
	override bool IsDanger()
	{
		return true;
	}
	
	override bool IsZombieMilitary()
	{
		/*if ( IsKindOf( "ZmbM_SoldierNormal_Base" ) )
		{
			return true;
		}*/
		
		return false;
	}
	
	bool IsMale()
	{
		return true;
	}
	
	override bool CanBeBackstabbed()
	{
		return true;
	}
	
	//-------------------------------------------------------------
	override AnimBootsType GetBootsType()
	{
		return AnimBootsType.Boots;
	}
	
	override bool CanBeSkinned()
	{
		return false;
	}
	//-------------------------------------------------------------
	override bool IsHealthVisible()
	{
		return false;
	}
	//-------------------------------------------------------------
	override bool IsRefresherSignalingViable()
	{
		return false;
	}

	//! returns hit component for attacking AI
	override string GetHitComponentForAI()
	{
		return GetDayZInfectedType().GetHitComponentForAI();
	}

	//! returns default hit component (fallback)
	override string GetDefaultHitComponent()
	{
		return GetDayZInfectedType().GetDefaultHitComponent();
	}

	override vector GetDefaultHitPosition()
	{
		return m_DefaultHitPosition;
	}
	
	protected vector SetDefaultHitPosition(string pSelection)
	{
		return GetSelectionPositionMS(pSelection);
	}

	//! returns suitable hit components for finisher attacks; DEPRECATED
	override array<string> GetSuitableFinisherHitComponents()
	{
		return GetDayZInfectedType().GetSuitableFinisherHitComponents();
	}
	
	int GetMindStateSynced()
	{
		return m_MindState;
	}
	
	//! returns rounded zombie yaw for sync purposes
	int GetOrientationSynced()
	{
		return m_OrientationSynced;
	}

	//-------------------------------------------------------------
	//!
	//! CommandHandler
	//! 

	void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		//! for mods
		if ( ModCommandHandlerBefore(pDt, pCurrentCommandID, pCurrentCommandFinished) )
		{
			return;
		}

		//! handle death
		if ( pCurrentCommandID != DayZInfectedConstants.COMMANDID_DEATH )		
		{
			if ( HandleDeath(pCurrentCommandID) )
				return;
		}
		else if (!pCurrentCommandFinished)
		{
			return;
		}
		

		//! movement handler (just for sync now)
		HandleMove(pCurrentCommandID);
		HandleOrientation(pDt,pCurrentCommandID);
		
		//! handle finished commands
		if (pCurrentCommandFinished)
		{			
			//! default behaviour after finish is to start move
			DayZInfectedCommandMove moveCommand = StartCommand_Move();
			moveCommand.SetStanceVariation(m_StanceVariation);
			
			return;
		}
		
		//! for mods
		if ( ModCommandHandlerInside(pDt, pCurrentCommandID, pCurrentCommandFinished) )
		{
			return;
		}
		
		//! crawl transition
		if ( HandleCrawlTransition(pCurrentCommandID) )
		{
			return;
		}
		
		//! damage hits
		if ( HandleDamageHit(pCurrentCommandID) )
		{
			return;
		}
		
		DayZInfectedInputController inputController = GetInputController();
		if ( inputController )
		{
			if ( HandleVault(pCurrentCommandID, inputController, pDt) )
			{
				return;
			}
			
			if ( HandleMindStateChange(pCurrentCommandID, inputController, pDt) )
			{
				return;
			}
			
			if ( FightLogic(pCurrentCommandID, inputController, pDt) )
			{
				return;
			}
		}
		
		//!
		if ( ModCommandHandlerAfter(pDt, pCurrentCommandID, pCurrentCommandFinished) )
		{
			return;
		}
	}

	//-------------------------------------------------------------
	//!
	//! ModOverrides
	//! 
	// these functions are for modded overide in script command mods 

	bool	ModCommandHandlerBefore(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		return false;
	}

	bool	ModCommandHandlerInside(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		return false;
	}
	
	bool	ModCommandHandlerAfter(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		return false;
	}

	//-------------------------------------------------------------
	//!
	//! CommandHandlerDebug
	//! 

	void  CommandHandlerDebug(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if ( GetPluginManager() )
		{
			PluginDayZInfectedDebug infectedDebug = PluginDayZInfectedDebug.Cast(GetPluginManager().GetPluginByType(PluginDayZInfectedDebug));
			if ( infectedDebug )
				infectedDebug.CommandHandler(this);
		}	
	}
	//-------------------------------------------------------------
	//!
	//! HandleMove
	//!
	
	void HandleMove(int pCurrentCommandID)
	{
		DayZInfectedInputController ic = GetInputController();
		m_MovementSpeed = ic.GetMovementSpeed();
		if (Math.AbsFloat(m_LastMovementSpeed - m_MovementSpeed) >= 0.9 && m_LastMovementSpeed != m_MovementSpeed)
		{
			SetSynchDirty();
		}
		
		m_LastMovementSpeed = m_MovementSpeed;
	}
	
	//-------------------------------------------------------------
	//!
	//! HandleOrientation
	//!
	
	void HandleOrientation(float pDt, int pCurrentCommandID)
	{
		//DayZInfectedInputController ic = GetInputController();
		//ic.GetLookAtDirectionWS(); //TODO: implement actual look direction check??
		
		m_OrientationTimer += pDt;
		
		int yaw;
		yaw = Math.Round(GetOrientation()[0]);
		yaw = Math.NormalizeAngle(yaw);
		
		//atan2(sin(x-y), cos(x-y))
		float rad_src = m_OrientationSynced * Math.DEG2RAD;
		float rad_tgt = yaw * Math.DEG2RAD;
		
		float diff = Math.Atan2(Math.Sin(rad_tgt - rad_src),Math.Cos(rad_src - rad_tgt));
		diff *= Math.RAD2DEG;
		diff = Math.Round(diff);
		
		if ( m_OrientationTimer >= 2.0 || m_OrientationSynced == -1 || Math.AbsInt(diff) > ORIENTATION_SYNC_THRESHOLD )
		{
			m_OrientationTimer = 0.0;
			
			if ( m_OrientationSynced == -1 || Math.AbsInt(diff) > 5 )
			{
				//Print("DbgSyncOrientation | HandleMove | original: " + m_OrientationSynced + " | target: " + yaw + " | diff: " + diff);
				SetSynchDirty();
				m_OrientationSynced = yaw;
			}
		}
	}
	
	//-------------------------------------------------------------
	//!
	//! HandleDeath
	//! 

	float m_DamageHitDirection = 0;
	int   m_DeathType = 0;

	bool HandleDeath(int pCurrentCommandID)
	{
		if ( !IsAlive() || m_FinisherInProgress )
		{
			StartCommand_Death(m_DeathType, m_DamageHitDirection);
			m_MovementSpeed = -1;
			m_MindState = -1;
			SetSynchDirty();
			return true;
		}

		return false;
	}	
	
	bool EvaluateDeathAnimationEx(EntityAI pSource, ZombieHitData data, out int pAnimType, out float pAnimHitDir)
	{
		bool ret = EvaluateDeathAnimation(pSource,data.m_DamageZone,data.m_AmmoType,pAnimType,pAnimHitDir);
				
		return ret;
	}
	
	bool EvaluateDeathAnimation(EntityAI pSource, string pComponent, string pAmmoType, out int pAnimType, out float pAnimHitDir)
	{
		//! 
		bool doPhxImpulse = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " doPhxImpulse") > 0;
		
		//! anim type
		pAnimType = doPhxImpulse;
		
		//! direction
		pAnimHitDir = ComputeHitDirectionAngle(pSource);
		
		//! add some impulse if needed
		if ( doPhxImpulse )
		{
			vector impulse = 80 * m_TransportHitVelocity;
			impulse[1] = 80 * 1.5;
			//Print("Impulse: " + impulse.ToString());
			
			dBodyApplyImpulse(this, impulse);
		}
				
		return true;
	}
	
	//-------------------------------------------------------------
	//!
	//! HandleVault
	//! 
	
	int m_ActiveVaultType = -1;
	
	int GetVaultType(float height)
	{
		if ( height <= 0.6 )
			return 0;
		else if ( height <= 1.1 )
			return 1;
		else if ( height <= 1.6 )
			return 2;
		else 
			return 3;
	}
	
	bool HandleVault(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if ( pCurrentCommandID == DayZInfectedConstants.COMMANDID_VAULT )
		{
			DayZInfectedCommandVault vaultCmd = GetCommand_Vault();
			if ( vaultCmd && vaultCmd.WasLand() )
			{
				m_KnuckleOutTimer = 0;
				m_KnuckleLand = true;
			}
			if ( m_KnuckleLand )
			{
				m_KnuckleOutTimer += pDt;
				if ( m_KnuckleOutTimer > 2.0 )
					StartCommand_Vault(-1);
			}
			
			return true;
		}
		
		if ( pInputController.IsVault() )
		{
			float vaultHeight = pInputController.GetVaultHeight();
			int vaultType = GetVaultType(vaultHeight);
			m_KnuckleLand = false;
			StartCommand_Vault(vaultType);
			return true; 
		}
		
		return false;
	}
	
	//-------------------------------------------------------------
	//!
	//! Mind state change
	//! 

	bool HandleMindStateChange(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		DayZInfectedCommandMove moveCommand = GetCommand_Move();
		
		m_MindState = pInputController.GetMindState();
		if ( m_LastMindState != m_MindState )
		{
			switch ( m_MindState )
			{
			case DayZInfectedConstants.MINDSTATE_CALM:
				if ( moveCommand && !moveCommand.IsTurning() )
					moveCommand.SetIdleState(0);
				break;

			case DayZInfectedConstants.MINDSTATE_DISTURBED:
				if ( moveCommand && !moveCommand.IsTurning() )
					moveCommand.SetIdleState(1);
				break;
			
			case DayZInfectedConstants.MINDSTATE_CHASE:
				if ( moveCommand && !moveCommand.IsTurning() && (m_LastMindState < DayZInfectedConstants.MINDSTATE_CHASE) )
					moveCommand.SetIdleState(2);
				break;
			}
			
			m_LastMindState = m_MindState;
			m_AttackCooldownTime = 0.0;
			SetSynchDirty();
		}
		return false;
	}

	//-------------------------------------------------------------
	//!
	//! Sound (client only)
	//!
	
	protected void HandleSoundEvents()
	{
		//! no sound handler - bail out
		if ( !m_InfectedSoundEventHandler )
		{
			return;
		}

		//! infected is dead
		if ( !IsAlive() )
		{
			//! stop all sounds
			m_InfectedSoundEventHandler.Stop();
			return;
		}

		switch ( m_MindState )
		{
		case DayZInfectedConstants.MINDSTATE_CALM:
			m_InfectedSoundEventHandler.PlayRequest(EInfectedSoundEventID.MINDSTATE_CALM_MOVE);
			break;
		case DayZInfectedConstants.MINDSTATE_ALERTED:
			m_InfectedSoundEventHandler.PlayRequest(EInfectedSoundEventID.MINDSTATE_ALERTED_MOVE);
			break;
		case DayZInfectedConstants.MINDSTATE_DISTURBED:
			m_InfectedSoundEventHandler.PlayRequest(EInfectedSoundEventID.MINDSTATE_DISTURBED_IDLE);
			break
		case DayZInfectedConstants.MINDSTATE_CHASE:
			m_InfectedSoundEventHandler.PlayRequest(EInfectedSoundEventID.MINDSTATE_CHASE_MOVE);
			break;
		default:
			m_InfectedSoundEventHandler.Stop();
			break;
		}
		
		DebugSound("[Infected @ " + this + "][MindState]" + typename.EnumToString(DayZInfectedConstants, m_MindState));
		DebugSound("[Infected @ " + this + "][SoundEventID]" + typename.EnumToString(EInfectedSoundEventID, m_InfectedSoundEventHandler.GetCurrentStateEventID()));
	}

	AbstractWave ProcessVoiceFX(string pSoundSetName)
	{
		ref SoundParams			soundParams;
		ref SoundObjectBuilder	soundObjectBuilder;
		ref SoundObject			soundObject;
		if (!GetGame().IsDedicatedServer())
		{
			soundParams = new SoundParams( pSoundSetName );
			if ( !soundParams.IsValid() )
			{
				//SoundError("Invalid sound set.");
				return null;
			}
			
			soundObjectBuilder = new SoundObjectBuilder( soundParams );
			soundObject = soundObjectBuilder.BuildSoundObject();
			AttenuateSoundIfNecessary(soundObject);

			return PlaySound(soundObject, soundObjectBuilder);
		}

		return null;
	}

	override void OnSoundVoiceEvent(int event_id, string event_user_string)
	{
		//super.OnSoundVoiceEvent(event_id, event_user_string);
		AnimSoundVoiceEvent voice_event = GetCreatureAIType().GetSoundVoiceEvent(event_id);
		if (voice_event != null)
		{
			//! stop state sound when playing anim SoundVoice
			if (m_InfectedSoundEventHandler) // && m_InfectedSoundEventHandler.IsPlaying())
			{
				m_InfectedSoundEventHandler.Stop();
				DebugSound("[Infected @ " + this + "][SoundEvent] InfectedSoundEventHandler - stop all");
			}

			//! stop playing of old SoundVoice from anim (if any)
			if (m_LastSoundVoiceAW != null)
			{
				DebugSound("[Infected @ " + this + "][AnimVoiceEvent] Stopping LastAW");
				m_LastSoundVoiceAW.Stop();
			}
			
			//! play new SoundVoice from anim
			ProcessSoundVoiceEvent(voice_event, m_LastSoundVoiceAW);
			
			HandleSoundEvents();
		}
	}
	
	protected void ProcessSoundVoiceEvent(AnimSoundVoiceEvent sound_event, out AbstractWave aw)
	{
		if (!GetGame().IsDedicatedServer())
		{
			SoundObjectBuilder objectBuilder = sound_event.GetSoundBuilder();
			if (NULL != objectBuilder)
			{
				objectBuilder.UpdateEnvSoundControllers(GetPosition());
				SoundObject soundObject = objectBuilder.BuildSoundObject();
				AttenuateSoundIfNecessary(soundObject);
				aw = PlaySound(soundObject, objectBuilder);
			}
		}
		
		if (GetGame().IsServer())
		{
			if (sound_event.m_NoiseParams != NULL)
				GetGame().GetNoiseSystem().AddNoise(this, sound_event.m_NoiseParams);
		}
	}
	
	//-------------------------------------------------------------
	//!
	//! Combat
	//! 
	
	EntityAI m_ActualTarget = NULL;
	float m_AttackCooldownTime = 0;	
	DayZInfectedAttackType m_ActualAttackType = NULL;
	
	bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if ( pCurrentCommandID == DayZInfectedConstants.COMMANDID_MOVE )
		{
			// we attack only in chase & fight state
			int mindState = pInputController.GetMindState();
			if ( mindState == DayZInfectedConstants.MINDSTATE_CHASE )
			{
				return ChaseAttackLogic(pCurrentCommandID, pInputController, pDt);
			}
			else if ( mindState == DayZInfectedConstants.MINDSTATE_FIGHT )
			{
				return FightAttackLogic(pCurrentCommandID, pInputController, pDt);
			}
		}
		else if ( pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK )
		{
			DayZInfectedCommandAttack attackCommand = GetCommand_Attack();
			if ( attackCommand && attackCommand.WasHit() )
			{
				if ( m_ActualTarget != NULL )
				{
					if (m_ActualTarget.GetMeleeTargetType() == EMeleeTargetType.NONALIGNABLE )
					{
						return false;
					}
					bool playerInBlockStance = false;
					vector targetPos = m_ActualTarget.GetPosition();
					vector hitPosWS = targetPos;
					vector zombiePos = GetPosition();

					PlayerBase playerTarget = PlayerBase.Cast(m_ActualTarget);
					if ( playerTarget )
					{
						playerInBlockStance = playerTarget.GetMeleeFightLogic() && playerTarget.GetMeleeFightLogic().IsInBlock();
					}

					if ( vector.DistanceSq(targetPos, zombiePos) <= m_ActualAttackType.m_Distance * m_ActualAttackType.m_Distance )
					{
						//! player is in block stance and facing the infected
						if ( playerInBlockStance && (Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(targetPos, MiscGameplayFunctions.GetHeadingVector(playerTarget), zombiePos))) <= GameConstants.AI_MAX_BLOCKABLE_ANGLE )
						{
							//! infected is playing heavy attack - decrease the dmg to light
							if ( m_ActualAttackType.m_IsHeavy != 0 )
							{
								hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition()); //! override hit pos by pos defined in type
								DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), "MeleeZombie", hitPosWS);
							}
							else
							{
								//! infected is playing light attack - do not send damage, play animation instead
								if ( GetGame().IsServer() )
								{
									hitPosWS = m_ActualTarget.GetDefaultHitPosition(); //! override hit pos by pos defined in type
									m_ActualTarget.EEHitBy(null, 0, this, -1, m_ActualTarget.GetDefaultHitComponent(), "Dummy_Light", hitPosWS, 1.0);
								}
							}
						}
						else
						{
							hitPosWS = m_ActualTarget.ModelToWorld(m_ActualTarget.GetDefaultHitPosition()); //! override hit pos by pos defined in type
							DamageSystem.CloseCombatDamageName(this, m_ActualTarget, m_ActualTarget.GetHitComponentForAI(), m_ActualAttackType.m_AmmoType, hitPosWS);
						}
					}
				}
			}
			
			return true;
		}
				
		return false;
	}
	
	bool ChaseAttackLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		// always update target - it can be destroyed		
		m_ActualTarget = pInputController.GetTargetEntity();
		
		//! do not attack players in vehicle - hotfix
		PlayerBase pb = PlayerBase.Cast(m_ActualTarget);
		if ( pb && pb.GetCommand_Vehicle() )
		{
			return false;
		}

		if ( m_ActualTarget == NULL )
			return false;
	
		vector targetPos = m_ActualTarget.GetPosition();
		if ( !CanAttackToPosition(targetPos) )
			return false;
		
		float targetDist = vector.Distance(targetPos, this.GetPosition());
		int pitch = GetAttackPitch(m_ActualTarget);
		
		m_ActualAttackType = GetDayZInfectedType().ChooseAttack(DayZInfectedAttackGroupType.CHASE, targetDist, pitch);
		if ( m_ActualAttackType )
		{
			Object target = DayZPlayerUtils.GetMeleeTarget(this.GetPosition(), this.GetDirection(), TARGET_CONE_ANGLE_CHASE, m_ActualAttackType.m_Distance, -1.0, 2.0, this, m_TargetableObjects, m_AllTargetObjects);
			//! target is outside the targeting cone; skip attack
			if (m_ActualTarget != target)
			{
				m_AllTargetObjects.Clear();
				return false;
			}

			StartCommand_Attack(m_ActualTarget, m_ActualAttackType.m_Type, m_ActualAttackType.m_Subtype);
			m_AttackCooldownTime = m_ActualAttackType.m_Cooldown;
			return true;
		}
		
		return false;
	}
	
	bool FightAttackLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		// always update target - it can be destroyed		
		m_ActualTarget = pInputController.GetTargetEntity();
		
		//! do not attack players in vehicle - hotfix
		PlayerBase pb = PlayerBase.Cast(m_ActualTarget);
		if ( pb && pb.GetCommand_Vehicle() )
		{
			return false;
		}

		if ( m_AttackCooldownTime > 0 )
		{
			m_AttackCooldownTime -= pDt * GameConstants.AI_ATTACKSPEED;
			return false;
		}
					
		if ( m_ActualTarget == NULL )
			return false;

		vector targetPos = m_ActualTarget.GetPosition();
		float targetDist = vector.Distance(targetPos, this.GetPosition());
		int pitch = GetAttackPitch(m_ActualTarget);
		
		if ( !CanAttackToPosition(targetPos) )
			return false;

		m_ActualAttackType = GetDayZInfectedType().ChooseAttack(DayZInfectedAttackGroupType.FIGHT, targetDist, pitch);
		if ( m_ActualAttackType )
		{
			Object target = DayZPlayerUtils.GetMeleeTarget(this.GetPosition(), this.GetDirection(), TARGET_CONE_ANGLE_FIGHT, m_ActualAttackType.m_Distance, -1.0, 2.0, this, m_TargetableObjects, m_AllTargetObjects);
			//! target is outside the targeting cone; skip attack
			if (m_ActualTarget != target)
			{
				m_AllTargetObjects.Clear();
				return false;
			}

			StartCommand_Attack(m_ActualTarget, m_ActualAttackType.m_Type, m_ActualAttackType.m_Subtype);
			m_AttackCooldownTime = m_ActualAttackType.m_Cooldown;
			return true;
		}

		return false;
	}

	int GetAttackPitch(EntityAI target)
	{
		vector attackRefPos;

		attackRefPos = target.GetDefaultHitPosition();
		//! no default hit pos fallback
		if ( attackRefPos != vector.Zero )
		{
			attackRefPos = target.ModelToWorld(attackRefPos);
		}
		else
		{
			attackRefPos = target.GetPosition();
		}

		// Now we have only erect stance, we need to get head position later too
		float headPosY = GetPosition()[1];
		headPosY += 1.8;
		
		float diff = Math.AbsFloat(attackRefPos[1] - headPosY);
		
		if ( diff < 0.3 )
			return 0;
		
		if ( headPosY > attackRefPos[1] )
			return -1;
		else
			return 1;
	}
		
	//-------------------------------------------------------------
	//!
	//! Crawl transition
	//! 
	
	int m_CrawlTransition = -1;
	
	bool HandleCrawlTransition(int pCurrentCommandID)
	{
		if ( m_CrawlTransition != -1 )
		{
			StartCommand_Crawl(m_CrawlTransition);
			
			m_CrawlTransition = -1;
			m_IsCrawling = true;
			SetSynchDirty();
			return true;
		}

		return pCurrentCommandID == DayZInfectedConstants.COMMANDID_CRAWL;
	}
	
	bool EvaluateCrawlTransitionAnimation(EntityAI pSource, string pComponent, string pAmmoType, out int pAnimType)
	{
		pAnimType = -1;
		if ( pComponent == "LeftLeg" && GetHealth(pComponent, "Health") == 0 )
			pAnimType = 0;
		else if ( pComponent == "RightLeg" && GetHealth(pComponent, "Health") == 0 )
			pAnimType = 2;
		
		if ( pAnimType != -1 )
		{
			vector targetDirection = GetDirection();
			vector toSourceDirection = (pSource.GetPosition() - GetPosition());

			targetDirection[1] = 0;
			toSourceDirection[1] = 0;

			targetDirection.Normalize();
			toSourceDirection.Normalize();

			float cosFi = vector.Dot(targetDirection, toSourceDirection);
			if ( cosFi >= 0 ) // front
				pAnimType++;
		}
		
		return pAnimType != -1;
	}
	
	//-------------------------------------------------------------
	//!
	//! Damage hits
	//! 
	
	bool m_DamageHitToProcess = false;
	
	bool m_DamageHitHeavy = false;
	int m_DamageHitType = 0;
	float m_ShockDamage = 0;
		
	bool HandleDamageHit(int pCurrentCommandID)
	{
		if ( pCurrentCommandID == DayZInfectedConstants.COMMANDID_HIT )
		{
			m_DamageHitToProcess = false;
			m_ShockDamage = 0;
			return true;
		}
		
		if ( m_DamageHitToProcess )
		{
			int randNum = Math.RandomIntInclusive(0, 100);
			float stunChange = SHOCK_TO_STUN_MULTIPLIER * m_ShockDamage;
			
			if ( m_DamageHitHeavy || randNum <= stunChange || ( m_MindState == DayZInfectedConstants.MINDSTATE_CALM || m_MindState == DayZInfectedConstants.MINDSTATE_DISTURBED ) )
				StartCommand_Hit(m_DamageHitHeavy, m_DamageHitType, m_DamageHitDirection);
			
			m_DamageHitToProcess = false;
			m_ShockDamage = 0;
			m_HeavyHitOverride = false;
			return true;
		}

		return false;
	}

	//! selects animation type and direction based on damage system data
	bool EvaluateDamageHitAnimation(EntityAI pSource, string pComponent, string pAmmoType, out bool pHeavyHit, out int pAnimType, out float pAnimHitDir)
	{
		int invertHitDir = 0; //Used to flip the heavy hit animation direction
		
		//! heavy hit
		pHeavyHit = ((GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " hitAnimation") > 0) || m_HeavyHitOverride);
		invertHitDir = GetGame().ConfigGetInt("cfgAmmo " + pAmmoType + " invertHitDir");
		
		//! anim type
		pAnimType = 0; // belly
		
		if ( !pHeavyHit )
		{
			if ( pComponent == "Torso" ) // body
				pAnimType = 1;
			else if ( pComponent == "Head" ) // head		
				pAnimType = 2;
		}
		
		//! direction
		//pAnimHitDir = ComputeHitDirectionAngle(pSource);
		pAnimHitDir = ComputeHitDirectionAngleEx(pSource, invertHitDir);
		//! shock GetDamage
		//m_ShockDamage = GetGame().ConfigGetFloat( "CfgAmmo " + pAmmoType + " DamageApplied " + "Shock " + "damage");
		return true;
	}
	
	float ComputeHitDirectionAngle(EntityAI pSource)
	{
		vector targetDirection = GetDirection();
		vector toSourceDirection = (pSource.GetPosition() - GetPosition());

		targetDirection[1] = 0;
		toSourceDirection[1] = 0;

		targetDirection.Normalize();
		toSourceDirection.Normalize();

		float cosFi = vector.Dot(targetDirection, toSourceDirection);
		vector cross = targetDirection * toSourceDirection;
		
		float dirAngle = Math.Acos(cosFi) * Math.RAD2DEG;
		if ( cross[1] < 0 )
			dirAngle = -dirAngle;
		
		return dirAngle;
	}
	
	float ComputeHitDirectionAngleEx(EntityAI pSource, int invertHitDir = 0)
	{
		vector targetDirection = GetDirection();
		vector toSourceDirection = (pSource.GetPosition() - GetPosition());

		targetDirection[1] = 0;
		toSourceDirection[1] = 0;

		targetDirection.Normalize();
		toSourceDirection.Normalize();

		float cosFi = vector.Dot(targetDirection, toSourceDirection);
		vector cross = targetDirection * toSourceDirection;
		
		float dirAngle = Math.Acos(cosFi) * Math.RAD2DEG;
		
		// We will invert direction of the hit
		if ( invertHitDir > 0 )
			dirAngle -= 180;

		if ( cross[1] < 0 )
			dirAngle = -dirAngle;
		
		return dirAngle;
	}
	
	//-------------------------------------------------------------
	//!
	//! Events from damage system
	//! 

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		m_TransportHitRegistered = false;
		
		if ( !IsAlive() )
		{
			dBodySetInteractionLayer(this, PhxInteractionLayers.RAGDOLL);
			
			ZombieHitData data = new ZombieHitData;
			data.m_Component = component;
			data.m_DamageZone = dmgZone;
			data.m_AmmoType = ammo;
			EvaluateDeathAnimationEx(source, data, m_DeathType, m_DamageHitDirection);
		}
		else
		{
			int crawlTransitionType = -1;
			if ( EvaluateCrawlTransitionAnimation(source, dmgZone, ammo, crawlTransitionType) )
			{
				m_CrawlTransition = crawlTransitionType;
				return;
			}
			
			if ( EvaluateDamageHitAnimation(source, dmgZone, ammo, m_DamageHitHeavy, m_DamageHitType, m_DamageHitDirection) )
			{
				if ( dmgZone )
					m_ShockDamage = damageResult.GetDamage( dmgZone, "Shock" );
				m_DamageHitToProcess = true;
				return;
			}
		}		
	}
	
	override void EEHitByRemote(int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		super.EEHitByRemote(damageType, source, component, dmgZone, ammo, modelPos);
	}

	//! sound debug messages
	protected void DebugSound(string s)
	{
		//Print(s);
	}
	
	//-------------------------------------------------------------
	//!
	//! Phx contact event
	//! 
	
	override protected void EOnContact(IEntity other, Contact extra)
	{
		if ( !IsAlive() )
			return;

		Transport transport = Transport.Cast(other);
		if ( transport )
		{
			if ( GetGame().IsServer() )
			{
				RegisterTransportHit(transport);
			}			
		}
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if ( !IsAlive() )
		{
			return false;
		}
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override vector GetCenter()
	{
		return GetBonePositionWS( GetBoneIndexByName( "spine3" ) );
	}
	
	//! returns true if backstab is in progress; used for suspending of AI targeting and other useful things besides
	override bool IsBeingBackstabbed()
	{
		return m_FinisherInProgress;
	}
	
	override void SetBeingBackstabbed(int backstabType)
	{
		// disable AI simulation
		GetAIAgent().SetKeepInIdle(true);
		
		// select death animation
		switch( backstabType )
		{
		case EMeleeHitType.FINISHER_LIVERSTAB:
			m_DeathType = DayZInfectedDeathAnims.ANIM_DEATH_BACKSTAB;
			break;
			
		case EMeleeHitType.FINISHER_NECKSTAB:
			m_DeathType = DayZInfectedDeathAnims.ANIM_DEATH_NECKSTAB;	
			break;
			
		default:
			m_DeathType = DayZInfectedDeathAnims.ANIM_DEATH_DEFAULT;
		}

		// set flag - death command will be executed
		m_FinisherInProgress = true;
				
		//Print("DbgZombies | DumbZombie on: " + GetGame().GetTime());
	}
	
	//! returns true if crawling; 'DayZInfectedCommandCrawl' is only for the transition, after that it remains 'DayZInfectedCommandMove' (unreliable)
	bool IsCrawling()
	{
		return m_IsCrawling;
	}
	
	// called from command death when stealth attack wan't successful
	void OnRecoverFromDeath()
	{
		// enable AI simulation again
		GetAIAgent().SetKeepInIdle(false);

		// reset flag
		m_FinisherInProgress = false;
		
		//Print("DbgZombies | DumbZombie off: " + GetGame().GetTime());
	}	
}

//! an extendable data container
class ZombieHitData
{
	int m_Component;
	string m_DamageZone;
	string m_AmmoType;
}