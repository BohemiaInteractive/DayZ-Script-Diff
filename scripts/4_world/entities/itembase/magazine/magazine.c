typedef Magazine Magazine_Base;

enum CartridgeType
{
	None = 0,
	Pistol = 1,
	Intermediate = 2,
	FullPower = 3,
	Shell = 4
}

enum ProjectileType
{
	None = 0,
	Tracer = 1,
	AP = 2
}


class AmmoData
{
	bool m_IsValid;
	CartridgeType m_CartridgeType;
	ProjectileType m_ProjectileType;
	
	void AmmoData( string init_type )
	{
		m_IsValid = GetGame().ConfigIsExisting( "CfgMagazines " + init_type );
		if ( m_IsValid )
		{
			m_CartridgeType = GetGame().ConfigGetInt( "CfgMagazines " + init_type + " iconCartridge" );
			m_ProjectileType = GetGame().ConfigGetInt( "CfgMagazines " + init_type + " iconType" );
		}
	}
}

class Magazine : InventoryItemSuper
{
	protected static ref map<string, ref AmmoData>	m_AmmoData;
	ref array<string>								m_CompatiableAmmo;
	ref array<float> 								m_ChanceToJam;
	protected float 								m_ManipulationDamage;
	
	void Magazine () 
	{
		m_ChanceToJam = new array<float>;
		InitReliability(m_ChanceToJam);
		m_ManipulationDamage = ConfigGetFloat("manipulationDamage");
		m_CompatiableAmmo = new array<string>;
		ConfigGetTextArray("ammoItems", m_CompatiableAmmo);
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( !m_AmmoData )
				m_AmmoData = new map<string, ref AmmoData>;
			
			string classname = ClassName();
			if ( !m_AmmoData.Contains(classname) )
			{
				ref AmmoData new_data = new AmmoData( classname );
				if ( new_data.m_IsValid )
					m_AmmoData.Insert( classname, new AmmoData( classname ) );
			}
		}
	}

	//! Gets magazine ammo count
	proto native int GetAmmoCount();
	//! Sets magazine ammo count
	proto native void ServerSetAmmoCount(int ammoCount);
	proto native void LocalSetAmmoCount(int ammoCount);

	/**@fn		AcquireCartridge
	 * @brief	acquires cartridge(damage, type) to magazine
	 * @param[out] ammoDamage \p  damage of the ammo
	 * @param[out] cartTypeName \p	 type name of the ejected ammo
	 * @return	true if acquired
	 **/
	proto bool LocalAcquireCartridge(out float dmg, out string cartTypeName);
	proto bool ServerAcquireCartridge(out float dmg, out string cartTypeName);
	/**@fn		StoreCartridge
	 * @brief	stores cartridge(damage, type) to magazine
	 * @param[in] ammoDamage \p  damage of the cartridge
	 * @param[in] cartTypeName \p	 type name of the stored cartridge
	 * @return	true if stored
	 **/
	proto native bool LocalStoreCartridge(float ammoDamage, string cartTypeName);
	proto native bool ServerStoreCartridge(float ammoDamage, string cartTypeName);

	/**@fn		GetCartridgeAtIndex
	 * @brief	queries cartridge(damage, type) info at specified index
	 * @param[in] cartIndex \p  index of the cartridge.
	 * @param[out] ammoDamage \p  damage of the cartridge
	 * @param[out] cartTypeName \p	 type name of the cartridge
	 * @return	true if index valid
	 **/
	proto bool GetCartridgeAtIndex(int cartIndex, out float dmg, out string cartTypeName);

	/**@fn		SetCartridgeAtIndex
	 * @brief	modifies cartridge(damage, type) info at specified index
	 * @param[in] cartIndex \p  index of the cartridge.
	 * @param[in] ammoDamage \p  damage of the cartridge
	 * @param[in] cartTypeName \p	 type name of the cartridge
	 * @return	true if index valid
	 **/
	proto bool SetCartridgeAtIndex(int cartIndex, out float dmg, out string cartTypeName);
	
	/**@fn		SetCartridgeDamageAtIndex
	 * @brief	modifies cartridge damage info at specified index
	 * @param[in] cartIndex \p  index of the cartridge.
	 * @param[in] ammoDamage \p  damage of the cartridge
	 * @return	true if index valid
	 **/
	proto bool SetCartridgeDamageAtIndex(int cartIndex, float dmg);

	
	static AmmoData GetAmmoData( string classname )
	{
		if ( !m_AmmoData )
			m_AmmoData = new map<string, ref AmmoData>;
		if ( !m_AmmoData.Contains(classname) )
		{
			ref AmmoData new_data = new AmmoData( classname );
			if ( new_data.m_IsValid )
				m_AmmoData.Insert( classname, new AmmoData( classname ) );
			return new_data;
		}
		else
		{
			return m_AmmoData.Get( classname );
		}
	}
	
	bool IsCompatiableAmmo( ItemBase ammo )
	{
		if ( m_CompatiableAmmo && ammo )
			return ( m_CompatiableAmmo.Find( ammo.GetType() ) > -1 );
		else
			return false;
	}
	
	bool CanAddCartridges(int count)
	{
		int spc_avail = GetAmmoMax() - GetAmmoCount();
		return count <= spc_avail;
	}

	//! Adds magazine ammo, MP safe
	void ServerAddAmmoCount(int ammoCount)
	{
		ServerSetAmmoCount(GetAmmoCount() + ammoCount);
	}
	void LocalAddAmmoCount(int ammoCount)
	{
		LocalSetAmmoCount(GetAmmoCount() + ammoCount);
	}
	//! returns max rounds for this mag (returns "count" config value)
	int GetAmmoMax()
	{
		return m_Count;
	}
	//! set max rounds for this mag
	void ServerSetAmmoMax()
	{
		ServerSetAmmoCount( GetAmmoMax() );
	}
	void LocalSetAmmoMax()
	{
		LocalSetAmmoCount( GetAmmoMax() );
	}
	//! Returns if this entity is Magazine
	override bool IsMagazine()
	{
		return true;
	}
	
	
	override bool CanBeSplit()
	{
		if ( m_CanThisBeSplit )
			return ( GetAmmoCount() > 1 );

		return false;
	}
	
	bool InitReliability(out array<float> reliability_array)
	{
		if (GetGame().ConfigIsExisting("cfgMagazines " + GetType() + " Reliability ChanceToJam"))
		{
			GetGame().ConfigGetFloatArray("cfgMagazines " + GetType() + " Reliability ChanceToJam",reliability_array);
			return true;
		}
		return false;
	}
	
	float GetChanceToJam()
	{
		int level = GetHealthLevel();

		if (level >= 0 && level < m_ChanceToJam.Count())
			return m_ChanceToJam[level];
		else
			return 0.0;
	}
	/*
	override void SplitItem()
	{
		if ( !CanBeSplit() )
		{
			return;
		}
		
		float quantity = this.GetAmmoCount();
		float split_quantity_new = Math.Floor( quantity / 2 );
		
		this.AddAmmoCount(-split_quantity_new);
		
		PlayerBase player = this.GetHierarchyRootPlayer();
		Magazine new_item;
		if( player )
		{
			new_item = player.CopyInventoryItem( this );
		}
		else
		{
			new_item = GetGame().CreateObject(this.GetType(), this.GetPosition() );
		}
			
		new_item.SetAmmoCount( split_quantity_new );
	}
	*/
	
	override void SplitItemToInventoryLocation( notnull InventoryLocation dst )
	{
		if ( !CanBeSplit() )
			return;
		
		Magazine new_pile = Magazine.Cast( GameInventory.LocationCreateEntity( dst, GetType(), ECE_IN_INVENTORY, RF_DEFAULT ) );
		if( new_pile )
		{
			MiscGameplayFunctions.TransferItemProperties(dst.GetItem(), new_pile);
		
			new_pile.ServerSetAmmoCount(0);
			int quantity = GetAmmoCount();
		
			for (int i = 0; i < Math.Floor( quantity * 0.5 ); ++i)
			{
				float damage;
				string cartrige_name;
				ServerAcquireCartridge(damage, cartrige_name);
				new_pile.ServerStoreCartridge(damage, cartrige_name);
			}
			new_pile.SetSynchDirty();
			SetSynchDirty();
		}
	}

	override void SplitItem(PlayerBase player)
	{
		if ( !CanBeSplit() )
			return;

		
		Magazine new_pile = Magazine.Cast( player.CreateCopyOfItemInInventoryOrGround( this ) );
		if( new_pile )
		{
			new_pile.ServerSetAmmoCount(0);
			int quantity = this.GetAmmoCount();
		
			for (int i = 0; i < Math.Floor( quantity / 2 ); i++)
			{
				float damage;
				string cartrige_name;
				ServerAcquireCartridge(damage, cartrige_name);
				new_pile.ServerStoreCartridge(damage, cartrige_name);
			}
			new_pile.SetSynchDirty();
			SetSynchDirty();
		}
	}
	
	void ApplyManipulationDamage()
	{
		AddHealth("","Health",-m_ManipulationDamage);
	}

	override bool IsFullQuantity()
	{
		if ( GetAmmoCount() == GetAmmoMax() )
		{
			return true;			
		}
		else
		{
			return false;			
		}
	}

	//TODO add functionality for multiple ammo types (does not exist yet)
	override int GetWeight()
	{
		float item_wetness = GetWet();
		int AmmoWeight = 0;
		int AmmoTypeWeight = 0;
		array<string> ammo_array = new array<string>;
		
		ConfigGetTextArray ("ammoItems", ammo_array);
		if (ammo_array.Count() > 0)
		{
			string AmmoType = ammo_array.Get(0);
			
			AmmoTypeWeight = GetGame().ConfigGetInt("cfgMagazines " + AmmoType + " weight");
			AmmoWeight = GetAmmoCount() * AmmoTypeWeight;
		}
		return Math.Round((item_wetness + 1) * m_ConfigWeight + AmmoWeight);
	}

	/*
	override void CombineItems( ItemBase other_item )
	{
		if( !CanBeCombined(other_item) ) return;
		if( other_item.GetType() != this.GetType() ) return;
		
		Magazine other_magazine = other_item;
		float other_item_quantity = other_magazine.GetAmmoCount();
		float this_free_space = this.GetAmmoMax() - this.GetAmmoCount();
		float quantity_used = 0;
		
		if( other_item_quantity > this_free_space )
		{
			quantity_used = this_free_space;
		}
		else
		{
			quantity_used = other_item_quantity;
		}
		if( quantity_used!= 0 )
		{
			this.AddAmmoCount(quantity_used);
			other_magazine.AddAmmoCount(-quantity_used);
			if(other_magazine.GetAmmoCount() <= 0) other_magazine.Delete();
		}
	}
	*/
	
	override bool IsCombineAll( ItemBase other_item, bool use_stack_max = false)
	{
		Magazine other_magazine = Magazine.Cast(other_item);
		int free_space = GetAmmoMax() - GetAmmoCount();
		
		return free_space >= other_magazine.GetAmmoCount();
	}
	
	override void CombineItems( ItemBase other_item, bool use_stack_max = false )
	{
		if ( !CanBeCombined(other_item) )
			return;
		
		if ( other_item.GetType() != GetType() )
			return;
		
		Magazine other_magazine;
		if ( Class.CastTo(other_magazine, other_item) )
		{
			//int other_item_quantity = other_magazine.GetAmmoCount();
			int this_free_space = GetAmmoMax() - GetAmmoCount();
			int numberOfTransferredBullets = 0;
			int currentAmount = GetAmmoCount();
			
			for (int i = 0; i < this_free_space && other_magazine.GetAmmoCount() > 0 ; i++)
			{
				float damage;
				string cartrige_name;
				other_magazine.ServerAcquireCartridge(damage, cartrige_name);
				if (ServerStoreCartridge(damage, cartrige_name))
					++numberOfTransferredBullets;
			}
			
			if (GetGame().IsServer())
			{
				float resultingHealth = (currentAmount * GetHealth() + numberOfTransferredBullets * other_magazine.GetHealth()) / GetAmmoCount();
				SetHealth("", "", resultingHealth);
			}
			
			other_magazine.SetSynchDirty();
			SetSynchDirty();
		}
	}
	
	override bool CanDetachAttachment(EntityAI parent)
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player)
		{
			Weapon_Base wpn = Weapon_Base.Cast(parent);
			if (wpn)
			{
				return player.GetWeaponManager().CanDetachMagazine(wpn,this);
			}
		}
		return super.CanDetachAttachment(parent);
	}
	
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		
		PlayerBase p = PlayerBase.Cast(player);
		p.GetWeaponManager().OnMagazineInventoryEnter(this);
	}
	
	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);

		PlayerBase p = PlayerBase.Cast(player);
		p.GetWeaponManager().OnMagazineInventoryExit(this);
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached(parent, slot_id);
		
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		Weapon_Base wpn = Weapon_Base.Cast(parent);
		if (wpn && player)
		{
			player.GetWeaponManager().OnMagazineAttach(this);
		}
		/*PlayerBase player = PlayerBase.Cast(parent);
		if (player)
			//player.SwitchItemTypeAttach(this, slot_id);
		//{
		//	player.UpdateQuickBarEntityVisibility(this);
		//}*/
		//Print("OnWasAttached: " + GetType());
	}
	
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached(parent, slot_id);
		
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		Weapon_Base wpn = Weapon_Base.Cast(parent);
		
		if (wpn && player)
		{
			player.GetWeaponManager().OnMagazineDetach(this);
		}
	}
	
	override void EEHealthLevelChanged( int oldLevel, int newLevel, string zone )
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		float damage = 1 - GetHealthLevelValue(newLevel) + 0.001;
			
		int cartridgeCount = GetAmmoCount();		
		for (int i = 0; i < cartridgeCount; ++i)
			SetCartridgeDamageAtIndex(i, damage);
	}
};

class MagazineStorage : Magazine
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionLoadMagazine);
		AddAction(ActionEmptyMagazine);
		AddAction(ActionLoadMagazineQuick);
	}
}
