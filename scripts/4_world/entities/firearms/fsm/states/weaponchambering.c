// load 1 bullet
class WeaponChambering_Start extends WeaponStartAction
{ 
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			m_weapon.SelectionBulletHide();
			m_weapon.ForceSyncSelectionState();
		}
	}
	
	override bool IsWaitingForActionFinish()
	{
		return true;
	}
};

class WeaponChambering_Base extends WeaponStateBase
{
	float m_damage;
	string m_type;
	string m_magazineType;
	Magazine m_srcMagazine; /// source of the cartridge
	
	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_damage))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot write m_damage for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Write(m_type))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot write m_type for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Write(m_magazineType))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot write m_magazineType for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot write m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_damage))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_damage for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Read(m_type))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_type for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Read(m_magazineType))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_magazineType for weapon=" + m_weapon);
			return false;
		}
		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};


class WeaponChambering_Cartridge extends WeaponChambering_Base
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			if (m_srcMagazine)
			{
				m_magazineType = m_srcMagazine.GetType();
				
				if (m_srcMagazine.ServerAcquireCartridge(m_damage, m_type))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - cartridge acquired: dmg=" + m_damage + " type=" + m_type); }
					m_weapon.SelectionBulletShow();
					m_weapon.EffectBulletShow( m_weapon.GetCurrentMuzzle(), m_damage, m_type);
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot take cartridge from magazine");
			}
			else
			{
				Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - no magazine to load from (m_srcMagazine=NULL)");
			}
		}
	}

	override void OnAbort(WeaponEventBase e)
	{
		int mi = m_weapon.GetCurrentMuzzle();
		
		string magazineTypeName;
		
		if (m_magazineType.Length() > 0)
			magazineTypeName = m_magazineType;
		else
			magazineTypeName = m_weapon.GetChamberAmmoTypeName(mi);
		
		if ( GetGame().IsServer() )
		{
			if (DayZPlayerUtils.HandleDropCartridge(e.m_player, m_damage, m_type, magazineTypeName))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - aborting, chambering cartridge dropped to ground"); }
			}
			else
				Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot abort removal from wpn (of old mag)");
		}
		
		m_weapon.EffectBulletHide(mi);
		m_weapon.SelectionBulletHide(); // force hide on abort
		
		m_magazineType = string.Empty;
		m_type = string.Empty;
		super.OnAbort(e);
	}

	override void OnExit (WeaponEventBase e)
	{
		int mi = m_weapon.GetCurrentMuzzle();
		//if ( m_weapon.IsChamberFiredOut(mi) )
		//	m_weapon.EjectCasing(mi);
		if (m_weapon.PushCartridgeToChamber(mi, m_damage, m_type))
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - loaded chamber"); }
		}
		else
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot load chamber chamber!"); }

		m_magazineType = string.Empty;
		m_type = string.Empty;
		super.OnExit(e);
	}
	
};

class WeaponChambering_Cartridge_ChambToMag extends WeaponChambering_Cartridge
{
	override void OnExit (WeaponEventBase e)
	{
		float ammoDamage;
 		string ammoTypeName;
		int mi = m_weapon.GetCurrentMuzzle();
		if (m_weapon.IsChamberFull(mi))
		{
			m_weapon.PopCartridgeFromChamber(mi, ammoDamage, ammoTypeName);
			if (m_weapon.PushCartridgeToInternalMagazine(mi, ammoDamage, ammoTypeName))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - loaded chamber"); }
			}
			else
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot load chamber chamber!"); }
		}
		super.OnExit(e);
	}
}

//-----------MAGNUM-----------
class WeaponChambering_MultiMuzzleMagnum extends WeaponChambering_Cartridge
{
	override bool IsWaitingForActionFinish () { return false; }
	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);

		/*for(int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
		{
			if(!m_weapon.IsChamberFull(i))
			{
				m_weapon.ShowBullet(i);
				m_weapon.EffectBulletShow(i, m_damage, m_type);
				return;
			}
		}*/
	}
	
	override void OnExit(WeaponEventBase e)
	{
		m_weapon.SelectionBulletHide();
		int muzzle = m_weapon.GetCurrentMuzzle();
		
		if (!m_weapon.IsChamberFull(muzzle))
		{
			if (m_weapon.PushCartridgeToChamber(muzzle, m_damage, m_type))
			{
				Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(m_weapon.GetAttachmentByType(Magnum_Cylinder));

				if (cylinder)
				{
					string bullet = "bullet";
					string bullet_nose = "bullet_nose";
					
					if (muzzle > 0)
					{
						bullet = string.Format("bullet_" + ( muzzle + 1 ));
						bullet_nose = string.Format("bullet_nose_" + ( muzzle + 1 ));
					}
					cylinder.ShowSelection(bullet);
					cylinder.ShowSelection(bullet_nose);
				}
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_MultiMuzzleMagnum, ok - loaded chamber"); }
			}
			else
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_MultiMuzzleMagnum, error - cannot load chamber chamber!"); }
			m_type = string.Empty;
			return;
		}
		else
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_MultiMuzzleMagnum, error - cannot load chamber chamber!"); }
		
		//super.OnExit(e);
	}
}



//----------------------------

class WeaponChambering_MultiMuzzle extends WeaponChambering_Cartridge
{
	override bool IsWaitingForActionFinish () { return true; }
	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);

		for(int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
		{
			if(!m_weapon.IsChamberFull(i))
			{
				m_weapon.ShowBullet(i);
				m_weapon.EffectBulletShow(i, m_damage, m_type);
				return;
			}
		}
	}
	
	override void OnExit (WeaponEventBase e)
	{
		for(int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
		{
			if(!m_weapon.IsChamberFull(i))
			{
				if (m_weapon.PushCartridgeToChamber(i, m_damage, m_type))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - loaded chamber"); }
				}
				else
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot load chamber chamber!"); }
				m_type = string.Empty;
				return;
			}
		}
		
		super.OnExit(e);
	}
}

class WeaponChambering_MultiMuzzle_W4T extends WeaponChambering_MultiMuzzle
{
	override bool IsWaitingForActionFinish () { return true; }
};


class WeaponChambering_Cartridge_InnerMag extends WeaponChambering_Base
{
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			if (m_srcMagazine)
			{
				m_magazineType = m_srcMagazine.GetType();
				
				if (m_srcMagazine.ServerAcquireCartridge(m_damage, m_type))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, ok - cartridge acquired: dmg=" + m_damage + " type=" + m_type); }		
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, error - cannot take cartridge from magazine");
			}
			else
			{
				Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, error - no magazine to load from (m_srcMagazine=NULL)");
			}
			
			m_weapon.SelectionBulletShow();
			m_weapon.EffectBulletShow(m_weapon.GetCurrentMuzzle(),m_damage,m_type);
		}
	}

	override void OnAbort(WeaponEventBase e)
	{
		int mi = m_weapon.GetCurrentMuzzle();
		string magazineTypeName;
		
		if (m_magazineType.Length() > 0)
			magazineTypeName = m_magazineType;
		else
			magazineTypeName = m_weapon.GetChamberAmmoTypeName(mi);
		
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			if (DayZPlayerUtils.HandleDropCartridge(e.m_player, m_damage, m_type, magazineTypeName))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, ok - aborting, chambering cartridge dropped to ground"); }
			}
			else
				Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, error - cannot abort removal from wpn (of old mag)");
		}

		m_weapon.SelectionBulletHide(); // force hide on abort
		m_weapon.EffectBulletHide(m_weapon.GetCurrentMuzzle());

		m_magazineType = string.Empty;
		m_type = string.Empty;
		super.OnAbort(e);
	}

	override void OnExit (WeaponEventBase e)
	{
		float ammoDamage;
 		string ammoTypeName;
		int mi = m_weapon.GetCurrentMuzzle();
		if (!m_weapon.IsInternalMagazineFull(mi))
		{
			if (m_weapon.PushCartridgeToInternalMagazine(mi, m_damage, m_type))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, ok - loaded chamber"); }
			}
			else
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge_InnerMag, error - cannot load chamber chamber!"); }
		}
		m_magazineType = string.Empty;
		super.OnExit(e);
	}
};

class WeaponChambering_W4T extends WeaponStateBase
{
	override bool IsWaitingForActionFinish () { return true; }
};

class WeaponChambering extends WeaponStateBase
{
	WeaponActions m_action;
	int m_actionType;
	Magazine m_srcMagazine; /// source of the cartridge
	ref InventoryLocation m_srcMagazinePrevLocation;

	ref WeaponStateBase m_start;
	ref WeaponEjectCasing m_eject;
	ref WeaponChambering_Cartridge m_chamber;
	ref WeaponChambering_W4T m_w4t;

	void WeaponChambering (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_actionType);
		m_chamber = new WeaponChambering_Cartridge(m_weapon, this);
		m_w4t = new WeaponChambering_W4T(m_weapon, this);
		m_eject = new WeaponEjectCasing(m_weapon, this);
		// events
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventAnimBulletInChamber __bc_ = new WeaponEventAnimBulletInChamber;
		WeaponEventAnimBulletShow  __bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletEject	__be_ = new WeaponEventAnimBulletEject;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start  , __be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_start  , __bs_, m_chamber));			
		m_fsm.AddTransition(new WeaponTransition(m_eject  , __bs_, m_chamber));		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bc_, m_w4t));
		m_fsm.AddTransition(new WeaponTransition(m_w4t    , _fin_, null));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_eject  , _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_start  , _fin_, null));	

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		if (e != NULL)
		{
			m_srcMagazine = e.m_magazine;
			if (m_srcMagazine != NULL)
			{
				InventoryLocation newSrc = new InventoryLocation;
				m_srcMagazine.GetInventory().GetCurrentInventoryLocation(newSrc);
				
				m_srcMagazinePrevLocation = newSrc;
			
				// move to LH
				InventoryLocation lhand = new InventoryLocation;
				lhand.SetAttachment(e.m_player, m_srcMagazine, InventorySlots.LEFTHAND);
				if (GameInventory.LocationSyncMoveEntity(newSrc, lhand))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - ammo pile removed from inv (inv->LHand)"); }
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, error - cannot remove ammo pile from inv");
				
				m_chamber.m_srcMagazine = m_srcMagazine;
			} 
			else
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering m_srcMagazine = NULL"); }
			}
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString()); }
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

	override void OnAbort (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(),leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort"); }
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - no inventory space for ammo pile - dropped to ground - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - ammo pile removed from left hand (LHand->inv) - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, error - cannot remove ammo pile from wpn - abort");
					}
				}
			}
		}
		
		super.OnAbort(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}

	override void OnExit (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit"); }
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - no inventory space for ammo pile - dropped to ground - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, ok - ammo pile removed from left hand (LHand->inv) - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering, error - cannot remove ammo pile from wpn - exit");
					}
				}
			}
		}
		
		super.OnExit(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}

	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationWriteToContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.SaveCurrentFSMState: cannot write m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationReadFromContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering.LoadCurrentFSMState: cannot read m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
class WeaponEndAction extends WeaponStartAction
{
	override bool IsWaitingForActionFinish()
	{
		return true;
	}
}

class ChamberMultiBullet extends WeaponStateBase
{
	WeaponActions m_action;
	int m_startActionType;
	int m_endActionType;
	Magazine m_srcMagazine; /// source of the cartridge
	ref InventoryLocation m_srcMagazinePrevLocation;

	ref WeaponStateBase m_start;
	ref WeaponEjectCasingMultiMuzzle m_eject;
	ref WeaponChambering_Base m_chamber;
	ref WeaponChambering_Base m_chamber_end;
	ref LoopedChambering_Wait4ShowBullet2 m_w4sb2;
	ref WeaponEndAction m_endLoop;
	ref BulletShow_W4T m_showB;
	ref BulletShow2_W4T m_showB2;

	void ChamberMultiBullet (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int startActionType = -1, int endActionType = -1)
	{
		m_action = action;
		m_startActionType = startActionType;
		m_endActionType = endActionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_startActionType);
		m_eject = new WeaponEjectCasingMultiMuzzle(m_weapon, this);
		m_chamber = new WeaponChambering_MultiMuzzle_W4T(m_weapon, this);
		m_chamber_end = new WeaponChambering_MultiMuzzle_W4T(m_weapon, this);
		m_w4sb2 = LoopedChambering_Wait4ShowBullet2(m_weapon, this);
		m_showB = new BulletShow_W4T(m_weapon, this);
		m_showB2= new BulletShow2_W4T(m_weapon, this);
		
		m_endLoop = new WeaponEndAction(m_weapon, this, m_action, m_endActionType); // @NOTE: termination playing action - dummy?
		// events
		WeaponEventBase							_fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventContinuousLoadBulletStart	__lS_ = new WeaponEventContinuousLoadBulletStart;
		WeaponEventContinuousLoadBulletEnd		__lE_ = new WeaponEventContinuousLoadBulletEnd;
		WeaponEventAnimBulletShow				__bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletShow2				_bs2_ = new WeaponEventAnimBulletShow2;
		WeaponEventAnimBulletHide				__bh_ = new WeaponEventAnimBulletHide;
		WeaponEventAnimBulletEject				__be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimBulletInChamber			__bc_ = new WeaponEventAnimBulletInChamber;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start  , __be_, m_eject));
		
		m_fsm.AddTransition(new WeaponTransition(m_start  , __bs_, m_chamber));
		m_fsm.AddTransition(new WeaponTransition(m_eject  , __bs_, m_chamber));
		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bc_, m_w4sb2, NULL, new GuardAnd(new GuardAnd(new WeaponGuardHasAmmoInLoopedState(m_chamber), new WeaponGuardChamberMultiHasRoomBulltet(m_weapon)),new WeaponGuardWeaponManagerWantContinue())));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bc_, m_endLoop));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, __bs_, m_chamber));
		
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2  , _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(m_chamber  , _fin_, NULL));
		m_fsm.AddTransition(new WeaponTransition(m_endLoop  , _fin_, NULL));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_eject  , _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_start  , _fin_, null));	
		
		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry (WeaponEventBase e)
	{
		if (e != NULL)
		{
			m_srcMagazine = e.m_magazine;
			if (m_srcMagazine != NULL)
			{
				InventoryLocation newSrc = new InventoryLocation;
				m_srcMagazine.GetInventory().GetCurrentInventoryLocation(newSrc);
				
				m_srcMagazinePrevLocation = newSrc;
			
				// move to LH
				InventoryLocation lhand = new InventoryLocation;
				lhand.SetAttachment(e.m_player, m_srcMagazine, InventorySlots.LEFTHAND);
				if (GameInventory.LocationSyncMoveEntity(newSrc, lhand))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - ammo pile removed from inv (inv->LHand)"); }
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, error - cannot remove ammo pile from inv");
				
				m_chamber.m_srcMagazine = m_srcMagazine;
			} 
			else
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet m_srcMagazine = NULL"); }
			}
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString()); }
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}
	override void OnExit (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit"); }
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - no inventory space for ammo pile - dropped to ground - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - ammo pile removed from left hand (LHand->inv) - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, error - cannot remove ammo pile from wpn - exit");
					}
				}
			}
		}
		
		super.OnExit(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
	override void OnAbort (WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if(leftHandIl.IsValid())
			{
				if(m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if(vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort"); }
								done = true;
							}
						}
					}
				}
				
				if( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if(!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - no inventory space for ammo pile - dropped to ground - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, ok - ammo pile removed from left hand (LHand->inv) - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet, error - cannot remove ammo pile from wpn - abort");
					}
				}
			}
		}
		
		super.OnAbort(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
	
	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationWriteToContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet.SaveCurrentFSMState: cannot write m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}

	override bool LoadCurrentFSMState (ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationReadFromContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " ChamberMultiBullet.LoadCurrentFSMState: cannot read m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};
//------------------------------------------------------
//------------------ROTATE------------------------------
//------------------------------------------------------
class WeaponCylinderRotate extends WeaponStateBase
{	
	bool FindNextFreeMuzzle(int currentMuzzle, out int nextMuzzle)
	{
		nextMuzzle = currentMuzzle;
		int nMuzzles = m_weapon.GetMuzzleCount();
		
		for (int i = 0; i < nMuzzles; ++i)
		{
			--nextMuzzle;
			nextMuzzle = Math.WrapInt(nextMuzzle, 0, nMuzzles);
			if (m_weapon.IsChamberEmpty(nextMuzzle))
				return true;
		}
		
		return false;
	}
	
	override void OnEntry(WeaponEventBase e)
	{
		int nextMuzzle;
		if (FindNextFreeMuzzle(m_weapon.GetCurrentMuzzle(), nextMuzzle))
		{
			Magnum_Base magnum = Magnum_Base.Cast(m_weapon);
			magnum.SetCylinderRotationAnimationPhase(magnum.GetCylinderRotation(nextMuzzle));
			m_weapon.SetCurrentMuzzle(nextMuzzle);
		}
		else
		{
			Print("WTF");
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}

};


//------------------------------------------------------
//------------------MAGNUM------------------------------
//------------------------------------------------------
class WeaponMagnumChambering extends WeaponStateBase
{
	WeaponActions m_action;
	int m_startActionType;
	int m_endActionType;
	Magazine m_srcMagazine; /// source of the cartridge
	ref InventoryLocation m_srcMagazinePrevLocation;

	ref WeaponStateBase m_start;
	ref WeaponEjectAllMuzzles m_eject;
	ref WeaponCylinderRotate m_rotate;
	ref WeaponChambering_Base m_chamber;
	ref LoopedChambering_Wait4ShowBullet2 m_w4sb2;
	ref WeaponStartAction m_endLoop;
	ref BulletHide_W4T m_hideB;

	void WeaponMagnumChambering(Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int startActionType = -1, int endActionType = -1)
	{
		m_action = action;
		m_startActionType = startActionType;
		m_endActionType = endActionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_startActionType);
		m_eject = new WeaponEjectAllMuzzles(m_weapon, this);
		m_rotate = new WeaponCylinderRotate(m_weapon, this);
		m_chamber = new WeaponChambering_MultiMuzzleMagnum(m_weapon, this);
		m_w4sb2 = LoopedChambering_Wait4ShowBullet2(m_weapon, this);
		m_hideB = new BulletHide_W4T(m_weapon, this);
		m_endLoop = new LoopedChambering_EndLoop(m_weapon, this, m_action, m_endActionType); // @NOTE: termination playing action - dummy?
		// events
		WeaponEventBase							_fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventContinuousLoadBulletStart	__lS_ = new WeaponEventContinuousLoadBulletStart;
		WeaponEventContinuousLoadBulletEnd		__lE_ = new WeaponEventContinuousLoadBulletEnd;
		WeaponEventCylinderRotate				__cr_ = new WeaponEventCylinderRotate;
		WeaponEventAnimBulletShow				__bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletHide				__bh_ = new WeaponEventAnimBulletHide;
		WeaponEventAnimBulletEject				__be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimBulletInMagazine			__bM_ = new WeaponEventAnimBulletInMagazine;
		WeaponEventAnimBulletShow2				_bs2_ = new WeaponEventAnimBulletShow2;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start,	__be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_start,	__cr_, m_rotate));
		
		m_fsm.AddTransition(new WeaponTransition(m_eject,	__cr_, m_rotate));			
		m_fsm.AddTransition(new WeaponTransition(m_rotate,	__be_, m_eject));
		
		m_fsm.AddTransition(new WeaponTransition(m_eject, __bs_, m_chamber));
		m_fsm.AddTransition(new WeaponTransition(m_rotate, __bs_, m_chamber));		
		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_w4sb2, null, new GuardAnd(new GuardAnd(new WeaponGuardHasAmmoInLoopedState(m_chamber), new WeaponGuardChamberMultiHasRoomBulltetIgnoreLast(m_weapon)),new WeaponGuardWeaponManagerWantContinue())));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_endLoop));
		//m_fsm.AddTransition(new WeaponTransition(m_rotate, __bh_, m_chamber));
		//m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	__bh_, m_hideB));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	__cr_, m_rotate));
		
		m_fsm.AddTransition(new WeaponTransition(m_endLoop, _fin_, null));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	_fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_rotate,  _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_eject  , _fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_start  , _fin_, null));		

		m_fsm.SetInitialState(m_start);
	}

	override void OnEntry(WeaponEventBase e)
	{
		if (e != NULL)
		{
			
			m_srcMagazine = e.m_magazine;
			if (m_srcMagazine != NULL)
			{
				InventoryLocation newSrc = new InventoryLocation;
				m_srcMagazine.GetInventory().GetCurrentInventoryLocation(newSrc);
				
				m_srcMagazinePrevLocation = newSrc;
		
				// move to LH
				InventoryLocation lhand = new InventoryLocation;
				lhand.SetAttachment(e.m_player, m_srcMagazine, InventorySlots.LEFTHAND);
				if (GameInventory.LocationSyncMoveEntity(newSrc, lhand))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - ammo pile removed from inv (inv->LHand)"); }
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, error - cannot remove ammo pile from inv");
				
				m_chamber.m_srcMagazine = m_srcMagazine;
			} 
			else
			{
				Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering m_srcMagazine = NULL");
			}
		}
		else
		{
			Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString());
		}

		super.OnEntry(e); // @NOTE: super at the end (prevent override from submachine start)
	}
	
	override void OnExit(WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if (leftHandIl.IsValid())
			{
				if (m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if ( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if ( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit"); }
								done = true;
							}
						}
					}
				}
				
				if ( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if (!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - no inventory space for ammo pile - dropped to ground - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - ammo pile removed from left hand (LHand->inv) - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, error - cannot remove ammo pile from wpn - exit");
					}
				}
			}
		}
		
		super.OnExit(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
	
	override void OnAbort(WeaponEventBase e)
	{
		bool done = false;
		if (m_srcMagazine)
		{
			e.m_player.GetInventory().ClearInventoryReservationEx( m_srcMagazine , m_srcMagazinePrevLocation );
			
			InventoryLocation leftHandIl = new InventoryLocation;
			m_srcMagazine.GetInventory().GetCurrentInventoryLocation(leftHandIl);
			if (leftHandIl.IsValid())
			{
				if (m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ )
					{
						if ( GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
						{
							if ( GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation) )
							{
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort"); }
								done = true;
							}
						}
					}
				}
				
				if ( !done)
				{
					InventoryLocation il = new InventoryLocation;
					e.m_player.GetInventory().FindFreeLocationFor( m_srcMagazine, FindInventoryLocationType.CARGO, il );
			
					if (!il || !il.IsValid())
					{
						if (DayZPlayerUtils.HandleDropMagazine(e.m_player, m_srcMagazine))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - no inventory space for ammo pile - dropped to ground - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering, ok - ammo pile removed from left hand (LHand->inv) - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from wpn - abort");
					}
				}
			}
		}
		
		super.OnAbort(e);
		m_srcMagazine = NULL;
		m_chamber.m_srcMagazine = NULL;
		m_srcMagazinePrevLocation = NULL;
	}
	
	override bool SaveCurrentFSMState(ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationWriteToContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering.SaveCurrentFSMState: cannot write m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		
		return true;
	}

	override bool LoadCurrentFSMState(ParamsReadContext ctx, int version)
	{
		if (!super.LoadCurrentFSMState(ctx, version))
			return false;

		if (!ctx.Read(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationReadFromContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponMagnumChambering.LoadCurrentFSMState: cannot read m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};