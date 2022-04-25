////////////////////////////////////////////////////////////////////
//DeRap: Scripts\config.bin
//Produced from mikero's Dos Tools Dll version 7.95
//https://mikero.bytex.digital/Downloads
//'now' is Wed Apr 20 15:17:41 2022 : 'file' last modified on Mon Apr 18 22:35:12 2022
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class LegacyGunplay_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};
class CfgMods
{
	class LegacyGunplay
	{
		dir = "LegacyGunplay";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "LegacyGunplay";
		author = "Jad";
		authorID = "0";
		version = "0.1";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"LegacyGunplay/Definitions","LegacyGunplay/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"LegacyGunplay/Definitions","LegacyGunplay/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"LegacyGunplay/Definitions","LegacyGunplay/5_Mission"};
			};
		};
	};
};
