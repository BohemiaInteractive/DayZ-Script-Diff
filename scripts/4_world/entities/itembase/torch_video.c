class Torch_Video : LongTorch
{
	protected float PARTICLE_DELAY = 15000;
	protected bool m_EnablePtc;
	
	void SetEnablePtc()
	{
		m_EnablePtc = true;
	}
		
	override void OnWork( float consumed_energy )
	{
		if (!m_EnablePtc)
			return;
		
		super.OnWork(consumed_energy);
	}
	
	override void OnWorkStart()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetEnablePtc, PARTICLE_DELAY);
		
		super.OnWorkStart();
	}
	
	override void OnWorkStop()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SetEnablePtc);
		m_EnablePtc = false;
		
		super.OnWorkStop();
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionLightTorchVideo);
		AddAction(ActionExtinquishTorchVideo);
	}
}