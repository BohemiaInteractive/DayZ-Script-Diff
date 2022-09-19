class HudDebugWinCharStomach extends HudDebugWinBase
{
	TextListboxWidget				m_WgtValues;
	TextWidget						m_WgtOverall;
	
	//============================================
	// Constructor
	//============================================
	void HudDebugWinCharStomach(Widget widget_root)
	{	
		m_WgtValues = TextListboxWidget.Cast( widget_root.FindAnyWidget("txl_StomachContents") );
		m_WgtOverall = TextWidget.Cast( widget_root.FindAnyWidget("InfoOverall") );
		//FitWindow();
	}

	//============================================
	// Destructor
	//============================================
	void ~HudDebugWinCharStomach()
	{	
		SetUpdate( false );
	}


	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_STOMACH;
	}
	
	//============================================
	// Show 
	//============================================
	override void Show()
	{
		super.Show();
		
		//Print("Show()");
		
		SetUpdate( true );
	}

	//============================================
	// Hide 
	//============================================
	override void Hide()
	{
		super.Hide();
		
		//Print("Hide()");
		
		SetUpdate( false );
	}

	//============================================
	// SetUpdate
	//============================================
	override void SetUpdate( bool state )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			ref Param1<bool> params = new Param1<bool>( state );
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_STOMACH_UPDATE, params, true );
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if ( developer_sync )
			{
				developer_sync.EnableUpdate( state, ERPCs.DEV_STOMACH_UPDATE, player );	
			}
		}
	}


	override void Update()
	{
		super.Update();
		
		//Print("Update()");
		
		//refresh notifiers
		SetContentValues();
	}
	
	
	void SetContentValues()
	{
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//clear window
		ClearValues();
			
		for ( int i = 0; i < developer_sync.m_PlayerStomachSynced.Count() - 1; i++ )
		{
			//new Param4<int,int,int,float>(id, food_stage, agents, amount);
			Param4<int,int,int,float> p4 = Param4<int,int,int,float>.Cast(developer_sync.m_PlayerStomachSynced.Get(i));
			AddValue( PlayerStomach.GetClassnameFromID(p4.param1), p4.param2, p4.param3, p4.param4);
		}
		
		if( developer_sync.m_PlayerStomachSynced.Count() )
		{
			int last_index = developer_sync.m_PlayerStomachSynced.Count() - 1;
			Param1<float> p1 = Param1<float>.Cast(developer_sync.m_PlayerStomachSynced.Get(last_index));
			m_WgtOverall.SetText("Overall volume:" + p1.param1.ToString());
		}
		else
		{
			m_WgtOverall.SetText("");
		}
		
		
		
		//fit to screen
		//FitWindow();
	}

	void AddValue( string classname, int food_stage, int agents, float amount)
	{
		int index = m_WgtValues.AddItem( classname, NULL, 0 );
		string stage = typename.EnumToString(FoodStageType, food_stage) + "(" + food_stage.ToString()+")";;
		m_WgtValues.SetItem( index, amount.ToString(), NULL, 1 );
		m_WgtValues.SetItem( index,stage , NULL, 2 );
		array<string> agent_list = GetAgentsArray(agents);
		string agent_line = "("+agents.ToString()+") ";
		
		for(int i = 0; i < agent_list.Count();i++)
		{
			agent_line +=  "," +agent_list.Get(i);
		}
		
		m_WgtValues.SetItem( index, agent_line , NULL, 3);
	}
	
	array<string> GetAgentsArray(int agents)
	{
		array<string> list = new array<string>;
		for(int i = 0; i < 32; i++)
		{
			int agent = agents & (1 << i);
			if(agent)
			{
				list.Insert(PluginTransmissionAgents.GetNameByID(agent));
			}
		}
		return list;
	}

	void ClearValues()
	{
		m_WgtValues.ClearItems();
	}

	void FitWindow()
	{
		FitWindowByContent( m_WgtValues );
	}
}
