class PlayerBaseClient extends PlayerBase
{	
	static ScriptedLightBase 	m_PersonalLight;
	static bool 				m_PersonalLightEnabledOnCurrentServer = false; // "disablePersonalLight" in server.cfg decides if this is true or false
	static bool 				m_PersonalLightDisabledByDebug = false;
	static bool 				m_PersonalLightIsSwitchedOn = true;
	
	//! Creates PL if it doesn't exist already.
	static void CreatePersonalLight()
	{
		if (!m_PersonalLight  &&  ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ))
		{
			m_PersonalLight = ScriptedLightBase.CreateLight(PersonalLight, "0 0 0");
		}
	}
	
	/*
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		switch( rpc_type )
		{
			case ERPCs.RPC_TOGGLE_PERSONAL_LIGHT:
			{
				Param1<bool> is_enabled = new Param1<bool>(false);
				
				if (ctx.Read(is_enabled))
				{
					m_PersonalLightEnabledOnCurrentServer = is_enabled.param1;
					UpdatePersonalLight();
				}
				
				break;
			}
		}
	}*/
	
	override void OnGameplayDataHandlerSync()
	{
		super.OnGameplayDataHandlerSync();
		m_PersonalLightEnabledOnCurrentServer = !CfgGameplayHandler.GetDisablePersonalLight();
		UpdatePersonalLight();
		UpdateHitDirectionValues();
	}
	
	
	//! Controls the ON/OFF switch of the Personal Light. PL will still shine only if the server allows it.
	static void SwitchPersonalLight(bool state)
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_PersonalLightIsSwitchedOn = state;
			UpdatePersonalLight();
		}
	}
	
	//! Updates state of PL
	static void UpdatePersonalLight()
	{
		string param;
		
		CreatePersonalLight();
		
		if ( ! GetCLIParam("disablePersonalLight", param)  &&  !m_PersonalLightDisabledByDebug  &&  m_PersonalLightIsSwitchedOn ) // Allow PL unless it's disabled by debug or client-side starting parameter
		{
			m_PersonalLight.SetEnabled(m_PersonalLightEnabledOnCurrentServer);
		}
		else	
		{
			m_PersonalLight.SetEnabled(false);
		}
	}
	
	static void UpdateHitDirectionValues()
	{
		HitDirectionEffectBase.CheckValues();
	}
}