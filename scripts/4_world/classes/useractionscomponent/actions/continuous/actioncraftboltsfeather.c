class ActionCraftBoltsFeatherCB : ActionContinuousBaseCB
{
	private const float TIME_TO_CRAFT = 3.0;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(TIME_TO_CRAFT);
	}
};

class ActionCraftBoltsFeather: ActionContinuousBase
{
	protected bool 				m_IsFeatherInHands;
	protected Ammunition_Base 	m_ResultEntity;
	
	void ActionCraftBoltsFeather()
	{
		m_CallbackClass 	= ActionCraftBoltsFeatherCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_HIGH;

		m_Text = "#STR_CraftBolt1";

	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNonRuined();
	}
	
	protected bool IsFeatherType(string itemInHandsType)
	{
		return itemInHandsType == "ChickenFeather";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (IsFeatherType(item.ClassName()))
		{
			//feather in hands
			if (target.GetObject())
				return (target.GetObject().ClassName() == "Ammo_ImprovisedBolt_1");
		}
		else if (target.GetObject())
		{
			// bolt in hands
			return IsFeatherType(target.GetObject().ClassName());
		}
		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		m_IsFeatherInHands = IsFeatherType(action_data.m_MainItem.ClassName());
		m_ResultEntity = null;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		ItemBase feather;
		Ammunition_Base bolt;

		bool added = false;
		
		if (m_IsFeatherInHands)
		{
			//feather in hands
			feather = action_data.m_MainItem;
			bolt = Ammunition_Base.Cast(action_data.m_Target.GetObject());
		}
		else
		{
			// bolt in hands
			bolt = Ammunition_Base.Cast(action_data.m_MainItem);
			feather = ItemBase.Cast(action_data.m_Target.GetObject());
		}
		
		if (!bolt || !feather)
			return;
		
		float dmg;
		string type;
		
		bolt.ServerAcquireCartridge(dmg, type);
		
		if (m_ResultEntity)
		{
			type = m_ResultEntity.ConfigGetString("Ammo");
			if (m_ResultEntity.GetAmmoCount() < m_ResultEntity.GetAmmoMax())
			{
				m_ResultEntity.ServerStoreCartridge(dmg, type);
				added = true;
			}
		}
		
		if (!added)
		{
			m_ResultEntity = Ammunition_Base.Cast(action_data.m_Player.SpawnEntityOnGroundRaycastDispersed("Ammo_ImprovisedBolt_2"));
			type = m_ResultEntity.ConfigGetString("Ammo");
			m_ResultEntity.ServerSetAmmoCount(0);
			m_ResultEntity.ServerStoreCartridge(dmg, type);
			m_ResultEntity.SetHealth01("", "", bolt.GetHealth01("", ""));
		}

		feather.AddQuantity(-1);
	}
};
