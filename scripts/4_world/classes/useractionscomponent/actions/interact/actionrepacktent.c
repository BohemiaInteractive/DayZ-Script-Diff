class ActionRepackTentCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.UNPACK );
	}
	
	void DropDuringRepacking()
	{
		vector orientation = m_ActionData.m_Player.GetOrientation();
		vector position = m_ActionData.m_Player.GetPosition() + m_ActionData.m_Player.GetDirection();
		vector rotation_matrix[3];
		float direction[4];
		InventoryLocation source = new InventoryLocation;
		InventoryLocation destination = new InventoryLocation;
		
		Math3D.YawPitchRollMatrix( orientation, rotation_matrix );
		Math3D.MatrixToQuat( rotation_matrix, direction );
			
		vector ground_position = position;
		ground_position[1] = GetGame().SurfaceY(ground_position[0],ground_position[2]);
		
		if ( vector.DistanceSq( m_ActionData.m_Player.GetPosition(), ground_position ) > UAMaxDistances.DEFAULT * UAMaxDistances.DEFAULT)
		{
			if ( m_ActionData.m_MainItem.GetInventory().GetCurrentInventoryLocation( source ) )
			{
				destination.SetGroundEx( m_ActionData.m_MainItem, position, direction );
				m_ActionData.m_Player.PredictiveTakeToDst(source, destination);
			}
		}
		else
		{
			if ( m_ActionData.m_MainItem.GetInventory().GetCurrentInventoryLocation( source ) )
			{
				destination.SetGroundEx( m_ActionData.m_MainItem, ground_position, direction );
				m_ActionData.m_Player.PredictiveTakeToDst(source, destination);
			}
		}
	}
};

class ActionRepackTent: ActionContinuousBase
{
	EntityAI m_RepackedEntity;
	bool m_IsFinished;
	
	void ActionRepackTent()
	{
		m_CallbackClass = ActionRepackTentCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = 0;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#repack_tent";
	}

	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasProgress()
	{
		return true;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		if ( !player.IsPlacingLocal() )
		{
			return true;
		}
		return false;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{	
		SetupAnimation( item );
		
		if ( super.SetupAction(player, target, item, action_data, extra_data ))
		{			
			return true;
		}
		return false;
	}
	
	override void OnStartServer( ActionData action_data )
	{
		m_RepackedEntity = null;
		m_IsFinished = false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{			
		Param1<bool> play = new Param1<bool>( false );
		GetGame().RPCSingleParam( action_data.m_MainItem, SoundTypeTent.REPACK, play, true );
		
		m_IsFinished = true;
	}
	
	override void OnEndServer( ActionData action_data )
	{		
		if ( m_IsFinished )
		{
			if ( action_data.m_MainItem.IsInherited( TentBase ) ) 
			{
				RepackLambda lambda_back_pack = new RepackLambda(action_data.m_MainItem, "LargeTentBackPack", action_data.m_Player);
				action_data.m_Player.ServerReplaceItemElsewhereWithNewInHands(lambda_back_pack);
			}
			
			if (  action_data.m_MainItem.IsInherited( Clothing ) ) 
			{
				RepackLambda lambda_tent = new RepackLambda(action_data.m_MainItem, "LargeTent", action_data.m_Player);
				action_data.m_Player.ServerReplaceItemElsewhereWithNewInHands(lambda_tent);
			}
		}
		else
		{
			if ( GetGame().IsMultiplayer() )
			{		
				if ( action_data.m_MainItem )
				{
					action_data.m_Player.ServerTakeEntityToHands( action_data.m_MainItem );
				}
			}
			else
			{
				if ( action_data.m_MainItem )
				{
					action_data.m_Player.LocalTakeEntityToHands( action_data.m_MainItem );
				}
			}
		}
	}
	
	void SetupAnimation( ItemBase item )
	{
		if ( item.IsHeavyBehaviour() )
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_HEAVY;
		}
		else if ( item.IsOneHandedBehaviour() )
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_1HD; 
		}
		else if ( item.IsTwoHandedBehaviour() )
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
		}
		else
		{
			Print("Error: check " + item + " behaviour");
		}
	}
	
	override void OnStartAnimationLoopClient( ActionData action_data )
	{					
		if ( action_data.m_Player.GetItemInHands() )
			ActionRepackTentCB.Cast(action_data.m_Callback).DropDuringRepacking();
	}
	
	override void OnStartAnimationLoopServer( ActionData action_data )
	{
		Param1<bool> play = new Param1<bool>( false );
		if ( !GetGame().IsMultiplayer() )
		{		
			if ( action_data.m_Player.GetItemInHands() )
				ActionRepackTentCB.Cast(action_data.m_Callback).DropDuringRepacking();
		}		

		if ( action_data.m_Player.GetItemInHands() )
		{
			play = new Param1<bool>( true );
			GetGame().RPCSingleParam( action_data.m_MainItem, SoundTypeTent.REPACK, play, true );
		}
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			Param1<bool> play = new Param1<bool>( false );
			GetGame().RPCSingleParam( action_data.m_MainItem, SoundTypeTent.REPACK, play, true );
		}
	}
};

class RepackLambda : TurnItemIntoItemLambda
{
	EntityAI m_RepackedNewEntity;
	
	void RepackLambda (EntityAI old_item, string new_item_type, PlayerBase player)
	{
		InventoryLocation hands = new InventoryLocation;
		hands.SetHands( player, null );
		OverrideNewLocation( hands );
	}
};