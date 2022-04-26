modded class CrossHairSelector extends ScriptedWidgetEventHandler
{
	string m_dynamicCrossHairName = "dynamic_crosshair";
	string m_statiCrosshairName = "old_crosshair";

	ref DynamicCrossHair dynamicCrossHair;
	ref StatiCrosshair   statiCrosshair;

	override protected void Init()
	{
		super.Init();
				
		Widget child = m_Root.GetChildren();
		while(child)
		{
			string wName = child.GetName();
			if (wName == m_dynamicCrossHairName)
			{
				dynamicCrossHair = new DynamicCrossHair(child);
			}
			else if (wName == m_statiCrosshairName)
			{
				statiCrosshair  = new StatiCrosshair(child);
			}
			child = child.GetSibling();
		}
	}

	override protected void ShowCrossHair(CrossHair crossHair)
	{
		if (crossHair && (crossHair.GetName() == m_statiCrosshairName || crossHair.GetName() == m_dynamicCrossHairName))
		{
			//! we will want to show our dynamic crosshair
			if (dynamicCrossHair)
			{
				if (!dynamicCrossHair.IsCurrent() && dynamicCrossHair.IsShown())
				{
					dynamicCrossHair.Show();
					//we show the - - with the dynamic one too.
					if (statiCrosshair)
						statiCrosshair.Show();
				}

				//! update our crosshair position
				dynamicCrossHair.UpdatePosition();
			}
		}
		else
		{
			if (dynamicCrossHair) dynamicCrossHair.Hide();
			if (statiCrosshair) statiCrosshair.Hide();
		}

		super.ShowCrossHair(crossHair);

		HumanInputController hic = m_Player.GetInputController();
		// hide static crosshair if in freelook
		if (GetCurrentCrossHair() && GetCurrentCrossHair().GetName() == m_statiCrosshairName && hic.CameraIsFreeLook())
		{
			GetCurrentCrossHair().Hide();
		}
	}

	override protected void SelectCrossHair()
	{
		if(!m_AM) return;
		
		bool firearmInHands = m_Player.GetItemInHands() && m_Player.GetItemInHands().IsWeapon();

		//! firearms
		if (firearmInHands && m_Player.IsRaised() && !m_Player.IsInIronsights() && !m_Player.IsInOptics() && !m_Player.GetCommand_Melee2())
		{
			ShowCrossHair(GetCrossHairByName(m_dynamicCrossHairName));
		}
		else
		{
			ShowCrossHair(null);
		}
	}
}