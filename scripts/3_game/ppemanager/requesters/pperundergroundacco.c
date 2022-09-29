class PPERUndergroundAcco extends PPERequester_GameplayBase
{
	void SetEyeAccommodation(float value)
	{
		SetTargetValueFloat(PPEExceptions.EYEACCOM,PPEEyeAccomodationNative.PARAM_INTENSITY,false,value,PPEEyeAccomodationNative.L_0_UNDERGROUND,PPOperators.SET);
	}
}