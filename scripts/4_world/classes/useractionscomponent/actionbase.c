enum ActionConditionMask
{
	ACM_NO_EXEPTION		= 0,
	ACM_IN_VEHICLE		= 1,
	ACM_ON_LADDER		= 2,
	ACM_SWIMMING		= 4,
	ACM_RESTRAIN		= 8,
	ACM_RAISED			= 16,
	ACM_ON_BACK			= 32,
	ACM_THROWING		= 64,
	ACM_LEANING			= 128,
	ACM_BROKEN_LEGS		= 256,
	ACM_IN_FREELOOK		= 512,
}
class ActionReciveData
{
	ItemBase							m_MainItem;
	ref ActionTarget					m_Target;
}

class ActionOverrideData : Managed
{
	int m_CommandUID = -1;
	int m_CommandUIDProne = -1;
	int m_StanceMask = -1;
}

class ActionData
{
	void ActionData()
	{
		m_State = UA_NONE;
	}
	
	ref ActionBase						m_Action;
	ItemBase							m_MainItem;
	ActionBaseCB 						m_Callback;
	ref CABase							m_ActionComponent;
	int									m_State;
	ref ActionTarget					m_Target;
	PlayerBase							m_Player;
	int 								m_PossibleStanceMask;
	ref array<ref InventoryLocation>	m_ReservedInventoryLocations; //used also for juncture
	int									m_RefreshReservationTimer;
	int									m_RefreshJunctureTimer;
	int									m_DelayedAnimationEventID;
	bool								m_WasExecuted;
	bool								m_WasActionStarted;
	bool								m_ReciveEndInput;
}

class ActionBase : ActionBase_Basic
{	
	//STATIC DATA
	protected int					m_RefreshReservationTimerValue = 140;
	// Configurable action parameters
	protected string				m_Sound; //sound played at the beggining of action
	protected string				m_Text;

	protected bool					m_LockTargetOnUse;	//this parameter sets wheter player can perform actions on target while other player is already performing action on it. defaulted as true
	protected bool 					m_FullBody; //tells whether action is full body or additive
	protected int					m_StanceMask;
	protected ref TStringArray		m_Sounds;			//User action sound is picked from this array randomly
	ref CCIBase 					m_ConditionItem;	//Condition Component
	ref CCTBase						m_ConditionTarget; 	//Condition Component
	protected ActionInput			m_Input;
	protected int 					m_ActionID;
	protected int					m_VariantID;
	int 							m_ConditionMask;
	protected ref ActionVariantManager	m_VariantManager;

	//RUNTIME DATA
	protected ref Param1<string> 	m_MessageParam; //used for passing messages from server to client
	//protected ref Param2<int,int>	m_MessagesParam;
	
	//SOFT SKILLS
	protected float					m_SpecialtyWeight;
	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void ActionBase() 
	{
		// definable
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_PRONE;
		m_FullBody = false;
		m_Sound = "";
		m_Text = "default action text";
		m_LockTargetOnUse = HasTarget();
		// dont override
		m_MessageParam = new Param1<string>("");
		//m_MessagesParam = new Param2<int,int>(0,0);
		m_Sounds = new TStringArray;
		m_Input = null;
		m_ActionID = 0;
		InitConditionMask();
	}
	
	bool IsLockTargetOnUse()
	{
		return m_LockTargetOnUse;
	}
	
	void InitConditionMask()
	{
		m_ConditionMask = ActionConditionMask.ACM_NO_EXEPTION;
		if (CanBeUsedInVehicle())
		{
			m_ConditionMask |= ActionConditionMask.ACM_IN_VEHICLE;
		}
		
		if (CanBeUsedOnLadder())
		{
			m_ConditionMask |= ActionConditionMask.ACM_ON_LADDER;
		}
		
		if (CanBeUsedSwimming())
		{
			m_ConditionMask |= ActionConditionMask.ACM_SWIMMING;
		}
		
		if (CanBeUsedInRestrain())
		{
			m_ConditionMask |= ActionConditionMask.ACM_RESTRAIN;
		}
		
		if (CanBeUsedRaised())
		{
			m_ConditionMask |= ActionConditionMask.ACM_RAISED;
		}
		
		if (CanBeUsedOnBack())
		{
			m_ConditionMask |= ActionConditionMask.ACM_ON_BACK;
		}
		
		if (CanBeUsedThrowing())
		{
			m_ConditionMask |= ActionConditionMask.ACM_THROWING;
		}
		
		if (CanBeUsedLeaning())
		{
			m_ConditionMask |= ActionConditionMask.ACM_LEANING;
		}
		
		if (CanBeUsedWithBrokenLegs())
		{
			m_ConditionMask |= ActionConditionMask.ACM_BROKEN_LEGS;
		}
		
		if (CanBeUsedInFreelook())
			m_ConditionMask |= ActionConditionMask.ACM_IN_FREELOOK;
	}
	
	bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{
		action_data = CreateActionData();
		action_data.m_Action = this;
		action_data.m_Player = player;
		action_data.m_Target = target;
		action_data.m_MainItem = item;
		action_data.m_PossibleStanceMask = GetStanceMask(player);
		action_data.m_ReservedInventoryLocations = new array<ref InventoryLocation>;
		action_data.m_RefreshReservationTimer = m_RefreshReservationTimerValue;
		action_data.m_WasExecuted = false;
		action_data.m_WasActionStarted = false;
		action_data.m_ReciveEndInput = false;
		
		ActionReciveData action_recive_data = player.GetActionManager().GetReciveData();
		if ( action_recive_data )
		{
			HandleReciveData(action_recive_data,action_data);
			
			if ( UseMainItem() && MainItemAlwaysInHands() )
			{
				if ( player.GetItemInHands() != action_data.m_MainItem )
				{
					return false;
				}
			}
		}
		
		if ( !Post_SetupAction( action_data ) )
			return false;
		
		if ( (!GetGame().IsDedicatedServer()) && !IsInstant() )
		{
			if (!InventoryReservation(action_data))
			{
				ClearInventoryReservationEx(action_data);
				return false;
			}
			
			if ( LogManager.IsActionLogEnable() )
			{
				for ( int i = 0; i < action_data.m_ReservedInventoryLocations.Count(); i++)
				{
					Debug.ActionLog( InventoryLocation.DumpToStringNullSafe( action_data.m_ReservedInventoryLocations[i] ), action_data.m_Action.ToString() , "n/a", "LockInventoryList", action_data.m_Player.ToString() );
				}
			}
		}
		
		return true;
	}
	
	bool Post_SetupAction( ActionData action_data )
	{
		return true;
	}
	
	void ActionCleanup( ActionData action_data )
	{}
	
	typename GetInputType()
	{
		return DefaultActionInput;
	}
	
	void SetInput( ActionInput  ai)
	{
		m_Input = ai;
	}
	
	ActionData CreateActionData()
	{
		return new ActionData;
	}
			
	void CreateConditionComponents() //Instantiates components, called once from ActionManager on actions construction
	{
		m_ConditionItem = new CCIDummy;
		m_ConditionTarget = new CCTDummy;
	}
	
	Object GetDisplayInteractObject(PlayerBase player, ActionTarget target)
	{
		return null;
	}

	//! Action is performed on target, not with item itself, when set to true.
	//!   * target will be synced to server
	//!   * UI: floating widget will be displayed at target pos
	bool HasTarget()
	{
		return true;
	}
	
	//! For UI: hiding of progress bar
	bool HasProgress()
	{
		return true;
	}
	
	//Action isn't synchronize to server 
	bool IsLocal()
	{
		return false;
	}
	
	//Action not using animation/callback action are executed instantly (OnStart)
	bool IsInstant()
	{
		return false;
	}

	//! not using plane object - it's using multiple proxies
	bool IsUsingProxies()
	{
		return false;
	}
	
	bool RemoveForceTargetAfterUse()
	{
		return true;
	}

	int GetActionCategory()
	{
		return AC_UNCATEGORIZED;
	}
	
	bool IsEat()
	{
		return false;
	}
	
	bool IsDrink()
	{
		return false;
	}
	
	bool IsShaveSelf()
	{
		return false;
	}

	string GetText() //text game displays in HUD hint 
	{
		return m_Text;
	}
	
	bool CanBePerformedFromQuickbar()
	{
		return false;
	}
	
	bool CanBePerformedFromInventory()
	{
		return false;
	}
	
	//! Condition used in PlayerBase::CanChangeStance to check if stance can be changed while action is performed/executed
	bool CanBePerformedWhileChangingStance()
	{
		return true;
	}
	
	bool CanBeSetFromInventory()
	{
		return !CanBePerformedFromInventory();
	}
	
	bool CanBeUsedInRestrain()
	{
		return false;
	}
	 
	bool CanBeUsedInVehicle()
	{
		return false;
	}
	
	bool CanTargetBeInVehicle()
	{
		return false;
	}
	
	bool CanBeUsedOnBack()
	{
		return false;
	}

	bool CanBeUsedSwimming()
	{
		return false;
	}
	
	bool CanBeUsedOnLadder()
	{
		return false;
	}
	
	bool CanBeUsedRaised()
	{
		return false;
	}
	
	bool CanBeUsedThrowing()
	{
		return false;
	}
	
	bool CanBeUsedLeaning()
	{
		return true;
	}
	
	bool CanBeUsedWithBrokenLegs()
	{
		return true;
	}
	
	bool CanBeUsedInFreelook()
	{
		return true;
	}
	
	//! Is an action directly related to deployment/advanced placing
	bool IsDeploymentAction()
	{
		return false;
	}
	
	bool UseMainItem()
	{
		return true;
	}
	
	bool MainItemAlwaysInHands()
	{
		return true;
	}
	
	/**
	Action will display the content of ActionBase::GetTargetName next to action action name (ActionTargetsCursor::GetActionDesc).
	\returns true if allowed; otherwise false
	*/
	bool DisplayTargetInActionText()
	{
		return false;
	}

	protected bool ActionConditionContinue(ActionData action_data) //condition for action
	{
		return ActionCondition(action_data.m_Player,action_data.m_Target,action_data.m_MainItem);
	}
	
	protected bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) //condition for action
	{
		return true;
	}

	/**
	Used to set the name of action target displayed in UI in case ActionBase::DisplayTargetInActionText is true.
	\param player reference to player running action
	\param target actual target reference
	\returns name of the target
	*/	
	string GetTargetName(PlayerBase player, ActionTarget target)
	{
		return string.Empty;
	}
	
	void ApplyModifiers(ActionData action_data);

	int GetRefreshReservationTimerValue()
	{
		return m_RefreshReservationTimerValue;
	}

	void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		int componentIndex = -1;
		int proxyBoneIdx = -1;
		vector cursorHitPos = vector.Zero;

		array<string> selectionNames = new array<string>();

		Object targetObject = null;
		Object targetParent = null;
		
		if (UseMainItem())
		{
			ctx.Write(action_data.m_MainItem);
		}
			
		if (HasTarget() && !IsUsingProxies())
		{
			// callback data
			targetObject = action_data.m_Target.GetObject();
			ctx.Write(targetObject);
			targetParent = action_data.m_Target.GetParent();
			ctx.Write(targetParent);
			componentIndex = action_data.m_Target.GetComponentIndex();
			ctx.Write(componentIndex);
			cursorHitPos = action_data.m_Target.GetCursorHitPos();
			ctx.Write(cursorHitPos);
		}
		else if( HasTarget() && IsUsingProxies() )
		{
			//! get proxy bone idx from parent and selection we are looking at
			//! ID is used for synchronisation to server where it's translated back to object
			Entity entParent = Entity.Cast(action_data.m_Target.GetParent());
			if (entParent)
			{
				action_data.m_Target.GetObject().GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selectionNames);
				for (int s = 0; s < selectionNames.Count(); s++)
				{
					proxyBoneIdx = entParent.GetBoneIndex(selectionNames[s]);
					if( proxyBoneIdx > -1 )
					{
						break;
					}
				}
			}

			ctx.Write(proxyBoneIdx);
			targetParent = action_data.m_Target.GetParent();
			ctx.Write(targetParent);
			componentIndex = action_data.m_Target.GetComponentIndex();
			ctx.Write(componentIndex);
			cursorHitPos = action_data.m_Target.GetCursorHitPos();
			ctx.Write(cursorHitPos);
		}
	}
	
	bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if ( !action_recive_data )
		{
			action_recive_data = new ActionReciveData;
		}
		Object actionTargetObject = null;
		Object actionTargetParent = null;
		int componentIndex = -1;
		int proxyBoneIdx = -1;
		vector cursorHitPos = vector.Zero;
		ItemBase mainItem = null;
		
		ref ActionTarget target;
		
		if ( UseMainItem() )
		{
			if ( !ctx.Read(mainItem) )
				return false;
		}

		if ( HasTarget() && !IsUsingProxies() )
		{			
			if ( !ctx.Read(actionTargetObject) )
				return false;
							
			if ( !ctx.Read(actionTargetParent))
				return false;

			if ( !ctx.Read(componentIndex) )
				return false;
			
			if ( !ctx.Read(cursorHitPos) )
				return false;
			
			target = new ActionTarget(actionTargetObject, actionTargetParent, componentIndex, cursorHitPos, 0);
			
			action_recive_data.m_Target = target;
		}
		else if( HasTarget() && IsUsingProxies() )
		{
			if ( !ctx.Read(proxyBoneIdx) )
				return false;
							
			if ( !ctx.Read(actionTargetParent))
				return false;

			if ( !ctx.Read(componentIndex) )
				return false;
			
			if ( !ctx.Read(cursorHitPos) )
				return false;

			//! create target object from proxyBoneIdx synced from client
			if ( proxyBoneIdx > -1 )
			{
				Entity entParent = Entity.Cast(actionTargetParent);

				if (entParent)
				{
					actionTargetObject = entParent.GetBoneObject(proxyBoneIdx);		
				}
			}
			else
			{
				return false;
			}
			
			target = new ActionTarget(actionTargetObject, actionTargetParent, componentIndex, cursorHitPos, 0);
						
			action_recive_data.m_Target = target;
		}

		action_recive_data.m_MainItem = mainItem;
		return true;
	}
	
	void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		action_data.m_MainItem = action_recive_data.m_MainItem;
		
		if(HasTarget())
		{
			if (action_recive_data.m_Target)
			{
				action_data.m_Target = action_recive_data.m_Target;
			}
			else
			{
				Error("Action target not created.");
				action_data.m_Target = new ActionTarget(NULL, NULL, -1, vector.Zero, 0);
			}
		}
	}
	
	//----------------------------------------------------------------------------------------------
	// Core methods don't override unless you know what you are doing
	//----------------------------------------------------------------------------------------------		
	// COMMANDS -----------------------------------------------------------------------		
	protected int GetStanceMaskEx(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return GetStanceMask(player);
	}
	
	protected int GetStanceMask(PlayerBase player)
	{
		if ( HasProneException() )
		{
			if ( player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH | DayZPlayerConstants.STANCEMASK_RAISEDPRONE))
				return -1;
			else if ( player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT))
				return DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
			else
				return DayZPlayerConstants.STANCEMASK_PRONE;
		}
		return m_StanceMask;
	}
	
	protected int GetClosestPossibleStance(PlayerBase player, ActionTarget target, ItemBase item)
	{
		int stanceMask = GetStanceMaskEx(player, target, item);
		
		if (player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_ERECT))
		{
			if (stanceMask & DayZPlayerConstants.STANCEMASK_ERECT)
				return DayZPlayerConstants.STANCEMASK_ERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT)
				return DayZPlayerConstants.STANCEMASK_RAISEDERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
				return DayZPlayerConstants.STANCEMASK_CROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH)
				return DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
				return DayZPlayerConstants.STANCEMASK_PRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE)
				return DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
		}
				
		if (player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT))
		{
			if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT)
				return DayZPlayerConstants.STANCEMASK_RAISEDERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_ERECT)
				return DayZPlayerConstants.STANCEMASK_ERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
				return DayZPlayerConstants.STANCEMASK_CROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH)
				return DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
				return DayZPlayerConstants.STANCEMASK_PRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE)
				return DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
		}
		
		if (player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH))
		{
			if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
				return DayZPlayerConstants.STANCEMASK_CROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH)
				return DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_ERECT)
				return DayZPlayerConstants.STANCEMASK_ERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT)
				return DayZPlayerConstants.STANCEMASK_RAISEDERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
				return DayZPlayerConstants.STANCEMASK_PRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE)
				return DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
		}
		
		if (player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDCROUCH))
		{
			if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH)
				return DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
				return DayZPlayerConstants.STANCEMASK_CROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_ERECT)
				return DayZPlayerConstants.STANCEMASK_ERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT)
				return DayZPlayerConstants.STANCEMASK_RAISEDERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
				return DayZPlayerConstants.STANCEMASK_PRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE)
				return DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
		}
		
		if (player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_PRONE))
		{
			if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
				return DayZPlayerConstants.STANCEMASK_PRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE)
				return DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH)
				return DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
				return DayZPlayerConstants.STANCEMASK_CROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_ERECT)
				return DayZPlayerConstants.STANCEMASK_ERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT)
				return DayZPlayerConstants.STANCEMASK_RAISEDERECT;
		}
		
		if (player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDPRONE))
		{
			if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDPRONE)
				return DayZPlayerConstants.STANCEMASK_RAISEDPRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_PRONE)
				return DayZPlayerConstants.STANCEMASK_PRONE;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDCROUCH)
				return DayZPlayerConstants.STANCEMASK_RAISEDCROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_CROUCH)
				return DayZPlayerConstants.STANCEMASK_CROUCH;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_ERECT)
				return DayZPlayerConstants.STANCEMASK_ERECT;
			else if (stanceMask & DayZPlayerConstants.STANCEMASK_RAISEDERECT)
				return DayZPlayerConstants.STANCEMASK_RAISEDERECT;
		}
		
		return -1;
	}
	
	protected bool IsFullBodyEx(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return IsFullBody(player);
	}
	
	bool IsFullBody(PlayerBase player)
	{
		if ( HasProneException() )
		{
			return !( player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT) );
		}
		return m_FullBody;
	}
	
	// if it is set to true if action have special fullbody animation for prone and additive for crouch and erect
	protected bool HasProneException()
	{
		return false;
	}
	
	//	ACTION LOGIC -------------------------------------------------------------------
	// called from actionmanager.c
	void Start( ActionData action_data ) //Setup on start of action
	{
		action_data.m_State = UA_START;
		
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog("Time stamp: " + action_data.m_Player.GetSimulationTimeStamp(), this.ToString() , "n/a", "OnStart", action_data.m_Player.ToString() );
		}
		
		OnStart(action_data);
		
		if ( GetGame().IsServer() )
		{
			OnStartServer(action_data);
			
			string soundCat = GetSoundCategory(action_data);
			if (soundCat)
				action_data.m_Player.SetSoundCategoryHash(soundCat.Hash());
		}
		else
		{
			OnStartClient(action_data);
		}	
		
		InformPlayers(action_data.m_Player,action_data.m_Target,UA_START);
	}
	
	void End( ActionData action_data )
	{
		if ( action_data.m_Player )
		{
			OnEnd(action_data);
			
			if ( GetGame().IsServer() )
			{
				OnEndServer(action_data);
			}
			else
			{
				OnEndClient(action_data);
			}
			
			action_data.m_Player.GetActionManager().OnActionEnd();
		}
	}
	
	void Interrupt(ActionData action_data)
	{
		End(action_data);
	}
	
	void OnEndInput(ActionData action_data)
	{}
	
	void EndInput(ActionData action_data)
	{
		action_data.m_ReciveEndInput = true;
		OnEndInput(action_data);
	}

	void OnEndRequest(ActionData action_data)
	{}
	
	void EndRequest(ActionData action_data)
	{
		OnEndRequest(action_data);
	}
	
	bool CanReceiveAction(ActionTarget target)
	{
		bool result = true;
		PlayerBase target_player = PlayerBase.Cast(target.GetObject());
		
		if (target_player)
		{
			result = !target_player.IsJumpInProgress();
			result = result && !(target_player.GetCommand_Ladder() || (target_player.GetCommand_Vehicle() && !CanTargetBeInVehicle()) || target_player.GetCommand_Swim());
		}
		
		return result;
	}
	
	static int ComputeConditionMask( PlayerBase player, ActionTarget target, ItemBase item )
	{
		int mask = 0;
		if ( player.GetCommand_Vehicle() )
		{
			mask |= ActionConditionMask.ACM_IN_VEHICLE;
		}
		
		if ( player.GetCommand_Ladder() )
		{
			mask |= ActionConditionMask.ACM_ON_LADDER;
		}
		
		if ( player.IsRestrained() )
		{
			mask |= ActionConditionMask.ACM_RESTRAIN;
		}
		
		if ( player.GetCommand_Swim() )
		{
			mask |= ActionConditionMask.ACM_SWIMMING;
		}
		
		if ( player.IsRaised() )
		{
			mask |= ActionConditionMask.ACM_RAISED;
		}
		
		if ( player.GetCommand_Move() && player.GetCommand_Move().IsOnBack() )
		{
			mask |= ActionConditionMask.ACM_ON_BACK;
		}
		
		if ( player.GetThrowing().IsThrowingModeEnabled())
		{
			mask |= ActionConditionMask.ACM_THROWING;
		}
		
		if (player.IsLeaning())
		{
			mask |= ActionConditionMask.ACM_LEANING;
		}
		
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			mask |= ActionConditionMask.ACM_BROKEN_LEGS;
		}

		if (player.GetInputController() && player.GetInputController().CameraIsFreeLook())
			mask |= ActionConditionMask.ACM_IN_FREELOOK;
		
		return mask;
	}

	bool Can( PlayerBase player, ActionTarget target, ItemBase item, int condition_mask )
	{
		if (( (condition_mask & m_ConditionMask) != condition_mask ) || ( !IsFullBodyEx(player, target, item) && !player.IsPlayerInStance(GetStanceMaskEx(player, target, item)) ) || player.IsRolling())
			return false;
		
		if (HasTarget())
		{
			if (!FirearmActionBase.Cast(this))
			{
				EntityAI entity = EntityAI.Cast(target.GetObject());
				if (entity && !target.GetObject().IsMan())
				{
					Man man = entity.GetHierarchyRootPlayer();
					if (man && man != player)
						return false;
				}
			}
			
			if (m_ConditionTarget && !m_ConditionTarget.Can(player, target))
				return false;
		}
		
		if (m_ConditionItem && !m_ConditionItem.Can(player, item))
			return false;
		
		if (!ActionCondition(player, target, item))
			return false;
		
		if (IsFullBodyEx(player, target, item))
		{
			int nextStance = GetClosestPossibleStance(player, target, item);
			int stanceIdx = DayZPlayerUtils.ConvertStanceMaskToStanceIdx(nextStance);
			if (stanceIdx != -1 && !DayZPlayerUtils.PlayerCanChangeStance(player, stanceIdx ))
				return false;
		}
		
		return true;
	}
	
	bool Can(PlayerBase player, ActionTarget target, ItemBase item)
	{
		int condition_mask = ComputeConditionMask(player, target, item);
		
		return Can( player, target, item, condition_mask);
	}
	
	protected bool CanContinue( ActionData action_data )
	{
		if (!action_data.m_Player.IsPlayerInStance(action_data.m_PossibleStanceMask) || !m_ConditionItem || !m_ConditionItem.CanContinue(action_data.m_Player,action_data.m_MainItem) || !m_ConditionTarget || !m_ConditionTarget.CanContinue(action_data.m_Player,action_data.m_Target))
			return false;

		return ActionConditionContinue(action_data);
	}
	
	bool HasVariants()
	{
		return m_VariantManager != null;
	}
	
	int GetVariantsCount()
	{
		if (m_VariantManager)
			return m_VariantManager.GetActionsCount();

		return 0;
	}
	
	int GetVariants(out array<ref ActionBase> variants)
	{
		if (m_VariantManager)
			return m_VariantManager.GetActions(variants);

		return 0;
	}
	
	void SetVariantID(int ID)
	{
		m_VariantID = ID;
	}
	
	int GetVariantID()
	{
		return m_VariantID;
	}
	
	void UpdateVariants(Object item, Object target, int componet_index)
	{
		if ( m_VariantManager )
		{
			m_VariantManager.UpdateVariants(item, target, componet_index);
		}
	}
	
	ActionVariantManager GetVariantManager()
	{
		if ( !m_VariantManager )
			m_VariantManager = new ActionVariantManager(this.Type());
		return m_VariantManager;
	}
	
	// Called when item changes location during performed action
	void OnItemLocationChanged(ItemBase item)
	{}
	
	// call only on client side for lock inventory before action
	// return if has successfuly reserved inventory
	bool InventoryReservation(ActionData action_data)
	{
		if ((IsLocal() || !UseAcknowledgment()) && IsInstant())
			return true;
		
		//action_data.m_ReservedInventoryLocations = new array<ref InventoryLocation>;
		bool success = true;
		InventoryLocation targetInventoryLocation = NULL;
		InventoryLocation handInventoryLocation = NULL;
		
		// lock target if it has target
		if (HasTarget())
		{
			ItemBase targetItem;
			if ( ItemBase.CastTo(targetItem,action_data.m_Target.GetObject()) )
			{
				targetInventoryLocation = new InventoryLocation;
				targetItem.GetInventory().GetCurrentInventoryLocation(targetInventoryLocation);
				if ( action_data.m_Player.GetInventory().HasInventoryReservation( targetItem, targetInventoryLocation) )
				{
					success = false;
				}
				else
				{
					action_data.m_Player.GetInventory().AddInventoryReservationEx( targetItem, targetInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
				}
			}
		}	
		
		handInventoryLocation = new InventoryLocation;
		handInventoryLocation.SetHands(action_data.m_Player,action_data.m_Player.GetItemInHands());

		if (action_data.m_Player.GetInventory().HasInventoryReservation( action_data.m_Player.GetItemInHands(), handInventoryLocation))
		{
			if (HasTarget())
			{
				action_data.m_Player.GetInventory().ClearInventoryReservation(targetItem, targetInventoryLocation);
			}

			success = false;
		}
		else
		{
			action_data.m_Player.GetInventory().AddInventoryReservationEx( action_data.m_Player.GetItemInHands(), handInventoryLocation, GameInventory.c_InventoryReservationTimeoutMS);
		}
		
		if (success)
		{
			if (targetInventoryLocation)
				action_data.m_ReservedInventoryLocations.Insert(targetInventoryLocation);
			
			if (handInventoryLocation)
				action_data.m_ReservedInventoryLocations.Insert(handInventoryLocation);
		}
		
		return success;
	}

	void ClearInventoryReservationEx(ActionData action_data)
	{
		if (action_data.m_Player.GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			if (action_data.m_ReservedInventoryLocations)
			{
				InventoryLocation il;
				for ( int i = 0; i < action_data.m_ReservedInventoryLocations.Count(); i++)
				{
					il = action_data.m_ReservedInventoryLocations.Get(i);
					action_data.m_Player.GetInventory().ClearInventoryReservationEx( il.GetItem() , il );
				}
		
				action_data.m_ReservedInventoryLocations.Clear();
			}
		}
	}
	
	void RefreshReservations(ActionData action_data)
	{
		if (action_data.m_ReservedInventoryLocations)
		{
			InventoryLocation il;
			for (int i = 0; i < action_data.m_ReservedInventoryLocations.Count(); i++)
			{
				il = action_data.m_ReservedInventoryLocations.Get(i);
				EntityAI entity = il.GetItem();
				action_data.m_Player.GetInventory().ExtendInventoryReservationEx(il.GetItem() , il, 10000);
			}
		}
	}
	
	bool AddActionJuncture(ActionData action_data)
	{
		bool accepted = true;
		if (action_data.m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			if (HasTarget() && IsLockTargetOnUse() && action_data.m_Target)
			{
				EntityAI targetEntity;
				if (EntityAI.CastTo(targetEntity, action_data.m_Target.GetObject()))
				{
					InventoryLocation targetIl = new InventoryLocation();
					targetEntity.GetInventory().GetCurrentInventoryLocation(targetIl);
					
					//Lock target
					if (!GetGame().AddInventoryJunctureEx(action_data.m_Player, targetEntity, targetIl, true, 10000))
					{
						accepted = false;
					}
					else
					{
						action_data.m_ReservedInventoryLocations.Insert(targetIl);
					}
				}
			}
		}
		
		return accepted;
	}

	void ClearActionJuncture(ActionData action_data)
	{
		if (action_data.m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER)
		{
			if (action_data.m_ReservedInventoryLocations)
			{
				InventoryLocation il;
				for ( int i = 0; i < action_data.m_ReservedInventoryLocations.Count(); i++)
				{
					il = action_data.m_ReservedInventoryLocations.Get(i);
					EntityAI entity = il.GetItem();
					if (entity)
					{
						GetGame().ClearJunctureEx(action_data.m_Player, entity);
					}
				}
	
				action_data.m_ReservedInventoryLocations.Clear();
			}
		}
	}
	
	void RefreshActionJuncture(ActionData action_data)
	{
		if (action_data.m_ReservedInventoryLocations)
		{
			InventoryLocation il;
			for (int i = 0; i < action_data.m_ReservedInventoryLocations.Count(); i++)
			{
				il = action_data.m_ReservedInventoryLocations.Get(i);
				EntityAI entity = il.GetItem();
				if (entity)
				{
					GetGame().ExtendActionJuncture(action_data.m_Player, entity, 10000);
				}
			}
		}
	}
	
	// action need first have permission from server before can start
	bool UseAcknowledgment()
	{
		return true;
	}
	
	//! DEPRECATED delivers message ids to clients based on given context
	protected void InformPlayers( PlayerBase player, ActionTarget target, int state );
		
	void SendMessageToClient( Object reciever, string message ) //sends given string to client, don't use if not nescessary
	{
		PlayerBase man;
		if (GetGame().IsServer() && Class.CastTo(man, reciever) && m_MessageParam && reciever.IsAlive() && message != "")
		{
			m_MessageParam.param1 = message;
			GetGame().RPCSingleParam(man, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, man.GetIdentity());
		}
	}
	
	// ActionCondition Rules
	// ------------------------------------------------------
	protected bool IsDamageDestroyed(ActionTarget target)
	{
		return target.GetObject() && target.GetObject().IsDamageDestroyed();
	}

	protected bool IsBuilding(ActionTarget target)
	{
		return target.GetObject() && target.GetObject().IsBuilding();
	}
	
	protected bool IsTransport(ActionTarget target)
	{
		return target.GetObject() && target.GetObject().IsTransport();
	}

	protected bool IsInReach(PlayerBase player, ActionTarget target, float maxDistance = 1.0)
	{
		Object obj = target.GetObject();
		if (!obj)
			return false;
			
		float distanceRoot, distanceHead;
		vector modelPos, worldPos, playerHeadPos;
		
		// we're using sq distance in comparison
		maxDistance = maxDistance * maxDistance;
		
		// get position of Head bone
		MiscGameplayFunctions.GetHeadBonePos(player, playerHeadPos);

		array<string> componentNames = new array<string>();
		obj.GetActionComponentNameList(target.GetComponentIndex(), componentNames);
		foreach (string componentName : componentNames)
		{
			if (componentName.Contains("doorstwin"))
				continue;

			modelPos = obj.GetSelectionPositionMS(componentName);
			worldPos = obj.ModelToWorld(modelPos);
			
			break;
		}

		distanceRoot = vector.DistanceSq(worldPos, player.GetPosition());
		distanceHead = vector.DistanceSq(worldPos, playerHeadPos);
	
		return distanceRoot <= maxDistance || distanceHead <= maxDistance;
	}
	// ------------------------------------------------------
	
	
	
	// SOUNDS ------------------------------------------------------
	SoundOnVehicle PlayActionSound( PlayerBase player )
	{
		if ( GetGame().IsServer() && player )
		{
			if ( m_Sound != "" )
			{
				return GetGame().CreateSoundOnObject(player, m_Sound, 6, false);
			}
			else if ( m_Sounds && m_Sounds.Count() > 0 )
			{
				int rand_num =  Math.RandomInt(0, m_Sounds.Count());
				return GetGame().CreateSoundOnObject(player, m_Sounds.Get(rand_num), 6, false);
			}
		}

		return NULL;
	}
	
	void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
	}
	
	//sound category matches with a soundtable category in config, selects correct soundset
	string GetSoundCategory(ActionData action_data)
	{
		return "";
	}
	
	// EVENTS ------------------------------------------------
	void OnUpdate(ActionData action_data)
	{}
	
	void OnUpdateClient(ActionData action_data)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if (action_data.m_RefreshReservationTimer > 0)
			{
				action_data.m_RefreshReservationTimer--;
			}
			else
			{
				action_data.m_RefreshReservationTimer = m_RefreshReservationTimerValue;
				RefreshReservations(action_data);
			}
		}
	}
	
	void OnUpdateServer(ActionData action_data)
	{
		if (action_data.m_RefreshJunctureTimer > 0)
		{
			action_data.m_RefreshJunctureTimer--;
		}
		else
		{
			action_data.m_RefreshJunctureTimer = m_RefreshReservationTimerValue;
			RefreshActionJuncture(action_data);
		}
	}
	
	void OnStart(ActionData action_data)
	{
		if (action_data.m_Player != NULL && action_data.m_Player.IsPlacingLocal() && !IsDeploymentAction())
			action_data.m_Player.PlacingCancelLocal();
	}
	
	void OnStartClient(ActionData action_data)
	{}
	
	void OnStartServer(ActionData action_data)
	{
		
	}
	
	void OnEnd(ActionData action_data)
	{

	}
	
	void OnEndClient(ActionData action_data)
	{}
	
	void OnEndServer(ActionData action_data)
	{
	}

	// SOFT SKILLS ------------------------------------------------
	float GetSpecialtyWeight()
	{
		if(m_SpecialtyWeight == 0)
		{
#ifdef DEVELOPER
			//Print("UserAction does not use SoftSkills");
#endif
		}	

		return m_SpecialtyWeight;	
	}
	
	int GetState( ActionData action_data )
	{
		return action_data.m_State;
	}
	
	float GetProgress( ActionData action_data )
	{
		return -1;
	}
	
	float GetProgressWidgetMultiplier( ActionData action_data ) // override when action length is not supposed to be the same length as progress widget
	{
		return 1;
	}
	
	ActionInput GetInput()
	{
		return m_Input;
	}
	
	void SetID(int actionId)
	{
		m_ActionID = actionId;
	}
	
	int GetID()
	{
		return m_ActionID;
	}
	
	string GetAdminLogMessage(ActionData action_data)
	{
		return "";
	}
};