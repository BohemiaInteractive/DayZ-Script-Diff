enum eConstructionIconItemState
{
	INVALID = -1,
	HIDDEN,
	SHOWN
}

class ConstructionInfoIcon: GenericIconBase
{
	protected int m_OriginalGhostIconColor;
	protected ConstructionPart m_Part;
	protected eConstructionIconItemState m_ItemState = eConstructionIconItemState.INVALID;
	
	override protected void InitIconWidgets(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIconWidgets(parent,root,index,slot_parent);
		
		m_OriginalGhostIconColor = m_GhostSlot.GetColor();
		m_ImageBackdrop.Show(true);
	}
	
	void SetConstructionPart(ConstructionPart part)
	{
		m_Part = part;
	}
	
	override void ResetIconValues()
	{
		super.ResetIconValues();
		
		m_Part = null;
	}
	
	override void Init(EntityAI obj, bool reservation = false)
	{
		super.Init(obj, reservation);
		
		m_ItemState = eConstructionIconItemState.SHOWN;
	}
	
	override void Clear()
	{
		super.Clear();
		
		m_ItemState = eConstructionIconItemState.HIDDEN;
	}
	
	//!disconnected from construction info handling, no need to select or focus anything
	override void SetActive(bool active){}
}

class ConstructionToolInfoIcon: ConstructionInfoIcon
{
	protected bool m_ShowIcon; //set from outside (wider context), the icon just handles its own updates
	protected ItemBase m_Tool;
	
	override protected void InitIcon(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIcon(parent,root,index,slot_parent);
		
		InitGhostIcon();
	}
	
	protected void InitGhostIcon()
	{
		string iconPath = "set:dayz_inventory image:missing";
		switch (m_IconIndex)
		{
			case EConstructionTools.TOOL_MOUNT_WIRE:
				iconPath = "set:dayz_inventory image:plierstools";
			break
			
			case EConstructionTools.TOOL_NAIL:
				iconPath = "set:dayz_inventory image:hammertools";
			break
			
			case EConstructionTools.TOOL_DIG_BASE:
				iconPath = "set:dayz_inventory image:shoveltools";
			break
			
			case EConstructionTools.TOOL_MOUNT_LOG:
				iconPath = "set:dayz_inventory image:hammertools";
			break
			
			case EConstructionTools.TOOL_HEAVY:
				iconPath = "set:dayz_inventory image:sladgehammertools";
			break
			
			case EConstructionTools.TOOL_EXCAVATION:
				iconPath = "set:dayz_inventory image:pickaxetools";
			break
			
			case EConstructionTools.TOOL_HANDS:
				iconPath = "set:dayz_inventory image:hands";
				m_GhostSlot.SetColor(0xD0FFFFFF);
			break
			
			case EConstructionTools.TOOL_BRICKLAYING:
				iconPath = "set:dayz_inventory image:bricklayingtools";
			break
		}
		
		m_GhostSlot.LoadImageFile(0, iconPath);
		m_GhostSlot.SetImage(0);
	}
	
	override void ResetIconValues()
	{
		super.ResetIconValues();
		
		m_Tool = null;
	}
	
	//TODO: remove!
	void SetTool(ItemBase tool)
	{
		m_Tool = tool;
	}
	
	void SetShowIcon(bool show)
	{
		m_ShowIcon = show;
	}
	
	void ToolCompatibilityWarning(bool show)
	{
		m_CrossedImage.Show(show);
	}
	
	//just visibility, all frontloaded, called on change
	override void UpdateInterval()
	{
		//int actionMask = m_Part.GetBuildActionTypeMask();
		if (m_ShowIcon)
		{
			//bool toolOptional = actionMask & EConstructionTools.TOOL_HANDS;
			bool highlight = (m_IconIndex == EConstructionTools.TOOL_HANDS && !m_Tool) || (m_IconIndex != EConstructionTools.TOOL_HANDS && m_Tool != null);
			//m_GenericWarning.Show(!toolOptional);
			
			if (m_Tool)
			{
				Init(m_Tool); //render variant
			}
			else
			{
				Clear();
			}
			m_ColWidget.Show(highlight);
			OnShow();
		}
		else
		{
			OnHide();
			//m_GenericWarning.Show(false);
		}
		
		super.UpdateInterval();
	}
}

class ConstructionMaterialInfoIcon: ConstructionInfoIcon
{
	const string QUANTITY_TEXT_COLOR_DARKER_ARGB = "0xFF7C7C7C";
	const string QUANTITY_TEXT_COLOR_LIGHTER_ARGB = "0xFFC8C8C8";
	protected bool m_ProcessDataChanges; //only true when a part or target entity changed //TODO: figure out something better?
	protected int m_LastSufficientMaterialState;
	protected EntityAI m_TargetedEntity; //owner of the construction part (and inventory)
	protected ConstructionMaterialData m_MaterialData;
	
	override protected void InitIcon(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIcon(parent,root,index,slot_parent);
		
		m_ProcessDataChanges = false;
		m_LastSufficientMaterialState = -1;
	}
	
	/*override protected void InitIconWidgets(LayoutHolder parent, Widget root, int index, EntityAI slot_parent)
	{
		super.InitIconWidgets(parent,root,index,slot_parent);
		
		m_ImageBackdrop.Show(true);
	}*/
	
	protected void InitGhostIcon()
	{
		string iconName = "set:dayz_inventory image:missing";
		string path = "CfgSlots" + " Slot_" + m_MaterialData.m_MaterialSlotName + " ghostIcon";
		if (GetGame().ConfigIsExisting(path))
			GetGame().ConfigGetText(path,iconName);
		m_GhostSlot.LoadImageFile(0,iconName);
		m_GhostSlot.SetImage(0);
	}
	
	void SetTargetEntity(EntityAI targetEnt)
	{
		if (m_TargetedEntity != targetEnt)
		{
			m_ProcessDataChanges = true;
			m_TargetedEntity = targetEnt;
		}
	}
	
	override void SetConstructionPart(ConstructionPart part)
	{
		if (m_Part != part)
			m_ProcessDataChanges = true;
		
		super.SetConstructionPart(part);
	}
	
	override void ResetIconValues()
	{
		super.ResetIconValues();
		
		m_TargetedEntity = null;
		m_MaterialData = null;
		m_ProcessDataChanges = false;
	}
	
	protected void InitMaterialInfo()
	{
		array<ref ConstructionMaterialData> materials = m_Part.GetMaterialsData();
		if (m_TargetedEntity && materials && materials.Count() > m_IconIndex)
		{
			bool show = false;
			
			m_MaterialData = materials[m_IconIndex];
			SetSlotID(InventorySlots.INVALID);
			
			if (m_MaterialData.m_MaterialSlotID != InventorySlots.INVALID)
			{
				SetSlotID(m_MaterialData.m_MaterialSlotID);
				InitGhostIcon();
				m_ItemState = eConstructionIconItemState.INVALID;
				show = CheckDisplayableAttachment();
			}
			else
			{
				ErrorEx("No valid slot index for slot named: " + m_MaterialData.m_MaterialSlotName);
			}
			
			if (show)
			{
				OnShow();
			}
			else
			{
				//ResetIconValues();
				OnHide();
			}
		}
		else
		{
			//ResetIconValues();
			OnHide();
		}
	}
	
	protected bool CheckDisplayableAttachment()
	{
		if (GetSlotID() != InventorySlots.INVALID && m_TargetedEntity.CanDisplayAttachmentSlot(m_MaterialData.m_MaterialSlotID))
		{
			EntityAI child = m_TargetedEntity.FindAttachmentBySlotName(m_MaterialData.m_MaterialSlotName);
			if (child)
			{
				if (m_ItemState != eConstructionIconItemState.SHOWN)
					Init(child);
			}
			else if (m_ItemState != eConstructionIconItemState.HIDDEN)
			{
				Clear();
			}
			
			return true;
		}
		
		return false;
	}
	
	override void UpdateInterval()
	{
		if (m_ProcessDataChanges)
		{
			Clear();
			InitMaterialInfo();
			m_ProcessDataChanges = false;
		}
		else
		{
			CheckDisplayableAttachment(); //have to check periodically. TODO: link some invoker method/s to add/remove/qtyChange events on target entity?
		}
		
		super.UpdateInterval();
	}
	
	//TODO: prototype stuff, remove and address in parent classes
	override void SetQuantity()
	{
		if (m_Item)
		{
			//m_QuantityItem.SetAlpha(1.0);
			int quantityType = QuantityConversions.HasItemQuantity(m_Item);
			if (quantityType != QUANTITY_HIDDEN && m_CurrQuantity != QuantityConversions.GetItemQuantity(m_Item))
			{
				float requiredQty;
				float lastQuantity = m_CurrQuantity;
				m_CurrQuantity = QuantityConversions.GetItemQuantity(m_Item);
				if (lastQuantity == m_CurrQuantity) //skip except quantity changed
					return;
				
				string color1, color2;
				
				if (quantityType == QUANTITY_COUNT)
				{
					requiredQty = GetContextualSlotMax();
					bool sufficient = m_CurrQuantity >= requiredQty;
					if (m_LastSufficientMaterialState != sufficient)
					{
						m_ColWidget.Show(sufficient);
						m_ItemPreview.Show(sufficient);
						m_GhostSlot.Show(!sufficient);
						
						if (!sufficient)
						{
							color1 = QUANTITY_TEXT_COLOR_DARKER_ARGB;
							color2 = QUANTITY_TEXT_COLOR_LIGHTER_ARGB;
							
							string iconPath = m_Item.GetInventoryItemType().GetGhostIconImagepath();
							if (iconPath != "")
							{
								m_GhostSlot.LoadImageFile(1, iconPath, false);
								m_GhostSlot.SetImage(1);
								m_GhostSlot.SetColor(0xFFFFFFFF);
							}
						}
						else
						{
							color1 = COLOR_WHITE.ToString();
							color2 = COLOR_WHITE.ToString();
						}
						
						m_QuantityStack.Show(requiredQty != 1);
						m_QuantityProgress.Show(false);
						m_LastSufficientMaterialState = sufficient;
						
						RichTextWidget.Cast(m_QuantityItem).SetText(string.Format("<color hex=\"%1\">%2</color><color hex=\"%3\">/%4</color>", color1, m_CurrQuantity.ToString(), color2, requiredQty.ToString()));
					}
				}
				/*else if (quantityType == QUANTITY_PROGRESS)
				{
					requiredQty 		= GetContextualSlotMax();
					
					float value = Math.Round((m_CurrQuantity / requiredQty) * 100);
					m_QuantityProgress.SetCurrent(value);
					m_QuantityStack.Show(false);
					m_QuantityProgress.Show(true);
				}*/
			}
		}
	}
	
	override void ClearRemainSelected()
	{
		super.ClearRemainSelected();
		
		m_GhostSlot.SetImage(0);
		m_GhostSlot.SetColor(m_OriginalGhostIconColor);
		m_LastSufficientMaterialState = -1;
		
		if (GetSlotID() != InventorySlots.INVALID)
		{
			int stackMax = GetContextualSlotMax();
			if (stackMax > 0)
			{
				m_QuantityPanel.Show(true);
				RichTextWidget.Cast(m_QuantityItem).SetText(string.Format("<color hex=\"%1\">%2</color><color hex=\"%3\">/%4</color>", QUANTITY_TEXT_COLOR_DARKER_ARGB, "0", QUANTITY_TEXT_COLOR_LIGHTER_ARGB, GetContextualSlotMax().ToString()));
				//m_QuantityItem.SetAlpha(0.5019); //TODO: ffs...
				m_QuantityItem.Show(true);
				m_QuantityStack.Show(true);
				m_QuantityProgress.Show(false);
				m_PanelWidget.Show(true);
				m_ItemPreview.Show(false);
			}
		}
	}
	
	override protected int GetContextualSlotMax()
	{
		return m_MaterialData.m_MaterialQuantity; //desired material quantity used as max for this purpose
	}
}

