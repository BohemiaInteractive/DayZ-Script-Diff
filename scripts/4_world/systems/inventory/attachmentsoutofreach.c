class AttachmentsOutOfReach
{
	protected static ref map<string, ref map<int, vector>> m_AttData;
	
	static bool IsAttachmentReachable(EntityAI e, string att_slot_name = "", int slot_id = -1, float range = 1.5)
	{
		if( !e.IgnoreOutOfReachCondition() )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if( player.IsInVehicle() )
			{
				return false;
			}
			else
			{
				
				vector pos_att;

				if( att_slot_name != "" )
				{
					if( e.MemoryPointExists(att_slot_name) )
					{
						vector mem_point = e.GetMemoryPointPos(att_slot_name);
						pos_att = e.ModelToWorld(mem_point);
					}
					else
					{
						pos_att = e.ModelToWorld(GetAttachmentPosition(e, InventorySlots.GetSlotIdFromString( att_slot_name ) ));
					}
					
				}
				else if( slot_id != -1 )
					pos_att = e.GetPosition() + GetAttachmentPosition(e, slot_id);
				
				vector pos_player = player.GetPosition();
				
				float height_diff = Math.AbsFloat( pos_player[1] - pos_att[1] );
				if( height_diff < range )
				{
					pos_player[1] = 0;
					pos_att[1] = 0;
					if ( vector.Distance(pos_player, pos_att) <= range )
					{
						return true;
					}
					return false;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return true;
		}
	}
	
	static vector GetAttachmentPosition(EntityAI e, int slot_id)
	{
		if ( m_AttData == NULL )
		{
			m_AttData = new map<string, ref map<int, vector>>();
		}
		
		string type_name = e.GetType();
		
		if ( !m_AttData.Contains( type_name ) )
		{
			map<int, vector> att = CreateAttachmentPosition( e );
			
			m_AttData.Insert( type_name, att );
		}
		
		return m_AttData.Get( type_name ).Get( slot_id );
	}
	
	static protected map<int, vector> CreateAttachmentPosition( EntityAI entity )
	{
		map<int, vector> ret_val = new map<int, vector>();
		
		string			type_name = entity.GetType();
		TStringArray	cfg_attachments = new TStringArray;
		
		string cfg_path;
		
		if ( GetGame().ConfigIsExisting(CFG_VEHICLESPATH+" "+type_name) )
		{
			cfg_path = CFG_VEHICLESPATH+" "+type_name+" attachments";
		}
		else if ( GetGame().ConfigIsExisting(CFG_WEAPONSPATH+" "+type_name) )
		{
			cfg_path = CFG_WEAPONSPATH+" "+type_name+" attachments";
		}
		else if ( GetGame().ConfigIsExisting(CFG_MAGAZINESPATH+" "+type_name) )
		{
			cfg_path = CFG_MAGAZINESPATH+" "+type_name+" attachments";
		}
		
		GetGame().ConfigGetTextArray(cfg_path, cfg_attachments);
		
		int child_count = GetGame().ConfigGetChildrenCount("CfgNonAIVehicles");
		
		for ( int x = 0; x < child_count; ++x )
		{
			string child_name;
			GetGame().ConfigGetChildName("CfgNonAIVehicles",x ,	child_name);
			
			string inventory_slot_name;
			GetGame().ConfigGetText("CfgNonAIVehicles "+ child_name +" inventorySlot", inventory_slot_name);
			
			if ( cfg_attachments.Find( inventory_slot_name ) > 0 )
			{
				string model_path;
				GetGame().ConfigGetText("CfgNonAIVehicles "+ child_name +" model", model_path);
				
				if ( model_path.Length() > 5 )
				{										
					LOD lod = entity.GetLODByName(LOD.NAME_VIEW);
					
					if ( lod )
					{
						model_path = model_path.Substring(0, model_path.Length() - 4);
						model_path.ToLower();
						
						array<Selection> selections = new array<Selection>();
						lod.GetSelections(selections);
						
						for ( int i = 0; i < selections.Count(); ++i )
						{							
							string selection = selections[i].GetName();
							selection.ToLower();
							
							if ( selection.Contains(model_path) )
							{
								ret_val.Set(InventorySlots.GetSlotIdFromString( inventory_slot_name ), selections[i].GetVertexPosition(lod, 0));
							}
						}
					}
				}
			}
		}
		
		return ret_val;
	}
}