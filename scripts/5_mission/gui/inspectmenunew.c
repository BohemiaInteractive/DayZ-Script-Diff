//--------------------------------------------------------------------------
class InspectMenuNew extends UIScriptedMenu
{
	private ItemPreviewWidget m_item_widget;
	private ItemPreviewWidget m_slot_widget;
	private int m_characterRotationX;
	private int m_characterRotationY;
	private int m_characterScaleDelta;
	private vector m_characterOrientation;

	
	void InspectMenuNew()
	{
		
	}
	
	//--------------------------------------------------------------------------
	void ~InspectMenuNew()
	{
		GetGame().GetDragQueue().RemoveCalls(this);
		if (GetGame().GetMission())
		{
			GetGame().GetMission().GetHud().ShowHudUI( true );
			GetGame().GetMission().GetHud().ShowQuickbarUI( true );
		}
	}
	
	//--------------------------------------------------------------------------
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/inventory_new/day_z_inventory_new_inspect.layout");
		
		
		return layoutRoot;
	}
	
	//--------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		
		switch (w.GetUserID())
		{
			case IDC_CANCEL:
				Close();
				return true;
		}
	
		return false;
	}
	
	//--------------------------------------------------------------------------
	void SetItem(EntityAI item)
	{
		if (item)
		{
			InspectMenuNew.UpdateItemInfo(layoutRoot, item);
	
			if (!m_item_widget)
			{
				Widget preview_frame = layoutRoot.FindAnyWidget("ItemFrameWidget");
				if (preview_frame)
				{
					m_item_widget = ItemPreviewWidget.Cast( preview_frame );
				}
			}
			
			m_item_widget.SetItem(item);
			m_item_widget.SetView( item.GetViewIndex() );
			m_item_widget.SetModelPosition(Vector(0,0,1));
			PPERequesterBank.GetRequester(PPERequester_InventoryBlur).Start();
		}
	}
	
	//--------------------------------------------------------------------------
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		super.OnMouseButtonDown(w, x, y, button);
		
		if (w == m_item_widget)
		{
			GetGame().GetDragQueue().Call(this, "UpdateRotation");
			g_Game.GetMousePos(m_characterRotationX, m_characterRotationY);
			return true;
		}
		return false;
	}
	
	//--------------------------------------------------------------------------
	void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging)
	{
		vector o = m_characterOrientation;
		o[0] = o[0] + (m_characterRotationY - mouse_y);
		o[1] = o[1] - (m_characterRotationX - mouse_x);
		
		m_item_widget.SetModelOrientation( o );
		
		if (!is_dragging)
		{
			m_characterOrientation = o;
		}
	}
	
	//--------------------------------------------------------------------------
	override bool OnMouseWheel(Widget  w, int  x, int  y, int wheel)
	{
		super.OnMouseWheel(w, x, y, wheel);
		
		if ( w == m_item_widget )
		{
			m_characterScaleDelta = wheel;
			UpdateScale();
		}
		return false;
	}
	
	//--------------------------------------------------------------------------
	void UpdateScale()
	{
		float w, h, x, y;		
		m_item_widget.GetPos(x, y);
		m_item_widget.GetSize(w,h);
		w = w + ( m_characterScaleDelta / 4);
		h = h + ( m_characterScaleDelta / 4 );
		if ( w > 0.5 && w < 3 )
		{
			m_item_widget.SetSize( w, h );
	
			// align to center 
			int screen_w, screen_h;
			GetScreenSize(screen_w, screen_h);
			float new_x = x - ( m_characterScaleDelta / 8 );
			float new_y = y - ( m_characterScaleDelta / 8 );
			m_item_widget.SetPos( new_x, new_y );
		}
	}
	//--------------------------------------------------------------------------
	static void UpdateItemInfo(Widget root_widget, EntityAI item)
	{
		if (!root_widget || !item) return;
	
		Widget panelInfo = root_widget.FindAnyWidget("InventoryInfoPanelWidget");
		if ( panelInfo )
		{
			if ( item.IsInherited( ZombieBase ) || item.IsInherited( Car ) )
			{
				panelInfo.Show( false );
			}
			else
			{
				panelInfo.Show( true );
			}
		}
	
		if ( !item.IsInherited( ZombieBase ) && !item.IsInherited( Car ) )
		{
			InventoryItem iItem = InventoryItem.Cast( item );
			WidgetTrySetText(root_widget, "ItemDescWidget", iItem.GetTooltip());
		}
	
		WidgetTrySetText(root_widget, "ItemNameWidget", item.GetDisplayName());
		InspectMenuNew.UpdateItemInfoDamage(root_widget, item);
		InspectMenuNew.UpdateItemInfoLiquidType(root_widget, item);
		InspectMenuNew.UpdateItemInfoTemperature(root_widget, item);
		InspectMenuNew.UpdateItemInfoWetness(root_widget, item);
		InspectMenuNew.UpdateItemInfoQuantity(root_widget, item);
		InspectMenuNew.UpdateItemInfoWeight(root_widget, item);
		InspectMenuNew.UpdateItemInfoFoodStage(root_widget, item);
		InspectMenuNew.UpdateItemInfoCleanness(root_widget, item);
		
		Widget content = root_widget.FindAnyWidget("InventoryInfoPanelWidget");
	}
	
	//--------------------------------------------------------------------------
	static void UpdateSlotInfo(Widget root_widget, string name, string desc = "")
	{
		if (!root_widget ) return;
	
		WidgetTrySetText(root_widget, "ItemNameWidget", name);
	}

	//--------------------------------------------------------------------------
	static void UpdateItemInfoDamage(Widget root_widget, EntityAI item)
	{
		if ( item.IsInherited( ZombieBase ) || item.IsInherited( Car ) ) return;
		
		int damageLevel = item.GetHealthLevel();
		
		switch(damageLevel)
		{
			case GameConstants.STATE_RUINED:
			{
				WidgetTrySetText(root_widget, "ItemDamageWidget", "#inv_inspect_ruined", Colors.COLOR_RUINED);
				break;
			}
			case GameConstants.STATE_BADLY_DAMAGED:
			{
				WidgetTrySetText(root_widget, "ItemDamageWidget", "#inv_inspect_badly", Colors.COLOR_BADLY_DAMAGED);
				break;
			}
			
			case GameConstants.STATE_DAMAGED:
			{
				WidgetTrySetText(root_widget, "ItemDamageWidget", "#inv_inspect_damaged", Colors.COLOR_DAMAGED);
				break;
			}
			
			case GameConstants.STATE_WORN:
			{
				WidgetTrySetText(root_widget, "ItemDamageWidget", "#inv_inspect_worn", Colors.COLOR_WORN);
				break;
			}
			
			case GameConstants.STATE_PRISTINE:
			{
				WidgetTrySetText(root_widget, "ItemDamageWidget", "#inv_inspect_pristine", Colors.COLOR_PRISTINE);
				break;
			}
					
			default:
			{
				WidgetTrySetText(root_widget, "ItemDamageWidget", "ERROR", Colors.COLOR_PRISTINE);
				break;
			}
		}
	
	}
	
	//--------------------------------------------------------------------------
	static void UpdateItemInfoLiquidType(Widget root_widget, EntityAI item)
	{
		if ( item.IsInherited( ZombieBase ) || item.IsInherited( Car ) ) return;
		
		ItemBase item_base = ItemBase.Cast( item );
		
		if( item_base && item_base.GetQuantity() > 0 && item_base.IsBloodContainer() )
		{
			BloodContainerBase blood_container = BloodContainerBase.Cast( item_base );
			
			if( blood_container.GetBloodTypeVisible() )
			{
				string type;
				bool positive;
				string blood_type_name = BloodTypes.GetBloodTypeName(blood_container.GetLiquidType(), type, positive);
				WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_blood: " + blood_type_name, Colors.COLOR_LIQUID);
			}
			else
			{
				WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_blood", Colors.COLOR_LIQUID);
			}
		}
		else if( item_base && item_base.GetQuantity() > 0 && item_base.IsLiquidContainer() )
		{
			int liquid_type = item_base.GetLiquidType();
			
			switch(liquid_type)
			{
				case LIQUID_WATER:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_water", Colors.COLOR_LIQUID);
					break;
				}
					
				case LIQUID_RIVERWATER:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_river_water", Colors.COLOR_LIQUID);
					break;
				}
					
				case LIQUID_VODKA:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_vodka", Colors.COLOR_LIQUID);
					break;
				}
				
				case LIQUID_BEER:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_beer", Colors.COLOR_LIQUID);
					break;
				}
				
				case LIQUID_GASOLINE:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_gasoline", Colors.COLOR_LIQUID);
					break;
				}
				
				case LIQUID_DIESEL:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_diesel", Colors.COLOR_LIQUID);
					break;
				}
				
				case LIQUID_DISINFECTANT:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_disinfectant", Colors.COLOR_LIQUID);
					break;
				}
	
				case LIQUID_SALINE:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "#inv_inspect_saline", Colors.COLOR_LIQUID);
					break;
				}
				
				default:
				{
					WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "ERROR", Colors.COLOR_LIQUID);
					break;
				}
			}
		}
		else
		{
			WidgetTrySetText(root_widget, "ItemLiquidTypeWidget", "");
		}
	}
	
	//--------------------------------------------------------------------------
	static void UpdateItemInfoTemperature(Widget root_widget, EntityAI item)
	{
		if ( item.IsInherited( ZombieBase ) || item.IsInherited( Car ) ) return;
		float temperature;
		ItemBase item_base = ItemBase.Cast( item );
		if( item_base )
		{
			temperature = item_base.GetTemperature();
		}
		
		if( temperature > 30 )
		{
			if ( temperature > 100 )
			{
				temperature = 100 * Math.Floor( temperature / 100.0 );
			}
			else
			{
				temperature = 10 * Math.Floor( temperature / 10.0 );
			}
			WidgetTrySetText(root_widget, "ItemTemperatureWidget",  "#inv_inspect_about " + temperature.ToString() +  " " + "#inv_inspect_celsius", GetTemperatureColor( temperature ) );
		}	
		else
		{
			WidgetTrySetText(root_widget, "ItemTemperatureWidget", "");
		}
	}
	
	//--------------------------------------------------------------------------
	static void UpdateItemInfoWetness(Widget root_widget, EntityAI item)
	{
		if ( item.IsInherited( ZombieBase ) || item.IsInherited( Car ) ) return;
		float wetness = 0;
		
		if ( item.IsInherited(ItemBase) )
		{
			ItemBase item_IB = ItemBase.Cast( item );
			wetness = item_IB.GetWet();
		}

		if( wetness < GameConstants.STATE_DAMP )
		{
			WidgetTrySetText(root_widget, "ItemWetnessWidget", "");
		}
		else if( wetness >= GameConstants.STATE_DAMP && wetness < GameConstants.STATE_WET )
		{
			WidgetTrySetText(root_widget, "ItemWetnessWidget", "#inv_inspcet_damp", Colors.COLOR_DAMP);
		}
		else if( wetness >= GameConstants.STATE_WET && wetness < GameConstants.STATE_SOAKING_WET )
		{
			WidgetTrySetText( root_widget, "ItemWetnessWidget", "#inv_inspect_wet", Colors.COLOR_WET );
		}
		else if( wetness >= GameConstants.STATE_SOAKING_WET && wetness < GameConstants.STATE_DRENCHED )
		{
			WidgetTrySetText( root_widget, "ItemWetnessWidget", "#inv_inspect_soaking_wet", Colors.COLOR_SOAKING_WET );
		}
		else
		{
			WidgetTrySetText( root_widget, "ItemWetnessWidget", "#inv_inspect_drenched", Colors.COLOR_DRENCHED );
		}
	}
	
	//--------------------------------------------------------------------------
	static void UpdateItemInfoQuantity(Widget root_widget, EntityAI item)
	{
		if ( item.IsInherited( ZombieBase ) || item.IsInherited( Car ) ) return;
		
		ItemBase item_base = ItemBase.Cast( item );
		if( item_base )
		{
			float item_quantity = item_base.GetQuantity();
			int max_quantity = item.GetQuantityMax();
			
			float quantity_ratio;
			
			if( max_quantity > 0 && !item.IsInherited( ClothingBase )) // Some items, like books, have max_quantity set to 0 => division by ZERO error in quantity_ratio
			{
				string quantity_str;
				if( item.ConfigGetString("stackedUnit") == "pc." )
				{
					if( item_quantity == 1 )
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", item_quantity.ToString() + " " + "#inv_inspect_piece", Colors.COLOR_DEFAULT );
					}
					else
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", item_quantity.ToString() + " " + "#inv_inspect_pieces", Colors.COLOR_DEFAULT );
					}		
				}
				else if( item.ConfigGetString("stackedUnit") == "percentage" )
				{
					quantity_ratio = Math.Round( ( item_quantity / max_quantity ) * 100 );
					
					quantity_str = "#inv_inspect_remaining " + quantity_ratio.ToString() + "#inv_inspect_percent";
					WidgetTrySetText( root_widget, "ItemQuantityWidget", quantity_str, Colors.COLOR_DEFAULT );			
				}
				else if( item.ConfigGetString("stackedUnit") == "g" )
				{
					quantity_ratio = Math.Round( ( item_quantity / max_quantity ) * 100 );
					
					quantity_str = "#inv_inspect_remaining " + quantity_ratio.ToString() + "#inv_inspect_percent";
					WidgetTrySetText( root_widget, "ItemQuantityWidget", quantity_str, Colors.COLOR_DEFAULT );			
				}
				else if( item.ConfigGetString("stackedUnit") == "ml" )
				{
					quantity_ratio = Math.Round( ( item_quantity / max_quantity ) * 100 );
					
					quantity_str = "#inv_inspect_remaining " + quantity_ratio.ToString() + "#inv_inspect_percent";
					WidgetTrySetText( root_widget, "ItemQuantityWidget", quantity_str, Colors.COLOR_DEFAULT );
				}
				else if( item.IsInherited( Magazine ) )
				{
					Magazine magazine_item;
					Class.CastTo(magazine_item, item);
					
					if( magazine_item.GetAmmoCount() == 1 )
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget",  magazine_item.GetAmmoCount().ToString() + " " + "#inv_inspect_piece", Colors.COLOR_DEFAULT );
					}
					else
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget",  magazine_item.GetAmmoCount().ToString() + " " + "#inv_inspect_pieces", Colors.COLOR_DEFAULT );
					}
				}
				else
				{
					WidgetTrySetText( root_widget, "ItemQuantityWidget", "" );
				}
			}
			else
			{
				if ( item.IsInherited( ClothingBase ) )
				{
					float heatIsolation = MiscGameplayFunctions.GetCurrentItemHeatIsolation( item_base );
					if ( heatIsolation <= GameConstants.HEATISO_THRESHOLD_BAD )
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", "#inv_inspect_iso_bad", GetTemperatureColor( 10 ) );
					}
					else if ( ( heatIsolation > GameConstants.HEATISO_THRESHOLD_BAD ) && ( heatIsolation <= GameConstants.HEATISO_THRESHOLD_LOW ) )
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", "#inv_inspect_iso_low", GetTemperatureColor( 20 ) );
					}
					else if ( ( heatIsolation > GameConstants.HEATISO_THRESHOLD_LOW ) && ( heatIsolation <= GameConstants.HEATISO_THRESHOLD_MEDIUM ) )
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", "#inv_inspect_iso_medium", GetTemperatureColor( 30 ) );
					}
					else if ( ( heatIsolation > GameConstants.HEATISO_THRESHOLD_MEDIUM ) && ( heatIsolation <= GameConstants.HEATISO_THRESHOLD_HIGH ) )
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", "#inv_inspect_iso_high", GetTemperatureColor( 50 ) );
					}
					else
					{
						WidgetTrySetText( root_widget, "ItemQuantityWidget", "#inv_inspect_iso_excel", GetTemperatureColor( 70 ) );
					}
				}
				else
				{
					WidgetTrySetText( root_widget, "ItemQuantityWidget", "" );
				}
			}
		}
	}
	
	//--------------------------------------------------------------------------
	static void UpdateItemInfoWeight(Widget root_widget, EntityAI item)
	{
		if (!item.CanDisplayWeight())
		{
			WidgetTrySetText(root_widget, "ItemWeightWidget", "", Colors.COLOR_DEFAULT);
			return;
		}
		
		ItemBase item_IB = ItemBase.Cast( item );
		if( item_IB )
		{
			item_IB.UpdateWeight();
			int weight = item_IB.GetWeight();
			
			if (root_widget.GetName() != "BackPanelWidget")
			{
				weight = item_IB.GetSingleInventoryItemWeight();
			}
			
			if (weight >= 1000)
			{
				int kilos = Math.Round(weight / 1000.0);
				WidgetTrySetText(root_widget, "ItemWeightWidget", "#inv_inspect_about" + " " + kilos.ToString() + " " + "#inv_inspect_kg", Colors.COLOR_DEFAULT);
			}
			else if (weight >= 500)
			{
				WidgetTrySetText(root_widget, "ItemWeightWidget", "#inv_inspect_under_1", Colors.COLOR_DEFAULT);
			} 
			else if (weight >= 250)
			{
				WidgetTrySetText(root_widget, "ItemWeightWidget", "#inv_inspect_under_05", Colors.COLOR_DEFAULT);
			}
			else 
			{
				WidgetTrySetText(root_widget, "ItemWeightWidget", "#inv_inspect_under_025", Colors.COLOR_DEFAULT);
			}
		}
	}
	
	//--------------------------------------------------------------------------
	static void UpdateItemInfoFoodStage(Widget root_widget, EntityAI item)
	{
		Edible_Base food_item = Edible_Base.Cast( item );
		if ( food_item && food_item.HasFoodStage() )
		{
			ref FoodStage food_stage = food_item.GetFoodStage();
			FoodStageType food_stage_type = food_stage.GetFoodStageType();
					
			switch( food_stage_type )
			{				
				case FoodStageType.RAW:
				{
					WidgetTrySetText( root_widget, "ItemFoodStageWidget", "#inv_inspect_raw", Colors.COLOR_RAW );
					break;
				}
				case FoodStageType.BAKED:
				{
					WidgetTrySetText( root_widget, "ItemFoodStageWidget", "#inv_inspect_baked", Colors.COLOR_BAKED );
					break;
				}
				case FoodStageType.BOILED:
				{
					WidgetTrySetText( root_widget, "ItemFoodStageWidget", "#inv_inspect_boiled", Colors.COLOR_BOILED );
					break;
				}
				case FoodStageType.DRIED:
				{
					WidgetTrySetText( root_widget, "ItemFoodStageWidget", "#inv_inspect_dried", Colors.COLOR_DRIED );
					break;
				}
				case FoodStageType.BURNED:
				{
					WidgetTrySetText( root_widget, "ItemFoodStageWidget", "#inv_inspect_burned", Colors.COLOR_BURNED );
					break;
				}
				case FoodStageType.ROTTEN:
				{
					WidgetTrySetText( root_widget, "ItemFoodStageWidget", "#inv_inspect_rotten", Colors.COLOR_ROTTEN );
					break;
				}
			}
		}
		else
		{
			WidgetTrySetText( root_widget, "ItemFoodStageWidget", "" );
		}
	}
	
	
		//--------------------------------------------------------------------------
	static void UpdateItemInfoCleanness(Widget root_widget, EntityAI item)
	{
		ItemBase ib = ItemBase.Cast(item);
		if(ib && ib.m_Cleanness==1)
		{
			WidgetTrySetText( root_widget, "ItemCleannessWidget", "#inv_inspect_cleaned", Colors.WHITEGRAY );
		}
		else
		{
			WidgetTrySetText( root_widget, "ItemCleannessWidget", "" );
		}
	}
	
	
	
	//--------------------------------------------------------------------------
	static void WidgetTrySetText(Widget root_widget, string widget_name, string text, int color = 0)
	{
		TextWidget widget = TextWidget.Cast( root_widget.FindAnyWidget(widget_name) );
		RichTextWidget rt_widget = RichTextWidget.Cast( root_widget.FindAnyWidget(widget_name) );
		if (widget)
		{
			widget.SetText(text);
			Widget widget_background = root_widget.FindAnyWidget(widget_name+"Background");
			if (widget_background)
			{
				if (color != 0)
				{
					widget_background.Show( true );
					widget_background.SetColor(color | 0x7F000000);
				}
				else
				{
					widget_background.Show( false );
				}
			}
		}
		if( rt_widget )
		{
			rt_widget.Update();
		}
	}

};