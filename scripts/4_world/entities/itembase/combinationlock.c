const float DIGIT_ROTATION_POSITION_0 = 0.9;
const float DIGIT_ROTATION_POSITION_1 = 0.0;
const float DIGIT_ROTATION_POSITION_2 = 0.1;
const float DIGIT_ROTATION_POSITION_3 = 0.2;
const float DIGIT_ROTATION_POSITION_4 = 0.3;
const float DIGIT_ROTATION_POSITION_5 = 0.4;
const float DIGIT_ROTATION_POSITION_6 = 0.5;
const float DIGIT_ROTATION_POSITION_7 = 0.6;
const float DIGIT_ROTATION_POSITION_8 = 0.7;
const float DIGIT_ROTATION_POSITION_9 = 0.8;

enum LockAction
{
	NONE,
	DIAL_NUMBER_CHANED_INSIDE,
	DIAL_NUMBER_CHANED_OUTSIDE,
	DIAL_INDEX_CHANGED_INSIDE,
	DIAL_INDEX_CHANGED_OUTSIDE,
	LOCKED,
	UNLOCKED,
	
	COUNT
}

class CombinationLock extends ItemBase
{
	static const string LOCK_SELECTION_OUTSIDE = "lock_attached_outside";
	static const string LOCK_SELECTION_INSIDE = "lock_attached_inside";
	static const int FACING_OUTSIDE = 0;
	static const int FACING_INSIDE = 1;
	
	int m_LockDigits;						//how many digits will the combination contain
	int m_Combination;						//actual combination that is dialed on lock outside
	int m_CombinationInside;				//actual combination that is dialed on lock inside
	int m_CombinationLocked;				//combination that was dialed on lock before the shuffle
	int m_DialIndex;						//index of current combination dial on lock outside
	int m_DialIndexInside;					//index of current combination dial on lock inside
	int m_LockDigitsCount;
	protected bool m_IsLocked;
	
	protected LockAction m_LockActionPerformed = LockAction.NONE;
	
	protected bool m_IsInitialized;
	
	//Sounds
	//build
	const string SOUND_LOCK_OPEN 			= "combinationlock_open_SoundSet";
	const string SOUND_LOCK_CLOSE 			= "combinationlock_close_SoundSet";
	const string SOUND_LOCK_CHANGE_NUMBER	= "combinationlock_changenumber_SoundSet";
	const string SOUND_LOCK_CHANGE_DIAL		= "combinationlock_changedial_SoundSet";
	const string SOUND_LOCK_WRONG_PIN 		= "combinationlock_wrong_combination_SoundSet";

	protected EffectSound m_Sound;

	void CombinationLock()
	{
		SetBaseLockValues();
		
		//synchronized variables
		int combinationLength = Math.Pow(10, m_LockDigits);
		RegisterNetSyncVariableBool("m_IsLocked");
		RegisterNetSyncVariableInt("m_Combination", 0, combinationLength - 1);
		RegisterNetSyncVariableInt("m_CombinationInside", 0, combinationLength - 1);
		RegisterNetSyncVariableInt("m_DialIndex", 0, m_LockDigits - 1);
		RegisterNetSyncVariableInt("m_DialIndexInside", 0, m_LockDigits - 1);
		RegisterNetSyncVariableInt("m_LockActionPerformed", 0, LockAction.COUNT);
		RegisterNetSyncVariableInt("m_LockDigitsCount", 0, combinationLength - 1);
	}
	
	protected void SetBaseLockValues()
	{
		//set lock init values
		m_LockDigits = 3;
		m_Combination = 111;
		m_CombinationInside = m_Combination;
		m_CombinationLocked = 999;
		m_LockDigitsCount = m_CombinationLocked.ToString().Length();
		m_IsLocked = false;
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SetInitialized, 1000, false );
		
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
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		if (g_Game.IsServer())
		{
			EntityAI oldParent = oldLoc.GetParent();
			if (oldParent && oldParent.CanUseConstruction() && oldLoc.GetType() == InventoryLocationType.ATTACHMENT)
				SetTakeable(true);
		}
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		//Check combination lock
		if (g_Game.IsServer())
		{
			if (IsInitialized() && new_owner && new_owner.CanUseConstruction())
			{
				LockServer(new_owner);
			}
		}
	}
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave(ctx);
		
		//write data
		ctx.Write( m_Combination );
		ctx.Write( m_CombinationLocked );
		ctx.Write( m_CombinationInside );
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		//--- Combination Lock data ---
		//combination one (outside lock)
		if (!ctx.Read(m_Combination))
		{
			m_Combination = 0;
			return false;
		}
		
		//combination locked
		if (!ctx.Read(m_CombinationLocked))
		{
			m_CombinationLocked = 0;
			m_LockDigitsCount = 0;
			return false;
		}
		
		//is lock attached
		if (version < 105)					//removed in 105
		{
			bool isLockAttached;
			if (!ctx.Read(isLockAttached))
			{
				return false;
			}
		}
		
		if (version >= 143)						//added with 143
		{
			//combination two (inside lock)
			if (!ctx.Read(m_CombinationInside))
			{
				m_CombinationInside = 0;
				return false;
			}
		}

		return true;
	}
	
	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();		
		
		m_LockDigitsCount = m_CombinationLocked.ToString().Length();
		
		//Check combination lock
		if ( g_Game.IsServer() )
		{
			EntityAI parent = GetHierarchyParent();
			if (parent && (parent.IsInherited(BaseBuildingBase) || parent.CanUseConstruction()))
			{
				LockServer(parent, true);
			}
			else
			{
				//synchronize
				Synchronize();
			}
		}
	}
	
	// --- SYNCHRONIZATION
	void Synchronize()
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] CombinationLock.Synchronize " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
		if ( g_Game.IsServer() )
		{
			SetSynchDirty();
			g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( ResetActionVar, 1000);//synced var used to trigger client sound needs to be reset after triggering the sound
			UpdateVisuals();
			
			if (m_LockActionPerformed != LockAction.NONE)
				UpdateSound();
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
		
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] CombinationLock.OnVariablesSynchronized " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
	}
	
	void SetCombination(int combination, bool setOtherSide = true)
	{
		m_Combination = combination;
		
		//! Set both combinations when lock is not attached and combination is changed (while in hands).
		if (!GetInventory().IsAttachment() && setOtherSide)
			SetCombinationInside(combination, false);
	}
	
	void SetCombinationInside(int combination, bool setOtherSide = true)
	{
		m_CombinationInside = combination;
		
		//! Set both combinations when lock is not attached and combination is changed (while in hands).
		if (!GetInventory().IsAttachment() && setOtherSide)
			SetCombination(combination, false);
	}
	
	void SetCombinationLocked(int combination)
	{
		m_CombinationLocked = combination;
		m_LockDigitsCount = m_CombinationLocked.ToString().Length();
	}
	
	int GetCombination()
	{
		return m_Combination;
	}
	
	int GetCombinationInside()
	{
		return m_CombinationInside;
	}
	
	int GetLockDigits()
	{
		return m_LockDigits;
	}
	
	// --- ACTIONS
	void DialNextNumber(int lockSide = FACING_OUTSIDE)
	{
		bool isInsideLock = (lockSide == FACING_INSIDE);
	
		string combinationText;
		string dialedText;
		
		if (!isInsideLock)
		{
			combinationText = m_Combination.ToString();
		}
		else
		{
			combinationText = m_CombinationInside.ToString();
		}
		
		//insert zeros to dials with 0 value
		int lengthDiff = m_LockDigits - combinationText.Length();
		for (int i = 0; i < lengthDiff; ++i)
		{
			combinationText = "0" + combinationText;
		}
		//assemble the whole combination with increased part
		for (int j = 0; j < combinationText.Length(); ++j)
		{
			if (!isInsideLock && j == m_DialIndex || isInsideLock && j == m_DialIndexInside)
			{
				int nextDialedNumber = combinationText.Get(j).ToInt() + 1;
				if (nextDialedNumber > 9)
				{
					nextDialedNumber = 0;
				}
				
				dialedText += nextDialedNumber.ToString();
			}
			else
			{
				dialedText += combinationText.Get(j);
			}
		}
		
		//set new number
		if (!isInsideLock)
		{
			SetCombination(dialedText.ToInt());
			m_LockActionPerformed = LockAction.DIAL_INDEX_CHANGED_OUTSIDE;
		}
		else
		{
			SetCombinationInside(dialedText.ToInt());
			m_LockActionPerformed = LockAction.DIAL_INDEX_CHANGED_INSIDE;
		}
		
		//synchronize
		Synchronize();
	}
	
	int GetDialIndex()
	{
		return m_DialIndex;
	}
	
	int GetDialIndexInside()
	{
		return m_DialIndexInside;
	}
	
	void SetNextDial(int lockSide = FACING_OUTSIDE)
	{
		bool isInsideLock = (lockSide == FACING_INSIDE);
	
		int dialIndex;
		if (!isInsideLock)
			dialIndex = m_DialIndex;
		else
			dialIndex = m_DialIndexInside;
	
		if (m_LockDigits > 1)
		{
			if (dialIndex <= m_LockDigits - 2)
			{
				dialIndex++;
			}
			else if (dialIndex >= m_LockDigits - 1)
			{
				dialIndex = 0;
			}
		}
		else
		{
			dialIndex = 0;
		}
		
		if (!isInsideLock)
		{
			m_DialIndex = dialIndex;
			//performed action
			m_LockActionPerformed = LockAction.DIAL_NUMBER_CHANED_OUTSIDE;
		}
		else
		{
			m_DialIndexInside = dialIndex;
			//performed action
			m_LockActionPerformed = LockAction.DIAL_NUMBER_CHANED_INSIDE;
		}
		
		//synchronize
		Synchronize();
	}
	
	//Lock lock
	void LockServer(EntityAI parent, bool ignore_combination = false)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] CombinationLock.LockServer " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
		if (IsLockAttached())
		{
			if (!ignore_combination)
			{
				SetCombinationLocked(m_Combination);
				
				//set slot lock
				InventoryLocation inventoryLocation = new InventoryLocation;
				GetInventory().GetCurrentInventoryLocation(inventoryLocation);		
				parent.GetInventory().SetSlotLock(inventoryLocation.GetSlot(), true);
			
				m_LockActionPerformed = LockAction.LOCKED;
			}
			ShuffleLock();
			SetTakeable(false);
			CheckLockedStateServer();
			//synchronize
			Synchronize();
		}
	}
	
	//! Unlock AND drop
	void UnlockOnServer(EntityAI player, EntityAI parent, bool isInsideLock = false)
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] CombinationLock.UnlockOnServer " + " m_Combination=" + m_Combination + " m_CombinationLocked=" + m_CombinationLocked);
		if (IsLockAttached())
		{
			//set slot unlock
			InventoryLocation inventoryLocation = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation(inventoryLocation);			
			parent.GetInventory().SetSlotLock(inventoryLocation.GetSlot(), false);	
			
			//drop entity from attachment slot
			if (g_Game.IsMultiplayer())
			{
				parent.GetInventory().DropEntity(InventoryMode.SERVER, parent, this);
			}
			else 
			{
				parent.GetInventory().DropEntity(InventoryMode.LOCAL, parent, this);
			}
			
			vector groundPos;
			if (player)
			{
				groundPos = player.GetPosition();
			}
			else
			{
				string slotName = InventorySlots.GetSlotName(inventoryLocation.GetSlot());
				if (parent.MemoryPointExists(slotName))
				{
					groundPos = parent.ModelToWorld(parent.GetMemoryPointPos(slotName));
				}
				else
				{
					groundPos = GetPosition();
				}
			}
			
			SetPosition(groundPos);
			PlaceOnSurface();
			
			m_LockActionPerformed = LockAction.UNLOCKED;
			SetTakeable(true);
			CheckLockedStateServer(isInsideLock);
			
			if (!isInsideLock)
			{
				m_CombinationInside = m_Combination; 
			}
			else
			{
				m_Combination = m_CombinationInside; 
			}
			
			UpdateLockDigits();
			
			//synchronize
			Synchronize();
		}
	}
			
	//Shuffle lock
	protected void ShuffleLock()
	{
		ShuffleCombination(m_Combination.ToString(), false);
		ShuffleCombination(m_CombinationInside.ToString(), true);
	}
	
	protected void ShuffleCombination(string combinationText, bool isInsideLock = false)
	{
		string shuffledText;
		
		//insert zeros to dials with 0 value
		int lengthDiff = m_LockDigits - combinationText.Length();
		for (int i = 0; i < lengthDiff; ++i)
		{
			combinationText = "0" + combinationText;
		}
		
		//assemble the whole combination with increased part
		for (int j = 0; j < combinationText.Length(); ++j)
		{
			int dialNumber = combinationText.Get(j).ToInt();
			dialNumber = ( dialNumber + Math.RandomInt( 1, 9 ) ) % 10;
			shuffledText = shuffledText + dialNumber.ToString();
			if (!isInsideLock)
				SetDigitRotationAnimationPhaseOutside((j+1).ToString(),GetDigitRotation(dialNumber));
			else
				SetDigitRotationAnimationPhaseInside((j+1).ToString(),GetDigitRotation(dialNumber));
		}
		
		if (!isInsideLock)
			SetCombination(shuffledText.ToInt());
		else
			SetCombinationInside(shuffledText.ToInt());
	}
	
	bool IsLocked()
	{
		return m_IsLocked;
	}
	
	void CheckLockedStateServer(bool insideLock = false)
	{
		if (!insideLock)
		{
			m_IsLocked = (m_Combination != m_CombinationLocked);
		}
		else
		{
			m_IsLocked = (m_CombinationInside != m_CombinationLocked);
		}
	}
	
	bool IsLockedOnParent()
	{
		EntityAI parent = GetHierarchyParent();
		if (parent && parent.CanUseConstruction() && IsLocked())
		{
			return true;
		}
		
		return false;
	}
	
	bool IsLockAttached()
	{
		EntityAI parent = GetHierarchyParent();
		if (parent && parent.CanUseConstruction())
		{
			return true;
		}
		
		return false;
	}
	
	//destroy lock
	protected void DestroyLock()
	{
		g_Game.ObjectDelete( this );
	}
		
	// --- VISUALS
	protected void UpdateVisuals()
	{
		//Client/Server
		if (IsLockedOnParent())
		{
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( HideItem, 		0, false );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ShowAttached, 	0, false );
		}
		else
		{
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( ShowItem, 		0, false );
			g_Game.GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( HideAttached, 	0, false );
		}
		
		bool changedOutside = (m_LockActionPerformed == LockAction.DIAL_INDEX_CHANGED_OUTSIDE);
		bool changedInside = (m_LockActionPerformed == LockAction.DIAL_INDEX_CHANGED_INSIDE);
		
		bool chanedOutside = (m_LockActionPerformed == LockAction.DIAL_NUMBER_CHANED_OUTSIDE);
		bool chanedInside = (m_LockActionPerformed == LockAction.DIAL_NUMBER_CHANED_INSIDE);
		
		string combinationText;		
		if (changedOutside)
		{
			combinationText = m_Combination.ToString();
		}
		else if (changedInside)
		{
			combinationText = m_CombinationInside.ToString();
		}
		
		//insert zeros to dials with 0 value
		int lengthDiff = m_LockDigits - combinationText.Length();
		for (int i = 0; i < lengthDiff; ++i)
		{
			combinationText = "0" + combinationText;
		}
		
		//assemble the whole combination with increased part
		for (int j = 0; j < combinationText.Length(); ++j)
		{			
			if (changedOutside && j == m_DialIndex || changedInside && j == m_DialIndexInside)
			{
				int nextDialedNumber = combinationText.Get(j).ToInt() + 1;				
				if (nextDialedNumber > 9)
				{
					nextDialedNumber = 0;
				}
					
				if (changedOutside)
				{
					AnimateDigitsOutside((j+1).ToString());
				}
				else if (changedInside)
				{
					AnimateDigitsInside((j+1).ToString());
				}
			}
		}
		
		if (!changedOutside && !changedInside && !chanedOutside && !chanedInside)
		{
			UpdateLockDigits();
		}
	}
	
	// --- SOUNDS
	protected void UpdateSound()
	{
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			// Lock was locked
			if (m_LockActionPerformed == LockAction.LOCKED)
			{
				StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_LOCK);
			}
			
			// Lock was unlocked
			if (m_LockActionPerformed == LockAction.UNLOCKED)
			{
				StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_UNLOCK);
			}
			
			// Lock next dial index
			if (m_LockActionPerformed == LockAction.DIAL_INDEX_CHANGED_INSIDE || m_LockActionPerformed == LockAction.DIAL_INDEX_CHANGED_OUTSIDE)
			{
				StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_CHANGE_DIAL);
			}		
			
			// Lock dialed new number
			if (m_LockActionPerformed == LockAction.DIAL_NUMBER_CHANED_INSIDE || m_LockActionPerformed == LockAction.DIAL_NUMBER_CHANED_OUTSIDE)
			{
				StartItemSoundServer(SoundConstants.ITEM_COMBINATION_LOCK_CHANGE_NUMBER);
			}
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
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionAttachToConstruction);		
		AddAction(ActionNextCombinationLockDial);
		AddAction(ActionDialCombinationLock);
		AddAction(ActionNextCombinationLockDialOnTarget);
		AddAction(ActionDialCombinationLockOnTarget);
		AddAction(ActionCombinationLockUnlock);
	}
	
	static float GetDigitRotation(int digitValue)
	{
		switch (digitValue)
		{
			case 0:
				return DIGIT_ROTATION_POSITION_0;
			case 1:
				return DIGIT_ROTATION_POSITION_1;
			case 2:
				return DIGIT_ROTATION_POSITION_2;
			case 3:
				return DIGIT_ROTATION_POSITION_3;
			case 4:
				return DIGIT_ROTATION_POSITION_4;
			case 5:
				return DIGIT_ROTATION_POSITION_5;
			case 6:
				return DIGIT_ROTATION_POSITION_6;
			case 7:
				return DIGIT_ROTATION_POSITION_7;
			case 8:
				return DIGIT_ROTATION_POSITION_8;
			case 9:
				return DIGIT_ROTATION_POSITION_9;
		}
		
		return DIGIT_ROTATION_POSITION_0;
	}
	
	protected void SetDigitRotationAnimationPhaseOutside(string digitIndex, float rot)
	{
		SetAnimationPhase("Lock_Attached_2_Digit" + digitIndex, rot);
		SetAnimationPhase("Lock_Item_2_Digit" + digitIndex, rot);
	}
	
	protected void SetDigitRotationAnimationPhaseInside(string digitIndex, float rot)
	{
		SetAnimationPhase("Lock_Attached_1_Digit" + digitIndex, rot);
		SetAnimationPhase("Lock_Item_1_Digit" + digitIndex, rot);
	}
	
	protected void AnimateDigitsOutside(string digitIndex)
	{
		float animPhaseOffset = 0.1;
		float animPhase = GetAnimationPhase("Lock_Attached_2_Digit" + digitIndex);
		
		if (animPhase >= 1.0)
		{
			animPhase = 0;
			ResetAnimationPhase("Lock_Attached_2_Digit" + digitIndex, animPhase);
			ResetAnimationPhase("Lock_Item_2_Digit" + digitIndex, animPhase);
		}
		
		SetAnimationPhase("Lock_Attached_2_Digit" + digitIndex, animPhase + animPhaseOffset);
		SetAnimationPhase("Lock_Item_2_Digit" + digitIndex, animPhase + animPhaseOffset);
	}
	
	protected void AnimateDigitsInside(string digitIndex)
	{
		float animPhaseOffset = 0.1;
		float animPhase = GetAnimationPhase("Lock_Attached_1_Digit" + digitIndex);
		
		if (animPhase >= 1.0)
		{
			animPhase = 0;
			ResetAnimationPhase("Lock_Attached_1_Digit" + digitIndex, animPhase);
			ResetAnimationPhase("Lock_Item_1_Digit" + digitIndex, animPhase);
		}

		SetAnimationPhase("Lock_Attached_1_Digit" + digitIndex, animPhase + animPhaseOffset);
		SetAnimationPhase("Lock_Item_1_Digit" + digitIndex, animPhase + animPhaseOffset);
	}
	
	protected void UpdateLockDigits()
	{
		//Set digit anim on outside lock
		string combinationText = m_Combination.ToString();
		for (int i = 0; i < combinationText.Length(); ++i)
		{
			SetDigitRotationAnimationPhaseOutside((i+1).ToString(), GetDigitRotation(combinationText.Get(i).ToInt()));
		}
		
		//Set digit anim on inside lock
		combinationText = m_CombinationInside.ToString();
		for (int j = 0; j < combinationText.Length(); ++j)
		{
			SetDigitRotationAnimationPhaseInside((j+1).ToString(), GetDigitRotation(combinationText.Get(j).ToInt()));
		}
	}
	
	override bool IsExternalLockType()
	{
		return true;
	}
	
	int GetLockSideByComponentIndex(int componentIndex)
	{
		array<string> selections = new array<string>;
		GetActionComponentNameList(componentIndex, selections);
		
		int facingSide = -1;
		foreach (string selectionName : selections)
		{
			if (selectionName == LOCK_SELECTION_OUTSIDE)
			{
				facingSide = FACING_OUTSIDE;
				break;
			}
			
			if (selectionName == LOCK_SELECTION_INSIDE)
			{
				facingSide = FACING_INSIDE;
				break;
			}
		}
		
		return facingSide;
	}
	
	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		handler.AddSound(SoundConstants.ITEM_COMBINATION_LOCK_LOCK, SOUND_LOCK_CLOSE);
		handler.AddSound(SoundConstants.ITEM_COMBINATION_LOCK_UNLOCK, SOUND_LOCK_OPEN);
		handler.AddSound(SoundConstants.ITEM_COMBINATION_LOCK_CHANGE_NUMBER, SOUND_LOCK_CHANGE_NUMBER);
		handler.AddSound(SoundConstants.ITEM_COMBINATION_LOCK_CHANGE_DIAL, SOUND_LOCK_CHANGE_DIAL);
		handler.AddSound(SoundConstants.ITEM_COMBINATION_LOCK_WRONG_PIN, SOUND_LOCK_WRONG_PIN);
		handler.AddSound(SoundConstants.ITEM_COMBINATION_LOCK_WRONG_COMBINATION, SOUND_LOCK_WRONG_PIN);
	}
	
	//! DEPRECTAED
	[Obsolete("1.30: Use ItemSoundHandler instead")]
	protected void SoundLockOpen()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_OPEN, 0, 0 );
	}

	[Obsolete("1.30: Use ItemSoundHandler instead")]
	protected void SoundLockClose()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_CLOSE, 0, 0 );
	}
	
	[Obsolete("1.30: Use ItemSoundHandler instead")]
	protected void SoundLockChangeNumber()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_CHANGE_NUMBER, 0, 0 );
	}

	[Obsolete("1.30: Use ItemSoundHandler instead")]
	protected void SoundLockChangeDial()
	{
		PlaySoundSet( m_Sound, SOUND_LOCK_CHANGE_DIAL, 0, 0 );
	}
	
	[Obsolete("1.30: Use IsLocked instead")]
	bool IsLockedOnGate()
	{
		Fence fence = Fence.Cast(GetHierarchyParent());
		if (fence)
		{
			if (IsLocked())
			{
				return true;
			}
		}
		
		return false;
	}
	
	[Obsolete("1.30: Use UnlockOnServer instead")]
	void Unlock();
	[Obsolete("1.30: Use SetDigitRotationAnimationPhaseOutside & SetDigitRotationAnimationPhaseInside instead")]
	void SetDigitRotationAnimationPhase(string digitIndex, float rot);
	[Obsolete("1.30: Use AnimateDigitsOutside & AnimateDigitsInside instead")]
	void AnimateDigits(string digitIndex);
}