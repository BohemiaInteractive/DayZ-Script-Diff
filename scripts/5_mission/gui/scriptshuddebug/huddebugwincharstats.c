class HudDebugWinCharStats extends HudDebugWinBase
{
	TextListboxWidget						m_WgtValues;
	Widget									m_WgtPanel;
	ref array<ref Widget> 					m_StatWidgets = new array<ref Widget>;
	ref map <ref SliderWidget, string> 		m_SliderWidgets = new map<ref SliderWidget, string>;
	ref array<ref TextWidget> 				m_StatValues = new array<ref TextWidget>;
	ref map<ref EditBoxWidget, string> 		m_StatValuesInput = new map<ref EditBoxWidget, string>;
	bool									m_Populated;
	bool									m_ChangingSlider;
	
	//============================================
	// Constructor
	//============================================
	void HudDebugWinCharStats(Widget widget_root)
	{
		m_WgtRoot = widget_root;
		m_WgtPanel = Widget.Cast(m_WgtRoot.FindAnyWidget("Stats") );
		//FitWindow();
	}

	//============================================
	// Destructor
	//============================================
	void ~HudDebugWinCharStats()
	{	
		SetUpdate( false );
	}


	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_CHAR_STATS;
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
				player.RPCSingleParam( ERPCs.DEV_STATS_UPDATE, params, true );
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if ( developer_sync )
			{
				developer_sync.EnableUpdate( state, ERPCs.DEV_STATS_UPDATE, player );	
			}
		}
	}


	override void Update()
	{
		super.Update();
		//Print("panel:" + m_WgtPanel);
		//Print("Update()");
		
		//refresh notifiers
		
		
		if(!m_Populated)
			SetupValues();
		
		UpdateValues();
	}
	
	void SetupValues()
	{
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
		//clear window
		//ClearValues();
			
		if ( developer_sync.m_PlayerStatsSynced.Count() > 0 )
		{
			//set
			for ( int i = 0; i < developer_sync.m_PlayerStatsSynced.Count(); i++ )
			{
				AddValue( developer_sync.m_PlayerStatsSynced.Get( i ).GetName(), developer_sync.m_PlayerStatsSynced.Get( i ).GetValue().ToString() );
			}
					//fit to screen
			FitWindow();
			m_Populated = true;
		}
	}
	
	
	void UpdateValues()
	{
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		if ( developer_sync.m_PlayerStatsSynced.Count() > 0 )
		{
			for ( int i = 0; i < developer_sync.m_PlayerStatsSynced.Count(); i++ )
			{
				string stat_name = developer_sync.m_PlayerStatsSynced.Get( i ).GetName();
				float val_norm = developer_sync.m_PlayerStatsSynced.Get( i ).GetValueNorm();
				float value = developer_sync.m_PlayerStatsSynced.Get( i ).GetValue();
				
				if( stat_name == "BloodType" )
				{
					string type, name;
					bool positive;
					
					name = value.ToString();
					name += "("+BloodTypes.GetBloodTypeName( Math.Round(value), type, positive)+")";
					m_StatValues.Get(i).SetText(name);
				}
				else
				{
					m_StatValues.Get(i).SetText(value.ToString());
					
				}
				if(!m_ChangingSlider)
					m_SliderWidgets.GetKeyByValue(stat_name).SetCurrent(val_norm * 100);
				/*
				EditBoxWidget w;
				w.
				AddValue( developer_sync.m_PlayerStatsSynced.Get( i ).GetName(), developer_sync.m_PlayerStatsSynced.Get( i ).GetValue().ToString() );
				*/
			}
		}
		
	}
	
	/*
	
	void AddValue( string title, string value )
	{
		int index = m_WgtValues.AddItem( title, NULL, 0 );
		m_WgtValues.SetItem( index, value, NULL, 1 );
	}	
	*/
	
	void AddValue( string title, string value )
	{
		Widget widget = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/debug/day_z_hud_debug_stat.layout", m_WgtPanel );
		
		TextWidget tw = TextWidget.Cast(widget.FindAnyWidget("StatName"));
		tw.SetText(title);
		m_StatWidgets.Insert(widget);
		
		TextWidget tw_output = TextWidget.Cast(widget.FindAnyWidget("OutputValue"));
		m_StatValues.Insert(tw_output);
		
		EditBoxWidget ebw_input = EditBoxWidget.Cast(widget.FindAnyWidget("InputValue"));
		m_StatValuesInput.Insert(ebw_input, title );
		
		SliderWidget sw = SliderWidget.Cast(widget.FindAnyWidget("StatSlider"));
		m_SliderWidgets.Insert(sw,title );
		
		
		//int index = m_WgtValues.AddItem( title, NULL, 0 );
		//m_WgtValues.SetItem( index, value, NULL, 1 );
		
		AutoHeightSpacer WgtModifiersContent_panel_script;
		m_WgtPanel.GetScript( WgtModifiersContent_panel_script );
		WgtModifiersContent_panel_script.Update();
	}

	void ClearValues()
	{
		m_StatWidgets.Clear();
		//m_WgtValues.ClearItems();
	}

	void FitWindow()
	{
		TextListboxWidget wgt = TextListboxWidget.Cast(m_WgtPanel);
		if(wgt)
			FitWindowByContent( wgt );
	}
	
	bool OnClick( Widget w, int x, int y, int button )
	{	
		if ( w.GetName() == "ResetStats" )
		{
			ResetStats();
			return true;
		}
		return false;
	}
	
	bool OnChange(Widget w, int x, int y, bool finished)
	{
		if ( m_StatValuesInput.Contains(EditBoxWidget.Cast(w)) && finished )
		{
			EditBoxWidget ebw = EditBoxWidget.Cast(w);
			RPCChangeStat(m_StatValuesInput.Get(EditBoxWidget.Cast(w)), ebw.GetText().ToFloat());
			return true;
		}
		if (m_SliderWidgets.Contains(SliderWidget.Cast(w)))
		{
			m_ChangingSlider = true;
			string stat_name = m_SliderWidgets.Get(SliderWidget.Cast(w));
			SliderWidget sw = SliderWidget.Cast(w);
			PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
			for ( int i = 0; i < player.m_PlayerStats.GetPCO().Get().Count(); i++ ) 
			{
				string label = player.m_PlayerStats.GetPCO().Get().Get( i ).GetLabel();
				if(label == stat_name)
				{
					float stat_min = player.m_PlayerStats.GetPCO().Get().Get( i ).GetMin();
					float stat_max = player.m_PlayerStats.GetPCO().Get().Get( i ).GetMax();
					float current_value_norm = sw.GetCurrent() / 100;
					float current_value_abs = stat_min + (stat_max - stat_min) * current_value_norm;
					
					RPCChangeStat(label, current_value_abs);
				}
			}
			//Print("OnChange " + finished);
			if(finished)
				m_ChangingSlider = false;
		}
		
		return false;
	}
	
	
	void ResetStats()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC

		ref Param1<bool> params = new Param1<bool>( false );
		if ( player )
		{
			player.RPCSingleParam( ERPCs.DEV_RPC_STATS_RESET, params, true );
		}
	}
	
	void RPCChangeStat(string stat, float value)
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC

		ref Param2<string, float> params = new Param2<string, float>( stat, value );
		if ( player )
		{
			player.RPCSingleParam( ERPCs.DEV_RPC_STAT_SET, params, true );
		}
	}
	
	
}
