class HudDebugWinTemperature extends HudDebugWinBase
{
	private PluginDeveloper 		m_ModuleDeveloper;
	
	private TextWidget 				m_EnviroTextWidget;
	protected PluginDeveloperSync 	m_PluginDeveloperSync;
	//============================================
	// HudDebugWinTemperature
	//============================================
	void HudDebugWinTemperature(Widget widget_root)
	{	
		m_EnviroTextWidget = TextWidget.Cast( widget_root.FindAnyWidget("txt_Temp") );
		m_PluginDeveloperSync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );
		
	}

	//============================================
	// ~HudDebugWinTemperature
	//============================================
	void ~HudDebugWinTemperature()
	{
	}
	
	//============================================
	// Set Update
	//============================================
	override void SetUpdate( bool state )
	{
		//Disable update on server (PluginDeveloperSync)
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		//if client, send RPC
		if ( GetGame().IsClient() )
		{
			CachedObjectsParams.PARAM1_BOOL.param1 = state;
			if ( player )
			{
				player.RPCSingleParam( ERPCs.DEV_TEMP_UPDATE, CachedObjectsParams.PARAM1_BOOL, true );
				SetRPCSent();
			}
		}
		//else set directly
		else
		{
			if ( m_PluginDeveloperSync )
			{
				m_PluginDeveloperSync.EnableUpdate( state, ERPCs.DEV_TEMP_UPDATE, player );	
			}
		}
	}

	//============================================
	// Show / Hide
	//============================================
	override void Show()
	{
		super.Show();
		
		SetUpdate( true );
	}

	override void Hide()
	{
		super.Hide();
		
		SetUpdate( false );
	}
	
	
	//============================================
	// Update
	//============================================
	override void Update()
	{
		super.Update();
		PluginDeveloperSync developer_sync = PluginDeveloperSync.Cast( GetPlugin( PluginDeveloperSync ) );		
		
		m_EnviroTextWidget.SetText(developer_sync.m_EnvironmentDebugMessage);
		int sX, sY;
		m_EnviroTextWidget.GetTextSize(sX, sY);
		m_EnviroTextWidget.SetSize(sX, sY);
		
		AutoHeightSpacer spacer;
		m_WgtRoot.GetScript(spacer);
		if (spacer)
		{
			spacer.Update();
		}
	}

	//============================================
	// GetWinType
	//============================================
	override int GetType()
	{
		return HudDebug.HUD_WIN_TEMPERATURE;
	}
}
