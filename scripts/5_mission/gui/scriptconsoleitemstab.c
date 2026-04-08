typedef Param3<string, bool, bool> PresetParams;//  param1 - ??, param2 - ??, param3 - ??

class ScriptConsoleItemsTab : ScriptConsoleTabBase
{
	protected const string 						CONST_DEFAULT_PRESET_PREFIX = "[Default]";
	protected static int 						ITEMS_SELECTED_ROW = -1;
	protected static int 						PRESETS_SELECTED_ROW = 0;
	protected static int 						ITEMS_IN_PRESET_SELECTED_ROW;
	protected static float 						DRAW_DISTANCE = 1000;
	protected static bool 						CLEAR_IVN;
	protected static int 						m_ObjectsScope = 2;
	protected static string 					m_LastSelectedObject;
	protected static int						m_SelectedObjectIsPreset = -1;
	protected static ref array<Shape>			m_DebugShapes = new array<Shape>;
	protected static EntityAI 					m_PreviewEntity;
	protected static float	 					m_ItemQuantity = 1;
	protected static bool	 					m_WithPhysics = false;
	
	
	protected ref array<Widget> 				m_CategoryButtonsWidgets = new array<Widget>;
	protected ref array<string> 				m_CategoryButtonsNames = {"FIREARMS","MELEE","ATTACHMENTS","MAGAZINES","AMMO","FOOD","MEDICAL","CARS","BACKPACKS","B-BUILDING"};
	protected ref map<Widget, string>			m_SpawnButtonsOriginalText = new map<Widget, string>();
	protected int 								m_CategoryMask;
	protected MissionGameplay					m_MissionGameplay;
	protected ref Timer 						m_RefreshFilterTimer = new Timer();
	protected PluginDeveloper 					m_Developer;
	protected bool 								m_FilterOrderReversed;
	protected string 							m_SelectedObject;
	protected string							m_FilterTextPrev;
	//Widgets
	protected CheckBoxWidget			m_CategoryMergeType;
	protected CheckBoxWidget			m_ItemPreviewCheckbox;
	protected CheckBoxWidget			m_ShowProtected;
	protected CheckBoxWidget			m_ClearInventory;
	protected TextListboxWidget 		m_ObjectsTextListbox;
	protected TextListboxWidget 		m_PresetsTextListbox;
	protected TextListboxWidget 		m_PresetItemsTextListbox;
	protected ButtonWidget 				m_DrawInWorld;
	protected ButtonWidget 				m_DrawInWorldClear;	
	protected ButtonWidget 				m_PresetNewButton;
	protected ButtonWidget 				m_PresetDeleteButton;
	protected ButtonWidget 				m_PresetRenameButton;
	protected ButtonWidget 				m_PresetSetDefaultButton;
	protected ButtonWidget 				m_PresetAddItemtButton;
	protected ButtonWidget 				m_ItemNameCopy;
	protected ButtonWidget 				m_PresetRemoveItemButton;
	protected ButtonWidget 				m_ItemMoveUpButton;
	protected ButtonWidget 				m_ItemMoveDownButton;
	protected ButtonWidget 				m_FilterOrderButton;
	protected ButtonWidget 				m_SpawnInInvButton;
	protected ButtonWidget 				m_SpawnGroundButton;
	protected ButtonWidget 				m_SpawnGroundPatternGrid;
	protected ButtonWidget 				m_SpawnOnTarget;
	protected ButtonWidget 				m_SpawnOnCursor;
	protected ButtonWidget 				m_SpawnInHands;
	protected ButtonWidget 				m_SpawnBatchButton;
	protected ButtonWidget				m_ListActions;
	protected MultilineEditBoxWidget 	m_ObjectFilter;
	protected EditBoxWidget 			m_SpawnDistanceEditBox;
	protected EditBoxWidget 			m_RectSpawnRow;
	protected EditBoxWidget 			m_RectSpawnColumn;
	protected EditBoxWidget 			m_RectSpawnRowStep;
	protected EditBoxWidget 			m_RectSpawnColumnStep;
	protected EditBoxWidget 			m_DrawDistanceWidget;
	protected EditBoxWidget 			m_QuantityEditBox;
	protected EditBoxWidget 			m_DamageEditBox;
	protected EditBoxWidget 			m_BatchSpawnQuantity;
	protected CheckBoxWidget			m_WithPhysicsCheckbox;
	protected ButtonWidget				m_ReloadShapeButton;
	protected TextWidget 				m_ItemDamageLabel;
	protected TextWidget 				m_ItemQuantityLabel;
	protected TextWidget 				m_SelectedObjectText;
	protected TextWidget 				m_SelectedObjectLocalized;
	protected ItemPreviewWidget 		m_ItemPreviewWidget;
	protected ImageWidget				m_FilterOrderImage;
	
	
	void ScriptConsoleItemsTab(Widget root, ScriptConsole console, Widget button, ScriptConsoleTabBase parent = null)
	{
		m_Developer					= PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
		
		m_DrawInWorld = ButtonWidget.Cast(root.FindAnyWidget("ButtonDrawItem"));
		m_DrawInWorldClear = ButtonWidget.Cast(root.FindAnyWidget("ButtonDrawItemClear"));
		
		m_CategoryMergeType = CheckBoxWidget.Cast(root.FindAnyWidget("CategoryMergeType"));
		m_ItemPreviewCheckbox	= CheckBoxWidget.Cast(root.FindAnyWidget("ItemPreviewCheckbox"));
		m_ReloadShapeButton	= ButtonWidget.Cast(root.FindAnyWidget("ReloadShapeButton"));
		m_ShowProtected		= CheckBoxWidget.Cast(root.FindAnyWidget("ShowProtectedCheckbox"));
		m_FilterOrderImage	= ImageWidget.Cast(root.FindAnyWidget("ObjectFilterSortPic"));
		m_ClearInventory		= CheckBoxWidget.Cast(root.FindAnyWidget("ForceClearCheckbox"));
		m_DrawDistanceWidget	= EditBoxWidget.Cast(root.FindAnyWidget("DrawItemDistance"));
		m_ObjectFilter = MultilineEditBoxWidget.Cast(root.FindAnyWidget("ObjectFilter"));
		m_SpawnDistanceEditBox = EditBoxWidget.Cast(root.FindAnyWidget("SpawnDistance"));
		m_SelectedObjectText = TextWidget.Cast(root.FindAnyWidget("SelectedObject"));
		m_SelectedObjectLocalized = TextWidget.Cast(root.FindAnyWidget("SelectedObjectLocalized"));
		m_ObjectsTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("ObjectsList"));
		m_PresetsTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("PresetList"));
		m_PresetItemsTextListbox = TextListboxWidget.Cast(root.FindAnyWidget("PresetItemsList"));
		m_PresetNewButton = ButtonWidget.Cast(root.FindAnyWidget("NewPresetButton"));
		m_PresetDeleteButton = ButtonWidget.Cast(root.FindAnyWidget("DeletePresetButton"));
		m_PresetRenameButton = ButtonWidget.Cast(root.FindAnyWidget("RenamePresetButton"));
		m_PresetSetDefaultButton = ButtonWidget.Cast(root.FindAnyWidget("SetDefaultPresetButton"));
		m_PresetAddItemtButton = ButtonWidget.Cast(root.FindAnyWidget("AddToPresetButton"));
		m_ItemNameCopy = ButtonWidget.Cast(root.FindAnyWidget("ItemNameCopy"));
		m_PresetRemoveItemButton = ButtonWidget.Cast(root.FindAnyWidget("RemoveFromPresetButton"));
		m_ItemMoveUpButton = ButtonWidget.Cast(root.FindAnyWidget("ItemMoveUpButton"));
		m_ItemMoveDownButton = ButtonWidget.Cast(root.FindAnyWidget("ItemMoveDownButton"));
		m_ItemPreviewWidget = ItemPreviewWidget.Cast(root.FindAnyWidget("ItemPreviewWidget"));

		m_QuantityEditBox = EditBoxWidget.Cast(root.FindAnyWidget("QuantityValue"));
		m_DamageEditBox = EditBoxWidget.Cast(root.FindAnyWidget("DamageValue"));
		m_ItemDamageLabel = TextWidget.Cast(root.FindAnyWidget("DamageLabel"));
		m_ItemQuantityLabel = TextWidget.Cast(root.FindAnyWidget("QuantityLabel"));
		m_WithPhysicsCheckbox = CheckBoxWidget.Cast(root.FindAnyWidget("WithPhysicsCheckbox"));

		m_SpawnInInvButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonSpawnInInv"));
		m_SpawnGroundButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonSpawnInGround"));
		m_SpawnOnTarget = ButtonWidget.Cast(root.FindAnyWidget("ButtonSpawnInTarget"));
		m_SpawnOnCursor = ButtonWidget.Cast(root.FindAnyWidget("ButtonSpawnCursor"));
		m_SpawnInHands = ButtonWidget.Cast(root.FindAnyWidget("ButtonSpawnHands"));
		m_SpawnBatchButton = ButtonWidget.Cast(root.FindAnyWidget("ButtonSpawnBatch"));
		
		string text;
		m_SpawnInInvButton.GetText(text);
		m_SpawnButtonsOriginalText.Insert(m_SpawnInInvButton, text);
		m_SpawnGroundButton.GetText(text);
		m_SpawnButtonsOriginalText.Insert(m_SpawnGroundButton, text);
		m_SpawnOnTarget.GetText(text);
		m_SpawnButtonsOriginalText.Insert(m_SpawnOnTarget, text);
		m_SpawnOnCursor.GetText(text);
		m_SpawnButtonsOriginalText.Insert(m_SpawnOnCursor, text);
		m_SpawnInHands.GetText(text);
		m_SpawnButtonsOriginalText.Insert(m_SpawnInHands, text);
		m_SpawnBatchButton.GetText(text);
		m_SpawnButtonsOriginalText.Insert(m_SpawnBatchButton, text);

		m_ListActions = ButtonWidget.Cast(root.FindAnyWidget("ListActions"));

		
		m_RectSpawnColumn = EditBoxWidget.Cast(root.FindAnyWidget("RectangleColumnValue"));
		m_RectSpawnRow = EditBoxWidget.Cast(root.FindAnyWidget("RectangleRowValue"));
		m_BatchSpawnQuantity = EditBoxWidget.Cast(root.FindAnyWidget("BatchCount"));
		
		m_RectSpawnColumnStep = EditBoxWidget.Cast(root.FindAnyWidget("RectangleStepXValue"));
		m_RectSpawnRowStep = EditBoxWidget.Cast(root.FindAnyWidget("RectangleStepYValue"));
		
		m_FilterOrderButton	= ButtonWidget.Cast(root.FindAnyWidget("ObjectFilterSortButton"));
		Init();
	}
	
	void ~ScriptConsoleItemsTab()
	{
		ITEMS_SELECTED_ROW = m_ObjectsTextListbox.GetSelectedRow();
		PRESETS_SELECTED_ROW = m_PresetsTextListbox.GetSelectedRow();
		ITEMS_IN_PRESET_SELECTED_ROW = m_PresetItemsTextListbox.GetSelectedRow();
				
		m_PresetsTextListbox.ClearItems();
		
		if (m_PreviewEntity)
		{
			m_PreviewEntity.Delete();
		}
		
		m_RefreshFilterTimer.Stop();
		
		if (m_ConfigDebugProfile)
		{
			m_ConfigDebugProfile.SetItemCategoryFlag(m_CategoryMask);
		}
		
		m_ItemQuantity = m_QuantityEditBox.GetText().ToFloat();
		m_WithPhysics = m_WithPhysicsCheckbox.IsChecked();
	}

	
	
	protected void Init()
	{
		m_RefreshFilterTimer.Run(0.85, this, "ChangeFilterItems", null, true);
		m_PresetDeleteButton.Show(false);
		m_PresetRenameButton.Show(false);
		m_DrawDistanceWidget.SetText(DRAW_DISTANCE.ToString());
		
		m_QuantityEditBox.SetText(m_ItemQuantity.ToString());
		m_WithPhysicsCheckbox.SetChecked(m_WithPhysics);
		
		if (m_ConfigDebugProfile)
		{
			string txt = (m_ConfigDebugProfile.GetItemSearch());
			m_ObjectFilter.SetText(m_ConfigDebugProfile.GetItemSearch());
			m_SpawnDistanceEditBox.SetText(m_ConfigDebugProfile.GetSpawnDistance().ToString());
			
			
			SetFilterOrder(m_ConfigDebugProfile.GetFilterOrderReversed());
			//m_ConfigDebugProfile.SetItemCategoryFlag(m_CategoryMask);
			
			m_BatchSpawnQuantity.SetText(m_ConfigDebugProfile.GetBatchSpawnQuantity().ToString());

			m_CategoryMask = m_ConfigDebugProfile.GetItemCategoryFlag();
			if (m_ItemPreviewCheckbox)
			{
				m_ItemPreviewCheckbox.SetChecked(m_ConfigDebugProfile.GetShowItemPreview());
			}
			
			array<ref CfgParam> params = m_ConfigDebugProfile.GetBatchSpawnRectangle();
			
			m_RectSpawnRow.SetText(CfgParamInt.Cast(params.Get(0)).GetValue().ToString());
			m_RectSpawnColumn.SetText(CfgParamInt.Cast(params.Get(1)).GetValue().ToString());
			m_RectSpawnRowStep.SetText(CfgParamFloat.Cast(params.Get(2)).GetValue().ToString());
			m_RectSpawnColumnStep.SetText(CfgParamFloat.Cast(params.Get(3)).GetValue().ToString());
		}
		
		
		if (m_ObjectsScope == 1)
			m_ShowProtected.SetChecked(true);
		else
			m_ShowProtected.SetChecked(false);
		
		int counter = 0;
		foreach (string name: m_CategoryButtonsNames)
		{
			string widget_name = "ItemCategory" + counter;
			
			CheckBoxWidget btw = CheckBoxWidget.Cast(m_ParentRoot.FindAnyWidget(widget_name));
			if (btw)
			{
				btw.SetText(name);
				m_CategoryButtonsWidgets.Insert(btw);
				int bit = 1 << counter;
				if (bit & m_CategoryMask)
				{
					btw.SetChecked(true);
					btw.SetTextColor(ARGB(255, 255, 0, 0));
				}
			}
			counter++;
		}
		
		if (ITEMS_IN_PRESET_SELECTED_ROW <= m_PresetItemsTextListbox.GetNumItems() && m_PresetItemsTextListbox.GetNumItems() > 0 && ITEMS_IN_PRESET_SELECTED_ROW > -1)
		{
			m_PresetItemsTextListbox.SelectRow(ITEMS_IN_PRESET_SELECTED_ROW);
			m_PresetItemsTextListbox.EnsureVisible(ITEMS_IN_PRESET_SELECTED_ROW);
			
			m_SelectedObjectText.SetText("Object : " + GetCurrentItemName());
			SetObject(GetCurrentItemName());
			m_SelectedObjectIsPreset = 0;
		}
		
		ChangeFilterItems(true);
		
		RenderPresets();
		//RenderPresetItems();
		
		m_MissionGameplay			= MissionGameplay.Cast(g_Game.GetMission());
		
		if (!m_SelectedObjectIsPreset == 1 && ITEMS_SELECTED_ROW <  m_ObjectsTextListbox.GetNumItems() && m_ObjectsTextListbox.GetNumItems() > 0 && ITEMS_SELECTED_ROW > -1)
		{
			m_ObjectsTextListbox.SelectRow(ITEMS_SELECTED_ROW);
			m_ObjectsTextListbox.EnsureVisible(ITEMS_SELECTED_ROW);
			
			
			m_SelectedObjectText.SetText(GetCurrentObjectName());

			
			SetObject(GetCurrentObjectName());
			m_LastSelectedObject = m_SelectedObject;
			m_PresetItemsTextListbox.SelectRow(-1);
			m_SelectedObjectIsPreset = 0;
		}
		
		if (m_PresetsTextListbox.GetNumItems() >= 0)
		{
			m_PresetsTextListbox.SelectRow(PRESETS_SELECTED_ROW);
			m_PresetsTextListbox.EnsureVisible(PRESETS_SELECTED_ROW);
			if (m_SelectedObjectIsPreset == 1)
				SelectPreset();
			else
				RenderPresetItems();
		}
		
		if (m_ObjectsScope == 1)
			m_ShowProtected.SetChecked(true);
		else
			m_ShowProtected.SetChecked(false);

		
		m_CategoryMergeType.SetChecked(m_ConfigDebugProfile.GetMergeType());
				
		HideItemButtons();
		ShowItemTransferButtons();
	}
	
	static void DrawItems(string type, float distance = 0)
	{	
		if (!distance)
		{
			distance = DRAW_DISTANCE;
		}
		array<Object> objects = new array<Object>;
		array<CargoBase> proxies = new array<CargoBase>;
		
		g_Game.GetObjectsAtPosition(g_Game.GetPlayer().GetPosition(), distance, objects, proxies);
		int i = 0;
		foreach (Object o: objects)
		{
			if (o.IsKindOf(type))
			{
				i++;
				ScriptConsole.m_MarkedEntities.Insert(new MapMarker(o.GetPosition(),"",COLOR_RED,eMapMarkerTypes.MARKERTYPE_MAP_BORDER_CROSS));
				Shape shape = Shape.CreateSphere(COLOR_RED, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.NOZBUFFER, o.GetPosition() + "0 30 0", 1);
				vector lines[2];
				lines[0] = o.GetPosition();
				lines[1] = o.GetPosition() + "0 30 0";
				Shape line = Shape.CreateLines(COLOR_RED, ShapeFlags.TRANSP|ShapeFlags.NOOUTLINE|ShapeFlags.NOZBUFFER, lines, 2);
				m_DebugShapes.Insert(shape);
				m_DebugShapes.Insert(line);
			}
		}
		
		//! need better logic for cleaning
		//g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(this, "DrawItemsClear", 5000);
		Debug.Log(string.Format("found %1 instances of type %1", i, type));
	}
	
	
	static void DrawItemsClear()
	{
		ScriptConsole.m_MarkedEntities.Clear();
		foreach (Shape s: m_DebugShapes)
		{
			s.Destroy();
		}
		m_DebugShapes.Clear();
	}
	
	//!
	static string	GetLastSelectedObject()
	{
		return m_LastSelectedObject;
	}
	
	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_D && m_SelectedObject != "")
		{
			DrawItems(m_SelectedObject);
			return true;
		}
		else
			return super.OnKeyDown(w, x, y, key);
	}
	
	void SelectObject(bool hide_presets = true)
	{
		if (hide_presets)
			m_PresetItemsTextListbox.SelectRow(-1);
		HideItemButtons();
		ShowItemTransferButtons();
		m_SelectedObjectText.SetText(GetCurrentObjectName());
		SetObject(GetCurrentObjectName());

		m_LastSelectedObject = m_SelectedObject;
		m_SelectedObjectIsPreset = 0;
	}
	
	
	void SelectPreset()
	{
		m_ObjectsTextListbox.SelectRow(-1);
		RenderPresetItems();
		ShowPresetButtons();
		ShowItemTransferButtons();
		m_SelectedObject = GetCurrentPresetName();
		m_SelectedObjectText.SetText("Preset : " + m_SelectedObject);
		m_SelectedObjectIsPreset = 1;
		
		TStringArray command_array = new TStringArray;
		
		m_ConfigDebugProfileFixed.GetPresetItems(m_SelectedObject, command_array, "cmd");
				
		bool clear = !IsPresetFixed(m_SelectedObject);
		if (command_array.IsValidIndex(0))
		{
			string first_line = command_array.Get(0);
			first_line.ToLower();
			clear = (first_line == "clear_inv");
		}

		m_ClearInventory.SetChecked(clear);
	}
	
	int GetCurrentPresetIndex()
	{
		return m_PresetsTextListbox.GetSelectedRow();
	}

	int GetCurrentItemIndex()
	{
		return m_PresetItemsTextListbox.GetSelectedRow();
	}
	
	bool IsCurrentPresetFixed()
	{
		int index = GetCurrentPresetIndex(); 
		if (index > -1 && index < m_PresetsTextListbox.GetNumItems())
		{
			PresetParams item_params_array;
			m_PresetsTextListbox.GetItemData(index, 0, item_params_array);
			return item_params_array.param2;
		}
		return false;
	}

	bool IsPresetFixed(string preset_name)
	{
		int preset_index = GetPresetIndexByName(preset_name);
		PresetParams item_params_array;
		if (preset_index > -1 && preset_index < m_PresetsTextListbox.GetNumItems())
		{
			m_PresetsTextListbox.GetItemData(preset_index, 0, item_params_array);
			return item_params_array.param2;
		}
		return false;
	}
	
	int GetPresetIndexByName(string preset_name)
	{
		int i;
		for (i = 0; i < m_PresetsTextListbox.GetNumItems(); i++)
		{
			PresetParams item_params_array;
			m_PresetsTextListbox.GetItemData(i, 0, item_params_array);

			if (item_params_array.param1 == preset_name)
			{
				return i;
			}
		}
		return -1;
	}
	
	void ShowItemButtons()
	{
		if (!IsCurrentPresetFixed())
		{
			ShowItemTransferButtons();

			string preset_name = GetCurrentPresetName();
			int item_index = GetCurrentItemIndex();

			m_ItemMoveUpButton.Show(true);
			m_ItemMoveDownButton.Show(true);

			m_ItemDamageLabel.Show(true);
			m_ItemQuantityLabel.Show(true);

			if (preset_name != "" && item_index > -1)
			{
				m_DamageEditBox.Show(true);
				m_QuantityEditBox.Show(true);
				m_WithPhysicsCheckbox.Show(true);

				float item_health= m_ConfigDebugProfile.GetItemHealth(GetCurrentPresetName(), GetCurrentItemIndex());
				int item_quantity = m_ConfigDebugProfile.GetItemQuantity(GetCurrentPresetName(), GetCurrentItemIndex());
				bool item_with_physics = m_ConfigDebugProfile.GetItemWithPhysics(GetCurrentPresetName(), GetCurrentItemIndex());

				// damage
				m_DamageEditBox.SetText(item_health.ToString());

				// quantity
				m_QuantityEditBox.SetText(item_quantity.ToString());

				// with physics
				m_WithPhysicsCheckbox.SetChecked(item_with_physics);
			}
			else
			{
				m_DamageEditBox.Show(false);
				m_QuantityEditBox.Show(false);
				m_WithPhysicsCheckbox.Show(false);
				
			}
		}
	}

	void HideItemButtons()
	{
		m_ItemMoveUpButton.Show(false);
		m_ItemMoveDownButton.Show(false);
	}
		
	void ShowItemTransferButtons()
	{
		int object_row = m_ObjectsTextListbox.GetSelectedRow();
		if (object_row >-1 && GetCurrentPresetIndex() >-1 && !IsCurrentPresetFixed())
		{
			m_PresetAddItemtButton.Show(true);
		}
		else
		{
			m_PresetAddItemtButton.Show(false);
		}

		if (GetCurrentItemIndex() >-1 && GetCurrentPresetIndex() >-1 && !IsCurrentPresetFixed())
		{
			m_PresetRemoveItemButton.Show(true);
		}
		else
		{
			m_PresetRemoveItemButton.Show(false);
		}

		if (object_row > -1)
		{
			m_ItemNameCopy.Show(true);
		}
		else
		{
			m_ItemNameCopy.Show(false);
		}

	}
	
	void ItemMoveUp()
	{
		int new_index = GetCurrentItemIndex() - 1;
		if (GetCurrentItemIndex() != -1 && GetCurrentPresetIndex() != -1 && new_index > -1)
		{
			m_ConfigDebugProfile.SwapItem(GetCurrentPresetName(), GetCurrentItemIndex(), new_index);
			RenderPresetItems();
			m_PresetItemsTextListbox.SelectRow (new_index);
		}
	}

	void ItemMoveDown()
	{
		int new_index = GetCurrentItemIndex() + 1;
		if (GetCurrentItemIndex() != -1 && GetCurrentPresetIndex() != -1 && new_index < m_PresetItemsTextListbox.GetNumItems())
		{
			m_ConfigDebugProfile.SwapItem(GetCurrentPresetName(), GetCurrentItemIndex(), new_index);
			RenderPresetItems();
			m_PresetItemsTextListbox.SelectRow (new_index);
		}
	}
	
	
	void SaveProfileSpawnDistance()
	{
		if (m_ConfigDebugProfile && m_SpawnDistanceEditBox)
		{
			m_ConfigDebugProfile.SetSpawnDistance(m_SpawnDistanceEditBox.GetText().ToFloat());
		}
	}
	
	void RenamePreset(string new_preset_name)
	{
		if (new_preset_name && GetCurrentPresetIndex() != -1)
		{
			bool result = m_ConfigDebugProfile.PresetRename(GetCurrentPresetName(), new_preset_name);
			RefreshLists();
		}
	}
	
	
	void SpawnPreset(EntityAI target, bool clear_inventory, string preset_name, InventoryLocationType location = InventoryLocationType.ATTACHMENT, float distance = 0)
	{
		// spawn preset items into inventory
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (GetCurrentPresetIndex() != -1)
		{
			bool is_preset_fixed = IsCurrentPresetFixed();
			TStringArray preset_array = new TStringArray;
			
			if (is_preset_fixed)
			{
				m_ConfigDebugProfileFixed.GetPresetItems(preset_name, preset_array);
				
			}
			else
			{
				m_ConfigDebugProfile.GetPresetItems(preset_name, preset_array);
			}
			
			if (clear_inventory)
			{
				m_Developer.ClearInventory(target);
			}

			for (int i = 0; i < preset_array.Count(); i++)
			{
				float health = -1;
				int quantity = 1;
				bool with_physics = false;
				if (is_preset_fixed)
				{
					health = m_ConfigDebugProfileFixed.GetItemHealth(preset_name, i);
					quantity = m_ConfigDebugProfileFixed.GetItemQuantity(preset_name, i);
					with_physics = m_ConfigDebugProfileFixed.GetItemWithPhysics(preset_name, i);
				}
				else
				{
					health = m_ConfigDebugProfile.GetItemHealth(preset_name, i);
					quantity = m_ConfigDebugProfile.GetItemQuantity(preset_name, i);
					with_physics = m_ConfigDebugProfile.GetItemWithPhysics(preset_name, i);
				}
				if (location == InventoryLocationType.ATTACHMENT)
					EntityAI ent = m_Developer.SpawnEntityInInventory(target, preset_array.Get(i), -1, quantity, false, preset_name);
					//m_Developer.SpawnEntityAsAttachment(player, target, preset_array.Get(i), -1, quantity, false, preset_name);
				else if (location == InventoryLocationType.GROUND)
				{
					m_Developer.SpawnEntityOnCursorDir(player, preset_array.Get(i), quantity, distance, health, false, preset_name, with_physics);
				}
			}
			if (g_Game.IsMultiplayer())
			{
				ScriptRPC rpc = new ScriptRPC();
				rpc.Write(preset_name);
				rpc.Write(target);
				rpc.Send(player, ERPCs.DEV_RPC_SPAWN_PRESET, true, player.GetIdentity());
			}
			else
			{
				PluginDeveloper plugin = PluginDeveloper.Cast(GetPlugin(PluginDeveloper));
				plugin.HandlePresetSpawn(player,preset_name,target);
			}
		}
	}
	
	void SetDefaultPreset(int preset_index)
	{
		// remove previous default parameter
		string default_preset = m_ConfigDebugProfile.GetDefaultPreset();
		if (default_preset != "")
		{
			int index = GetPresetIndexByName(default_preset);
			// if is fixed
			if (IsPresetFixed(default_preset))
			{
				default_preset = "[" + default_preset + "]";
			}
			PresetParams prev_preset_params_array;
			if (index > -1 && index < m_PresetsTextListbox.GetNumItems())
			{
				m_PresetsTextListbox.GetItemData(index, 0, prev_preset_params_array);
				prev_preset_params_array.param3 = false; // remove DEFAULT
				m_PresetsTextListbox.SetItem(index, default_preset, prev_preset_params_array, 0);
			}
		}

		// set preset on preset_index to default
		// if is fixed
		string preset_name = GetCurrentPresetName();
		if (IsPresetFixed(preset_name))
		{
			preset_name = "[" + preset_name + "]";
		}
		// set new default preset
		PresetParams preset_params_array;
		index = GetCurrentPresetIndex();
		if (index > -1 && index < m_PresetsTextListbox.GetNumItems())
		{
			m_PresetsTextListbox.GetItemData(index, 0, preset_params_array);
			preset_params_array.param3 = true; // DEFAULT
			m_PresetsTextListbox.SetItem(index, preset_name + CONST_DEFAULT_PRESET_PREFIX, preset_params_array, 0);
		}
		// store preset
		m_ConfigDebugProfile.SetDefaultPreset(GetCurrentPresetName());
	}
	
	void SetObject(string object)
	{
		m_SelectedObject = object;
		
		if (m_ConfigDebugProfile && m_ConfigDebugProfile.GetShowItemPreview())
		{
			SetPreviewObject(object);
		}
		
	}

	void SetPreviewObject(string object)
	{
		#ifdef DEVELOPER
		if (m_PreviewEntity)
		{
			m_PreviewEntity.Delete();
		}
		
		m_SelectedObjectLocalized.SetText("");
		
		if (!g_Game.IsKindOf(object, "DZ_LightAI") && !g_Game.IsKindOf(object, "Man"))
		{
			DayZGame.m_IsPreviewSpawn = true;
			//! Don't use ECE_UPDATEPATHGRAPH !!!
			m_PreviewEntity = EntityAI.Cast(g_Game.CreateObjectEx(object, "0 0 0", ECE_LOCAL | ECE_CREATEPHYSICS | ECE_TRACE));
			DayZGame.m_IsPreviewSpawn = false;		
			if (m_PreviewEntity)
			{
				dBodyDestroy(m_PreviewEntity);//prevents cars from acting up when spawned this way(we only want the preview pic, no physics required)
				m_PreviewEntity.DisableSimulation(true);
				m_ItemPreviewWidget.SetItem(m_PreviewEntity);
				m_PreviewEntity.SetAllowDamage(false);
				
				m_SelectedObjectLocalized.SetText(m_PreviewEntity.GetDisplayName());
			}
		}
		#endif
	}
	
	void AddItemToPreset()
	{
		int selected_row_index = m_ObjectsTextListbox.GetSelectedRow();
		if (selected_row_index != -1 && GetCurrentPresetIndex() != -1)
		{
			string item_name;
			m_ObjectsTextListbox.GetItemText(selected_row_index, 0, item_name);
			m_ConfigDebugProfile.ItemAddToPreset(GetCurrentPresetName(), item_name);
			RenderPresetItems();
		}
	}
	
	void ShowPresetButtons()
	{
		bool show = true;
		if (IsCurrentPresetFixed())
		{
			show = false;
		}
		m_PresetDeleteButton.Show(show);
		m_PresetRenameButton.Show(show);

		HideItemButtons();
		
	}
/*
	void AddItemToClipboard(TextListboxWidget text_listbox_widget)
	{
		int selected_row_index = text_listbox_widget.GetSelectedRow();
		if (selected_row_index != -1)
		{
			string item_name;
			text_listbox_widget.GetItemText(selected_row_index, 0, item_name);
			g_Game.CopyToClipboard(item_name);
		}
	}*/

	void RemoveItemFromPreset()
	{
		if (GetCurrentItemIndex() != -1 && GetCurrentPresetIndex() != -1)
		{
			m_ConfigDebugProfile.ItemRemoveFromPreset(GetCurrentPresetName(), GetCurrentItemIndex());
			RenderPresetItems();
		}
	}
	
	string GetCurrentPresetName()
	{
		int index = GetCurrentPresetIndex();
		// load preset items list
		if (index > -1 && index < m_PresetsTextListbox.GetNumItems())
		{
			PresetParams item_params_array;
			m_PresetsTextListbox.GetItemData(index, 0, item_params_array);
			return item_params_array.param1;
		}
		return "";
	}
	
	string GetCurrentItemName()
	{
		if (GetCurrentItemIndex() != -1)
		{
			string item_name;
			m_PresetItemsTextListbox.GetItemText(GetCurrentItemIndex(), 0, item_name);
			return item_name;
		}
		return "";
	}
	
	string GetCurrentObjectName()
	{
		int selected_row_index = m_ObjectsTextListbox.GetSelectedRow();
		if (selected_row_index != -1)
		{
			string item_name;
			m_ObjectsTextListbox.GetItemText(selected_row_index, 0, item_name);
			return item_name;
		}
		return "";
	}
	
	void RefreshLists()
	{
		RenderPresets();
		RenderPresetItems();
	}
	
	// Render specific Preset Items
	void RenderPresets()
	{
		m_PresetsTextListbox.ClearItems();

		int i;
		TBoolArray preset_params;
		
		// load custom presets list
		TStringArray custom_presets_array = m_ConfigDebugProfile.GetPresets();
		for (i = custom_presets_array.Count() - 1; i >= 0; i--)
		{
			m_PresetsTextListbox.AddItem(custom_presets_array.Get(i), new PresetParams (custom_presets_array.Get(i),false, false), 0);
		}
		
		// load fixed presets list
		TStringArray presets_array = m_ConfigDebugProfileFixed.GetPresets();
		for (i = 0; i < presets_array.Count(); i++)
		{
			m_PresetsTextListbox.AddItem("["+presets_array.Get(i)+"]", new PresetParams (presets_array.Get(i), true, false), 0);
		}
		string default_preset = m_ConfigDebugProfile.GetDefaultPreset();
		if (default_preset != "")
		{
			// if is fixed
			int index = GetPresetIndexByName(default_preset);
			if (IsPresetFixed(default_preset))
			{
				default_preset = "[" + default_preset + "]";
			}
			PresetParams preset_params_array;
			if (index > -1 && index < m_PresetsTextListbox.GetNumItems())
			{
				m_PresetsTextListbox.GetItemData(index, 0, preset_params_array);
				m_PresetsTextListbox.SetItem(index, default_preset + CONST_DEFAULT_PRESET_PREFIX, preset_params_array, 0);
			}
		}
	}
	
	// Render specific Preset Items
	void RenderPresetItems()
	{
		// load preset items list
		int i;
		m_PresetItemsTextListbox.ClearItems();
		if (GetCurrentPresetIndex() != -1)
		{
			bool isFixed = IsCurrentPresetFixed();
			TStringArray preset_array = new TStringArray;

			if (isFixed)
			{
				 m_ConfigDebugProfileFixed.GetPresetItems(GetCurrentPresetName(), preset_array);
			}
			else
			{
				m_ConfigDebugProfile.GetPresetItems(GetCurrentPresetName(), preset_array);
			}

			if (preset_array)
			{
				for (i = 0; i < preset_array.Count(); i++)
				{
					m_PresetItemsTextListbox.AddItem(preset_array.Get(i), NULL, 0);
				}
			}
		}
	}
	
	void NewPreset(string preset_name)
	{
		m_ConfigDebugProfile.PresetAdd(preset_name);
		RefreshLists();
	}

	void DeletePreset()
	{
		if (GetCurrentPresetIndex() != -1)
		{
			bool result = m_ConfigDebugProfile.PresetRemove(GetCurrentPresetName());
			RefreshLists();
		}
	}

	int GetFilterColor(string color)
	{
		switch (color)
		{
			case "r":
				return ARGB(255,255,0,0);
			case "g":
				return ARGB(255,0,255,0);
			case "b":
				return ARGB(255,130,130,255);
			case "y":
				return ARGB(255,255,255,0);
			case "o":
				return ARGB(255,255,169,0);
		}
		return -1;//white
	}
	
	void PrepareFilters(string filter, out TStringArray filters, out TIntArray colors)
	{
		filter.Trim();
		filter.ToLower();

		filters = new TStringArray;
		TStringArray rawFilters = new TStringArray;
		filter.Split(" ", rawFilters);
		
		colors = new TIntArray;

		
		foreach (int i, string f:rawFilters)
		{
			int color = -1;
			if (f.IndexOf(":") == 1)//detect color prefix
			{
				string clr = f.Substring(0, 1);
				f = f.Substring(2, f.Length() - 2);//crop off color prefix
				color = GetFilterColor(clr);
			}
			else//autocolor
			{
				if (i%2 == 1)
				{
					color = ARGB(200,255,255,255);
				}
			}
			filters.Insert(f);
			colors.Insert(color);
		}
	}
	
	
	void ChangeFilter(TStringArray classes, TextListboxWidget widget, MultilineEditBoxWidget filterWidget, int categoryMask = -1, bool ignoreScope = false)
	{
		widget.ClearItems();
		
		TStringArray filters;
		TIntArray filterColors;
		
		string widgetText;
		filterWidget.GetText(widgetText);
		PrepareFilters(widgetText, filters, filterColors);
		
		map<string,int> itemsAndColors = new map<string,int>();
		map<string,ref TStringArray> itemsByFilters = new map<string,ref TStringArray>();

		TStringArray itemsArray = TStringArray();
		
		for (int i = 0; i < classes.Count(); i++)
		{
			string config_path = classes.Get(i);

			int objects_count = g_Game.ConfigGetChildrenCount(config_path);
			for (int j = 0; j < objects_count; j++)
			{
				string child_name;
				
				g_Game.ConfigGetChildName(config_path, j, child_name);

				int scope = g_Game.ConfigGetInt(config_path + " " + child_name + " scope");
				
				if (scope >= m_ObjectsScope || ignoreScope)
				{
					int category_bit = g_Game.ConfigGetInt(config_path + " " + child_name + " debug_ItemCategory") - 1;
					category_bit = (1 << category_bit);
					
					bool display = false;
					int color = 0;
					if (category_bit & categoryMask)
					{
						display = true;
					}
					else if ((!m_ConfigDebugProfile.GetMergeType() || categoryMask == 0) && filters.Count() > 0)
					{
						string child_name_lower = child_name;
						child_name_lower.ToLower();
						
						foreach (int indx, string filter:filters)
						{
							if (child_name_lower.Contains(filter))
							{
								display = false;
								color = filterColors.Get(indx);
								TStringArray arr = itemsByFilters.Get(filter);
								if (!arr)
								{
									arr = new TStringArray();
									itemsByFilters.Set(filter, arr);
								}
								arr.Insert(child_name);
								itemsAndColors.Insert(child_name,color);
								break;
							}
						}
					}
					else if (categoryMask == 0)
					{
						display = true;
					}

					if (display)
					{
						itemsArray.Insert(child_name);
						
					}
				}
			}
		}
	
		if (itemsArray)
		{
			itemsArray.Sort();
			foreach (string it:itemsArray)
			{
				widget.AddItem(it, NULL, 0);
			}
		}
		
		if (filters)
		{
			if (m_FilterOrderReversed)
				filters.Invert();
			
			foreach (string f:filters)
			{
				TStringArray arr2 = itemsByFilters.Get(f);
				if (arr2)
				{
					arr2.Sort();
					foreach (string itm: arr2)
					{
						int row = widget.AddItem(itm, NULL, 0);
						int clr = itemsAndColors.Get(itm);
						if (clr)
						{
							widget.SetItemColor(row, 0, clr);
						}
					}
				}
			}
		}
		
		
	}
	
	
	void SetFilterOrder(bool reversed)
	{
		m_FilterOrderReversed = reversed;
		if (reversed)
			m_FilterOrderImage.SetRotation(0,0,180,true);
		else
			m_FilterOrderImage.SetRotation(0,0,0,true);
		ChangeFilterItems(true);
		if (m_ConfigDebugProfile)
		{
			m_ConfigDebugProfile.SetFilterOrderReversed(reversed);
		}
	}
	
	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		super.OnChange(w, x, y, finished);
		if (w == m_ShowProtected)
		{
			if (m_ShowProtected.IsChecked())
			{
				m_ObjectsScope = 1;	
			}
			else
			{
				m_ObjectsScope = 2;
			}
			ChangeFilterItems(true);
			return true;
		}
		else if (w == m_ClearInventory)
		{
			CLEAR_IVN = m_ClearInventory.IsChecked();
			return true;
		}
		else if (m_CategoryButtonsWidgets.Find(w) >= 0)
		{
			
			int pos = m_CategoryButtonsWidgets.Find(w);
			int bit = Math.Pow(2,pos);
			CheckBoxWidget cbw = CheckBoxWidget.Cast(w);
			if (cbw.IsChecked())
			{
				m_CategoryMask = m_CategoryMask | bit;
				cbw.SetTextColor(ARGB(255, 255, 0, 0));
			}
			else
			{
				m_CategoryMask = (m_CategoryMask & ~bit);
				cbw.SetTextColor(ARGB(255, 255, 255,255));
			}

			ChangeFilterItems(true);
			return true;
		}
		
		/*
		if (w == m_ObjectFilter)
		{
			m_RefreshFilterTimer.Run(0.85, this, "ChangeFilterItems", null, false);
			return true;
		}*/
		if (w == m_QuantityEditBox && (GetCurrentItemIndex() >= 0 || GetCurrentPresetName() != ""))
		{
			m_ConfigDebugProfile.SetItemQuantity(GetCurrentPresetName(), GetCurrentItemIndex(), m_QuantityEditBox.GetText().ToFloat());
			return true;
		}
		else if (w == m_DrawDistanceWidget)
		{
			DRAW_DISTANCE = m_DrawDistanceWidget.GetText().ToFloat();
			return true;
		}
		else if (w == m_DamageEditBox && (GetCurrentItemIndex() >= 0 || GetCurrentPresetName() != ""))
		{
			m_ConfigDebugProfile.SetItemHealth(GetCurrentPresetName(), GetCurrentItemIndex(), m_DamageEditBox.GetText().ToFloat());
			return true;
		}
		else if (w == m_WithPhysicsCheckbox && (GetCurrentItemIndex() >= 0 || GetCurrentPresetName() != ""))
		{
			m_ConfigDebugProfile.SetItemWithPhysics(GetCurrentPresetName(), GetCurrentItemIndex(), m_WithPhysicsCheckbox.IsChecked());
			return true;
		}
		else if (w == m_BatchSpawnQuantity)
		{
			m_ConfigDebugProfile.SetBatchSpawnQuantity(m_BatchSpawnQuantity.GetText().ToInt());
			return true;
		}
		else if ((w == m_RectSpawnRow || w == m_RectSpawnColumn ||  w == m_RectSpawnRowStep ||  w == m_RectSpawnColumnStep))
		{
			if (w == m_RectSpawnRow || w == m_RectSpawnColumn)
			{
				int rowQ = m_RectSpawnRow.GetText().ToInt();
				int columnQ = m_RectSpawnColumn.GetText().ToInt();
				int result = columnQ * rowQ;
				m_BatchSpawnQuantity.SetText(result.ToString());
				m_ConfigDebugProfile.SetBatchSpawnQuantity(m_BatchSpawnQuantity.GetText().ToInt());
			}
			
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetBatchSpawnRectangle(m_RectSpawnRow.GetText().ToInt(),m_RectSpawnColumn.GetText().ToInt(), m_RectSpawnRowStep.GetText().ToFloat(),m_RectSpawnColumnStep.GetText().ToFloat());
			}
			return true;
		}
		return false;
	}
	
	override bool OnItemSelected(Widget w, int x, int y, int row, int  column,	int  oldRow, int  oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
		if (w == m_PresetsTextListbox && (m_SelectedObjectIsPreset == 1 || m_SelectedObjectIsPreset == -1))
		{
			if (m_PresetsTextListbox.GetSelectedRow() != -1)
			{
				SelectPreset();
				return true;
			}
		}
		else if (w == m_PresetItemsTextListbox)
		{
			if (m_PresetItemsTextListbox.GetSelectedRow() != -1)
			{
				ShowItemButtons();
				ShowItemTransferButtons();
				m_SelectedObjectText.SetText(GetCurrentItemName());
				SetObject(GetCurrentItemName());
				m_SelectedObjectIsPreset = 0;
			}
			return true;
		}
		else if (w == m_ObjectsTextListbox)
		{
			SelectObject(false);
			return true;
		}
		return false;
	}
	
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w,x,y,button);
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		int objects_row_index;
		if (w == m_CategoryMergeType)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetMergeType(m_CategoryMergeType.IsChecked());
			}
			ChangeFilterItems(true);
			// Refresh UI by new settings
			m_MissionGameplay.GetHudDebug().RefreshByLocalProfile();
			return true;
		}
		else if (w == m_ObjectFilter)
		{
			ChangeFilterItems(true);
			return true;
		}
		else if (w == m_ItemPreviewCheckbox)
		{
			if (m_ConfigDebugProfile)
			{
				m_ConfigDebugProfile.SetShowItemPreview(m_ItemPreviewCheckbox.IsChecked());
				if (!m_ItemPreviewCheckbox.IsChecked())
				{
					m_ItemPreviewWidget.SetItem(null);
				}
			}
		}
		else if (w == m_SpawnInInvButton || w == m_SpawnGroundButton || w == m_SpawnOnTarget || w == m_SpawnOnCursor || w == m_SpawnInHands)
		{
			SaveProfileSpawnDistance();

			objects_row_index = m_ObjectsTextListbox.GetSelectedRow();
			
			vector rayStart = g_Game.GetCurrentCameraPosition();
			vector rayEnd = rayStart + g_Game.GetCurrentCameraDirection() * 50;		
			vector hitPos;
			vector hitNormal;
			int hitComponentIndex;		
			set<Object> hitObjects = new set<Object>;
			//DayZPhysics.RaycastRV(rayStart, rayEnd, hitPos, hitNormal, hitComponentIndex, hitObjects, NULL, player,);
			DayZPhysics.RaycastRV(rayStart, rayEnd, hitPos, hitNormal, hitComponentIndex, hitObjects, NULL, player, false, false, ObjIntersectView, 0.3);
					
			EntityAI target;
			if (hitObjects.Count())
				target = EntityAI.Cast(hitObjects.Get(0));
			
			if (m_SelectedObject != "")
			{
				// @NOTE: duplicate code in PluginDeveloper.c
				float distance = m_SpawnDistanceEditBox.GetText().ToFloat();
				
				
				if (m_SelectedObjectIsPreset == 1)
				{
					//------------ PRESETS -------------
					switch (w)
					{
						case m_SpawnOnTarget:
						{
							if (target && target.IsInherited(EntityAI))
							{
								EntityAI att_parent = EntityAI.Cast(target) ;
								SpawnPreset(att_parent, CLEAR_IVN, m_SelectedObject);
							}
							break;
						}
						case m_SpawnGroundButton:
						{
							SpawnPreset(player, false, m_SelectedObject, InventoryLocationType.GROUND, distance);
							break;
						}						
						default:
						{
							SpawnPreset(player, CLEAR_IVN, m_SelectedObject);
							break;
						}
					}
				}
				else
				{
					//------------SINGLE ITEMS ------------
					
					
					float health = m_DamageEditBox.GetText().ToFloat() * MiscGameplayFunctions.GetTypeMaxGlobalHealth(m_SelectedObject);
					float quantity = m_QuantityEditBox.GetText().ToFloat();
					bool withPhysics = m_WithPhysicsCheckbox.IsChecked();

					bool spawnOnCrossHair = DeveloperFreeCamera.IsFreeCameraEnabled() || w == m_SpawnOnCursor;


					switch (w)
					{
						case m_SpawnGroundPatternGrid:
						{
							m_Developer.SpawnEntityOnCursorDir(player, m_SelectedObject, quantity, distance, health, m_IsShiftDown, "", withPhysics);
							break;
						}
						
						case m_SpawnOnTarget:
						{
							if (target && target.IsInherited(EntityAI))
								m_Developer.SpawnEntityInInventory(target, m_SelectedObject, health, quantity, m_IsShiftDown);
							break;
						}

						case m_SpawnInInvButton:
						{
							m_Developer.SpawnEntityInPlayerInventory(player, m_SelectedObject, -1, quantity, m_IsShiftDown);
							break;
						}
						case m_SpawnInHands:
						{
							m_Developer.SpawnEntityInPlayerInventory(player, m_SelectedObject, -1, quantity, m_IsShiftDown,"", FindInventoryLocationType.HANDS);
							break;
						}
						case m_SpawnOnCursor:
						case m_SpawnGroundButton:
						{
							if (spawnOnCrossHair)
							{
								m_Developer.SpawnItemOnCrosshair(player, m_SelectedObject, health, quantity, 40, true, m_IsShiftDown, withPhysics );
							}
							else
							{
								m_Developer.SpawnEntityOnCursorDir(player, m_SelectedObject, quantity, distance, health, m_IsShiftDown, "", withPhysics);
							}
							break;
						}
					}
				}
			}
			return true;
		}
		else if (w == m_FilterOrderButton)
		{
			SetFilterOrder(!m_FilterOrderReversed);
			return true;
		}
		else if (w == m_PresetsTextListbox)
		{
			SelectPreset();
			return true;
		}
		else if (w == m_PresetItemsTextListbox)
		{
			m_ObjectsTextListbox.SelectRow(-1);
			ShowItemButtons();
			ShowItemTransferButtons();
			m_SelectedObjectText.SetText("Object : " + GetCurrentItemName());
			SetObject(GetCurrentItemName());
			m_SelectedObjectIsPreset = 0;

			return true;
		}
		else if (w == m_PresetSetDefaultButton)
		{
			if (GetCurrentPresetName()!= "")
			{
				SetDefaultPreset(GetCurrentPresetIndex());
			}
			return true;
		}
		else if (w == m_PresetNewButton)
		{
			g_Game.GetUIManager().EnterScriptedMenu(MENU_SCRIPTCONSOLE_DIALOG_PRESET_NAME, m_ScriptConsole);
			return true;
		}
		else if (w == m_PresetDeleteButton)
		{
			if (GetCurrentPresetName()!= "")
			{
				DeletePreset();
			}
			return true;
		}
		else if (w == m_PresetRenameButton)
		{
			if (GetCurrentPresetName()!= "")
			{
				g_Game.GetUIManager().EnterScriptedMenu(MENU_SCRIPTCONSOLE_DIALOG_PRESET_RENAME, m_ScriptConsole);
			}
			return true;
		}
		else if (w == m_PresetAddItemtButton)
		{
			AddItemToPreset();
			return true;
		}
		else if (w == m_ItemNameCopy)
		{
			AddItemToClipboard(m_ObjectsTextListbox);
			return true;
		}
		else if (w == m_PresetRemoveItemButton)
		{
			RemoveItemFromPreset();
			return true;
		}
		else if (w == m_ItemMoveUpButton)
		{
			ItemMoveUp();
			return true;
		}
		else if (w == m_ItemMoveDownButton)
		{
			ItemMoveDown();
			return true;
		}
		else if (w == m_SpawnBatchButton)
		{
			int count = m_BatchSpawnQuantity.GetText().ToInt();
			int rows = m_RectSpawnRow.GetText().ToInt();
			int columns = m_RectSpawnColumn.GetText().ToInt();
			float rowStep = m_RectSpawnRowStep.GetText().ToFloat();
			float columnStep = m_RectSpawnColumnStep.GetText().ToFloat();
			m_Developer.SpawnEntityOnGroundPatternGrid(player, m_SelectedObject,count, m_DamageEditBox.GetText().ToFloat(), 1, rows, columns, rowStep, columnStep, m_IsShiftDown, m_WithPhysicsCheckbox.IsChecked());
			return true;
		}
		else if (w == m_ReloadShapeButton)
		{
#ifdef DEVELOPER
			Object obj = m_ItemPreviewWidget.GetItem();
			if (obj)
			{
				g_Game.ReloadShape(obj);
			}
#endif
		}
		else if (w == m_ListActions)
		{
			if (!m_PreviewEntity)
				return false;
			
			return ListItemRelatedActions();
		}
		else if (w == m_ObjectsTextListbox)
		{
			SelectObject();
			return true;
		}
		else if (w == m_DrawInWorld)
		{
			DrawItems(m_SelectedObject);
			return true;
			
		}
		else if (w == m_DrawInWorldClear)
		{
			DrawItemsClear();
			return true;
		}

		return false;
	}
	
	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		super.OnDoubleClick(w, x, y, button);

		int i;
		int objects_row_index;
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		
		if (w == m_ObjectsTextListbox)
		{
			//select item
			m_PresetItemsTextListbox.SelectRow(-1);
			HideItemButtons();
			ShowItemTransferButtons();
			m_SelectedObjectText.SetText("Object : " + GetCurrentObjectName());

			m_SelectedObject = GetCurrentObjectName();
			m_LastSelectedObject = m_SelectedObject;
			m_SelectedObjectIsPreset = 0;
			
			//spawn item
			SaveProfileSpawnDistance();

			objects_row_index = m_ObjectsTextListbox.GetSelectedRow();
			
			vector rayStart = g_Game.GetCurrentCameraPosition();
			vector rayEnd = rayStart + g_Game.GetCurrentCameraDirection() * 1.5;		
			vector hitPos;
			vector hitNormal;
			int hitComponentIndex;		
			set<Object> hitObjects = new set<Object>;
			DayZPhysics.RaycastRV(rayStart, rayEnd, hitPos, hitNormal, hitComponentIndex, hitObjects, NULL, player);
					
			Object target = NULL;
			if (hitObjects.Count())
				target = hitObjects.Get(0);
			
			if (m_SelectedObject != "")
			{
				float distance = m_SpawnDistanceEditBox.GetText().ToFloat();
				
				if (button == 0)		//LMB
				{
					m_Developer.SpawnEntityOnCursorDir(player, m_SelectedObject, 1, distance);
					return true;
				}
				else if (button == 1)		//RMB
				{
					if (g_Game.IsMultiplayer())
					{
						m_Developer.SpawnEntityInPlayerInventory(player, m_SelectedObject, -1, -1);
					}
					else
					{
						EntityAI spawned_entity = m_Developer.SpawnEntityOnCursorDir(player, m_SelectedObject, -1, distance);
						player.LocalTakeEntityToHands(spawned_entity);
					}
					return true;
				}
				else if (button == 2) //MMB
				{
					m_Developer.SpawnEntityOnCursorDir(player, m_SelectedObject, 1, distance,-1, true);
					return true;
				}
			}
			
		}
		else if (w == m_QuantityEditBox)
		{
			m_QuantityEditBox.SetText("1.0");
			return true;
		}
		return false;
	}
	
	override void Update(float timeslice)
	{
		super.Update(timeslice);

		UpdateButtonNames();
	}
	
	void SetTextSpawnButton(ButtonWidget w, bool special, string suffix = " Special")
	{
		string text = m_SpawnButtonsOriginalText.Get(w);
		if (special)
			text += suffix;
		w.SetText(text);
	}
	
	void UpdateButtonNames()
	{
		SetTextSpawnButton(m_SpawnGroundButton, m_IsShiftDown);
		SetTextSpawnButton(m_SpawnInInvButton, m_IsShiftDown);
		SetTextSpawnButton(m_SpawnOnTarget, m_IsShiftDown);
		SetTextSpawnButton(m_SpawnOnCursor, m_IsShiftDown);
		SetTextSpawnButton(m_SpawnBatchButton, m_IsShiftDown);
		SetTextSpawnButton(m_SpawnInHands, m_IsShiftDown);
	}
	
	TStringArray GetItemsClasses()
	{
		return {CFG_VEHICLESPATH, CFG_WEAPONSPATH, CFG_MAGAZINESPATH, CFG_AMMO};
	}
	
	protected string PreprocessFilterText(MultilineEditBoxWidget w)
	{
		string widgetText;
		w.GetText(widgetText);
		
		if (widgetText.Contains(","))
		{
			widgetText.Replace(","," ");
			w.SetLine(0,widgetText);
		}
		return widgetText;
	}
	
	void ChangeFilterItems(bool forced = false)
	{
		string widgetText = PreprocessFilterText(m_ObjectFilter);
		if (widgetText == m_FilterTextPrev && !forced)
			return;
		
		m_FilterTextPrev = widgetText;
		string txt = m_ConfigDebugProfile.GetItemSearch();

		m_ConfigDebugProfile.SetItemSearch(widgetText);
		ChangeFilter(GetItemsClasses(), m_ObjectsTextListbox,m_ObjectFilter, m_CategoryMask);
	}
	
	private bool ListItemRelatedActions()
	{
		string message = "";
		
		array<ActionBase_Basic> interactActions = new array<ActionBase_Basic>();
		m_PreviewEntity.GetActions(InteractActionInput, interactActions);
		if (interactActions)
		{
			message += "\n== Interact (F)\n\n";
			foreach (ActionBase_Basic iaction : interactActions)
			{
			  	if (iaction.Type() == ActionWorldCraft)
			    	continue;
			
			  	ActionBase ia = ActionBase.Cast(iaction);
			  	message += string.Format("%1 <%2>\n", Widget.TranslateString(ia.GetText()), ia.ClassName());
			}
		}
		
		array<ActionBase_Basic> continuousInteractActions = new array<ActionBase_Basic>();
		m_PreviewEntity.GetActions(ContinuousInteractActionInput, continuousInteractActions);
		if (continuousInteractActions)
		{
			message += "\n== Continuous Interact (F hold)\n\n";
			foreach (ActionBase_Basic ciaction : continuousInteractActions)
			{
			  	if (ciaction.Type() == ActionWorldCraft)
			    	continue;
			
			  	ActionBase cia = ActionBase.Cast(ciaction);
			  	message += string.Format("%1 <%2>\n", Widget.TranslateString(cia.GetText()), cia.ClassName());
			}
		}

		array<ActionBase_Basic> singleActions = new array<ActionBase_Basic>();
		m_PreviewEntity.GetActions(DefaultActionInput, singleActions);
		if (singleActions)
		{
			message += "\n== Single (LMB)\n\n";
			foreach (ActionBase_Basic saction : singleActions)
			{
				if (saction.Type() == ActionWorldCraft)
			    	continue;
			
			  	ActionBase sa = ActionBase.Cast(saction);
			  	message += string.Format("%1 <%2>\n", Widget.TranslateString(sa.GetText()), sa.ClassName());
			}
		}

		array<ActionBase_Basic> continuousActions = new array<ActionBase_Basic>();
		m_PreviewEntity.GetActions(ContinuousDefaultActionInput, continuousActions);
		if (continuousActions)
		{
			ActionBase_Basic craftingAction;
			message += "\n== Continuous (LMB hold)\n\n";
			foreach (ActionBase_Basic caction : continuousActions)
			{
				if (caction.Type() != ActionWorldCraft)
				{
			  		ActionBase ca = ActionBase.Cast(caction);
			  		message += string.Format("%1 <%2>\n", Widget.TranslateString(ca.GetText()), ca.ClassName());
				}
				else
				{
					craftingAction = caction;
				}
			}
			message += "\n== Crafting recipes \n\n";
			if (craftingAction)
			{	
				PluginRecipesManager pluginRecipesManager = PluginRecipesManager.Cast(GetPlugin(PluginRecipesManager));
				array<RecipeBase> recipes = pluginRecipesManager.GetRecipesForItem(m_PreviewEntity.GetType());
				foreach (RecipeBase recipe : recipes)
				{
					if (recipe.IsItemInRecipe("Inventory_Base"))
						continue;
					
					message += string.Format("%1 <%2>\n", Widget.TranslateString(recipe.GetName()), recipe.Type());
				}
			}
		}
		
		ScriptConsoleUniversalInfoDialog infoDialog = ScriptConsoleUniversalInfoDialog.Cast(g_Game.GetUIManager().EnterScriptedMenu(MENU_SCRIPTCONSOLE_UNIVERSAL_INFO_DIALOG, m_ScriptConsole));
		infoDialog.SetLabel(string.Format("%1 related actions", m_PreviewEntity.GetType()));
		infoDialog.SetContent(message);

		return true;
	}
}
