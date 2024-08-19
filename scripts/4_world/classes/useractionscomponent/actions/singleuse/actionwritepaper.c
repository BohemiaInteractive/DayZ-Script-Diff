class ActionWritePaperCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		EnableStateChangeCallback();
		m_ActionData.m_ActionComponent = new CAContinuousTime(-1.0);
	}
	
	override void EndActionComponent()
	{
	}
	
	override void OnStateChange(int pOldState, int pCurrentState)
	{
		if (pCurrentState == STATE_NONE && (!GetGame().IsDedicatedServer()))
		{
			if (GetGame().GetUIManager() && GetGame().GetUIManager().IsMenuOpen(MENU_NOTE))
				GetGame().GetUIManager().FindMenu(MENU_NOTE).Close();
		}
		else if (pCurrentState == STATE_LOOP_LOOP && pOldState != STATE_LOOP_LOOP && (!GetGame().IsMultiplayer() || GetGame().IsServer()))
		{
			ItemBase paper_item;
			if (m_ActionData.m_MainItem.ConfigIsExisting("writingColor"))
			{
				paper_item = ItemBase.Cast(m_ActionData.m_Target.GetObject());
			}
			else
			{
				paper_item = ItemBase.Cast(m_ActionData.m_MainItem);
			}
			Param1<string> text = new Param1<string>(paper_item.GetWrittenNoteData().GetNoteText());
			paper_item.RPCSingleParam(ERPCs.RPC_WRITE_NOTE, text, true,m_ActionData.m_Player.GetIdentity());
		}
	}
};

class ActionWritePaper: ActionContinuousBase
{
	const float TARGET_DISTANCE = 2;
	
	void ActionWritePaper()
	{
		m_CallbackClass = ActionWritePaperCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_VIEWNOTE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_PRONE;
		m_Text = "#write_note";
	}
	
	override void CreateConditionComponents()
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;//CCTNonRuined(TARGET_DISTANCE);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if ((item && item.ConfigIsExisting("writingColor") && target.GetObject() && Paper.Cast(target.GetObject())) || (target.GetObject() && target.GetObject().ConfigIsExisting("writingColor") && item && Paper.Cast(item)))
		{
			return true;
		}
		return false;
	}
	
	override void OnStartClient( ActionData action_data )
	{
		if (action_data.m_Target.GetObject().ConfigIsExisting("writingColor"))
		{
			action_data.m_MainItem.GetWrittenNoteData().InitNoteInfo(ItemBase.Cast(action_data.m_Target.GetObject()),action_data.m_MainItem);
		}
		else
		{
			ItemBase.Cast(action_data.m_Target.GetObject()).GetWrittenNoteData().InitNoteInfo(action_data.m_MainItem,ItemBase.Cast(action_data.m_Target.GetObject()));
		}
	}
	
	override void OnStartServer( ActionData action_data )
	{
		if (action_data.m_Target.GetObject().ConfigIsExisting("writingColor"))
		{
			action_data.m_MainItem.GetWrittenNoteData().InitNoteInfo(ItemBase.Cast(action_data.m_Target.GetObject()),action_data.m_MainItem);
		}
		else
		{
			ItemBase.Cast(action_data.m_Target.GetObject()).GetWrittenNoteData().InitNoteInfo(action_data.m_MainItem,ItemBase.Cast(action_data.m_Target.GetObject()));
		}
	}
	
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);
		
		if(!GetGame().IsDedicatedServer())
		{
			if (action_data.m_State == UA_FINISHED && GetGame().GetUIManager() && !GetGame().GetUIManager().IsMenuOpen(MENU_NOTE))
			{
				ActionManagerClient.Cast(action_data.m_Player.GetActionManager()).RequestEndAction();
			}
		}
	}
	
	override void OnEndRequest(ActionData action_data)
	{
		if (action_data.m_Callback)
		{
			action_data.m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
	}
};