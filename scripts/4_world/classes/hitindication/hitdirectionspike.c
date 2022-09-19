
class HitDirectionEffectSpike extends HitDirectionEffectBase
{
	override HitDirectionImagesBase GetImageData()
	{
		typename type = HitDirectionImagesSpike;
		HitDirectionImagesSpike data = HitDirectionImagesSpike.Cast(type.Spawn());
		return data;
	}
	
	override void FinalizePositionCalculation()
	{
		//blobs move around the edges, the rest on elypsis
		float distance_x = (m_DistanceAdjust * m_SizeXEnf) + (m_SizeXEnf / 2.5);
		float distance_y = (m_DistanceAdjust * m_SizeYEnf) + (m_SizeYEnf / 2.5);
		m_PosX = Math.Sin(m_AngleRad) * distance_x;
		m_PosY = -Math.Cos(m_AngleRad) * distance_y;
	}
	
	override void SetIndicatorRotation(float timeslice = -1.0)
	{
		if (m_RotationOverride == HitDirectionConstants.ROTATION_DEFAULT)
		{
			m_LayoutRoot.SetRotation(0,0,Math.RAD2DEG * m_AngleRad,true);
		}
		else
		{
			m_LayoutRoot.SetRotation(0,0,m_RotationOverride,true);
		}
	}
}