class PPERUndergroundAcco extends PPERequester_GameplayBase
{
	void SetEyeAccommodation(float value)
	{
		SetTargetValueFloat(PPEExceptions.EYEACCOM,PPEEyeAccomodationNative.PARAM_INTENSITY,false,value,PPEEyeAccomodationNative.L_0_UNDERGROUND,PPOperators.SET);
	}
	
	override protected void OnStart( Param par = null )
	{
		super.OnStart(par);
	}
	
	override void OnStop(Param par = null)
	{
		super.OnStop(par);
	}
}