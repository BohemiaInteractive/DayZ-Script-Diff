// *************************************************************************************
// ! DayZPlayerSyncJunctures - sync junctures for dayz player - static functions
// *************************************************************************************
class DayZPlayerSyncJunctures
{
	static const int SJ_DAMAGE_HIT						= 0;
	static const int SJ_INJURY							= 1;
	static const int SJ_ACTION_INTERRUPT				= 2;
	static const int SJ_PLAYER_STATES					= 3;
	static const int SJ_QUICKBAR_SET_SHORTCUT			= 4;
	static const int SJ_INVENTORY						= 5;
	static const int SJ_ACTION_ACK_ACCEPT				= 6;
	static const int SJ_ACTION_ACK_REJECT				= 7;
	static const int SJ_WEAPON_ACTION_ACK_ACCEPT		= 8;
	static const int SJ_WEAPON_ACTION_ACK_REJECT		= 9;
	static const int SJ_WEAPON_SET_JAMMING_CHANCE		= 10;
	static const int SJ_UNCONSCIOUSNESS					= 11;
	static const int SJ_DEATH					     	= 12;
	static const int SJ_PLAYER_FB_MODIFIER				= 13;
	static const int SJ_PLAYER_ADD_MODIFIER				= 14;
	static const int SJ_KURU_REQUEST					= 15;
	static const int SJ_GESTURE_REQUEST					= 16;
	static const int SJ_INVENTORY_REPAIR				= 17;
	static const int SJ_WEAPON_LIFT						= 18;
	static const int SJ_WEAPON_RAISE_COMPLETED			= 19;
	static const int SJ_DELETE_ITEM						= 20;
	static const int SJ_BROKEN_LEGS						= 21;
	static const int SJ_SHOCK							= 22;
	static const int SJ_STAMINA							= 23;
	static const int SJ_STAMINA_MISC					= 24;
	
	//-------------------------------------------------------------
	//!
	//! Death
	//! 

	static void SendDeath(DayZPlayer pPlayer, int pType, float pHitDir)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		
		ctx.Write(pType);
		ctx.Write(pHitDir);
		pPlayer.SendSyncJuncture(SJ_DEATH, ctx);
	}
	
	static bool ReadDeathParams(ParamsReadContext pCtx, out int pType, out float pHitDir)
	{
		if (!pCtx.Read(pType))
			return false;
		if (!pCtx.Read(pHitDir))
			return false;

		return true;
	}

	//-------------------------------------------------------------
	//!
	//! DamageHit
	//! 

	static void SendDamageHit(DayZPlayer pPlayer, int pType, float pHitDir, bool pFullbody)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		
		ctx.Write(pType);
		ctx.Write(pHitDir);
		ctx.Write(pFullbody);
		pPlayer.SendSyncJuncture(SJ_DAMAGE_HIT, ctx);
	}
	
	static void SendDamageHitEx(DayZPlayer pPlayer, int pType, float pHitDir, bool pFullbody, TotalDamageResult pDamageResult, int pDamageType, EntityAI pSource, string pComponent, string pAmmoType, vector pModelPos)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		SyncHitInfo data = new SyncHitInfo;
		
		data.m_AnimType = pType;
		data.m_HitDir = pHitDir;
		data.m_Fullbody = pFullbody;
		data.m_HasSource = pSource != null;
		if ( !pDamageResult )
		{
			data.m_HealthDamage = -1.0;
		}
		else
		{
			data.m_HealthDamage = pDamageResult.GetHighestDamage("Health");
		}
		
		ctx.Write(data);
		pPlayer.SendSyncJuncture(SJ_DAMAGE_HIT, ctx);
	}
	
	static bool ReadDamageHitParams(ParamsReadContext pCtx, out int pType, out float pHitDir, out bool pFullbody)
	{
		if (!pCtx.Read(pType))
			return false;
		if (!pCtx.Read(pHitDir))
			return false;
		if (!pCtx.Read(pFullbody))
			return false;	
		return true;
	}
	
	static bool ReadDamageHitParamsEx(ParamsReadContext pCtx, out SyncHitInfo pData)
	{
		if (!pCtx.Read(pData))
			return false;
		return true;
	}

	//-------------------------------------------------------------
	//!
	//! Injury
	//! 
	
	static void SendInjury(DayZPlayer pPlayer, bool pEnable, eInjuryHandlerLevels level)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(pEnable);
		ctx.Write(level);

		pPlayer.SendSyncJuncture(SJ_INJURY, ctx);
	}
	
	static bool ReadInjuryParams(ParamsReadContext pCtx, out bool pEnable, out eInjuryHandlerLevels level)
	{
		if ( !pCtx.Read(pEnable) )
			return false; // error		
		if ( !pCtx.Read(level) )
			return false; // error
		
		return true;
	}

	//-------------------------------------------------------------
	//!
	//! Conscious / Unconscious
	//! 
	
	static void SendPlayerUnconsciousness(DayZPlayer pPlayer, bool enable)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;

		ctx.Write(enable);
		
		pPlayer.SendSyncJuncture(SJ_UNCONSCIOUSNESS, ctx);
	}
	
	static bool ReadPlayerUnconsciousnessParams(ParamsReadContext pCtx, out bool enable)
	{
		if ( !pCtx.Read(enable) )
		{
			return false;
		}
		return true;
	}
	
	//-------------------------------------------------------------
	//!
	//! Full body
	//! 
	
	static void SendPlayerFBModifier(PlayerBase pPlayer, int type)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(type);
		
		pPlayer.SendSyncJuncture(SJ_PLAYER_FB_MODIFIER, ctx);
	}
	
	static bool ReadPlayerFBModifier(ParamsReadContext pCtx, out int type)
	{
		if ( !pCtx.Read(type) )
			return false; // error
		return true;
	}
	
	//-------------------------------------------------------------
	//!
	//! Additive Symptoms
	//!
	
	static void SendPlayerSymptomADD(DayZPlayer pPlayer, int type, int state_type)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(state_type);
		ctx.Write(type);


		pPlayer.SendSyncJuncture(SJ_PLAYER_ADD_MODIFIER, ctx);
	}
	
	static bool ReadPlayerSymptomADDParams(ParamsReadContext pCtx, out int type)
	{
		if ( !pCtx.Read(type) )
			return false; // error		
		return true;
	}
	
	
	//-------------------------------------------------------------
	//!
	//! Full body Symptoms
	//! 
	
	static void SendPlayerSymptomFB(DayZPlayer pPlayer, DayZPlayerConstants anim_id, int state_type, int stance_mask, float duration)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(state_type);
		ctx.Write(anim_id);
		ctx.Write(stance_mask);
		ctx.Write(duration);
		//ctx.Write(pPlayer);
		
		pPlayer.SendSyncJuncture(SJ_PLAYER_STATES, ctx);
	}
	
	static bool ReadPlayerSymptomFBParams(ParamsReadContext pCtx, out DayZPlayerConstants anim_id, out int stance_mask, out float duration)
	{
		if ( !pCtx.Read(anim_id) )
			return false; // error
		if ( !pCtx.Read(stance_mask) )
			return false; // error
		if ( !pCtx.Read(duration) )
			return false; // error
		//if ( !pCtx.Read(pPlayer) )
			//return false; // error
		return true;
	}


	//-------------------------------------------------------------
	//!
	//! Action interrupt
	//! 
	
	static void SendActionInterrupt(DayZPlayer pPlayer)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		pPlayer.SendSyncJuncture(SJ_ACTION_INTERRUPT, ctx);
	}
	
	static bool ReadActionInterruptParams(ParamsReadContext pCtx)
	{		
		return true;
	}
	
	//-------------------------------------------------------------
	//!
	//! Action Acknowledgment
	//! 
	static void SendActionAcknowledgment(DayZPlayer pPlayer, int AckID, bool accept)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(AckID);
		if (accept)
			pPlayer.SendSyncJuncture(SJ_ACTION_ACK_ACCEPT, ctx);
		else
			pPlayer.SendSyncJuncture(SJ_ACTION_ACK_REJECT, ctx);
	}
	
	
	static void SendWeaponActionAcknowledgment(DayZPlayer pPlayer, int AckID, bool accept)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(AckID);
		if (accept)
			pPlayer.SendSyncJuncture(SJ_WEAPON_ACTION_ACK_ACCEPT, ctx);
		else
			pPlayer.SendSyncJuncture(SJ_WEAPON_ACTION_ACK_REJECT, ctx);
	}
		
	//-------------------------------------------------------------
	//!
	//! Kuru Disease Shake
	//! 
	
	static bool ReadKuruRequest(ParamsReadContext pCtx, out float amount)
	{
		if ( !pCtx.Read(amount) )
			return false; // error		
		return true;
	}
	
	static void SendKuruRequest(DayZPlayer pPlayer, float amount)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(amount);
		pPlayer.SendSyncJuncture(SJ_KURU_REQUEST, ctx);
	}
	
	//-------------------------------------------------------------
	//!
	//! Quickbar
	//! 
	
	static void SendQuickbarSetShortcut(DayZPlayer pPlayer, EntityAI item, int index, bool force = false )
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(item);
		ctx.Write(index);
		ctx.Write(force);

		pPlayer.SendSyncJuncture(SJ_QUICKBAR_SET_SHORTCUT, ctx);
	}
	
	static void SendWeaponJamChance(DayZPlayer pPlayer, float jamChance )
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(jamChance);

		pPlayer.SendSyncJuncture(SJ_WEAPON_SET_JAMMING_CHANCE, ctx);
	}

	/*static bool ReadQuickbarSetShortcut(ParamsReadContext pCtx, out EntityAI item, out int index)
	{	
		Param2<EntityAI,int> shortcutParam = new Param2<EntityAI,int>(NULL,-1);
		if( pCtx.Read(shortcutParam))
		{
			item = shortcutParam.param1;
			index = shortcutParam.param2;
			return true;	
		}

		return false;
	}*/
	
	static void SendDeleteItem( DayZPlayer pPlayer, EntityAI item )
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(item);

		pPlayer.SendSyncJuncture(SJ_DELETE_ITEM, ctx);
	}
	
	
	//-------------------------------------------------------------
	//!
	//! BrokenLegs
	//! 
	
	static void SendBrokenLegs(DayZPlayer pPlayer, bool canPlaySound, eBrokenLegs currentState, eBrokenLegs localState)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(canPlaySound);
		ctx.Write(currentState);
		ctx.Write(localState);

		pPlayer.SendSyncJuncture(SJ_BROKEN_LEGS, ctx);
	}
	
	static bool ReadBrokenLegsParams(ParamsReadContext pCtx, out bool canPlaySound, out eBrokenLegs currentState, out eBrokenLegs localState)
	{
		if ( !pCtx.Read(canPlaySound) )
			return false; // error		
		if ( !pCtx.Read(currentState) )
			return false; // error
		if ( !pCtx.Read(localState) )
			return false;
		
		return true;
	}
	
	static void SendBrokenLegsEx(DayZPlayer pPlayer, int currentState)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(currentState);

		pPlayer.SendSyncJuncture(SJ_BROKEN_LEGS, ctx);
	}
	
	static bool ReadBrokenLegsParamsEx(ParamsReadContext pCtx, out int currentState)
	{
		if ( !pCtx.Read(currentState) )
			return false;
		
		return true;
	}
	
	//-------------------------------------------------------------
	//!
	//! Shock
	//! 
	
	static void SendShock(DayZPlayer pPlayer, float shockValue)
	{
		ScriptJunctureData ctx = new ScriptJunctureData;
		ctx.Write(shockValue);

		pPlayer.SendSyncJuncture(SJ_SHOCK, ctx);
	}
	
	static bool ReadShockParams(ParamsReadContext pCtx, out float shockValue)
	{
		if ( !pCtx.Read(shockValue) )
			return false; // error
		
		return true;
	}
}