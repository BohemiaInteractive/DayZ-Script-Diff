//! Light / Heavy punches
enum EFightLogicCooldownCategory
{
	EVADE = 0
}

class DayZPlayerMeleeFightLogic_LightHeavy
{
	//! cooldown timers settings
	const float EVADE_COOLDOWN			= 0.5;
	
	//
	const float CLOSE_TARGET_DISTANCE	= 1.5;
	
	//! dummy ammo types 
	const string DUMMY_LIGHT_AMMO		= "Dummy_Light";
	const string DUMMY_HEAVY_AMMO		= "Dummy_Heavy";

	protected PlayerBase							m_Player;
	protected ref DayZPlayerImplementMeleeCombat	m_MeleeCombat;
	protected EMeleeHitType							m_HitType;
	protected ref map <int, ref Timer>				m_CooldownTimers;
	protected Mission 								m_Mission;

	protected bool									m_IsInBlock;
	protected bool									m_IsEvading;

	protected bool									m_IsInComboRange;	
	protected bool 									m_WasPreviousHitProcessed;
	
	void DayZPlayerMeleeFightLogic_LightHeavy(DayZPlayerImplement player)
	{
		Init(player);
	}
	
	void Init(DayZPlayerImplement player)
	{
		m_DZPlayer					= player;
		m_Player 					= PlayerBase.Cast(player);
		m_MeleeCombat				= m_Player.GetMeleeCombat();
		m_Mission					= GetGame().GetMission();

		m_IsInBlock 				= false;
		m_IsEvading	 				= false;
		m_WasPreviousHitProcessed	= false;
		m_HitType					= EMeleeHitType.NONE;
		
		RegisterCooldowns();
	}

	void ~DayZPlayerMeleeFightLogic_LightHeavy() {}

	bool IsInBlock()
	{
		return m_IsInBlock;
	}
	
	void SetBlock(bool block)
	{
		m_IsInBlock = block;
	}
	
	bool IsEvading()
	{
		return m_IsEvading;
	}
	
	bool GetCurrentHitType()
	{
		return m_HitType;
	}
	
	bool CanFight()
	{
		if (m_Player.IsEmotePlaying())
			return false;
		
		if (m_Player.GetActionManager() && m_Player.GetActionManager().GetRunningAction())
			return false;

		return true;
	}

	protected void RegisterCooldowns()
	{
		m_CooldownTimers = new map<int, ref Timer>();
		m_CooldownTimers.Insert(EFightLogicCooldownCategory.EVADE, new Timer(CALL_CATEGORY_SYSTEM)); // evades
	}

	protected EMeleeHitType GetAttackTypeFromInputs(HumanInputController pInputs)
	{
		if (pInputs.IsMeleeFastAttackModifier() && m_Player.CanConsumeStamina(EStaminaConsumers.MELEE_HEAVY))
		{
			return EMeleeHitType.HEAVY;
		}

		return EMeleeHitType.LIGHT;
	}
	
	protected EMeleeHitType GetAttackTypeByWeaponAttachments(EntityAI pEntity)
	{
		if (!m_Player.CanConsumeStamina(EStaminaConsumers.MELEE_HEAVY))
		{
			return EMeleeHitType.NONE;
		}

		//! use stabbing if the bayonet/knife is attached to firearm
		if (pEntity.HasBayonetAttached())
		{
			return EMeleeHitType.WPN_STAB;
		}
		else if (pEntity.HasButtstockAttached())
		{
			return EMeleeHitType.WPN_HIT_BUTTSTOCK;
		}

		return EMeleeHitType.WPN_HIT;
	}
	
	protected float GetAttackTypeByDistanceToTarget(EntityAI pTarget, EMeleeTargetType pTargetType = EMeleeTargetType.ALIGNABLE)
	{
		if (pTargetType == EMeleeTargetType.NONALIGNABLE)
		{
			return 1.0;
		}

		//! target in short distance - in place attack
		if (IsShortDistance(pTarget, Math.SqrFloat(CLOSE_TARGET_DISTANCE)))
		{
			return 1.0;
		}
				
		//! target is far from the player - forward movement attack
		return 0.0;
	}
	
	bool HandleFightLogic(int pCurrentCommandID, HumanInputController pInputs, EntityAI pEntityInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		HumanInventoryWithFSM invFSM = HumanInventoryWithFSM.Cast(m_Player.GetHumanInventory());
		if (invFSM && !invFSM.IsIdle())
			return false;

		HumanCommandMove hcm 		= m_Player.GetCommand_Move();
		InventoryItem itemInHands	= InventoryItem.Cast(pEntityInHands);
		bool isFireWeapon			= itemInHands && itemInHands.IsWeapon();
		
		//! Check if we need to damage
		if (HandleHitEvent(pCurrentCommandID, pInputs, itemInHands, pMovementState, pContinueAttack))
			return false;

		//! Actually pressing a button to start a melee attack
		if ((pInputs.IsAttackButtonDown() && !isFireWeapon) || (pInputs.IsMeleeWeaponAttack() && isFireWeapon) || (pContinueAttack && isFireWeapon))
		{
			//Debug.MeleeLog(m_Player, string.Format("HandleFightLogic[1] attackButtonDown=%1, meleeWeaponAttack=%2, pContinueAttack=%3, isFireWeapon=%4", pInputs.IsAttackButtonDown(), pInputs.IsMeleeWeaponAttack(), pContinueAttack, isFireWeapon));
			
			//! do not perform attacks when blocking
			if (m_IsInBlock || m_IsEvading)
				return false;
			
			//! if the item in hands cannot be used as melee weapon
			if (itemInHands && !itemInHands.IsMeleeWeapon() && !isFireWeapon)
				return false;
			
			//! Currently not performing any attacks, so here we start the initial
			if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_MOVE)
			{
				//Debug.MeleeLog(m_Player, "HandleFightLogic[1a]");
				//! melee with firearm
				if (isFireWeapon)
				{
					return HandleInitialFirearmMelee(pCurrentCommandID, pInputs, itemInHands, pMovementState, pContinueAttack);
				}
				else
				{
					//! first attack in raised erc (light or heavy if modifier is used). Also added support for finishers
					if (pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT)
					{
						return HandleInitialMeleeErc(pCurrentCommandID, pInputs, itemInHands, pMovementState, pContinueAttack);
					}
					//! kick from raised pne
					else if (pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
					{
						return HandleProneKick(pCurrentCommandID, pInputs, itemInHands, pMovementState, pContinueAttack);
					}
					//! sprint attack in erc stance
					else if (pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_ERECT && m_Player.IsSprintFull())
					{
						return HandleSprintAttack(pCurrentCommandID, pInputs, itemInHands, pMovementState, pContinueAttack);
					}
				}
			}
			//! combo hits - when we are already in Melee command and clicking UseButton
			else if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_MELEE2)
			{
				m_IsInComboRange = m_Player.GetCommand_Melee2().IsInComboRange();
				if (m_IsInComboRange)
				{
					return HandleComboHit(pCurrentCommandID, pInputs, itemInHands, pMovementState, pContinueAttack);
				}
			}
		}
		//! evade and blocking logic
		else if (!isFireWeapon && pCurrentCommandID == DayZPlayerConstants.COMMANDID_MOVE)
		{
			//! evades in raised erc stance while moving
			if (pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT)
			{
				int roll = pInputs.IsMeleeLREvade();
				if (roll != 0)
				{
					//! not enough stamina to do evades
					if (!m_Player.CanConsumeStamina(EStaminaConsumers.MELEE_EVADE))
					{
						return false;
					}
					
					float angle;
					if (roll == 1)
					{
						angle = -90;	// left
					}
					else
					{
						angle = 90;		// right
					}
	
					// start melee evade
					m_IsEvading = true;
					SetCooldown(EVADE_COOLDOWN, EFightLogicCooldownCategory.EVADE);
					hcm.StartMeleeEvadeA(angle);
					m_Player.DepleteStamina(EStaminaModifiers.MELEE_EVADE);
					
					//Inflict shock when sidestepping with broken legs
					if (m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
					{
						m_Player.m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_LIGHT_MELEE_SHOCK);
						m_Player.m_ShockHandler.CheckValue(true);
					}
				}
			}

			//! stand up when crouching and raised pressed
			if (pInputs.IsWeaponRaised() && pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_CROUCH)
			{
				//Debug.MeleeLog(m_Player, "HandleFightLogic[2b]");
				if (DayZPlayerUtils.PlayerCanChangeStance(m_Player, DayZPlayerConstants.STANCEIDX_RAISEDERECT))
				{
					//Debug.MeleeLog(m_Player, "HandleFightLogic[2c]");
					hcm.ForceStance(DayZPlayerConstants.STANCEIDX_RAISEDERECT);
					
					return false;
				}
			}

			//! blocks in raised erc/pro stance
			//! (bare hand or with melee weapon only)
			if (!isFireWeapon && (pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT || pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE))
			{
				float angle2;

				if (hcm.GetCurrentInputAngle(angle2) && (angle2 < -130.0 || angle2 > 130))
				{
					hcm.SetMeleeBlock(true);
					SetBlock(true);
				}
				else
				{
					hcm.SetMeleeBlock(false);
					SetBlock(false);
				}
			}
			else
			{
				hcm.SetMeleeBlock(false);
				SetBlock(false);
			}
		}

		return false;
	}

	//! Handle the event that our melee hit something
	protected bool HandleHitEvent(int pCurrentCommandID, HumanInputController pInputs, InventoryItem itemInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		//! Check if we need to damage
		if (pCurrentCommandID == DayZPlayerConstants.COMMANDID_MELEE2)
		{
			HumanCommandMelee2 hmc2a = m_Player.GetCommand_Melee2();
			if (hmc2a)
			{
				//! on anim event Hit received
				if (hmc2a.WasHit())
				{
					//Debug.MeleeLog(m_Player, "HandleHitEvent[START]");
					//Debug.MeleeLog(m_Player, string.Format("HandleHitEvent[1] target=%1, hitPos=%2, hitZoneIdx=%3, finisherType=%4", m_MeleeCombat.GetTargetEntity(), m_MeleeCombat.GetHitPos().ToString(), m_MeleeCombat.GetHitZoneIdx(), m_MeleeCombat.GetFinisherType()));
					if (m_MeleeCombat.GetFinisherType() == -1)
					{
						//Debug.MeleeLog(m_Player, string.Format("HandleHitEvent[2] target=%1, hitPos=%2, hitZoneIdx=%3, finisherType=%4", m_MeleeCombat.GetTargetEntity(), m_MeleeCombat.GetHitPos().ToString(), m_MeleeCombat.GetHitZoneIdx(), m_MeleeCombat.GetFinisherType()));
						//! re-target (enemy can have moved out of range or disappeared)
						m_MeleeCombat.Update(itemInHands, m_HitType, true);
					}

					//! evaluate hit - selection of cfg 'ammo' type
					EvaluateHit(itemInHands);					
					
					//Get gloves
					ClothingBase gloves;
					if (m_HitType == EMeleeHitType.KICK)
					{
						//We kick so "gloves" will be the shoes
						gloves = ClothingBase.Cast(m_Player.GetItemOnSlot("FEET"));
					}
					else
					{
						gloves = ClothingBase.Cast(m_Player.GetItemOnSlot("GLOVES"));
					}
					
					//If we hit something, inflict damage
					DamageHands(m_Player, gloves, itemInHands);
					
					m_MeleeCombat.ResetTarget();
					
					EnableControls();
					
					m_WasPreviousHitProcessed = true;
					
					//Debug.MeleeLog(m_Player, "HandleHitEvent[END]");
					
					return true;
				}
			}
		}
		
		return false;
	}
	
	//! NOTE: Only singular (or first) hits, combo attacks are handled in combo
	protected bool HandleInitialFirearmMelee(int pCurrentCommandID, HumanInputController pInputs, InventoryItem itemInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{	
		//! don't allow bash to interfere with actions like chambering or ejecting bullets
		Weapon_Base weapon = Weapon_Base.Cast(itemInHands);
		if (weapon.IsWaitingForActionFinish())
			return false;		
		
		HumanCommandMove hcm = m_Player.GetCommand_Move();
		
		//! perform firearm melee from raised erect or continue with attack after character stand up from crouch
		if (hcm.GetCurrentMovementSpeed() <= 2.05 && (pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDERECT || pContinueAttack) && !m_Player.IsFighting())
		{
			m_HitType = GetAttackTypeByWeaponAttachments(itemInHands);
			if ( m_HitType == EMeleeHitType.NONE )
			{
				pContinueAttack = false;
				return false; //! exit if there is no suitable attack
			}
			
			m_MeleeCombat.Update(itemInHands, m_HitType);

			EntityAI target;
			EMeleeTargetType targetType;
			GetTargetData(target, targetType);
			float attackByDistance = GetAttackTypeByDistanceToTarget(target, targetType);

			m_Player.StartCommand_Melee2(target, m_HitType == EMeleeHitType.WPN_STAB, attackByDistance, m_MeleeCombat.GetHitPos());			
			m_Player.DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
			DisableControls();

			pContinueAttack = false; // reset continueAttack flag

			return true;
		}				
		//! char stand up when performing firearm melee from crouch
		else if ( pMovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDCROUCH )
		{
			if (hcm)
			{
				hcm.ForceStance(DayZPlayerConstants.STANCEIDX_RAISEDERECT);
				pContinueAttack = true;
				return false;
			}
		}
		
		return false;
	}
	
	//! First attack in raised erc (light or heavy if modifier is used). Also added support for finishers
	protected bool HandleInitialMeleeErc(int pCurrentCommandID, HumanInputController pInputs, InventoryItem itemInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		//Debug.MeleeLog(m_Player, "HandleInitialMeleeErc[1]");
		
		m_HitType = GetAttackTypeFromInputs(pInputs);
		m_MeleeCombat.Update(itemInHands, m_HitType);
		
		//Debug.MeleeLog(m_Player, string.Format("HandleInitialMeleeErc[2] %1", EnumTools.EnumToString(EMeleeHitType, m_HitType)));
		
		EntityAI target;
		EMeleeTargetType targetType;
		GetTargetData(target, targetType);
		float attackByDistance = GetAttackTypeByDistanceToTarget(target, targetType);
		
		// finisher attack hittype override
		int finisherType = m_MeleeCombat.GetFinisherType();
		if (finisherType != -1)
		{
			int animationType = DetermineFinisherAnimation(finisherType);
			if (animationType > -1)
			{
				//Debug.MeleeLog(m_Player, string.Format("HandleInitialMeleeErc[3a] target=%1, finisherType=%2, animationType=%3, hitPositionWS=%4", target, finisherType, animationType, m_MeleeCombat.GetHitPos().ToString()));
				m_Player.StartCommand_Melee2(target, animationType, attackByDistance, m_MeleeCombat.GetHitPos());
			}
			else
			{
				//Debug.MeleeLog(m_Player, string.Format("HandleInitialMeleeErc[3b] target=%1, finisherType=%2, animationType=%3, hitPositionWS=%4", target, finisherType, (m_HitType == EMeleeHitType.HEAVY).ToString(), m_MeleeCombat.GetHitPos().ToString()));
				m_Player.StartCommand_Melee2(target, m_HitType == EMeleeHitType.HEAVY, attackByDistance, m_MeleeCombat.GetHitPos());
			}
			
			m_HitType = finisherType;
			target.SetBeingBackstabbed(finisherType);
		}
		else
		{
			//Debug.MeleeLog(m_Player, string.Format("HandleInitialMeleeErc[3d] target=%1, animationType=%2, hitPositionWS=%3", target, (m_HitType == EMeleeHitType.HEAVY).ToString(), m_MeleeCombat.GetHitPos().ToString()));
			m_Player.StartCommand_Melee2(target, m_HitType == EMeleeHitType.HEAVY, attackByDistance, m_MeleeCombat.GetHitPos());
		}
		
		if (m_HitType == EMeleeHitType.HEAVY)
		{
			m_Player.DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
			if (m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			{	
				m_Player.m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_HEAVY_MELEE_SHOCK);
				m_Player.m_ShockHandler.CheckValue(true);
			}
		}
		else
		{
			m_Player.DepleteStamina(EStaminaModifiers.MELEE_LIGHT);
			if (m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			{
				m_Player.m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_LIGHT_MELEE_SHOCK);
				m_Player.m_ShockHandler.CheckValue(true);
			}
		}	

		m_WasPreviousHitProcessed = false;
		
		DisableControls();
		
		return true;
	}
	
	//! kick from raised pne
	protected bool HandleProneKick(int pCurrentCommandID, HumanInputController pInputs, InventoryItem itemInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		HumanCommandWeapons hcw = m_Player.GetCommandModifier_Weapons();

		float hcw_angle = hcw.GetBaseAimingAngleLR();
		//! check if player is on back
		//! (situation where the player is raised in prone and on back is not in anim graph)
		if ( ( hcw_angle < -90 || hcw_angle > 90 ) && m_Player.GetBrokenLegs() != eBrokenLegs.BROKEN_LEGS )
		{
			// targetting
			m_HitType = EMeleeHitType.KICK;
			m_MeleeCombat.Update(itemInHands, m_HitType);

			EntityAI target;
			EMeleeTargetType targetType;
			GetTargetData(target, targetType);
			float attackByDistance = GetAttackTypeByDistanceToTarget(target, targetType);

			// command
			m_Player.StartCommand_Melee2(target, false, attackByDistance, m_MeleeCombat.GetHitPos());
			m_Player.DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
			DisableControls();
			
			return true;
		}
		
		return false;
	}
	
	//! sprint attack in erc stance
	protected bool HandleSprintAttack(int pCurrentCommandID, HumanInputController pInputs, InventoryItem itemInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		//! targetting
		m_HitType = EMeleeHitType.SPRINT;
		m_MeleeCombat.Update(itemInHands, m_HitType);

		EntityAI target;
		EMeleeTargetType targetType;
		GetTargetData(target, targetType);
		float attackByDistance = GetAttackTypeByDistanceToTarget(target, targetType);

		//! command
		//Debug.MeleeLog(m_Player, string.Format("HandleSprintAttack[1] target=%1, animationType=%2, hitPositionWS=%3", target, false, m_MeleeCombat.GetHitPos().ToString()));
		
		m_Player.StartCommand_Melee2(target, EMeleeHitType.HEAVY, attackByDistance, m_MeleeCombat.GetHitPos());
		m_Player.DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
		DisableControls();

		return true;
	}
	
	//! combo hits - when we are already in Melee command and clicking UseButton
	protected bool HandleComboHit(int pCurrentCommandID, HumanInputController pInputs, InventoryItem itemInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
		//Debug.MeleeLog(m_Player, "HandleComboHit::");
		EMeleeHitType hitType;
		
		//! select if the next attack will be light or heavy (based on input/attachment modifier)
		if (itemInHands && itemInHands.IsWeapon())
			hitType = GetAttackTypeByWeaponAttachments(itemInHands);
		else
			hitType = GetAttackTypeFromInputs(pInputs);

		//! no suitable attack - skip that
		if (hitType == EMeleeHitType.NONE)
			return false;
		
		//! No combos for the finisher command
		if (m_MeleeCombat.GetFinisherType() > -1)
			return false;
		
		bool isHeavy = (hitType == EMeleeHitType.HEAVY || hitType == EMeleeHitType.WPN_STAB);
		EMeleeHitType previousHitType = m_HitType;

		//! wait for the previous hit commit before hitting again if the next attack is heavy and the previous wasn't - otherwise the previous light attack will turn into heavy
		if (!m_WasPreviousHitProcessed && previousHitType != EMeleeHitType.HEAVY && isHeavy)
		{
			return false;
		}

		m_HitType = hitType;

		//! targetting
		//Debug.MeleeLog(m_Player, string.Format("HandleComboHit:: %1", EnumTools.EnumToString(EMeleeHitType, m_HitType)));
	 	m_MeleeCombat.Update(itemInHands, m_HitType);

		EntityAI target;
		EMeleeTargetType targetType;
		GetTargetData(target, targetType);
		float attackByDistance = GetAttackTypeByDistanceToTarget(target, targetType);

		//! continue 'combo' - left hand attacks
		m_Player.GetCommand_Melee2().ContinueCombo(isHeavy, attackByDistance, target, m_MeleeCombat.GetHitPos());				
		DisableControls();
		
		//! broken legs shock
		if (m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			float shock = PlayerConstants.BROKEN_LEGS_HEAVY_MELEE_SHOCK;
			
			if (!isHeavy)
				shock = PlayerConstants.BROKEN_LEGS_LIGHT_MELEE_SHOCK;
			
			m_Player.m_ShockHandler.SetShock(PlayerConstants.BROKEN_LEGS_HEAVY_MELEE_SHOCK);
			m_Player.m_ShockHandler.CheckValue(true);
		}

		//! stamina depletion per attack
		switch (m_HitType)
		{
			case EMeleeHitType.HEAVY:
			case EMeleeHitType.SPRINT:
			case EMeleeHitType.WPN_STAB:
			case EMeleeHitType.WPN_HIT_BUTTSTOCK:
			case EMeleeHitType.WPN_HIT:
				m_Player.DepleteStamina(EStaminaModifiers.MELEE_HEAVY);
				break;
			default:
				m_Player.DepleteStamina(EStaminaModifiers.MELEE_LIGHT);
				break;
		}

		return true;
	}

	protected void EvaluateHit(InventoryItem weapon)
	{
		EntityAI target = m_MeleeCombat.GetTargetEntity();
	
		if (target)
		{
			if (target.IsInherited(DayZPlayer))
			{
				EvaluateHit_Player(weapon, target);
			}
			else if (target.IsInherited(DayZInfected))
			{
				EvaluateHit_Infected(weapon, target);
			}
			else if (target.GetMeleeTargetType() == EMeleeTargetType.NONALIGNABLE)
			{
				EvaluateHit_NonAlignableObjects(weapon, target);
			}
			else
			{
				EvaluateHit_Common(weapon, target);
			}
			
			m_MeleeCombat.CheckMeleeItem();
		}
	}

	protected void EvaluateHit_Player(InventoryItem weapon, Object target)
	{
		int hitZoneIdx = m_MeleeCombat.GetHitZoneIdx();
		int weaponMode = m_MeleeCombat.GetWeaponMode();
		int forcedWeaponMode = -1;
		vector hitPosWS;
		bool forcedDummy = false;

		PlayerBase targetPlayer = PlayerBase.Cast(target);
		//! Melee Hit/Impact modifiers
		if (targetPlayer)
		{
			vector targetPos = targetPlayer.GetPosition();
			vector agressorPos = m_Player.GetPosition();
			
			// We get the angle from which an infected hit the blocking player
			float hitAngle = Math.RAD2DEG * Math.AbsFloat(Math3D.AngleFromPosition(targetPos, MiscGameplayFunctions.GetHeadingVector(targetPlayer), agressorPos));
							
			//! if the oponnent is in Melee Block shift the damage down
			if (targetPlayer.GetMeleeFightLogic() && targetPlayer.GetMeleeFightLogic().IsInBlock() && hitAngle <= GameConstants.PVP_MAX_BLOCKABLE_ANGLE)
			{
				if (weaponMode > 0)
				{
					forcedWeaponMode = --weaponMode; // Heavy -> Light shift
				}
				else
					forcedDummy = true; // dummy hits, cannot shift lower than 0
			}
		}

		EvaluateHit_Common(weapon, target, forcedDummy, forcedWeaponMode);
	}
	
	protected void EvaluateHit_Infected(InventoryItem weapon, Object target)
	{
		//If the finisher attack haven't been performed, then use normal hit
		if (!EvaluateFinisherAttack(weapon, target))
			EvaluateHit_Common(weapon, target);
	}
	
	//! Check and evaluate stealth kill, if applicable
	protected bool EvaluateFinisherAttack(InventoryItem weapon, Object target)
	{
		//Debug.MeleeLog(m_Player, string.Format("EvaluateFinisherAttack[1] target=%1, hitPos=%2, hitZoneIdx=%3, finisherType=%4", m_MeleeCombat.GetTargetEntity(), m_MeleeCombat.GetHitPos().ToString(), m_MeleeCombat.GetHitZoneIdx(), m_MeleeCombat.GetFinisherType()));
		
		if (m_MeleeCombat.GetFinisherType() > -1)
		{
			if (GetGame().IsServer())
				DamageSystem.CloseCombatDamage(m_Player, target, m_MeleeCombat.GetHitZoneIdx(), DetermineFinisherAmmo(m_MeleeCombat.GetFinisherType()), m_MeleeCombat.GetHitPos(), ProcessDirectDamageFlags.NO_ATTACHMENT_TRANSFER);
				
			return true;
		}

		return false;
	}
	
	protected void EvaluateHit_Common(InventoryItem weapon, Object target, bool forcedDummy=false, int forcedWeaponMode = -1)
	{
		int hitZoneIdx = m_MeleeCombat.GetHitZoneIdx();
		int weaponMode = m_MeleeCombat.GetWeaponMode();
		vector hitPosWS;
		string ammo;
		
		if (forcedWeaponMode > -1)
			weaponMode = forcedWeaponMode;

		EntityAI targetEntity = EntityAI.Cast(target);

		//! check if we need to use dummy hit
		if (!DummyHitSelector(m_HitType, ammo) && !forcedDummy)
		{
			//! normal hit with applied damage to targeted component
			if (hitZoneIdx >= 0)
			{
				hitPosWS = targetEntity.ModelToWorld(targetEntity.GetDefaultHitPosition());
				
				if (WeaponDestroyedCheck(weapon, ammo))
				{
					DamageSystem.CloseCombatDamage(m_Player, target, hitZoneIdx, ammo, hitPosWS);
					//Debug.MeleeLog(m_Player, string.Format("EvaluateHit_Common[a]::CloseCombatDamage:: target: %1, hitzone: %2, ammo: %3", target, hitZoneIdx, ammo));
				}
				else
				{
					m_Player.ProcessMeleeHit(weapon, weaponMode, target, hitZoneIdx, hitPosWS);
					//Debug.MeleeLog(m_Player, string.Format("EvaluateHit_Common[b]::ProcessMeleeHit:: target: %1, hitzone: %2, meleeMode: %3, forcedWeaponMode:%4", target, hitZoneIdx, weaponMode));
				}
			}
		}
		else
		{
			//! play hit animation for dummy hits
			if (GetGame().IsServer() && targetEntity)
			{
				DummyHitSelector(m_HitType, ammo);
				hitPosWS = targetEntity.ModelToWorld(targetEntity.GetDefaultHitPosition()); //! override hit pos by pos defined in type
				DamageSystem.CloseCombatDamage(m_Player, target, hitZoneIdx, ammo, hitPosWS);
				//Debug.MeleeLog(m_Player, string.Format("EvaluateHit_Common[c]::CloseCombatDamage:: target: %1, hitzone: %2, meleeMode: %3, ammo: %4", target, hitZoneIdx, weaponMode, ammo));
			}
		}
	}

	protected void EvaluateHit_NonAlignableObjects(InventoryItem weapon, Object target)
	{
		int hitZoneIdx = m_MeleeCombat.GetHitZoneIdx();
		vector hitPosWS;
		string ammo;
		
		if (hitZoneIdx >= 0)
		{
			if (WeaponDestroyedCheck(weapon,ammo))
				DamageSystem.CloseCombatDamage(m_Player, target, hitZoneIdx, ammo, m_MeleeCombat.GetHitPos());
			else
				m_Player.ProcessMeleeHit(weapon, m_MeleeCombat.GetWeaponMode(), target, hitZoneIdx, m_MeleeCombat.GetHitPos());
		}

		return;
	}
	
	bool WeaponDestroyedCheck(InventoryItem weapon, out string ammo)
	{
		if (!weapon)
			return false;
		
		Weapon_Base firearm = Weapon_Base.Cast(weapon);
		ItemBase bayonet = ItemBase.Cast(weapon.GetInventory().FindAttachment(weapon.GetBayonetAttachmentIdx()));
		
		if ( firearm && bayonet && bayonet.IsRuined() )
		{
			ammo = bayonet.GetRuinedMeleeAmmoType();
			return true;
		}
		else if (weapon.IsRuined())
		{
			ammo = weapon.GetRuinedMeleeAmmoType();
			return true;
		}
		else
		{
			return false;
		}
	}

	protected void GetTargetData(out EntityAI target, out EMeleeTargetType targetType)
	{
		target = m_MeleeCombat.GetTargetEntity();
		targetType = EMeleeTargetType.ALIGNABLE; //! default
		
		// If there is no hitzone defined, then we won't be able to hit the target anyways..
		if (m_MeleeCombat.GetHitZoneIdx() < 0)
		{
			target = null;
		}
		
		if ( target )
		{
			targetType = target.GetMeleeTargetType();
		}

		//! nullify target for nonalignable objects (big objects)
		if ( targetType == EMeleeTargetType.NONALIGNABLE )
		{
			target = null;
		}
	}

	//! evaluation of hit player vs. player
	protected bool DummyHitSelector(EMeleeHitType hitType, out string ammoType)
	{
		switch (hitType)
		{
		//! in case of kick (on back or push from erc) change the ammo type to dummy
		case EMeleeHitType.KICK:
			ammoType = DUMMY_HEAVY_AMMO;
			return true;
		break;
		}
		
		ammoType = DUMMY_LIGHT_AMMO;
		
		return false;
	}
	
	//! DEPRECATED - moved into DayZPlayerImplementMeleeCombat
	protected bool IsBehindEntity(int angle, DayZPlayer source, Object target)
	{
		return false;
	}

	protected void SetCooldown(float time, EFightLogicCooldownCategory cooldownCategory)
	{
		//! stops currently running cooldown timer (for specific category)
		if ( m_CooldownTimers.Get(cooldownCategory).IsRunning() )
		{
			m_CooldownTimers.Get(cooldownCategory).Stop();
		}
		
		//! param for ResetCooldown
		Param1<int> param = new Param1<int>(cooldownCategory);
		m_CooldownTimers.Get(cooldownCategory).Run(time, this, "ResetCooldown", param);
	}
	
	protected void ResetCooldown(EFightLogicCooldownCategory cooldownCategory)
	{
		switch (cooldownCategory)
		{
		case EFightLogicCooldownCategory.EVADE:
			m_IsEvading = false;
		break;
		}
	}

	protected void EnableControls();
	protected void DisableControls();
	
	protected void DamageHands(DayZPlayer DZPlayer, ClothingBase gloves, InventoryItem itemInHands)
	{
		EntityAI target = m_MeleeCombat.GetTargetEntity();
		
		//We did not hit anything
		if (itemInHands || !target || !DZPlayer)
			return;
		
		//Check if server side
		if (GetGame().IsServer())
		{
			int randNum;
			
			//If gloves, damage gloves
			if (gloves && gloves.GetHealthLevel() < GameConstants.STATE_RUINED)
			{
				gloves.DecreaseHealth("", "", 1);
			}
			else
			{
				//Do not add bleeding if hitting player, zombie or animal
				if (PlayerBase.Cast(target) || DayZCreatureAI.Cast(target))
					return;
				
				BleedingSourcesManagerServer bleedingManager;
				
				//We don't inflict bleeding to hands when kicking
				if (m_HitType != EMeleeHitType.KICK)
				{
					//Random bleeding source
					randNum = Math.RandomIntInclusive(1, 15);
					switch (randNum)
					{
						case 1:
							if (m_Player)
							{
								bleedingManager = m_Player.GetBleedingManagerServer();
								if (bleedingManager)
									bleedingManager.AttemptAddBleedingSourceBySelection("RightForeArmRoll");
							}
							break;
						
						case 2:
							if (m_Player)
							{
								bleedingManager = m_Player.GetBleedingManagerServer();
								if (bleedingManager)
									bleedingManager.AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
							}
							break;
					}
				}
				else
				{
					//Random bleeding source
					randNum = Math.RandomIntInclusive(1, 15);
					//We only add bleeding to left foot as character kicks with left foot
					switch (randNum)
					{
						case 1:
							if (m_Player)
							{
								bleedingManager = m_Player.GetBleedingManagerServer();
								if (bleedingManager)
									bleedingManager.AttemptAddBleedingSourceBySelection("LeftToeBase");
							}
							break;
						
						case 2:
							if (m_Player)
							{
								bleedingManager = m_Player.GetBleedingManagerServer();
								if (bleedingManager)
									bleedingManager.AttemptAddBleedingSourceBySelection("LeftFoot");
							}
							break;
					}
				}
			}
		}
	}
	
	//! Picks a specific finisher animation. Not used for mounted bayonet stabs
	int DetermineFinisherAnimation(int finisher_type)
	{
		int animation = -1;
		
		switch (finisher_type)
		{
			case EMeleeHitType.FINISHER_LIVERSTAB:
				animation = HumanCommandMelee2.HIT_TYPE_FINISHER;
			break;
			
			case EMeleeHitType.FINISHER_NECKSTAB:
				animation = HumanCommandMelee2.HIT_TYPE_FINISHER_NECK;
			break;
		}
		
		return animation;
	}
	
	//! Picks a specific finisher ammo fot the hit type. This gets synchronized and guides further behaviour of the target.
	string DetermineFinisherAmmo(int finisher_type)
	{
		string ret = "";
		
		switch (finisher_type)
		{
			case EMeleeHitType.FINISHER_NECKSTAB:
				ret = "FinisherHitNeck";
				break;
			
			default:
				ret = "FinisherHit";
				break;
		}

		return ret;
	}

	protected bool IsShortDistance(EntityAI pTarget, float pDistanceSq)
	{
		return pTarget && vector.DistanceSq(m_Player.GetPosition(), m_MeleeCombat.GetHitPos()) <= pDistanceSq || vector.DistanceSq(m_Player.GetBonePositionWS(m_Player.GetBoneIndexByName("Head")), m_MeleeCombat.GetHitPos()) <= pDistanceSq)
	}

	//!
	//! DEPRECATED
	protected DayZPlayerImplement					m_DZPlayer;
	protected bool 									m_IsFinisher;
	
	int GetFinisherType(InventoryItem weapon, EntityAI target)
	{
		return -1;
	}
	
	bool IsHitTypeFinisher(int type)
	{
		return false;
	}
	
	int DetermineSpecificFinisherType(ItemBase weapon, int component)
	{
		return -1;
	}
}