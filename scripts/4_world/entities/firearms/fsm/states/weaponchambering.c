// load 1 bullet
class WeaponChambering_Start extends WeaponStartAction
{ 
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		if (e)
		{
			m_weapon.EffectBulletHide(m_weapon.GetCurrentMuzzle());
			m_weapon.SelectionBulletHide();
			//m_weapon.ForceSyncSelectionState();
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
	
	bool ShowBullet(int muzzleIndex)
	{
		if (m_srcMagazine)
		{
			if (m_srcMagazine.GetCartridgeAtIndex(0, m_damage, m_type))
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering ShowBullet, ok - cartridge shown: dmg=" + m_damage + " type=" + m_type); }
				m_weapon.SelectionBulletShow();
				m_weapon.ShowBullet(muzzleIndex);
				m_weapon.EffectBulletShow(muzzleIndex, m_damage, m_type);
				return true;
			}
			else
			{
				Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering ShowBullet, error - cannot take cartridge from magazine");
			}
		}
		else
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering ShowBullet, error - no magazine to load from (m_srcMagazine=NULL)");
		}
		
		return false;
	}
	
	void HideBullet(int muzzleIndex)
	{
		m_weapon.EffectBulletHide(muzzleIndex);
		m_weapon.SelectionBulletHide();
	}
	
	void OpenBolt()
	{
		m_weapon.SetWeaponOpen(true);
	}
	
	void CloseBolt()
	{
		m_weapon.SetWeaponOpen(false);
	}
	
	bool AcquireCartridgeFromMagazine()
	{
		m_magazineType = m_srcMagazine.GetType();
		if (m_srcMagazine.ServerAcquireCartridge(m_damage, m_type))
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering AcquireCartridgeFromMagazine, ok - bullet acquire " + m_type); }
			return true;
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering AcquireCartridgeFromMagazine, error -Cannot acquire bullet from magazine!"); }
			m_magazineType = string.Empty;
			m_type = string.Empty;
		}
		return false;
	}
	
	bool DropBullet(WeaponEventBase e)
	{
		if ( g_Game.IsServer() )
		{
			if(m_magazineType.Length() > 0 && m_type.Length() > 0)
			{
				if (DayZPlayerUtils.HandleDropCartridge(e.m_player, m_damage, m_type, m_magazineType))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering DropBullet, ok - " + m_type + " - dropped to ground"); }
					return true;
				}
				else
				{
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering DropBullet, error - cannot drop " + m_type + " - lost)");
			
				}
			}
			else
			{
				Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering DropBullet, error - magazine or bullet type is not set");
			}
			return false;
		}
		return true;
	}
	
	bool PushBulletToChamber(int muzzleIndex)
	{
		if(m_type.Length() > 0)
		{
			if (!m_weapon.IsChamberFull(muzzleIndex))
			{
				if (m_weapon.PushCartridgeToChamber(muzzleIndex, m_damage, m_type))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToChamber, ok - " + m_type + " - chamber"); }
					return true;
				}
				else
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToChamber, error - cannot load " + m_type + " to chamber!"); }
				}
			}
			else
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToChamber, error - chamber is already full!"); }
			}
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToChamber, error - bullet type is not set!"); }
		}
		return false;
	}
	
	bool PushBulletToInternalMagazine(int muzzleIndex)
	{
		if(m_type.Length() > 0)
		{
			if (!m_weapon.IsInternalMagazineFull(muzzleIndex))
			{
				if (m_weapon.PushCartridgeToInternalMagazine(muzzleIndex, m_damage, m_type))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToInternalMagazine, ok - " + m_type + " - internal magazine"); }
					return true;
				}
				else
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToInternalMagazine, error - " + m_type + " cannot load to internal magazine!"); }
				}
			}
			else
			{
				if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToInternalMagazine, error - " + m_type + " cannot load to internal magazine(full)!"); }
			}

		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletToInternalMagazine, error - bullet type is not set!"); }
		}
		return false;
	}
	
	bool PushBulletFromChamberToInternalMagazine(int muzzleIndex)
	{
		float ammoDamage;
 		string ammoTypeName;

		if (m_weapon.IsChamberFull(muzzleIndex))
		{
			if(m_weapon.PopCartridgeFromChamber(muzzleIndex, ammoDamage, ammoTypeName))
			{
				if (m_weapon.PushCartridgeToInternalMagazine(muzzleIndex, ammoDamage, ammoTypeName))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering PushBulletFromChamberToInnerMagazine, ok - " + ammoTypeName + " - chamber -> internal magazine"); }
					return true;
				}
				else
				{
					if (m_weapon.PushCartridgeToChamber(muzzleIndex, ammoDamage, ammoTypeName))
					{
						if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + "WeaponChambering PushBulletFromChamberToInnerMagazine, error - " + ammoTypeName + " - chamber"); }
					}
					else
					{
						if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + "WeaponChambering PushBulletFromChamberToInnerMagazine, error - " + ammoTypeName + " - lost"); } 
					}
				}	
			}
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + "WeaponChambering PushBulletFromChamberToInnerMagazine, error - cannot pop bullet from chamber"); }
		}
		else
		{
			if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + "WeaponChambering PushBulletFromChamberToInnerMagazine, ok - not bullet in chamber"); }
			return true;
		}
		return false;
	}
};

class WeaponChambering_Preparation extends WeaponChambering_Base
{
	override bool IsWaitingForActionFinish() { return false; }
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		
		int mi = m_weapon.GetCurrentMuzzle();
		ShowBullet(mi);
	}

	override void OnAbort(WeaponEventBase e)
	{
		super.OnAbort(e);
		
		int mi = m_weapon.GetCurrentMuzzle();
		if(AcquireCartridgeFromMagazine())
		{
			DropBullet(e);
		}
		HideBullet(mi);
		
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
}

class WeaponChambering_Cartridge extends WeaponChambering_Preparation
{
	override bool IsWaitingForActionFinish() { return false; }
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e); 
		OpenBolt();
	}
	
	override void OnExit(WeaponEventBase e)
	{
		super.OnExit(e);
		
		int mi = m_weapon.GetCurrentMuzzle();	
		if(AcquireCartridgeFromMagazine())
		{
			if(!PushBulletToChamber(mi))
			{
				DropBullet(e);
			}
		}
		
		m_weapon.SetCharged(true);
		CloseBolt();
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
};



class WeaponChambering_Cartridge_ChambToMag extends WeaponChambering_Preparation
{
	override bool IsWaitingForActionFinish() { return true; }
	
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
		
		int mi = m_weapon.GetCurrentMuzzle();
		if(PushBulletFromChamberToInternalMagazine(mi))
		{
			if(AcquireCartridgeFromMagazine())
			{
				if(!PushBulletToChamber(mi))
				{
					DropBullet(e);
				}
			}
		}
		
		m_weapon.SetCharged(true);
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
}

//-----------MAGNUM-----------
class WeaponChambering_MultiMuzzleMagnum extends WeaponChambering_Base
{
	override bool IsWaitingForActionFinish() { return false; }

	override void OnAbort(WeaponEventBase e)
	{
		super.OnAbort(e);
		
		int mi = m_weapon.GetCurrentMuzzle();
		if(AcquireCartridgeFromMagazine())
		{
			DropBullet(e);
		}
		
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
	override void OnExit(WeaponEventBase e)
	{
		super.OnExit(e);
		
		m_weapon.SelectionBulletHide();
		int mi = m_weapon.GetCurrentMuzzle();
		
		if(AcquireCartridgeFromMagazine())
		{
			if(PushBulletToChamber(mi))
			{
				Magnum_Cylinder cylinder = Magnum_Cylinder.Cast(m_weapon.GetAttachmentByType(Magnum_Cylinder));
					
				if (cylinder)
				{
					string bullet = "bullet";
					string bullet_nose = "bullet_nose";
					
					if (mi > 0)
					{
						bullet = string.Format("bullet_" + ( mi + 1 ));
						bullet_nose = string.Format("bullet_nose_" + ( mi + 1 ));
					}
					cylinder.ShowSelection(bullet);
					cylinder.ShowSelection(bullet_nose);
				}
			}
			else
			{
				DropBullet(e);
			}
		}
		
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
}



//----------------------------

class WeaponChambering_MultiMuzzle extends WeaponChambering_Base
{
	override bool IsWaitingForActionFinish () { return true; }
	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);

		for(int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
		{
			if(!m_weapon.IsChamberFull(i))
			{
				ShowBullet(i);
				return;
			}
		}
		if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_MultiMuzzle OnEntry, error - all chambers full!"); }
	}
	
	override void OnAbort(WeaponEventBase e)
	{
		super.OnAbort(e);
		
		int mi = m_weapon.GetCurrentMuzzle();
		if(AcquireCartridgeFromMagazine())
		{
			DropBullet(e);
		}
		HideBullet(mi);
		
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
	
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
		
		for(int i = 0; i < m_weapon.GetMuzzleCount(); i++ )
		{
			if(!m_weapon.IsChamberFull(i))
			{
				if(AcquireCartridgeFromMagazine())
				{					
					if (m_weapon.PushCartridgeToChamber(i, m_damage, m_type))
					{
						if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, ok - loaded chamber"); }
					}
					else
					{
						if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " WeaponChambering_Cartridge, error - cannot load chamber chamber!"); }
						DropBullet(e);
					}
	
					m_type = string.Empty;
					
					return;
				}
			}
		}
	}
}

/*class WeaponChambering_MultiMuzzle_W4T extends WeaponChambering_MultiMuzzle
{
	override bool IsWaitingForActionFinish () { return true; }
};*/
	
class WeaponChambering_InternalMagazine_OnExit extends WeaponChambering_Preparation
{
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
		
		int mi = m_weapon.GetCurrentMuzzle();	
		if(AcquireCartridgeFromMagazine())
		{
			if(!PushBulletToInternalMagazine(mi))
			{
				DropBullet(e);
			}
		}
		
		m_weapon.SetCharged(true);
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
};

class WeaponChambering_Chamber_OnEntry extends WeaponChambering_Base
{
	override bool IsWaitingForActionFinish() {return true;}
	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);
		
		int mi = m_weapon.GetCurrentMuzzle();	
		if(AcquireCartridgeFromMagazine())
		{
			if(!PushBulletToChamber(mi))
			{
				DropBullet(e);
			}
		}
		
		m_weapon.SetCharged(true);
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
};
	
class WeaponChambering_InternalMagazine_OnEntry extends WeaponChambering_Base
{
	override bool IsWaitingForActionFinish() {return true;}		
	override void OnEntry(WeaponEventBase e)
	{
		super.OnEntry(e);
		
		int mi = m_weapon.GetCurrentMuzzle();	
		if(AcquireCartridgeFromMagazine())
		{
			if(!PushBulletToInternalMagazine(mi))
			{
				DropBullet(e);
			}
		}
		
		m_weapon.SetCharged(true);
		m_magazineType = string.Empty;
		m_type = string.Empty;
	}
}

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
	ref WeaponCharging_CK m_onCK;

	void WeaponChambering (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_action = action;
		m_actionType = actionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_actionType);
		m_chamber = new WeaponChambering_Cartridge(m_weapon, this);
		m_w4t = new WeaponChambering_W4T(m_weapon, this);
		m_eject = new WeaponEjectCasing(m_weapon, this);
		m_onCK = new WeaponCharging_CK(m_weapon, this);
		// events
		WeaponEventBase _fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventAnimBulletInChamber __bc_ = new WeaponEventAnimBulletInChamber;
		WeaponEventAnimBulletShow  __bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletEject	__be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimCocked __ck_ = new WeaponEventAnimCocked;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start  , __be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_start  , __ck_, m_onCK));
		m_fsm.AddTransition(new WeaponTransition(m_start  , __bs_, m_chamber));
		m_fsm.AddTransition(new WeaponTransition(m_onCK  , __be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_onCK  , __bs_, m_chamber));	
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
			if (leftHandIl.IsValid())
			{
				if (m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(),leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ)
					{
						if (GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
						{
							if (GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
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
			if (leftHandIl.IsValid())
			{
				if (m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ)
					{
						if (GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
						{
							if (GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
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
		m_chamber = new WeaponChambering_MultiMuzzle(m_weapon, this);
		m_w4sb2 = new LoopedChambering_Wait4ShowBullet2(m_weapon, this);
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
			if (leftHandIl.IsValid())
			{
				if (m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ)
					{
						if (GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
						{
							if (GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
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
			if (leftHandIl.IsValid())
			{
				if (m_srcMagazinePrevLocation && m_srcMagazinePrevLocation.IsValid())
				{
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ)
					{
						if (GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
						{
							if (GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
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
		
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_w4sb2, null, new GuardAnd(new GuardAnd(new WeaponGuardHasAmmoInLoopedState(m_chamber), new WeaponGuardChamberMultiHasRoomBulltet(m_weapon)),new WeaponGuardWeaponManagerWantContinue())));
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
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ)
					{
						if (GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
						{
							if (GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
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
					if (vector.DistanceSq(m_srcMagazinePrevLocation.GetPos(), leftHandIl.GetPos()) < WeaponManager.MAX_DROP_MAGAZINE_DISTANCE_SQ)
					{
						if (GameInventory.LocationCanMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
						{
							if (GameInventory.LocationSyncMoveEntity(leftHandIl,m_srcMagazinePrevLocation))
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