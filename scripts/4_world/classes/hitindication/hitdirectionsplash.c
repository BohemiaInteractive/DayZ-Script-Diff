
class HitDirectionEffectSplash extends HitDirectionEffectBase
{
	override HitDirectionImagesBase GetImageData()
	{
		typename type = HitDirectionImagesSplash;
		HitDirectionImagesSplash data = HitDirectionImagesSplash.Cast(type.Spawn());
		return data;
	}
	
	override void FinalizePositionCalculation()
	{
		//blobs move around the edges, the rest on elypsis
		m_PosX = m_PosXScreenEdge;
		m_PosY = m_PosYScreenEdge;
	}
	
	override void SetIndicatorRotation(float timeslice = -1.0)
	{
		if ( timeslice == -1.0) //does not rotate on dynamic mode update
		{
			if (m_RotationOverride == HitDirectionConstants.ROTATION_DEFAULT)
			{
				m_LayoutRoot.SetRotation(0,0,Math.RandomIntInclusive(0,359),true);
			}
			else
			{
				m_LayoutRoot.SetRotation(0,0,m_RotationOverride,true);
			}
		}
	}
}