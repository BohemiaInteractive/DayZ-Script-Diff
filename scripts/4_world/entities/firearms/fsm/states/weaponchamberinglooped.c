// load x bullets
class LoopedChambering_EndLoop extends WeaponStartAction
{
	override bool IsWaitingForActionFinish () { return true; }
};

class LoopedChambering_Wait4ShowBullet2 extends WeaponStateBase
{ 
	override bool IsWaitingForActionFinish () { return false; }
};

class LoopedChambering extends WeaponStateBase
{
	WeaponActions m_action;
	int m_startActionType;
	int m_endActionType;
	Magazine m_srcMagazine; /// source of the cartridge
	ref InventoryLocation m_srcMagazinePrevLocation;

	ref WeaponStateBase m_start;
	ref WeaponEjectCasing m_eject;
	ref WeaponChambering_Base m_chamber;
	ref LoopedChambering_Wait4ShowBullet2 m_w4sb2;
	ref WeaponStartAction m_endLoop;
	ref BulletHide_W4T m_hideB;

	void LoopedChambering (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int startActionType = -1, int endActionType = -1)
	{
		m_action = action;
		m_startActionType = startActionType;
		m_endActionType = endActionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_startActionType);
		m_eject = new WeaponEjectCasing(m_weapon, this);
		m_chamber = new WeaponChambering_Cartridge_ChambToMag(m_weapon, this);
		m_w4sb2 = LoopedChambering_Wait4ShowBullet2(m_weapon, this);
		m_hideB = new BulletHide_W4T(m_weapon, this);
		m_endLoop = new LoopedChambering_EndLoop(m_weapon, this, m_action, m_endActionType); // @NOTE: termination playing action - dummy?
		// events
		WeaponEventBase							_fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventContinuousLoadBulletStart	__lS_ = new WeaponEventContinuousLoadBulletStart;
		WeaponEventContinuousLoadBulletEnd		__lE_ = new WeaponEventContinuousLoadBulletEnd;
		WeaponEventAnimBulletShow				__bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletHide				__bh_ = new WeaponEventAnimBulletHide;
		WeaponEventAnimBulletEject				__be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimBulletInMagazine			__bM_ = new WeaponEventAnimBulletInMagazine;
		WeaponEventAnimBulletShow2				_bs2_ = new WeaponEventAnimBulletShow2;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start,	__be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_start,	__bs_, m_chamber));
		m_fsm.AddTransition(new WeaponTransition(m_eject,	__bs_, m_chamber));
		

		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_w4sb2, NULL, new GuardAnd(new GuardAnd(new WeaponGuardHasAmmoInLoopedState(m_chamber), new WeaponGuardChamberHasRoomForMoreThanOne(m_weapon)),new WeaponGuardWeaponManagerWantContinue())));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_endLoop));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	__bh_, m_hideB));
		m_fsm.AddTransition(new WeaponTransition(m_hideB, 	__bs_, m_chamber));

		m_fsm.AddTransition(new WeaponTransition(m_endLoop, _fin_, NULL));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_hideB, 	_fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	_fin_, null));
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
				m_weapon.SelectionBulletHide();
				InventoryLocation newSrc = new InventoryLocation;
				m_srcMagazine.GetInventory().GetCurrentInventoryLocation(newSrc);
				
				m_srcMagazinePrevLocation = newSrc;
		
				// move to LH
				InventoryLocation lhand = new InventoryLocation;
				lhand.SetAttachment(e.m_player, m_srcMagazine, InventorySlots.LEFTHAND);
				if (GameInventory.LocationSyncMoveEntity(newSrc, lhand))
				{
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from inv (inv->LHand)"); }
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from inv");
				
				m_chamber.m_srcMagazine = m_srcMagazine;
			} 
			else
			{
				Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering m_srcMagazine = NULL");
			}
		}
		else
		{
			Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString());
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
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit"); }
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
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - no inventory space for ammo pile - dropped to ground - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand (LHand->inv) - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot remove ammo pile from wpn - exit");
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
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort"); }
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
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - no inventory space for ammo pile - dropped to ground - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering, ok - ammo pile removed from left hand (LHand->inv) - abort"); }
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
	
	override bool SaveCurrentFSMState (ParamsWriteContext ctx)
	{
		if (!super.SaveCurrentFSMState(ctx))
			return false;

		if (!ctx.Write(m_srcMagazine))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationWriteToContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.SaveCurrentFSMState: cannot write m_srcMagazinePrevLocation for weapon=" + m_weapon);
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
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationReadFromContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChambering.LoadCurrentFSMState: cannot read m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};

class LoopedChamberingEjectLast extends WeaponStateBase
{
	WeaponActions m_action;
	int m_startActionType;
	int m_endActionType;
	Magazine m_srcMagazine; /// source of the cartridge
	ref InventoryLocation m_srcMagazinePrevLocation;

	ref WeaponStateBase m_start;
	ref WeaponEjectCasing m_eject;
	ref WeaponChambering_Base m_chamber;
	ref LoopedChambering_Wait4ShowBullet2 m_w4sb2;
	ref WeaponStartAction m_endLoop;
	ref BulletHide_W4T m_hideB;
	ref WeaponChamberFromInnerMag_W4T m_chamberFromInnerMag;

	void LoopedChamberingEjectLast (Weapon_Base w = NULL, WeaponStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int startActionType = -1, int endActionType = -1)
	{
		m_action = action;
		m_startActionType = startActionType;
		m_endActionType = endActionType;

		// setup nested state machine
		m_start = new WeaponChambering_Start(m_weapon, this, m_action, m_startActionType);
		m_eject = new WeaponEjectCasing(m_weapon, this);
		m_chamber = new WeaponChambering_Cartridge_InnerMag(m_weapon, this);
		m_w4sb2 = new LoopedChambering_Wait4ShowBullet2(m_weapon, this);
		m_hideB = new BulletHide_W4T(m_weapon, this);
		m_chamberFromInnerMag = new WeaponChamberFromInnerMag_W4T(m_weapon, this);
		m_endLoop = new LoopedChambering_EndLoop(m_weapon, this, m_action, m_endActionType); // @NOTE: termination playing action - dummy?
		// events
		WeaponEventBase							_fin_ = new WeaponEventHumanCommandActionFinished;
		WeaponEventContinuousLoadBulletStart	__lS_ = new WeaponEventContinuousLoadBulletStart;
		WeaponEventContinuousLoadBulletEnd		__lE_ = new WeaponEventContinuousLoadBulletEnd;
		WeaponEventAnimCocked					__wc_ = new WeaponEventAnimCocked;
		WeaponEventAnimBulletShow				__bs_ = new WeaponEventAnimBulletShow;
		WeaponEventAnimBulletHide				__bh_ = new WeaponEventAnimBulletHide;
		WeaponEventAnimBulletEject				__be_ = new WeaponEventAnimBulletEject;
		WeaponEventAnimBulletInMagazine			__bM_ = new WeaponEventAnimBulletInMagazine;
		WeaponEventAnimBulletShow2				_bs2_ = new WeaponEventAnimBulletShow2;

		m_fsm = new WeaponFSM(this); // @NOTE: set owner of the submachine fsm
		m_fsm.AddTransition(new WeaponTransition(m_start  , __bs_, m_chamber));
		

		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_w4sb2, NULL, new GuardAnd(new GuardAnd(new WeaponGuardHasAmmoInLoopedState(m_chamber), new WeaponGuardChamberHasRoomForMoreThanOne(m_weapon)),new WeaponGuardWeaponManagerWantContinue())));
		m_fsm.AddTransition(new WeaponTransition(m_chamber, __bM_, m_endLoop));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2,	__bh_, m_hideB));
		m_fsm.AddTransition(new WeaponTransition(m_hideB,	__bs_, m_chamber));

		m_fsm.AddTransition(new WeaponTransition(m_endLoop, _fin_, NULL));
		
		m_fsm.AddTransition(new WeaponTransition(m_endLoop,				__be_, m_eject));
		m_fsm.AddTransition(new WeaponTransition(m_eject,				__wc_, m_chamberFromInnerMag));
		m_fsm.AddTransition(new WeaponTransition(m_chamberFromInnerMag,	_fin_, NULL));
		
		// Safety exits
		m_fsm.AddTransition(new WeaponTransition(m_hideB, 	_fin_, null));
		m_fsm.AddTransition(new WeaponTransition(m_w4sb2, 	_fin_, null));
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
					if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - ammo pile removed from inv (inv->LHand)"); }
				}
				else
					Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, error - cannot remove ammo pile from inv");
				
				m_chamber.m_srcMagazine = m_srcMagazine;
			} 
			else
			{
				Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast m_srcMagazine = NULL");
			}
		}
		else
		{
			Print("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast (e=NULL), m_srcMagazine=" + m_srcMagazine.ToString());
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
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - ammo pile removed from left hand to previous location (LHand->inv) - exit"); }
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
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - no inventory space for ammo pile - dropped to ground - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - exit");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - ammo pile removed from left hand (LHand->inv) - exit"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, error - cannot remove ammo pile from wpn - exit");
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
								if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - ammo pile removed from left hand to previous location (LHand->inv) - abort"); }
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
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - no inventory space for ammo pile - dropped to ground - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, error - cannot drop ammo pile from left hand after not found inventory space for ammo pile - abort");
					
					}
					else
					{
						if (GameInventory.LocationSyncMoveEntity(leftHandIl, il))
						{
							if (LogManager.IsWeaponLogEnable()) { wpnDebugPrint("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, ok - ammo pile removed from left hand (LHand->inv) - abort"); }
						}
						else
							Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast, error - cannot remove ammo pile from wpn - abort");
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
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast.SaveCurrentFSMState: cannot save m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationWriteToContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast.SaveCurrentFSMState: cannot write m_srcMagazinePrevLocation for weapon=" + m_weapon);
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
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast.LoadCurrentFSMState: cannot read m_srcMagazine for weapon=" + m_weapon);
			return false;
		}
		
		if (!OptionalLocationReadFromContext(m_srcMagazinePrevLocation, ctx))
		{
			Error("[wpnfsm] " + Object.GetDebugName(m_weapon) + " LoopedChamberingEjectLast.LoadCurrentFSMState: cannot read m_srcMagazinePrevLocation for weapon=" + m_weapon);
			return false;
		}
		return true;
	}
};