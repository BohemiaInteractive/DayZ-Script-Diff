enum LockAction
{
	NONE,
	DIAL_NUMBER_CHANED,
	DIAL_INDEX_CHANGED,
	LOCKED,
	UNLOCKED,
	
	COUNT
}

class CombinationLock extends ItemBase
{
	int m_LockDigits;					//how many digits will the combination contain
	int m_Combination;					//actual combination that is dialed on lock
	int m_CombinationLocked;			//combination that was dialed on lock before the shuffle
	int m_DialIndex;					//index of current combination dial
	protected bool m_IsLocked;
	
	protected LockAction m_LockActionPerformed 		= LockAction.NONE;
	
	protected bool m_IsInitialized;
	
	//Sounds
	//build
	const string SOUND_LOCK_OPEN 			= "combinationlock_open_SoundSet";
	const string SOUND_LOCK_CLOSE 			= "combinationlock_close_SoundSet";
	const string SOUND_LOCK_CHANGE_NUMBER	= "combinationlock_changenumber_SoundSet";
	const string SOUND_LOCK_CHANGE_DIAL		= "combinationlock_changedial_SoundSet";

	protected EffectSound m_Sound;

	void CombinationLock()
	{
		SetBaseLockValues();
		
		//synchronized variables
		int combination_length = Math.Pow( 10, m_LockDigits );
		RegisterNetSyncVariableBool( "m_IsLocked" );
		RegisterNetSyncVariableInt( "m_Combination", 0, combination_length - 1 );
		RegisterNetSyncVariableInt( "m_DialIndex", 0, m_LockDigits - 1 );
		RegisterNetSyncVariableInt( "m_LockActionPerformed", 0, LockAction.COUNT );
	}
	
	protected void SetBaseLockValues()
	{
		//set lock init values
		m_LockDigits 			= 3;
		m_Combination		 	= 111;
		m_CombinationLocked 	= 999;		
		m_IsLocked 				= false;
	}

	override void EEInit()
	{
		super.EEInit();
		
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SetInitialized, 1000, false );
		//SetInitialized();
		
		//set visual on init
		UpdateVisuals();
	}
	
	void SetInitialized()
	{
		m_IsInitialized = true;
	}
	
	override bool IsInitialized()
	{
		return m_IsInitialized;
	}
	
	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged( old_owner, new_owner );
		
		//Check combination lock
		if ( GetGame().IsServer() )
		{
			if ( IsInitialized() && new_owner && new_owner.IsInherited( BaseBuildingBase ) )
			{
				LockServer( new_owner );
			}
		}		
	}	
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		//write data
		ctx.Write( m_Combination );
		ctx.Write( m_CombinationLocked );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		//--- Combination Lock data ---
		//combination
		if ( !ctx.Read( m_Combination ) )
		{
			m_Combination = 0;
			return false;
		}
		
		//combination locked
		if ( !ctx.Read( m_CombinationLocked ) )
		{
			m_CombinationLocked = 0;
			return false;
		}
		
		//is lock attached
		if ( version < 105 )					//removed in 105
		{
			bool is_lock_attached;
			if ( !ctx.Read( is_lock_attached ) )
			{
				return false;
			}
		}

		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();		
		
		//Check combination lock
		if ( GetGame().IsServer() )
		{
			EntityAI parent = GetHierarchyParent();
			if ( parent && parent.IsInherited( BaseBuildingBase ) )
			{
				LockServer( parent, true );
			}
		}		
		
		//synchronize
		Synchronize();
	}
	
	// --- SYNCHRONIZATION
	void Synchronize()
	{
		bsbDebugPrint("[bsb] CombinationLock.Synchronize " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
		if ( GetGame().IsServer() )
		{
			SetSynchDirty();
			GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( ResetActionVar, 1000);//synced var used to trigger client sound needs to be reset after triggering the sound
			UpdateVisuals();
		}
	}

	
	void ResetActionVar()
	{
		m_LockActionPerformed = LockAction.NONE;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		//update visuals (client)
		UpdateVisuals();
		
		//update sound (client)
		if(m_LockActionPerformed)
			UpdateSound();
		
		bsbDebugPrint("[bsb] CombinationLock.OnVariablesSynchronized " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
	}
	
	void SetCombination( int combination )
	{
		m_Combination = combination;
	}
	
	void SetCombinationLocked( int combination )
	{
		m_CombinationLocked = combination;
	}
	
	int GetCombination()
	{
		return m_Combination;
	}
	
	int GetLockDigits()
	{
		return m_LockDigits;
	}
	
	// --- ACTIONS
	void DialNextNumber()
	{
		string combination_text = m_Combination.ToString();
		string dialed_text;
		
		//insert zeros to dials with 0 value
		int length_diff = m_LockDigits - combination_text.Length();
		for ( int i = 0; i < length_diff; ++i )
		{
			combination_text = "0" + combination_text;
		}
		
		//assemble the whole combination with increased part
		for ( int j = 0; j < combination_text.Length(); ++j )
		{
			if ( j == m_DialIndex )
			{
				int next_dialed_number = combination_text.Get( j ).ToInt() + 1;
				if ( next_dialed_number > 9 )
				{
					next_dialed_number = 0;
				}
				
				dialed_text += next_dialed_number.ToString();
			}
			else
			{
				dialed_text += combination_text.Get( j );
			}
		}
		
		//set new number		
		SetCombination( dialed_text.ToInt() );
		m_LockActionPerformed = LockAction.DIAL_INDEX_CHANGED;
		CheckLockedStateServer();
		
		//synchronize
		Synchronize();
	}
	
	int GetDialIndex()
	{
		return m_DialIndex;
	}
	
	void SetNextDial()
	{
		if ( m_LockDigits > 1 )
		{
			if ( m_DialIndex <= m_LockDigits - 2 )
			{
				m_DialIndex++;
			}
			else if ( m_DialIndex >= m_LockDigits >  - 1 )
			{
				m_DialIndex = 0;
			}
		}
		else
		{
			m_DialIndex = 0;
		}
		
		//performed action
		m_LockActionPerformed = LockAction.DIAL_NUMBER_CHANED;
		//synchronize
		Synchronize();
	}	
	
	//Lock lock
	void LockServer( EntityAI parent, bool ignore_combination = false )
	{
		bsbDebugPrint("[bsb] CombinationLock.LockServer " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
		if ( IsLockAttached() )
		{
			if ( !ignore_combination )
			{
				SetCombinationLocked( m_Combination );
			
				//set slot lock
				InventoryLocation inventory_location = new InventoryLocation;
				GetInventory().GetCurrentInventoryLocation( inventory_location );		
				parent.GetInventory().SetSlotLock( inventory_location.GetSlot(), true );
			
				m_LockActionPerformed = LockAction.LOCKED;
			}
			ShuffleLock();
			SetTakeable(false);
			CheckLockedStateServer();
			//synchronize
			Synchronize();
		}
		
		//reset performed action
		//m_LockActionPerformed = LockAction.NONE;
	}
	
	void UnlockServer( EntityAI player, EntityAI parent )
	{
		bsbDebugPrint("[bsb] CombinationLock.UnlockServer " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
		if ( IsLockAttached() )
		{
			Fence fence = Fence.Cast( parent );
			
			//set slot unlock
			InventoryLocation inventory_location = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( inventory_location );			
			fence.GetInventory().SetSlotLock( inventory_location.GetSlot(), false );			
	
			//drop entity from attachment slot
			if (player)
				player.ServerDropEntity( this );
			else
				parent.GetInventory().DropEntity(InventoryMode.SERVER, parent, this);
			SetPosition( fence.GetKitSpawnPosition() );
			PlaceOnSurface();
			
			m_LockActionPerformed = LockAction.UNLOCKED;
			SetTakeable(true);
			CheckLockedStateServer();
			//synchronize
			Synchronize();
		}
		
		//reset performed action
		//m_LockActionPerformed = LockAction.NONE;		
	}
		
	//Shuffle lock
	void ShuffleLock()
	{
		string combination_text = m_Combination.ToString();
		string shuffled_text;
		
		//insert zeros to dials with 0 value
		int length_diff = m_LockDigits - combination_text.Length();
		for ( int i = 0; i < length_diff; ++i )
		{
			combination_text = "0" + combination_text;
		}
		
		//assemble the whole combination with increased part
		for ( int j = 0; j < combination_text.Length(); ++j )
		{
			int dial_number = combination_text.Get( j ).ToInt();
			dial_number = ( dial_number + Math.RandomInt( 1, 9 ) ) % 10;
			shuffled_text = shuffled_text + dial_number.ToString();
		}
		
		SetCombination( shuffled_text.ToInt() );
	}
	
	bool IsLocked()
	{
		return m_IsLocked;
	}
	
	void CheckLockedStateServer()
	{
		m_IsLocked = m_Combination != m_CombinationLocked;
	}
	
	bool IsLockedOnGate()
	{
		Fence fence = Fence.Cast( GetHierarchyParent() );
		if ( fence )
		{
			if ( IsLocked() )
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool IsLockAttached()
	{
		Fence fence = Fence.Cast( GetHierarchyParent() );
		if ( fence )
		{
			return true;
		}
		
		return false;
	}
	
	//destroy lock
	void DestroyLock()
	{
		GetGame().ObjectDelete( this );
	}
		
	// --- VISUALS
	void UpdateVisuals()
	{
		//Client/Server
		if ( IsLockedOnGate() )
		{
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( HideItem, 		0, false );
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ShowAttached, 	0, false );
		}
		else
		{
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ShowItem, 		0, false );
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( HideAttached, 	0, false );
		}
	}
	
	void UpdateSound()
	{
		//was locked
		if ( m_LockActionPerformed == LockAction.LOCKED )
		{
			SoundLockClose();
		}
		
		//was unlocked
		if ( m_LockActionPerformed == LockAction.UNLOCKED )
		{
			SoundLockOpen();
		}
		
		//next dial index
		if ( m_LockActionPerformed == LockAction.DIAL_INDEX_CHANGED )
		{
			SoundLockChangeDial();
		}		
		
		//dialed new number
		if ( m_LockActionPerformed == LockAction.DIAL_NUMBER_CHANED )
		{
			SoundLockChangeNumber();
		}
	}
	
	//Show/Hide anims
	protected void ShowItem()
	{
		SetAnimationPhase( "Combination_Lock_Item", 0 );
		SetAnimationPhase( "Lock_Item_1", 			0 );
		SetAnimationPhase( "Lock_Item_2", 			0 );
	}
	
	protected void HideItem()
	{
		SetAnimationPhase( "Combination_Lock_Item", 1 );
		SetAnimationPhase( "Lock_Item_1", 			1 );
		SetAnimationPhase( "Lock_Item_2", 			1 );
	}
	
	protected void ShowAttached()
	{
		SetAnimationPhase( "Combination_Lock_Attached", 0 );
		SetAnimationPhase( "Lock_Attached_1", 			0 );
		SetAnimationPhase( "Lock_Attached_2", 			0 );
	}
	
	protected void HideAttached()
	{
		SetAnimationPhase( "Combination_Lock_Attached", 1 );
		SetAnimationPhase( "Lock_Attached_1", 			1 );
		SetAnimationPhase( "Lock_Attached_2", 			1 );
	}	
	// ---	
	
	//================================================================
	// SOUNDS
	//================================================================
	protected void SoundLockOpen()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_OPEN, 0, 0 );
	}

	protected void SoundLockClose()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_CLOSE, 0, 0 );
	}
	
	void SoundLockChangeNumber()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_CHANGE_NUMBER, 0, 0 );
	}

	void SoundLockChangeDial()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_CHANGE_DIAL, 0, 0 );
	}	
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttachToConstruction);		
		AddAction(ActionNextCombinationLockDial);
		AddAction(ActionDialCombinationLock);
		AddAction(ActionNextCombinationLockDialOnTarget);
		AddAction(ActionDialCombinationLockOnTarget);
	}
}
