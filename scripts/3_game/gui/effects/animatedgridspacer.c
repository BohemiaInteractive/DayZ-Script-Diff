// Different easing functions
enum AnimatedGridSpacerEasingType
{
	EASE_LINEAR,
	EASE_IN_OUT_CUBIC,
	EASE_IN_OUT_QUART,
	EASE_OUT_BOUNCE,
	EASE_OUT_BACK
}
	
class AnimatedGridSpacer
{
	Widget m_GridSpacer;
	float m_ColumWidth = 0;
	
	float m_AnimationDuration = 0.25;
	float m_AnimationTime = 0;
	
	float m_CurrentPosX, m_CurrentPosY;
	float m_BasePosX, m_BasePosY;
	float m_StartPosX, m_TargetPosX;
	
	bool m_IsAnimating = false;
	int m_CurrentColum = 0;
	int m_MaxColums = 3;
		
	AnimatedGridSpacerEasingType m_EasingType = AnimatedGridSpacerEasingType.EASE_IN_OUT_CUBIC;
	
	ref ScriptInvoker m_OnAnimationComplete;
	
	void AnimatedGridSpacer(Widget gridSpacer, int maxColums, float animDuration = 0.25, AnimatedGridSpacerEasingType easing = AnimatedGridSpacerEasingType.EASE_IN_OUT_CUBIC)
	{
		m_GridSpacer = gridSpacer;
		m_MaxColums = maxColums;
		m_AnimationDuration = animDuration;
		m_EasingType = easing;
		
		if (m_GridSpacer)
		{
			SetColumWidth();
			
			m_GridSpacer.GetScreenPos(m_CurrentPosX, m_CurrentPosY);
			m_GridSpacer.GetScreenPos(m_BasePosX, m_BasePosY);
			m_CurrentPosX = m_BasePosX;
			m_CurrentPosY = m_BasePosY;
		}
		
		m_OnAnimationComplete = new ScriptInvoker();
	}
	
	void SetColumWidth()
	{
		if (!m_GridSpacer)
			return;
		
		Widget panel = m_GridSpacer.GetChildren();
		if (panel)
		{
			float w, h;
			panel.GetSize(w, h);
			m_ColumWidth = w;
		}
	}
	
	float ApplyEasing(float t)
	{
		switch(m_EasingType)
		{
			case AnimatedGridSpacerEasingType.EASE_LINEAR:
			{
				return t;
			}
			case AnimatedGridSpacerEasingType.EASE_IN_OUT_CUBIC:
			{
				return Easing.EaseInOutCubic(t);
			}
			case AnimatedGridSpacerEasingType.EASE_IN_OUT_QUART:
			{
				return Easing.EaseInOutQuad(t);
			}
			case AnimatedGridSpacerEasingType.EASE_OUT_BOUNCE:
			{
				return Easing.EaseOutBounce(t);
			}
			case AnimatedGridSpacerEasingType.EASE_OUT_BACK:
			{
				return Easing.EaseOutBack(t);
			}
		}
		return t;
	}
	
	void AnimateToColum(int targetColumn)
	{
		if (m_IsAnimating || targetColumn == m_CurrentColum)
			return;
		
		if (m_IsAnimating || targetColumn == m_CurrentColum) 
			return;
				
		m_StartPosX = m_CurrentPosX;
		m_TargetPosX = m_BasePosX - (targetColumn * m_ColumWidth);
		m_CurrentColum = targetColumn;
		m_AnimationTime = 0;
		m_IsAnimating = true;
	}
	
	void MoveRight()
	{
		if (!m_IsAnimating)
		{
			int nextColum = m_CurrentColum + 1;
			if (nextColum <= m_MaxColums)
			{
				AnimateToColum(nextColum);
			}
		}
	}
	
	void MoveLeft()
	{
		if (!m_IsAnimating)
		{
			int prevColum = m_CurrentColum - 1;
			if (prevColum >= 0)
			{
				AnimateToColum(prevColum);
			}
		}
	}
	
	void Update(float timeslice)
	{		
		if (!m_IsAnimating || !m_GridSpacer) 
			return;
		
		m_AnimationTime += timeslice;		
		if (m_AnimationTime >= m_AnimationDuration)
		{
			m_CurrentPosX = m_TargetPosX;
			m_GridSpacer.SetScreenPos(m_CurrentPosX, m_CurrentPosY);
			m_AnimationTime = 0;
			m_IsAnimating = false;
			
			m_GridSpacer.GetScreenPos(m_CurrentPosX, m_CurrentPosY);
		}
		else
		{
			float progress = m_AnimationTime / m_AnimationDuration;
			float easedProgress = ApplyEasing(progress);
			
			m_CurrentPosX = m_StartPosX + (m_TargetPosX - m_StartPosX) * easedProgress;
			m_GridSpacer.SetScreenPos(m_CurrentPosX, m_CurrentPosY);
			
			m_OnAnimationComplete.Invoke();
		}
	}
		
	void SetAnimationDuration(float duration)
	{
		m_AnimationDuration = duration;
	}
	
	void SetEasingType(AnimatedGridSpacerEasingType easing)
	{
		m_EasingType = easing;
	}
	
	bool IsAnimating()
	{
		return m_IsAnimating;
	}
	
	int GetCurrentRow()
	{
		return m_CurrentColum;
	}
}