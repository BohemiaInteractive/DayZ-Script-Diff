//-----------------------------
// LIFESPAN plugin
//-----------------------------
/*
Lifespan plugin handles player facial hair, bloody hands, blood type in HUD
*/

enum eBloodyHandsTypes
{
	CLEAN = 0,//clean needs to be 0
	SALMONELA,
	JUST_BLOOD,
	//--- ONLY LAST_INDEX BELLOW !!!
	LAST_INDEX,
}

enum LifeSpanState
{
	BEARD_NONE		= 0,
	BEARD_MEDIUM	= 1,
	BEARD_LARGE		= 2,	
	BEARD_EXTRA		= 3,	
	COUNT			= 4,
}

class PluginLifespan extends PluginBase
{
	protected static const int LIFESPAN_MIN = 0;
	protected static const int LIFESPAN_MAX = 240; // value in minutes when player achieved maximum age in order to have full beard
	protected int m_FakePlaytime;
	
	protected ref map<PlayerBase, ref LifespanLevel> m_PlayerCurrentLevel;
	protected ref map<string, ref array< ref LifespanLevel>> m_LifespanLevels;
	protected ref map<string, ref BloodyHands> m_BloodyHands;
	protected ref map<PlayerBase, int> m_BloodType;

	//========================================
	// GetInstance
	//========================================
	static PluginLifespan GetInstance()
	{
		return PluginLifespan.Cast( GetPlugin(PluginLifespan) );
	}
	
	void PluginLifespan()
	{
		LoadFromCfg();
		m_PlayerCurrentLevel = new map<PlayerBase, ref LifespanLevel>;
		m_BloodType = new map<PlayerBase, int>;
		m_FakePlaytime = 0;
	}

//-----------------------------
// Player load from config
//-----------------------------
	void LoadFromCfg()
	{
		m_LifespanLevels = new map<string, ref array< ref LifespanLevel>>;
		m_BloodyHands = new map<string, ref BloodyHands>;
		
		const int LIFESPAN_LENGTH = LIFESPAN_MAX - LIFESPAN_MIN;

		//cfg_name = 'CfgVehicles'
		//	config_name = 'CfgVehicles'
		//cfg_class_count = 2348
		//	config_count = 2348
		//cfg_class_name = 'SurvivorMale_Base'
		//	survivor_name = 'SurvivorMale_Base'
		//cfg_class_fullname = 'CfgVehicles SurvivorMale_Base'
		//	survivor_path = 'CfgVehicles SurvivorMale_Base'
		//cfg_class_member_count = 10
		//	survivor_lifespan_count = 10
		//cfg_class_member_name = 'Lifespan'
		//	survivor_lifespan_name = 'Lifespan'
		//cfg_class_member_fullname = 'CfgVehicles SurvivorMale_Base Lifespan';
		//	survivor_lifespan_path = 'CfgVehicles SurvivorMale_Base Lifespan';
		//cfg_class_member_member_count = 1
		//	survivor_lifespan_beard_count = 1
		//cfg_class_member_member_name = 'Beard'
		//	survivor_lifespan_beard_name = 'Beard'
		//cfg_class_member_member_fullname = 'CfgVehicles SurvivorMale_Base Lifespan Beard'
		//	survivor_lifespan_beard_path = 'CfgVehicles SurvivorMale_Base Lifespan Beard'
		//cfg_class_member_member_variable_name = 'mat'
		//	survivor_lifespan_beard_material_name = 'mat'
		//cfg_class_member_member_variable_fullname = 'CfgVehicles SurvivorMale_Base Lifespan Beard mat'
		//	survivor_lifespan_beard_material_path = 'CfgVehicles SurvivorMale_Base Lifespan Beard mat'

		string config_name = "CfgVehicles";
		int config_count = GetGame().ConfigGetChildrenCount( config_name );
			
		int i, j, k, l, m;
		
		//Print( "config_count: " + config_count );
		for ( i = 0; i < config_count; i++ )
		{
			string survivor_name = "";
			GetGame().ConfigGetChildName( config_name, i, survivor_name );
			
			if ( survivor_name != "" && survivor_name != "access" )
			{
				if ( GetGame().IsKindOf(survivor_name, "SurvivorMale_Base")  ||  GetGame().IsKindOf(survivor_name, "SurvivorFemale_Base") )
				{
					string survivor_path = config_name + " " + survivor_name;
					int survivor_lifespan_count = GetGame().ConfigGetChildrenCount( survivor_path );

					//Print( "survivor_path: " + survivor_path );
					for ( j = 0; j < survivor_lifespan_count; j++ )
					{
						string survivor_lifespan_name = "";
						GetGame().ConfigGetChildName( survivor_path, j, survivor_lifespan_name );
						
						string survivor_lifespan_path = survivor_path + " " + survivor_lifespan_name;

						if ( survivor_lifespan_name == "Lifespan" )
						{
							int survivor_lifespan_beard_count = GetGame().ConfigGetChildrenCount( survivor_lifespan_path );
							
							for ( k = 0; k < survivor_lifespan_beard_count; k++ )
							{
								string survivor_lifespan_beard_name = "";
								GetGame().ConfigGetChildName( survivor_lifespan_path, k, survivor_lifespan_beard_name );
								
								string survivor_lifespan_beard_path = survivor_lifespan_path + " " + survivor_lifespan_beard_name;
																
								if ( survivor_lifespan_beard_name == "Beard" )
								{									
									ref TStringArray materials = new TStringArray;
									int cfg_class_member_member_variable_count = GetGame().ConfigGetChildrenCount( survivor_lifespan_beard_path );
									
									for ( l = 0; l < cfg_class_member_member_variable_count; l++ )
									{
										string survivor_lifespan_beard_material_name = "";
										GetGame().ConfigGetChildName( survivor_lifespan_beard_path, l, survivor_lifespan_beard_material_name );
										string survivor_lifespan_beard_material_path = survivor_lifespan_beard_path + " " + survivor_lifespan_beard_material_name;
										
										if ( survivor_lifespan_beard_material_name == "mat" )
										{
											GetGame().ConfigGetTextArray( survivor_lifespan_beard_material_path, materials );
											
											ref array<ref LifespanLevel> lifespan_levels = new array< ref LifespanLevel>;

											int level_count = materials.Count() / 2;
											for ( m = 0; m < level_count; m++ )
											{
												int tex = m*2;
												int mat = ((m*2)+1);
												if ( mat < materials.Count() )
												{
													float threshold = (((float)m / (float)(level_count - 1)) * (float)LIFESPAN_LENGTH) + (float)LIFESPAN_MIN;
													lifespan_levels.Insert( new LifespanLevel(m, threshold, materials.Get(tex), materials.Get(mat)) );
													//Print("material a textura a threshold: " + materials.Get(tex) + " " + materials.Get(mat) + " " + threshold );
												}
											}
											
											if ( lifespan_levels.Count() > 0 )
											{
												m_LifespanLevels.Set( survivor_name, lifespan_levels );
											}
										}
									}
								}	
							}
						}
						else if ( survivor_lifespan_name == "BloodyHands" )
						{
							string bloody_material, normal_material;
							string path_normal = survivor_lifespan_path + " mat_normal";
							string path_bloody = survivor_lifespan_path + " mat_blood";
							GetGame().ConfigGetText(path_normal, normal_material);
							GetGame().ConfigGetText(path_bloody, bloody_material);
							m_BloodyHands.Set( survivor_name, new BloodyHands(normal_material, bloody_material) );
						}
					}
				}
			}
		}
		
		/*for ( int cl = 0; cl < m_LifespanLevels.Count(); cl++ )
		{
			string class_name = m_LifespanLevels.GetKey( cl );
			array<ref LifespanLevel> levels = m_LifespanLevels.GetElement( cl );

			Print( class_name );

			for ( int ll = 0; ll < levels.Count(); ll++)
			{
				Print( "lvl: " + levels.Get( ll ).GetLevel() );
				Print( "treshold: " + levels.Get( ll ).GetThreshold() );
				Print( "texture: " + levels.Get( ll ).GetTextureName() );
				Print( "material: " + levels.Get( ll ).GetMaterialName() );
			}
		}*/
	}

//-----------------------------
// Support functions
//-----------------------------
	
	void SynchLifespanVisual( PlayerBase player, int beard_state_visible, bool bloody_hands_visible, bool blood_type_visible, int blood_type )
	{
		SynchBeardVisual( player, beard_state_visible );
		SynchBloodyHandsVisual( player, bloody_hands_visible );
		SynchShowBloodTypeVisual( player, blood_type_visible );
		SynchShowBloodType( player, blood_type );
	}
	
	void ChangeFakePlaytime( PlayerBase player, int change )
	{
		if ( !GetGame().IsMultiplayer() )
		{
			m_FakePlaytime = change;
			UpdateLifespan( player, true );
		}
	}
	
//-----------------------------
// Facial hair
//-----------------------------
	
	void UpdateLifespan( PlayerBase player, bool force_update = false )
	{
		if ( player != NULL )
		{
			// NEW STATS API
			if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			{
				float player_playtime = player.StatGet("playtime");
			}
			else
			{
				player_playtime = m_FakePlaytime;
			}
						
			float player_beard = player_playtime - player.GetLastShavedSeconds();
			player_beard = player_beard / 60.0;
			
			UpdateLifespanLevel( player, player_beard, force_update );
		}
	} 

	protected void UpdateLifespanLevel( PlayerBase player, float player_beard, bool force_update = false )
	{
		if ( !player.IsAlive() )
			return;
		
		if ( m_PlayerCurrentLevel.Contains(player) )
		{
			LifespanLevel current_level = m_PlayerCurrentLevel.Get( player );

			if ( player_beard > current_level.GetThreshold() || force_update )
			{
				LifespanLevel next_level = GetLifespanLevel( player.GetPlayerClass(), player_beard );
			
				if ( next_level != NULL )
				{
					SetPlayerLifespanLevel( player, next_level );
					m_PlayerCurrentLevel.Set( player, next_level );
				}
			}
		}
		else
		{
			if ( m_LifespanLevels.Contains( player.GetPlayerClass() ) )
			{
				LifespanLevel level = GetLifespanLevel( player.GetPlayerClass(), player_beard );

				if ( level != NULL )
				{
					SetPlayerLifespanLevel( player, level );
					m_PlayerCurrentLevel.Set( player, level );
				}
			}
		}
	}

	protected LifespanLevel GetLifespanLevel( string player_class, float age = 0 )
	{
		array< ref LifespanLevel> lifespan_levels = m_LifespanLevels.Get( player_class );
		
		if ( lifespan_levels != NULL )
		{
			for ( int i = lifespan_levels.Count() - 1; i >= 0; i-- )
			{
				LifespanLevel level = lifespan_levels.Get( i );
				
				if ( age >= level.GetThreshold() )
				{
					return level;
				}
			}
		}
		
		return NULL;
	}

	protected void SetPlayerLifespanLevel( PlayerBase player, LifespanLevel level )
	{
		if (player.m_CorpseState != 0)
			return;
		int slot_id = InventorySlots.GetSlotIdFromString("Head");	
		EntityAI players_head = player.GetInventory().FindPlaceholderForSlot( slot_id );
		
		if( players_head )
		{
			switch(level.GetLevel())
			{
				case LifeSpanState.BEARD_NONE:
				{
					players_head.SetObjectTexture( 0, "");
					players_head.SetObjectMaterial( 0, "");		
					
					player.SetFaceTexture( level.GetTextureName() );
					player.SetFaceMaterial( level.GetMaterialName() );

					player.SetLifeSpanStateVisible(LifeSpanState.BEARD_NONE);
					//Print("LifeSpanState.BEARD_NONE");
					break;
				}
				case LifeSpanState.BEARD_MEDIUM:
				{
					players_head.SetObjectTexture( 0, "");
					players_head.SetObjectMaterial( 0, "");	
					
					player.SetFaceTexture( level.GetTextureName() );
					player.SetFaceMaterial( level.GetMaterialName() );
										
					player.SetLifeSpanStateVisible(LifeSpanState.BEARD_MEDIUM);
					//Print("LifeSpanState.BEARD_MEDIUM");
					break;
				}
				
				case LifeSpanState.BEARD_LARGE:
				{	
					players_head.SetObjectTexture( 0, "");
					players_head.SetObjectMaterial( 0, "");	
							
					player.SetFaceTexture( level.GetTextureName() );
					player.SetFaceMaterial( level.GetMaterialName() );
			
					player.SetLifeSpanStateVisible(LifeSpanState.BEARD_LARGE);
					//Print("LifeSpanState.BEARD_LARGE");
					break;
				}
				
				case LifeSpanState.BEARD_EXTRA:
				{
					players_head.SetObjectTexture( 0, level.GetTextureName() );
					players_head.SetObjectMaterial( 0, level.GetMaterialName() );
					
					array< ref LifespanLevel> lifespan_levels = m_LifespanLevels.Get( player.GetPlayerClass() );
					LifespanLevel prev_level = lifespan_levels.Get(LifeSpanState.BEARD_LARGE);

					player.SetFaceTexture( prev_level.GetTextureName() );
					player.SetFaceMaterial( prev_level.GetMaterialName() );
					
					player.SetLifeSpanStateVisible(LifeSpanState.BEARD_EXTRA);
					//Print("LifeSpanState.BEARD_EXTRA");
					break;
				}
									
				default:
				{
					Print("Lifespan state missing");
					break;
				}
			}	
		}
	}
	
	void SynchBeardVisual( PlayerBase player, int state )
	{
		float player_beard; 
		
		switch(state)
		{
			case LifeSpanState.BEARD_NONE:
			{
				// first out of 4 states
				player_beard = LIFESPAN_MIN;
				break;
			}
			case LifeSpanState.BEARD_MEDIUM:
			{
				// second out of 4 states
				player_beard = LIFESPAN_MAX/3;
				break;
			}
			
			case LifeSpanState.BEARD_LARGE:
			{
				// third out of 4 states
				player_beard = LIFESPAN_MAX/1.5;
				break;
			}
			
			case LifeSpanState.BEARD_EXTRA:
			{
				// fourth out of 4 states
				player_beard = LIFESPAN_MAX;
				break;
			}
								
			default:
			{
				Print("Lifespan state missing");
				break;
			}
		}
		
		UpdateLifespanLevel( player, player_beard, true );
	}
	
//-----------------------------
// Bloody hands
//-----------------------------
	
	void UpdateBloodyHandsVisibilityEx( PlayerBase player, eBloodyHandsTypes type )
	{
		if ( CanMakeHandsBloody( player ) )
		{
			SetBloodyHandsEx( player, type );			
		}
	}
	
	void UpdateBloodyHandsVisibility( PlayerBase player, bool show )
	{
		if ( CanMakeHandsBloody( player ) )
		{
			SetBloodyHands( player, show );			
		}
	}
	
	void SynchBloodyHandsVisual( PlayerBase player, bool has_bloody_hands )
	{
		UpdateBloodyHandsVisibility( player, has_bloody_hands );
	}
	

	void SetBloodyHandsEx( PlayerBase player, eBloodyHandsTypes type )
	{
		player.SetBloodyHandsEx( type );
		
		if ( type )
		{
			SetHandsMaterial( player, BloodyHands.MATERIAL_TYPE_BLOODY );
		}
		else
		{
			SetHandsMaterial( player, BloodyHands.MATERIAL_TYPE_NORMAL );
		}
		
	}
	
	void SetBloodyHands( PlayerBase player, bool show )
	{
		player.SetBloodyHands( show );
		
		if ( show )
		{
			SetHandsMaterial( player, BloodyHands.MATERIAL_TYPE_BLOODY );
		}
		else
		{
			SetHandsMaterial( player, BloodyHands.MATERIAL_TYPE_NORMAL );
		}
	}

	protected void SetHandsMaterial( PlayerBase player, int material_type )
	{
		string player_class = player.GetPlayerClass();
		int slot_id;
		EntityAI eai;

		if ( m_BloodyHands.Contains(player_class))
		{	
			slot_id = InventorySlots.GetSlotIdFromString("Gloves");
			eai = player.GetInventory().FindPlaceholderForSlot( slot_id );
			
			if (eai)
			{
				eai.SetObjectMaterial( 0, m_BloodyHands.Get(player_class).GetMaterial(material_type) );
			}
		}
		else
		{
			//Print("Error! Player class <" + player_class + "> does not contain valid configuration for bloody hands!");
		}
	}

	bool CanMakeHandsBloody( PlayerBase player )
	{
		if ( !player.FindAttachmentBySlotName( "Gloves" ) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
//-----------------------------
// Blood type in HUD
//-----------------------------
	
	void UpdateBloodTypeVisibility( PlayerBase player, bool show )
	{
		player.SetBloodTypeVisible( show );
		
		if ( player.m_Hud )
		{
			player.m_Hud.UpdateBloodName();
		}
	}
	
	void UpdateBloodType( PlayerBase player, int blood_type )
	{
		player.SetBloodType( blood_type );
	}
	
	void SynchShowBloodTypeVisual( PlayerBase player, bool show )
	{
		UpdateBloodTypeVisibility( player, show );
	}

	void SynchShowBloodType( PlayerBase player, int blood_type )
	{
		UpdateBloodType( player, blood_type );
	}
	
//-----------------------------
// Getters for other systems
//-----------------------------
	
	string GetCurrentHeadTexture(PlayerBase player)
	{
		if ( m_PlayerCurrentLevel.Contains(player) )
		{
			LifespanLevel current_level = m_PlayerCurrentLevel.Get( player );
			return current_level.GetTextureName();
		}
		return "";
	}
	
	string GetCurrentHeadMaterial(PlayerBase player)
	{
		if ( m_PlayerCurrentLevel.Contains(player) )
		{
			LifespanLevel current_level = m_PlayerCurrentLevel.Get( player );
			return current_level.GetMaterialName();
		}
		return "";
	}
}