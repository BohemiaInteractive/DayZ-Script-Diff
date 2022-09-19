enum DayZInfectedConstants
{
	//! anim commands
	COMMANDID_MOVE,
	COMMANDID_VAULT,
	COMMANDID_DEATH,
	COMMANDID_HIT,
	COMMANDID_ATTACK,
	COMMANDID_CRAWL,
	COMMANDID_SCRIPT,
	
	//! mind states
	MINDSTATE_CALM,
	MINDSTATE_DISTURBED,
	MINDSTATE_ALERTED,
	MINDSTATE_CHASE,
	MINDSTATE_FIGHT,
};

enum DayZInfectedConstantsMovement
{
	MOVEMENTSTATE_IDLE = 0,
	MOVEMENTSTATE_WALK,
	MOVEMENTSTATE_RUN,
	MOVEMENTSTATE_SPRINT
}

enum DayZInfectedDeathAnims
{
	ANIM_DEATH_DEFAULT = 0,
	ANIM_DEATH_IMPULSE = 1,
	ANIM_DEATH_BACKSTAB = 2,
	ANIM_DEATH_NECKSTAB = 3
}

class DayZInfectedCommandMove extends AnimCommandBase
{
	proto native void SetStanceVariation(int pStanceVariation);
	proto native void SetIdleState(int pIdleState);
	proto native void StartTurn(float pDirection, int pSpeedType);
	proto native bool IsTurning();
}

class DayZInfectedCommandDeath extends AnimCommandBase
{
	
}

class DayZInfectedCommandHit   extends AnimCommandBase
{

}

class DayZInfectedCommandAttack extends AnimCommandBase
{
	proto native bool WasHit();
}
class DayZInfectedCommandVault extends AnimCommandBase
{
	proto native bool WasLand();
}

class DayZInfectedCommandCrawl extends AnimCommandBase
{

}

/**
*\brief DayZInfectedCommandScript fully scriptable command
*	\warning NON-MANAGED, will be managed by C++ once it is sent to the CommandHandler through DayZInfected.StartCommand_Script
*	\note So ideally, it is best to set up the DayZInfectedCommandScript, not create any instances and start it through DayZInfected.StartCommand_ScriptInst
*			In case an instance needs to be created, it needs manual deletion if not sent to the CommandHandler
*			But deleting it while it is in the CommandHandler will cause crashes
*/
class DayZInfectedCommandScript extends AnimCommandBase
{
	//! constructor must have 1st parameter to be DayZInfected
	void DayZInfectedCommandScript(DayZInfected pInfected) {}
	void ~DayZInfectedCommandScript() {}

	//---------------------------------------------------------------
	// usable everywhere

	//! this terminates command script and shows CommandHandler(  ... pCurrentCommandFinished == true );
	proto native void 	SetFlagFinished(bool pFinished);

	//---------------------------------------------------------------
	// PrePhys Update 

	//! script function usable in PrePhysUpdate
	proto native 	bool	PrePhys_GetTranslation(out vector pOutTransl);		// vec3 in local space !
	proto native 	bool	PrePhys_GetRotation(out float pOutRot[4]);         	// quaternion in local space !
	proto native 	void	PrePhys_SetTranslation(vector pInTransl); 			// vec3 in local space !
	proto native 	void	PrePhys_SetRotation(float pInRot[4]);				// quaternion in local space !

	//---------------------------------------------------------------
	// PostPhys Update 

	//! override this !
	//! final adjustment of physics state (after physics was applied)
	//! returns true if command continues running / false if command should end (or you can use SetFlagFinished(true))
	bool	PostPhysUpdate(float pDt);

	//! script function usable in PostPhysUpdate
	proto native 	void	PostPhys_GetPosition(out vector pOutTransl);		//! vec3 in world space
	proto native 	void	PostPhys_GetRotation(out float pOutRot[4]);        	//! quaternion in world space
	proto native 	void	PostPhys_SetPosition(vector pInTransl);				//! vec3 in world space
	proto native 	void	PostPhys_SetRotation(float pInRot[4]);				//! quaternion in world space
	proto native 	void	PostPhys_LockRotation();							//! do not process rotations !
}


class DayZInfected extends DayZCreatureAI
{	
	proto native DayZInfectedType GetDayZInfectedType();
	proto native DayZInfectedInputController GetInputController();	
	proto native DayZInfectedCommandMove StartCommand_Move();	
	proto native DayZInfectedCommandVault StartCommand_Vault(int pType);	
	proto native void StartCommand_Death(int pType, float pDirection);
	proto native void StartCommand_Hit(bool pHeavy, int pType, float pDirection);
	proto native DayZInfectedCommandAttack StartCommand_Attack(EntityAI pTarget, int pType, float pSubtype);
	proto native void StartCommand_Crawl(int pType);
	
	proto native bool CanAttackToPosition(vector pTargetPosition);
	
	proto native DayZInfectedCommandMove GetCommand_Move();
	proto native DayZInfectedCommandVault GetCommand_Vault();
	proto native DayZInfectedCommandAttack GetCommand_Attack();
	
	//! scripted commands
	proto native DayZInfectedCommandScript StartCommand_Script(DayZInfectedCommandScript pInfectedCommand);
	proto native DayZInfectedCommandScript StartCommand_ScriptInst(typename pCallbackClass);
	proto native DayZInfectedCommandScript GetCommand_Script();
	
	//! gets transform in World Space
	proto native	void 		GetTransformWS(out vector pTm[4]);
	
	//---------------------------------------------------------
	// bone transforms 

	//! returns bone index for a name (-1 if pBoneName doesn't exist)
	proto native 	int 		GetBoneIndexByName(string pBoneName);

	//! returns local space, model space, world space position of the bone 
	proto native	vector		GetBonePositionLS(int pBoneIndex);
	proto native 	vector		GetBonePositionMS(int pBoneIndex);
	proto native 	vector		GetBonePositionWS(int pBoneIndex);

	//! returns local space, model space, world space orientation (quaternion) of a bone 
	proto native	void 		GetBoneRotationLS(int pBoneIndex, out float pQuat[4]);
	proto native 	void 		GetBoneRotationMS(int pBoneIndex, out float pQuat[4]);
	proto native 	void 		GetBoneRotationWS(int pBoneIndex, out float pQuat[4]);

	//! returns local space, model space, world space orientation (quaternion) of a bone 
	proto native	void 		GetBoneTransformLS(int pBoneIndex, out vector pTm[4]);
	proto native 	void 		GetBoneTransformMS(int pBoneIndex, out vector pTm[4]);
	proto native 	void 		GetBoneTransformWS(int pBoneIndex, out vector pTm[4]);
	
	const float LEG_CRIPPLE_THRESHOLD = 74.0;
	bool 		m_HeavyHitOverride;
	//-------------------------------------------------------------
	void DayZInfected()
	{
	}
	
	//-------------------------------------------------------------
	void ~DayZInfected()
	{
	}
	
	//-------------------------------------------------------------
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		
		/*Print("damageResult: " + damageResult.GetDamage(dmgZone,"Health"));
		Print("dmgZone: " + dmgZone);
		Print("component: " + component);
		Print("----------------");*/
		
		if ( ammo.ToType().IsInherited(Nonlethal_Base) )
		{
			//Print("DayZInfected | EEHitBy | nonlethal hit");
			float dam = damageResult.GetDamage(dmgZone,"Shock");
			//Print("shock damage: " + damageResult.GetDamage(dmgZone,"Shock"));
			//Print("GetHealth - before: " + GetHealth());
			HandleSpecialZoneDamage(dmgZone,dam);
			AddHealth("","Health",-ConvertNonlethalDamage(dam));
			//Print("GetHealth - after: " + GetHealth());
		}
		
		if ( !IsAlive() )
		{
			if ( !m_DeathSyncSent ) //to be sent only once on hit/death
			{
				Man killer = source.GetHierarchyRootPlayer();
				
				if ( !m_KillerData ) //only one player is considered killer in the event of crossfire
				{
					m_KillerData = new KillerData;
					m_KillerData.m_Killer = killer;
					m_KillerData.m_MurderWeapon = source;
				}
				
				if ( killer && killer.IsPlayer() )
				{
					// was infected killed by headshot?
					if ( dmgZone == "Head" ) //no "Brain" damage zone defined (nor can it be caught like on player, due to missing command handler), "Head" is sufficient
					{
						m_KilledByHeadshot = true;
						if (m_KillerData.m_Killer == killer)
							m_KillerData.m_KillerHiTheBrain = true;
					}
				}
				SyncEvents.SendEntityKilled(this, m_KillerData.m_Killer, m_KillerData.m_MurderWeapon, m_KillerData.m_KillerHiTheBrain);
				m_DeathSyncSent = true;
			}
		}
	}
	
	float ConvertNonlethalDamage(float damage)
	{
		float converted_dmg = damage * GameConstants.PROJECTILE_CONVERSION_INFECTED;
		//Print("ConvertNonlethalDamage | " + converted_dmg);
		return converted_dmg;
	}
	
	void HandleSpecialZoneDamage(string dmgZone, float damage)
	{
		if ( damage < LEG_CRIPPLE_THRESHOLD )
			return;
		
		if (dmgZone == "LeftLeg" || dmgZone == "RightLeg")
		{
			SetHealth(dmgZone,"Health",0.0);
		}
		if (dmgZone == "Torso" || dmgZone == "Head") //TODO separate behaviour for head hits, anim/AI
		{
			m_HeavyHitOverride = true;
		}
	}
	
	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}
	
	//void SetCrawlTransition(string zone) {}
}
