/* 
DayZPlayerImplement

this file is implemenation of dayzPlayer in script 
- logic of movement 
- camera switching logic

*/

class DayZPlayerImplementHeading
{

	//-------------------------------------------------------------
	//!
	//! This HeadingModel - Clamps heading
	//! 

	//!
	static bool	ClampHeading (float pDt, SDayZPlayerHeadingModel pModel, out float pLastHeadingDiff)
	{
		float		aDiff = pModel.m_fHeadingAngle - pModel.m_fOrientationAngle;
		if (aDiff < -Math.PI)
		{
			aDiff += Math.PI2;
		}
		else if (aDiff > Math.PI)
		{
			aDiff -= Math.PI2;
		}

		// Print("Heading model: or: " + pModel.m_fOrientationAngle.ToString() + " head:" + pModel.m_fHeadingAngle.ToString() + " dif:" + aDiff.ToString());

		if (pLastHeadingDiff < -Math.PI_HALF && aDiff > 0)
		{
			aDiff					= -Math.PI + 0.01;
			pLastHeadingDiff		= aDiff;
			pModel.m_fHeadingAngle 	= pModel.m_fOrientationAngle + aDiff;
		
        	// Print("-APA- : or: " + pModel.m_fOrientationAngle.ToString() + " head:" + pModel.m_fHeadingAngle.ToString() + " dif:" + aDiff.ToString());

			return true;		// modify heading
		}
		else if (pLastHeadingDiff > Math.PI_HALF && aDiff < 0)
		{
			aDiff 					= Math.PI - 0.01;
			pLastHeadingDiff		= aDiff;
			pModel.m_fHeadingAngle 	= pModel.m_fOrientationAngle + aDiff;

        	// Print("-APA- : or: " + pModel.m_fOrientationAngle.ToString() + " head:" + pModel.m_fHeadingAngle.ToString() + " dif:" + aDiff.ToString());

			return true;		// modify heading
		}

		pLastHeadingDiff	= aDiff;
		// Print("Heading model diff " + aDiff.ToString());
		return false;
	}

	//-------------------------------------------------------------
	//!
	//! This HeadingModel - Rotates orientations - so player slides 
	//! 

	static float 	CONST_ROTLIMIT = Math.PI * 0.95;

	//!
	static bool	RotateOrient (float pDt, SDayZPlayerHeadingModel pModel, out float pLastHeadingDiff)
	{
		float		aDiff = pModel.m_fHeadingAngle - pModel.m_fOrientationAngle;

		while (aDiff < -Math.PI)
		{
			aDiff += Math.PI2;
		}
		while (aDiff > Math.PI)
		{
			aDiff -= Math.PI2;
		}

		// Print("Heading model: or: " + pModel.m_fOrientationAngle.ToString() + " head:" + pModel.m_fHeadingAngle.ToString() + " dif:" + aDiff.ToString());

		if (pLastHeadingDiff < -Math.PI_HALF && aDiff > 0)
		{
			aDiff -= Math.PI2;
		}
		else if (pLastHeadingDiff > Math.PI_HALF && aDiff < 0)
		{
			aDiff += Math.PI2;
		}

		pLastHeadingDiff	= aDiff;		
		if (aDiff < -CONST_ROTLIMIT)
		{
			// character is somehow stucked (happens in prone stance)
			if (aDiff < -(Math.PI_HALF + CONST_ROTLIMIT))
			{
				pLastHeadingDiff = 0;
				return false;
			}
			
			pModel.m_fOrientationAngle += aDiff +  CONST_ROTLIMIT;
			return true;
		}
		else if (aDiff > CONST_ROTLIMIT)
		{
			// character is somehow stucked (happens in prone stance)
			if (aDiff > (Math.PI_HALF + CONST_ROTLIMIT))
			{
				pLastHeadingDiff = 0;
				return false;
			}
			
			pModel.m_fOrientationAngle += aDiff - CONST_ROTLIMIT;
			return true;
		}

		// Print("Heading model diff " + aDiff.ToString());
		return false;

	}
		
	static bool RestrictHeading(float pDt, SDayZPlayerHeadingModel pModel, out float pLastHeadingDiff, HeadingRestrictData restrictData)
	{	
		pModel.m_fHeadingAngle = ClampAngle(pModel.m_fHeadingAngle, restrictData);	
		return true;
	}
	
	// Clamp angle between -PI to PI
	protected static float ClampAngle(float angle, HeadingRestrictData restrictData)
	{
		float testOtherDir;
		
		if (restrictData.m_RestrictedR > restrictData.m_RestrictedL)
		{
			if (angle > restrictData.m_RestrictedL && angle < restrictData.m_RestrictedR)
			{
				//Print("STANDARD NOCLAMP: " + angle*Math.RAD2DEG + " |MIN: " + restrictData.m_RestrictedL*Math.RAD2DEG + " |MAX: " + restrictData.m_RestrictedR*Math.RAD2DEG);
				return angle;
			}
			else 
			{
				//Print("STANDARD CLAMP: " + angle*Math.RAD2DEG + " |MIN: " + restrictData.m_RestrictedL*Math.RAD2DEG + " |MAX: " + restrictData.m_RestrictedR*Math.RAD2DEG);

				if (angle > restrictData.m_RestrictedR + 90 * Math.DEG2RAD)	 // check if restrictData.m_RestrictedR/restrictData.m_RestrictedL is close to -PI and the new angle flips to +PI or vice versa
					return restrictData.m_RestrictedL;
				else if (angle < restrictData.m_RestrictedL - 90 * Math.DEG2RAD)
					return restrictData.m_RestrictedR;
				
				return Math.Clamp(angle, restrictData.m_RestrictedL, restrictData.m_RestrictedR);	
			}
		}
		else if (restrictData.m_RestrictedR < restrictData.m_RestrictedL) // angle is restrited through 180 -> -180, clamping follows different rules
		{
			if ((angle >= -180 && angle < restrictData.m_RestrictedR) || (angle <= 180 && angle > restrictData.m_RestrictedL))
			{
				//Print("INVERSE NOCLAMP: " + angle*Math.RAD2DEG + " |MIN: " + restrictData.m_RestrictedL*Math.RAD2DEG + " |MAX: " + restrictData.m_RestrictedR*Math.RAD2DEG);
				return angle;
			}
			else 
			{			
				//Print("INVERSE CLAMP: " + angle*Math.RAD2DEG + " |MIN: " + restrictData.m_RestrictedL*Math.RAD2DEG + " |MAX: " + restrictData.m_RestrictedR*Math.RAD2DEG);
				
				if (angle < 0)
				{
					testOtherDir = Math.AbsFloat(restrictData.m_RestrictedR - angle);
					if (testOtherDir < restrictData.m_AngleRangeInverted - testOtherDir)
						return restrictData.m_RestrictedR;
					else 
						return restrictData.m_RestrictedL;
				}
				else if (angle >= 0)
				{
					testOtherDir = Math.AbsFloat(restrictData.m_RestrictedL - angle);
					if (testOtherDir < restrictData.m_AngleRangeInverted - testOtherDir)
						return restrictData.m_RestrictedL;
					else 
						return restrictData.m_RestrictedR;
				}
					
				return angle;
			}
		}
		
		return angle;
	}
	
	static bool NoHeading(float pDt, SDayZPlayerHeadingModel pModel, out float pLastHeadingDiff)
	{
		pLastHeadingDiff = 0;
		pModel.m_fHeadingAngle = pModel.m_fOrientationAngle;
		return true;
	}
}

class HeadingRestrictData
{
	float m_RestrictedL;		// restricted angle left
	float m_RestrictedR;		// restricted angle right
	float m_AngleRangeInverted; // the size of the restricted angle
	
	void InitData(float currentHeading, Vector2 restrictedAngles)
	{
		m_RestrictedL = currentHeading + (Math.DEG2RAD * restrictedAngles.x);
		if (m_RestrictedL < -Math.PI)
			m_RestrictedL += Math.PI2;
		
		m_RestrictedR = currentHeading + (Math.DEG2RAD * restrictedAngles.y);
		if (m_RestrictedR > Math.PI)
			m_RestrictedR -= Math.PI2;
		
		m_AngleRangeInverted = Math.PI2 - (Math.AbsFloat(restrictedAngles.x * Math.DEG2RAD) + restrictedAngles.y * Math.DEG2RAD);
	}
}