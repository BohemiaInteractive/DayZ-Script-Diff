enum LightSourceType
{
	NotDef,
	PointLight,
	SpotLight
};

class EntityLightSource extends Entity
{
	//---------------------------------------------------------------------------
	// Generic interface
	//---------------------------------------------------------------------------
	proto native void 			SetLightType(int pType); // This works only if it's caled in the light's constructor!
	proto native int  			GetLightType();

	proto native void 			SetEnabled(bool pState);
	proto native bool 			IsEnabled();

	proto native void 			SetCastShadow(bool pState);
	proto native bool 			GetCastShadow();

	proto native bool 			EnableSpecular(bool pState);
	proto native bool 			EnableLinear(bool pState);

	proto native void 			SetPulseCoef(float pState);

	proto native void 			SetVisibleDuringDaylight(bool pState);
	proto native bool 			IsVisibleDuringDaylight();

	proto native void 			SetRadius(float pValue);
	proto native float 			GetRadius();
	
	//---------------------------------------------------------------------------
	// heat haze
	//---------------------------------------------------------------------------
	proto native void 			EnableHeatHaze(bool pState);
	
	proto native void 			SetHeatHazeRadius(float pValue);
	proto native float 			GetHeatHazeRadius();

	proto native void 			SetHeatHazePower(float pValue);
	proto native float 			GetHeatHazePower();
	
	//---------------------------------------------------------------------------
	// colors & brightness
	//---------------------------------------------------------------------------
	proto native void   		SetDiffuseColor(float pRed, float pGreen, float pBlue);
	proto native void   		SetDiffuseColorV(vector pColorVector);
	proto native void   		SetDiffuseAlpha(float pAlpha);
	proto native vector 		GetDiffuseColorV();
	proto native float  		GetDiffuseAlpha();
	
	proto native void   		SetAmbientColor(float pRed, float pGreen, float pBlue);
	proto native void   		SetAmbientColorV(vector pColorVector);
	proto native void   		SetAmbientAlpha(float pAlpha);
	proto native vector 		GetAmbientColorV();
	proto native float  		GetAmbientAlpha();

	proto native void   		SetBrightness(float pValue);
	proto native float  		GetBrightness();
	
	//---------------------------------------------------------------------------
	// flares
	//---------------------------------------------------------------------------
	proto native void 			SetFlareVisible(bool pState);
	proto native bool			IsFlareVisible();
	proto native void			SetFlareRelPosition(vector pPosition);
	proto native vector			GetFlareRelPosition();

	//---------------------------------------------------------------------------
	// spot light specifics
	//---------------------------------------------------------------------------
	proto native void  			SetSpotLightAngle(float pValue);
	proto native float 			GetSpotLightAngle();
};