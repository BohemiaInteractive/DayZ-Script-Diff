class WeaponManager
{
	const float MAX_DROP_MAGAZINE_DISTANCE_SQ = 4;
	
	protected PlayerBase 					m_player;
	
	protected int							m_LastAcknowledgmentID;
	
	protected int 							m_PendingWeaponActionAcknowledgmentID;
	protected Magazine 						m_PendingTargetMagazine;
	protected ref InventoryLocation			m_TargetInventoryLocation;
	protected int 							m_PendingWeaponAction; 
	protected ref InventoryLocation			m_PendingInventoryLocation;
	
	protected bool							m_canEnd;
	protected bool							m_justStart;
	protected bool							m_InProgress;
	protected bool							m_IsEventSended;
	protected bool							m_WantContinue;
	protected bool							m_InIronSight;
	protected bool							m_InOptic;
	protected bool 							m_readyToStart;
	protected Weapon_Base					m_WeaponInHand;
	protected MagazineStorage				m_MagazineInHand;
	protected ActionBase					m_ControlAction;
	protected int 							m_ForceEjectBulletTimestamp;
	
	protected const int 					FORCE_EJECT_BULLET_TIMEOUT = 2000;
#ifdef DEVELOPER	
	protected int 							m_BurstOption;
#endif
	//Reload
	protected ref array<Magazine>			m_MagazinePilesInInventory;
	protected ref array<MagazineStorage>	m_MagazineStorageInInventory;
	protected ref array<Magazine>			m_SuitableMagazines;
	protected Magazine						m_PreparedMagazine;
	
	//Jamming
	protected float m_NewJamChance;
	protected bool m_WaitToSyncJamChance;
	
	
	protected int m_AnimationRefreshCooldown;
	
	void WeaponManager(PlayerBase player)
	{
		m_ForceEjectBulletTimestamp = -1;
		m_player = player;
		m_PendingWeaponActionAcknowledgmentID = -1;
		m_PendingTargetMagazine = NULL;
		m_PendingInventoryLocation = NULL;
		m_WeaponInHand = NULL;
		m_MagazineInHand = NULL;
		m_ControlAction = NULL;
		m_PendingWeaponAction = -1;
		m_LastAcknowledgmentID = 1;
		m_InProgress = false;
		m_WantContinue = true;
		m_IsEventSended = false;
		m_canEnd = false;
		m_readyToStart = false;
		m_AnimationRefreshCooldown = 0;
		
		m_NewJamChance = -1;
		m_WaitToSyncJamChance = false;
		
		m_MagazinePilesInInventory = new array<Magazine>;
		m_MagazineStorageInInventory = new array<MagazineStorage>;
		m_SuitableMagazines = new array<Magazine>;
		m_PreparedMagazine = null;
		
#ifdef DEVELOPER	
		m_BurstOption = 0;
#endif
	}
//----------------------------------------------------------------------------	
// Weapon Action conditions
//----------------------------------------------------------------------------	
	bool CanFire(Weapon_Base wpn)
	{
		if( m_player.GetHumanInventory().GetEntityInHands() != wpn )
			return false;
		
		if( m_player.IsLiftWeapon() || !m_player.IsRaised() || wpn.IsDamageDestroyed() || m_player.GetDayZPlayerInventory().IsProcessing() || !m_player.IsWeaponRaiseCompleted() || m_player.IsFighting() ) 
			return false;
		
		return true;
		
	}
	
	
	bool CanAttachMagazine(Weapon_Base wpn, Magazine mag, bool reservationCheck = true )
	{	
		if ( !wpn || !mag )
			return false;
		
		if ( m_player.GetHumanInventory().GetEntityInHands() != wpn )
			return false;
		
		if ( wpn.IsDamageDestroyed())
			return false;
		
		//if ( mag.GetHierarchyRootPlayer() && mag.GetHierarchyRootPlayer() != m_player )
			//return false;
		
		if( m_player.IsItemsToDelete())
			return false;
		
		if ( reservationCheck && (m_player.GetInventory().HasInventoryReservation(wpn, null) || m_player.GetInventory().HasInventoryReservation(mag, null)))
			return false;
		
		InventoryLocation invLoc;
		invLoc = new InventoryLocation();
		
		mag.GetInventory().GetCurrentInventoryLocation(invLoc);
		Weapon_Base wnp2;
		

		//magazine is already in weapon
		if ( Class.CastTo(wnp2,  invLoc.GetParent()) )
			return false;
			
		int muzzleIndex = wpn.GetCurrentMuzzle();
		
		if (wpn.CanAttachMagazine(muzzleIndex, mag))
			return true;

		return false;
		
	}
//---------------------------------------------------------------------------	
	
	bool CanSwapMagazine(Weapon_Base wpn, Magazine mag, bool reservationCheck = true)
	{
		if ( !wpn || !mag )
			return false;
		
		if ( m_player.GetHumanInventory().GetEntityInHands() != wpn )
			return false;
		
		if ( mag.IsDamageDestroyed() || wpn.IsDamageDestroyed())
			return false;
		
		//if ( mag.GetHierarchyRootPlayer() && mag.GetHierarchyRootPlayer() != m_player )
			//return false;

		if( m_player.IsItemsToDelete())
			return false;
		
		if ( reservationCheck && (m_player.GetInventory().HasInventoryReservation(wpn, null) || m_player.GetInventory().HasInventoryReservation(mag, null)))
			return false;
		

		InventoryLocation invLoc;
		invLoc = new InventoryLocation();
		
		mag.GetInventory().GetCurrentInventoryLocation(invLoc);
		Weapon_Base wnp2;
		
		//second magazine is already in weapon
		if( Class.CastTo(wnp2,  invLoc.GetParent()) )
			return false;		
		
		int muzzleIndex = wpn.GetCurrentMuzzle();	
					
		Magazine mag2;
		if( !Class.CastTo(mag2, wpn.GetMagazine(muzzleIndex)) ) 
			return false;
		
		if( GameInventory.CanSwapEntitiesEx( mag, mag2 ) )
			return true;
		
		InventoryLocation il = new InventoryLocation();
		
		if( GameInventory.CanForceSwapEntitiesEx( mag, null, mag2, il ) )
			return true;
		
		return false;
	}
//----------------------------------------------------------------------------			
	bool CanDetachMagazine(Weapon_Base wpn, Magazine mag, bool reservationCheck = true)
	{
		if ( !wpn || !mag )
			return false;
		
		if ( m_player.GetHumanInventory().GetEntityInHands() != wpn )
			return false;
		
		if ( mag.GetHierarchyParent() != wpn )
			return false;
		
		if( m_player.IsItemsToDelete())
			return false;
		
		if ( reservationCheck && (m_player.GetInventory().HasInventoryReservation(wpn, null) || m_player.GetInventory().HasInventoryReservation(mag, null)))
			return false;
		
		return true;
	}
//---------------------------------------------------------------------------
	bool CanLoadBullet(Weapon_Base wpn, Magazine mag, bool reservationCheck = true)
	{
		if ( !wpn || !mag )
			return false;
		
		if ( m_player.GetHumanInventory().GetEntityInHands() != wpn )
			return false;
		
		if( mag.IsDamageDestroyed() || wpn.IsDamageDestroyed())
			return false;
		
		//if ( mag.GetHierarchyRootPlayer() && mag.GetHierarchyRootPlayer() != m_player )
		//	return false;
		
		if( wpn.IsJammed(/*wpn.GetCurrentMuzzle()*/) )
			return false;
	
		if( m_player.IsItemsToDelete())
			return false;
		
		if ( reservationCheck && (m_player.GetInventory().HasInventoryReservation(wpn,null) || m_player.GetInventory().HasInventoryReservation(mag,null)))
			return false;
		
		for( int i = 0; i < wpn.GetMuzzleCount(); i++)
		{
			if( wpn.CanChamberBullet( i, mag ) )
				return true;
		}
		
		return false;
	}
//---------------------------------------------------------------------------	
	bool CanUnjam(Weapon_Base wpn, bool reservationCheck = true)
	{
		if( !wpn )
			return false;
		 
		if( m_player.GetHumanInventory().GetEntityInHands() != wpn)
			return false;
		
		if( wpn.IsDamageDestroyed())
			return false;
		
		if( m_player.IsItemsToDelete())
			return false;
		
		if ( reservationCheck && m_player.GetInventory().HasInventoryReservation(wpn, null))
			return false;

		
		if( !wpn.IsJammed(/*wpn.GetCurrentMuzzle()*/) )
			return false;
		 
		return true;
	}
	
	bool CanEjectBullet(Weapon_Base wpn, bool reservationCheck = true)
	{
		if( !wpn )
			return false;
		 
		if( m_player.GetHumanInventory().GetEntityInHands() != wpn)
			return false;
		
		if( m_player.IsItemsToDelete())
			return false;
		
		if( reservationCheck && m_player.GetInventory().HasInventoryReservation(wpn, null))
			return false;
		
		if( !wpn.CanEjectBullet() )
			return false;
		
		if( wpn.IsJammed(/*wpn.GetCurrentMuzzle()*/) )
			return false;
		 
		return true;
	}
	
	void SetEjectBulletTryTimestamp()
	{
		m_ForceEjectBulletTimestamp = GetGame().GetTime();
	}
//----------------------------------------------------------------------------
	bool InventoryReservation( Magazine mag, InventoryLocation invLoc)
	{
		Weapon_Base weapon;
		InventoryLocation ilWeapon = new InventoryLocation();
		if (Weapon_Base.CastTo(weapon, m_player.GetItemInHands()) )
		{
			weapon.GetInventory().GetCurrentInventoryLocation(ilWeapon);
			if ( m_player.GetInventory().HasInventoryReservation(weapon, ilWeapon) )
			{
				return false;
			}
			else
			{
				m_player.GetInventory().AddInventoryReservationEx(weapon,ilWeapon,GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
		
		if( invLoc )
		{
			if ( m_player.GetInventory().HasInventoryReservation(invLoc.GetItem(),invLoc) )
			{
				m_player.GetInventory().ClearInventoryReservationEx(weapon, ilWeapon);
				return false;
			}
			else
			{
				m_player.GetInventory().AddInventoryReservationEx(invLoc.GetItem(),invLoc,GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
		
		if( mag )
		{
			m_TargetInventoryLocation = new InventoryLocation();
			m_TargetInventoryLocation.SetAttachment( m_WeaponInHand, mag, InventorySlots.MAGAZINE);
			
			if ( m_player.GetInventory().HasInventoryReservation(mag, m_TargetInventoryLocation) )
			//if ( !m_player.GetInventory().AddInventoryReservationEx( mag, m_TargetInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS) )
			{
				m_player.GetInventory().ClearInventoryReservationEx(weapon, ilWeapon);
				if (invLoc)
				{
					m_player.GetInventory().ClearInventoryReservationEx(invLoc.GetItem(), invLoc);
				}
				return false;
			}
			else
			{
				m_player.GetInventory().AddInventoryReservationEx(mag, m_TargetInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
			}
		}
		m_PendingTargetMagazine = mag;
		m_PendingInventoryLocation = invLoc;
		
		return true;
	}
	
//----------------------------------------------------------------------------	
// Weapon Actions	
//----------------------------------------------------------------------------
	bool AttachMagazine( Magazine mag , ActionBase control_action = NULL )
	{
		return StartAction(AT_WPN_ATTACH_MAGAZINE, mag, NULL, control_action);
	}
	
	bool DetachMagazine( InventoryLocation invLoc, ActionBase control_action = NULL)
	{
		return StartAction(AT_WPN_DETACH_MAGAZINE, NULL, invLoc, control_action);
	}
	
	bool SwapMagazine( Magazine mag, ActionBase control_action = NULL )
	{
		InventoryLocation il = new InventoryLocation();
		if (PrepareInventoryLocationForMagazineSwap(m_WeaponInHand, mag, il) )
		{
			return StartAction(AT_WPN_SWAP_MAGAZINE, mag, il, control_action);
		}
		return false;
	}
	
	bool SwapMagazineEx( Magazine mag, InventoryLocation invLoc, ActionBase control_action = NULL )
	{
		return StartAction(AT_WPN_SWAP_MAGAZINE, mag, invLoc, control_action);
	}
	
	bool LoadBullet( Magazine mag, ActionBase control_action = NULL )
	{
		return StartAction(AT_WPN_LOAD_BULLET, mag, NULL, control_action);
	}
	
	bool LoadMultiBullet( Magazine mag, ActionBase control_action = NULL )
	{
		return StartAction(AT_WPN_LOAD_MULTI_BULLETS_START, mag, NULL, control_action);
	}
	
	void LoadMultiBulletStop( )
	{
		if(m_InProgress) m_WantContinue = false;
	}
	
	bool Unjam( ActionBase control_action = NULL )
	{
		return StartAction(AT_WPN_UNJAM, NULL, NULL, control_action);
	}

	bool EjectBullet( ActionBase control_action = NULL )
	{
		return StartAction(AT_WPN_EJECT_BULLET, NULL, NULL, control_action);
	}
	
	bool CanEjectBulletVerified()
	{
		int mi = m_WeaponInHand.GetCurrentMuzzle();
		if( !m_WeaponInHand.IsChamberFiredOut(mi) && !m_WeaponInHand.IsChamberEmpty(mi) )
		{
			int actual_time = GetGame().GetTime();
			if ( actual_time - m_ForceEjectBulletTimestamp > FORCE_EJECT_BULLET_TIMEOUT )
			{
				return false;
			}
		}
		return true;
	}
	
	bool EjectBulletVerified( ActionBase control_action = NULL )
	{
		if ( m_WeaponInHand )
		{	
			int mi = m_WeaponInHand.GetCurrentMuzzle();
			if ( !m_WeaponInHand.IsChamberFiredOut(mi) && !m_WeaponInHand.IsChamberEmpty(mi) )
			{
				m_ForceEjectBulletTimestamp = GetGame().GetTime();
				return StartAction(AT_WPN_EJECT_BULLET, NULL, NULL, control_action);
			}
			else
			{
				return StartAction(AT_WPN_EJECT_BULLET, NULL, NULL, control_action);
			}
		}	
		return false;
	}
	
	bool SetNextMuzzleMode ()
	{
		return StartAction(AT_WPN_SET_NEXT_MUZZLE_MODE, NULL, NULL, NULL);
	}
	
	void Fire(Weapon_Base wpn)
	{
		int mi = wpn.GetCurrentMuzzle();
		if( wpn.IsChamberFiredOut(mi) || wpn.IsJammed() || wpn.IsChamberEmpty(mi) )
		{
			wpn.ProcessWeaponEvent(new WeaponEventTrigger(m_player));
			return;
		}
		
		if(wpn.JamCheck(0))
		{
			wpn.ProcessWeaponEvent(new WeaponEventTriggerToJam(m_player));
		}
		else
		{
			wpn.ProcessWeaponEvent(new WeaponEventTrigger(m_player));
		}
	}
#ifdef DEVELOPER
	int GetBurstOption()
	{
		return m_BurstOption;
	}
	
	void SetBurstOption(int value)
	{
		m_BurstOption = value;
	}
#endif	
//-------------------------------------------------------------------------------------	
// Synchronize - initialize from client side
//-------------------------------------------------------------------------------------

	//Client
	private void Synchronize( )
	{
		if( GetGame().IsClient() )
		{
			m_PendingWeaponActionAcknowledgmentID = ++m_LastAcknowledgmentID;
			ScriptInputUserData ctx = new ScriptInputUserData;
			
			ctx.Write(INPUT_UDT_WEAPON_ACTION);
			ctx.Write(m_PendingWeaponAction);
			ctx.Write(m_PendingWeaponActionAcknowledgmentID);
			
			
			switch (m_PendingWeaponAction)
			{
				case AT_WPN_ATTACH_MAGAZINE:
				{	
					ctx.Write(m_PendingTargetMagazine);
					break;
				}
				case AT_WPN_SWAP_MAGAZINE:
				{
					ctx.Write(m_PendingTargetMagazine);
					m_PendingInventoryLocation.WriteToContext(ctx);
					break;
				}
				case AT_WPN_DETACH_MAGAZINE:
				{
					m_PendingInventoryLocation.WriteToContext(ctx);
					break;
				}
				case AT_WPN_LOAD_BULLET:
				{
					ctx.Write(m_PendingTargetMagazine);
					break;
				}
				case AT_WPN_LOAD_MULTI_BULLETS_START:
				{
					ctx.Write(m_PendingTargetMagazine);
					break;
				}
				case AT_WPN_UNJAM:
				{
					break;
				}
				case AT_WPN_EJECT_BULLET:
				{
					break;
				}
				default: 
				break;
			}
			ctx.Send();
			//if( !m_player.GetDayZPlayerInventory().HasLockedHands() )
			//	m_player.GetDayZPlayerInventory().LockHands();
		}
	}
	

	
	void OnSyncJuncture(int pJunctureID, ParamsReadContext pCtx)
	{
		if (pJunctureID == DayZPlayerSyncJunctures.SJ_WEAPON_SET_JAMMING_CHANCE )
		{
			pCtx.Read(m_NewJamChance);
		}
		else
		{
			int AcknowledgmentID;
			pCtx.Read(AcknowledgmentID);
			if ( AcknowledgmentID == m_PendingWeaponActionAcknowledgmentID)
			{
				if (pJunctureID == DayZPlayerSyncJunctures.SJ_WEAPON_ACTION_ACK_ACCEPT)
				{ 
					m_readyToStart = true;
				}
				else if (pJunctureID == DayZPlayerSyncJunctures.SJ_WEAPON_ACTION_ACK_REJECT)
				{
					if(m_PendingWeaponAction >= 0 )
					{
						if(!(GetGame().IsServer() && GetGame().IsMultiplayer()))
						{
							InventoryLocation ilWeapon = new InventoryLocation();
							ItemBase weapon = m_player.GetItemInHands();
							weapon.GetInventory().GetCurrentInventoryLocation(ilWeapon);
							m_player.GetInventory().ClearInventoryReservationEx(m_player.GetItemInHands(),ilWeapon);
							
							if( m_PendingTargetMagazine )
							{
								m_PendingTargetMagazine.GetInventory().ClearInventoryReservationEx(m_PendingTargetMagazine, m_TargetInventoryLocation );
							}
							
							if( m_PendingInventoryLocation )
							{
								m_player.GetInventory().ClearInventoryReservationEx( NULL, m_PendingInventoryLocation );
							}
						}
						m_PendingWeaponActionAcknowledgmentID = -1;
						m_PendingTargetMagazine = NULL;
						m_PendingWeaponAction = -1;
						m_PendingInventoryLocation = NULL;
						m_InProgress = false;
					}
				}
			}
		}
	}
	

		//Server
	bool OnInputUserDataProcess(int userDataType, ParamsReadContext ctx)
	{
		Weapon_Base wpn;
		int mi;
		InventoryLocation il;
		int slotID;
		bool accepted = false;
		if( userDataType == INPUT_UDT_WEAPON_ACTION)
		{
			if (!ctx.Read(m_PendingWeaponAction))
				return false;
			
			if (!ctx.Read(m_PendingWeaponActionAcknowledgmentID))
				return false;
			
			if(m_PendingTargetMagazine)
			{
				GetGame().ClearJuncture(m_player, m_PendingTargetMagazine);
				m_PendingTargetMagazine = NULL;
			}
			m_InProgress = true;
			m_IsEventSended = false;
			Magazine mag = NULL;
			
			Weapon_Base.CastTo( wpn, m_player.GetItemInHands() );
			if ( wpn )
				mi = wpn.GetCurrentMuzzle();
			
			switch (m_PendingWeaponAction)
			{
				case AT_WPN_ATTACH_MAGAZINE:
				{
					if ( !ctx.Read(mag) )
						break;
					
					if ( !mag || !wpn )
						break;

					slotID = wpn.GetSlotFromMuzzleIndex(mi);
					il = new InventoryLocation();
					il.SetAttachment(wpn,mag,slotID);
					if( GetGame().AddInventoryJunctureEx(m_player, mag, il, false, 10000) )
						accepted = true;

					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_SWAP_MAGAZINE:
				{
					if ( !ctx.Read(mag) )
						break;
					
					il = new InventoryLocation();
					if (!il.ReadFromContext(ctx))
						break;
					
					if ( !mag || !wpn )
						break;

					if ( !wpn.GetMagazine(mi) )
						break;
					
					if ( GetGame().AddActionJuncture(m_player,mag,10000) )
						accepted = true;
					m_PendingInventoryLocation = il;
					m_PendingTargetMagazine = mag;
					
					break;
				}
				case AT_WPN_DETACH_MAGAZINE:
				{
					il = new InventoryLocation();
					if ( !il.ReadFromContext(ctx) )
						break;
					
					if ( !il.IsValid() )
						break;
					
					if ( !wpn )
						break;
					
					Magazine det_mag = wpn.GetMagazine(mi);
					mag = Magazine.Cast(il.GetItem());
					if ( !det_mag || ( mag != det_mag) )
						break;
					
					if( GetGame().AddInventoryJunctureEx(m_player, il.GetItem(), il, true, 10000))
						accepted = true;
					m_PendingInventoryLocation = il;
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_LOAD_BULLET:
				{
					ctx.Read(mag);
					
					if ( !mag )
						break;
					
					if( GetGame().AddActionJuncture(m_player,mag,10000) )
						accepted = true;
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_LOAD_MULTI_BULLETS_START:
				{
					ctx.Read(mag);
					
					if ( !mag )
						break;
					
					if( GetGame().AddActionJuncture(m_player,mag,10000) )
						accepted = true;
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_UNJAM:
				{
					accepted = true;
					//Unjam();
					break;
				}
				case AT_WPN_EJECT_BULLET:
				{
					accepted = true;
					break;
				}
				case AT_WPN_SET_NEXT_MUZZLE_MODE:
				{
					accepted = true;
					break;
				}
				default:
					Error("unknown actionID=" + m_PendingWeaponAction);
					break;
			}
			DayZPlayerSyncJunctures.SendWeaponActionAcknowledgment(m_player, m_PendingWeaponActionAcknowledgmentID, accepted);
		}
		
		return accepted;
	}
	
	bool StartAction(int action, Magazine mag, InventoryLocation il, ActionBase control_action = NULL)
	{
		//if it is controled by action inventory reservation and synchronization provide action itself
		if(control_action)
		{
			m_ControlAction = ActionBase.Cast(control_action);
			m_PendingWeaponAction = action;
			m_InProgress = true;
			m_IsEventSended = false;
			m_PendingTargetMagazine = mag;
			m_PendingInventoryLocation = il;
			StartPendingAction();
			
			return true;
		}
		
		
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			return false;
		
		if ( !ScriptInputUserData.CanStoreInputUserData() )
			return false;
		if ( !InventoryReservation(mag, il) )
			return false;

		m_PendingWeaponAction = action;
		m_InProgress = true;
		m_IsEventSended = false;
		
		if ( !GetGame().IsMultiplayer() )
			m_readyToStart = true;
		else
			Synchronize();
		
		return true;
	}
	
	void StartPendingAction()
	{		
		m_WeaponInHand = Weapon_Base.Cast(m_player.GetItemInHands());
		if(!m_WeaponInHand)
		{
			OnWeaponActionEnd();
			return;
		}
		switch (m_PendingWeaponAction)
		{
			case AT_WPN_ATTACH_MAGAZINE:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventAttachMagazine(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_SWAP_MAGAZINE:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventSwapMagazine(m_player, m_PendingTargetMagazine, m_PendingInventoryLocation) );
				break;
			}
			case AT_WPN_DETACH_MAGAZINE:
			{
				Magazine mag = Magazine.Cast(m_PendingInventoryLocation.GetItem());
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventDetachMagazine(m_player, mag, m_PendingInventoryLocation) );
				break;
			}
			case AT_WPN_LOAD_BULLET:
			{
				m_WantContinue = false;
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventLoad1Bullet(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_LOAD_MULTI_BULLETS_START:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventLoad1Bullet(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_LOAD_MULTI_BULLETS_END:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventContinuousLoadBulletEnd(m_player) );
				break;
			}
			case AT_WPN_UNJAM:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventUnjam(m_player, NULL) );
				break;
			}
			case AT_WPN_EJECT_BULLET:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventMechanism(m_player, NULL) );
				break;
			}
			case AT_WPN_SET_NEXT_MUZZLE_MODE:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventSetNextMuzzleMode(m_player, NULL) );
				break;
			}
			default:
				m_InProgress = false;
				Error("unknown actionID=" + m_PendingWeaponAction);
		}	
		m_IsEventSended = true;
		m_canEnd = false;
	}
	
	bool IsRunning()
	{
		return m_InProgress;
	}
	
	void SetRunning( bool value)
	{
		m_InProgress = value;
	}
	
	void Refresh()
	{
		OnWeaponActionEnd();
	}
	
	void Update( float deltaT )
	{

		if (m_WeaponInHand != m_player.GetItemInHands())
		{
			if( m_WeaponInHand )
			{
				m_SuitableMagazines.Clear();
				OnWeaponActionEnd();
			}
			m_WeaponInHand = Weapon_Base.Cast(m_player.GetItemInHands());
			if ( m_WeaponInHand )
			{
				m_MagazineInHand = null;
				//SET new magazine
				SetSutableMagazines();
				m_WeaponInHand.SetSyncJammingChance(0);
			}
			m_AnimationRefreshCooldown = 0;
		}
		
		if (m_WeaponInHand)
		{
			if(m_AnimationRefreshCooldown)
			{
				m_AnimationRefreshCooldown--;
			
				if( m_AnimationRefreshCooldown == 0)
				{
					RefreshAnimationState();
				}
			}
		
			if (!GetGame().IsMultiplayer())
			{
				m_WeaponInHand.SetSyncJammingChance(m_WeaponInHand.GetChanceToJam());
			}
			else
			{
				if ( m_NewJamChance >= 0)
				{
					m_WeaponInHand.SetSyncJammingChance(m_NewJamChance);
					m_NewJamChance = -1;
					m_WaitToSyncJamChance = false;
				}
				if (GetGame().IsServer() && !m_WaitToSyncJamChance )
				{
					float actual_chance_to_jam;
					actual_chance_to_jam = m_WeaponInHand.GetChanceToJam();
					if ( Math.AbsFloat(m_WeaponInHand.GetSyncChanceToJam() - m_WeaponInHand.GetChanceToJam()) > 0.001 )
					{
						DayZPlayerSyncJunctures.SendWeaponJamChance(m_player, m_WeaponInHand.GetChanceToJam());
						m_WaitToSyncJamChance = true;
					}
				}
			}
			
			if(m_readyToStart)
			{
				StartPendingAction();
				m_readyToStart = false;
				return;
			}
		
			if( !m_InProgress || !m_IsEventSended )
				return;
		
			if(m_canEnd)
			{
			
				if(m_WeaponInHand.IsIdle())
				{
					OnWeaponActionEnd();
				}
				else if(m_justStart)
				{
					m_InIronSight = m_player.IsInIronsights();
					m_InOptic = m_player.IsInOptics();
		
					if(m_InIronSight || m_InOptic)
					{
						m_player.GetInputController().ResetADS();
						m_player.ExitSights();
					}
				
					m_justStart = false;
				}
			
			}
			else
			{
				m_canEnd = true;
				m_justStart = true;
			}
		}
		else
		{
			if ( m_MagazineInHand != m_player.GetItemInHands() )
			{
				m_MagazineInHand = MagazineStorage.Cast(m_player.GetItemInHands());
				if ( m_MagazineInHand )
				{
					SetSutableMagazines();
				}
			}
		
		
		}
	}

	void OnWeaponActionEnd()
	{
		if ( !m_InProgress )
			return;
		
		if (!m_ControlAction)
		{
			if(GetGame().IsServer() && GetGame().IsMultiplayer())
			{
				if(m_PendingTargetMagazine)
				{
					GetGame().ClearJuncture(m_player,m_PendingTargetMagazine);
				}
			}
			else
			{
				InventoryLocation il = new InventoryLocation();
				il.SetHands(m_player,m_player.GetItemInHands());
				m_player.GetInventory().ClearInventoryReservationEx(m_player.GetItemInHands(),il);
						
				if( m_PendingTargetMagazine )
				{
					m_player.GetInventory().ClearInventoryReservationEx(m_PendingTargetMagazine, m_TargetInventoryLocation );
				}
						
				if( m_PendingInventoryLocation )
				{
					m_player.GetInventory().ClearInventoryReservationEx( m_PendingInventoryLocation.GetItem(), m_PendingInventoryLocation );
				}
			}
		}
		
		if(m_WeaponInHand && !m_justStart && m_player.IsRaised())
		{
			if(m_InIronSight)
				m_player.SetIronsights(m_InIronSight);
			if(m_InOptic)
			{
				ItemOptics optic = m_WeaponInHand.GetAttachedOptics();
				if(optic)
					m_player.SwitchOptics(optic,true);
			}
		}
		m_ControlAction = NULL;
		m_PendingWeaponAction = -1;
		m_PendingTargetMagazine = NULL;
		m_PendingInventoryLocation = NULL;
		m_TargetInventoryLocation = NULL;
		m_PendingWeaponActionAcknowledgmentID = -1;
		//m_WeaponInHand = NULL;
		m_InProgress = false;
		m_readyToStart = false;
		m_WantContinue = true;
		
	}
	
	void DelayedRefreshAnimationState(int delay)
	{	
		if(m_WeaponInHand)
		{
			if(delay == 0)
			{
				RefreshAnimationState();
			}
			m_AnimationRefreshCooldown = delay;
		}
	}
	
	void RefreshAnimationState()
	{
		if(m_WeaponInHand)
		{
			WeaponStableState state = WeaponStableState.Cast( m_WeaponInHand.GetCurrentState() );
		
			if (state)
			{
				HumanCommandWeapons hcw = m_player.GetCommandModifier_Weapons();
				if (hcw)
				{
					hcw.SetInitState(state.m_animState);
				}
			}
		}
	}
	
	bool WantContinue()
	{
		return m_WantContinue;
	}
	
	Magazine GetPreparedMagazine()
	{
		for (int i = 0; i < m_SuitableMagazines.Count(); i++)
		{
			if(!m_SuitableMagazines.Get(i) || m_SuitableMagazines.Get(i).IsRuined() )
			{
				m_SuitableMagazines.Remove(i);
				i--;
				continue;
			}
			if (m_SuitableMagazines.Get(i).GetAmmoCount() > 0)
				return m_SuitableMagazines.Get(i);
		}
		
		return null;
	}
	
	Magazine GetNextPreparedMagazine( out int startIdx )
	{
		for (int i = startIdx; i < m_SuitableMagazines.Count(); i++)
		{
			if (m_SuitableMagazines.Get(i).GetAmmoCount() > 0)
			{
				startIdx = i;
				return m_SuitableMagazines.Get(i);
			}				
		}
		return null;
	}
	
	void OnMagazineInventoryEnter(Magazine mag)
	{
		if (mag)
		{
			Weapon_Base weapon = Weapon_Base.Cast(mag.GetHierarchyParent());
			
			if (weapon)
				return;
		}
		
		int i;
		MagazineStorage sMag = MagazineStorage.Cast(mag);
		if(sMag)
		{
			for(i = 0; i < m_MagazineStorageInInventory.Count(); i++ ) 
			{
				MagazineStorage s_mag_i = m_MagazineStorageInInventory[i];
				if(!s_mag_i)
				{
					m_MagazineStorageInInventory.RemoveOrdered(i);
					i--;
					continue;
				}
				
				if(CompareMagazinesSuitability(s_mag_i,sMag)<0)
					break;
			}
			m_MagazineStorageInInventory.InsertAt(sMag,i);
			
			SetSutableMagazines(); //TODO optimalize
			return;
		}
		
		if(mag)
		{

			for(i = 0; i < m_MagazinePilesInInventory.Count(); i++ ) 
			{
				Magazine mag_i = m_MagazinePilesInInventory[i];
				if(!mag_i)
				{
					m_MagazinePilesInInventory.RemoveOrdered(i);
					i--;
					continue;
				}
				
				
				if(CompareMagazinesSuitability(mag_i,mag)<0)
				{
					break;
				}
			}
			m_MagazinePilesInInventory.InsertAt(mag,i);
			SetSutableMagazines(); //TODO optimalize
		}
		
	}
	
	void OnMagazineInventoryExit(Magazine mag)
	{
		m_SuitableMagazines.RemoveItem(mag);

		MagazineStorage sMag = MagazineStorage.Cast(mag);
		if(sMag)
		{
			m_MagazineStorageInInventory.RemoveItem(sMag);
			return;
		}
		
		if(mag)
		{
			m_MagazinePilesInInventory.RemoveItem(mag);
		}
	}
	
	void OnMagazineAttach(Magazine mag)
	{
		OnMagazineInventoryExit(mag);
	}
	
	void OnMagazineDetach(Magazine mag)
	{
		OnMagazineInventoryEnter(mag);
	}
	
	int CompareMagazinesSuitability( Magazine mag1, Magazine mag2 )
	{
		return mag1.GetAmmoCount() - mag2.GetAmmoCount();
	}
	
	void SortMagazineAfterLoad()
	{
		array<MagazineStorage>	magazines = new array<MagazineStorage>;
		array<Magazine>	magazines_piles = new array<Magazine>;
		int low_mag1, high_mag1;
		int low_mag2, high_mag2;
		int i, j;
		
		for(i = 0; i < m_MagazineStorageInInventory.Count(); i++ ) 
		{
			MagazineStorage mag = m_MagazineStorageInInventory.Get(i);
			mag.GetNetworkID(low_mag1,high_mag1);
			for( j = 0; j < magazines.Count(); j++)
			{
				magazines.Get(j).GetNetworkID(low_mag2,high_mag2);
				if(low_mag1 > low_mag2)
				{
					break;
				}
				else if (low_mag1 == low_mag2)
				{
					if( high_mag1 > high_mag2 )
					{
						break;
					}
				}
			}
			magazines.InsertAt(mag,j);
		}
		
		m_MagazineStorageInInventory.Clear();
		m_MagazineStorageInInventory.Copy(magazines);
		
		for(i = 0; i < m_MagazinePilesInInventory.Count(); i++ ) 
		{
			Magazine pile = m_MagazinePilesInInventory.Get(i);
			pile.GetNetworkID(low_mag1,high_mag1);
			for( j = 0; j < magazines_piles.Count(); j++)
			{
				magazines_piles.Get(j).GetNetworkID(low_mag2,high_mag2);
				if(low_mag1 > low_mag2)
				{
					break;
				}
				else if (low_mag1 == low_mag2)
				{
					if( high_mag1 > high_mag2 )
					{
						break;
					}
				}
			}
			magazines_piles.InsertAt(pile,j);
		}
		
		m_MagazinePilesInInventory.Clear();
		m_MagazinePilesInInventory.Copy(magazines_piles);
	
		SetSutableMagazines();
	}
	
	void SetSutableMagazines()
	{
		m_SuitableMagazines.Clear();
		int i;
		
		if (m_WeaponInHand)
		{
			int mi = m_WeaponInHand.GetCurrentMuzzle();
			
			for (i = 0; i < m_MagazineStorageInInventory.Count(); i++ )
			{
				MagazineStorage s_mag = m_MagazineStorageInInventory[i];
				
				if (!s_mag)	
				{
					m_MagazineStorageInInventory.RemoveOrdered(i);
					i--;
					continue;
				}
				
				if ( m_WeaponInHand.TestAttachMagazine(mi, s_mag, false, true))
					m_SuitableMagazines.Insert(s_mag);
			}
			
			for (i = 0; i < m_MagazinePilesInInventory.Count(); i++ )
			{
				Magazine mag = m_MagazinePilesInInventory[i];
				if (!mag)	
				{
					m_MagazinePilesInInventory.RemoveOrdered(i);
					i--;
					continue;
				}
				
				if (m_WeaponInHand.CanChamberFromMag(mi, mag))
					m_SuitableMagazines.Insert(mag);	
			}
//TODO m_MagazineStorageInInventory and m_MagazinePilesInInventory always sort
		}
		else if (m_MagazineInHand)
		{
			for (i = 0; i < m_MagazinePilesInInventory.Count(); i++ )
			{
				Magazine m_mag = m_MagazinePilesInInventory[i];
				if (!m_mag)	
				{
					m_MagazinePilesInInventory.RemoveOrdered(i);
					i--;
					continue;
				}
				if (m_MagazineInHand.IsCompatiableAmmo( m_mag ))
					m_SuitableMagazines.Insert(m_mag);
			}
		}
		else
		{
			m_PreparedMagazine = null;
		}
	
	}
	
	void OnLiftWeapon()
	{
		if( m_WeaponInHand )
			m_WeaponInHand.ResetBurstCount();
	}
	
	string GetCurrentModeName()
	{
		if( m_WeaponInHand )
		{
			int mi = m_WeaponInHand.GetCurrentMuzzle();
			return m_WeaponInHand.GetCurrentModeName(mi);
		}
		return "";
	}
	
	bool PrepareInventoryLocationForMagazineSwap( notnull Weapon_Base wpn, notnull Magazine new_mag, out InventoryLocation new_il )
	{
		int muzzleIndex = wpn.GetCurrentMuzzle();
		Magazine old_mag = Magazine.Cast(wpn.GetMagazine(muzzleIndex));
		InventoryLocation temp = new InventoryLocation();
		
		if (old_mag)
		{
			bool result = GameInventory.CanSwapEntitiesEx(new_mag, old_mag);
			if ( result )
			{
				new_mag.GetInventory().GetCurrentInventoryLocation(new_il);
				new_il.SetItem(old_mag);
			}
			else
			{
				result = GameInventory.CanForceSwapEntitiesEx(new_mag, null, old_mag, new_il);
				float dir[4];
				if ( !result )
				{
					new_il.SetGroundEx( old_mag,  m_player.GetPosition(), dir);
				}
			}
		}
		return true;
	}
}
