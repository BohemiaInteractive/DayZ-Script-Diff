//!static info (non-constants)
class DbgBleedingIndicationStaticInfo
{
	static bool m_DbgEnableBleedingIndication = true;
	static bool m_DbgUseOverrideValues = true;
	static bool m_DbgDropRotationRandom = true;
	static float m_DbgSequenceDuration = BleedingIndicationConstants.SEQUENCE_DURATION_DEFAULT;
	static float m_DbgDropDurationBase = BleedingIndicationConstants.DROP_DURATION_MEDIUM;
	static float m_DbgDropMinDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MIN_MEDIUM;
	static float m_DbgDropMaxDelay = BleedingIndicationConstants.SEQUENCE_DROP_DELAY_MAX_MEDIUM;
	static float m_DbgDropProgressTreshold = BleedingIndicationConstants.DROP_PROGRESS_THRESHOLD;
	static float m_DbgDropStartSize = BleedingIndicationConstants.DROP_SIZE_START_LOW;
	static float m_DbgDropEndSize = BleedingIndicationConstants.DROP_SIZE_END_LOW;
	static float m_DbgDropSizeVariationMin = BleedingIndicationConstants.DROP_SIZE_VARIATION_MIN_LOW;
	static float m_DbgDropSizeVariationMax = BleedingIndicationConstants.DROP_SIZE_VARIATION_MAX_LOW;
	static int m_DbgDropScatter = BleedingIndicationConstants.DROP_SCATTER_LOW;
	static float m_DbgDropSlideDistance = BleedingIndicationConstants.DROP_SLIDE_DISTANCE_LOW;
	//colors
	static bool m_DbgDropDesaturate = true;
	static float m_DbgDesaturationEnd = BleedingIndicationConstants.DROP_COLOR_DESATURATIONEND;
	static int m_DbgDropColorRed = BleedingIndicationConstants.DROP_COLOR_RED;
	static int m_DbgDropColorGreen = BleedingIndicationConstants.DROP_COLOR_GREEN;
	static int m_DbgDropColorBlue = BleedingIndicationConstants.DROP_COLOR_BLUE;
	static int m_DbgDropColorAlphaStart = BleedingIndicationConstants.DROP_COLOR_ALPHA_START;
	static int m_DbgDropColorAlphaEnd = BleedingIndicationConstants.DROP_COLOR_ALPHA_END;
}
