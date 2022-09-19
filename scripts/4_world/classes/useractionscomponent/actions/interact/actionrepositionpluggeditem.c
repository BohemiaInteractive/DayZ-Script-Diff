class ActionRepositionPluggedItem: ActionInteractBase
{
	protected ItemBase m_SourceForReplug = NULL;

// Through this action players can reposition already placed electric devices without unplugging them from the power source.
	void ActionRepositionPluggedItem()
	{
		m_CallbackClass = ActionInteractBaseCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		/*m_CallbackClass = ActionBuildPartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;*/
		m_Text = "#reposition";
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return false;
	}

	/*override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase target_IB = ItemBase.Cast( target.GetObject() );
		
		if ( !player.GetItemInHands() || ItemIsBeingRepositioned() )
		{
			if (target_IB.HasEnergyManager())
			{
				if ( target_IB.GetCompEM().IsPlugged() || ItemIsBeingRepositioned() )
				{
					return true;
				}
			}
		}

		return false;
	}*/

	/*override void OnStartServer( ActionData action_data )
	{	
		Object targetObject = action_data.m_Target.GetObject();
		ItemBase target_IB = ItemBase.Cast( targetObject );
		EntityAI replug_device = target_IB.GetCompEM().GetEnergySource();
		m_SourceForReplug = ItemBase.Cast(replug_device);
	
		action_data.m_Player.ServerTakeEntityToHands( target_IB );
	}

	override void OnStartClient( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		ItemBase target_IB = ItemBase.Cast( targetObject );
		EntityAI replug_device = target_IB.GetCompEM().GetEnergySource();
		m_SourceForReplug = ItemBase.Cast(replug_device);
	}

	override void OnExecuteServer( ActionData action_data )
	{	
		Object targetObject = action_data.m_Target.GetObject();	
		ItemBase target_IB = ItemBase.Cast( targetObject );
		
		if ( m_SourceForReplug.HasEnergyManager() )
		{
			ItemBase attached_device = GetAttachedDevice(m_SourceForReplug);
			
			if (attached_device)
			{
				m_SourceForReplug = attached_device;
			}
			
			target_IB.GetCompEM().PlugThisInto(m_SourceForReplug);
					
			if ( !action_data.m_Player.IsPlacingServer() )
			{
				target_IB.GetInventory().TakeEntityAsAttachment( InventoryMode.LOCAL, m_SourceForReplug );
			}
		}
		
		m_SourceForReplug = NULL;
	}

	override void OnExecuteClient( ActionData action_data )
	{	
		if ( !action_data.m_Player.IsPlacingLocal() )
		{
			Object targetObject = action_data.m_Target.GetObject();	
			ItemBase target_IB = ItemBase.Cast( targetObject );
			action_data.m_Player.TogglePlacingLocal(target_IB);
		}
		
		m_SourceForReplug = NULL;
	}

	ItemBase GetAttachedDevice(ItemBase parent)
	{
		string parent_type = parent.GetType();
		
		if ( parent.IsInherited(CarBattery)  ||  parent.IsInherited(TruckBattery) )
		{
			ItemBase parent_attachment = ItemBase.Cast( parent.GetAttachmentByType(MetalWire) );
			
			if (!parent_attachment)
			{
				parent_attachment = ItemBase.Cast( parent.GetAttachmentByType(BarbedWire) );
			}
			return parent_attachment;
		}
		
		return NULL;
	}

	ItemBase ItemIsBeingRepositioned()
	{
		return m_SourceForReplug;
	}*/
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase tgt_item = ItemBase.Cast( target.GetObject() );
		EntityAI tgt_parent = EntityAI.Cast(tgt_item.GetHierarchyParent());
		if ( !tgt_item || !tgt_item.IsTakeable() || tgt_item.IsBeingPlaced() || (tgt_item.GetCompEM() && !tgt_item.GetCompEM().IsPlugged()) || ( tgt_item.GetHierarchyParent() && !BaseBuildingBase.Cast(tgt_item.GetHierarchyParent()) ) || ((tgt_parent && !tgt_item.CanDetachAttachment(tgt_parent)) || (tgt_parent && !tgt_parent.CanReleaseAttachment(tgt_item))) )
			return false;
		
		if ( tgt_item.HasEnergyManager() && tgt_item.GetCompEM().IsPlugged() )
		{
			return player.GetInventory().CanAddEntityIntoHands(tgt_item);
		}
		
		return false;
	}
	
	override bool CanContinue( ActionData action_data )
	{
		return true;
	}
	
	override void Start( ActionData action_data ) //Setup on start of action
	{
		super.Start( action_data );
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		action_data.m_Player.TogglePlacingLocal(ntarget);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if (GetGame().IsMultiplayer())
			return;
		
		InventoryLocation il = new InventoryLocation;
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		ClearInventoryReservationEx(action_data);
		
		action_data.m_Player.PredictiveTakeEntityToHands( ntarget );
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		InventoryLocation il = new InventoryLocation;
		ItemBase ntarget = ItemBase.Cast(action_data.m_Target.GetObject());
		ClearInventoryReservationEx(action_data);
		
		action_data.m_Player.PredictiveTakeEntityToHands( ntarget );
	}
	
	override void CreateAndSetupActionCallback( ActionData action_data )
	{
		//Print("starting - CreateAndSetupActionCallback");
		EntityAI target = EntityAI.Cast(action_data.m_Target.GetObject());
		ActionBaseCB callback;
		if (!target)
			return;
		
		if (target.IsHeavyBehaviour())
		{
			//Print("heavybehaviour");
			Class.CastTo(callback, action_data.m_Player.StartCommand_Action(DayZPlayerConstants.CMD_ACTIONFB_PICKUP_HEAVY,GetCallbackClassTypename(),DayZPlayerConstants.STANCEMASK_ERECT));
		}
		else
		{
			Class.CastTo(callback, action_data.m_Player.AddCommandModifier_Action(DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS,GetCallbackClassTypename()));
		}
		callback.SetActionData(action_data); 
		callback.InitActionComponent();
		action_data.m_Callback = callback;
	}
	
		
	override bool CanBeUsedOnBack()
	{
		return false;
	}
};