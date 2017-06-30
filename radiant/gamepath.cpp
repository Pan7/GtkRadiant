//TODO: fix and include preferences.h or create a header for gameType_e
  enum gameType_e {
	GAME_NONE = 0,
	GAME_Q3 = 1,
	GAME_URT,
	GAME_UFOAI,
	GAME_QUETOO,
	GAME_WARSOW,
	GAME_NEXUIZ,
	GAME_Q2,
	GAME_TREMULOUS,
	GAME_JA,
	GAME_REACTION,
	GAME_ET,
	GAME_QL,
	GAME_STVEF,
	GAME_WOLF,
	GAME_Q1,
	GAME_COUNT
  };

#if defined(_MSC_VER) || defined(__CYGWIN__)
#include <windows.h>
#include <fstream>
#include <iostream>
//C++11 check
#if __cplusplus >= 201103L
#include <regex>
#endif
#include <string>

#include <sys/types.h>
#include <sys/stat.h>


static bool directoryExists(const char *path)
{
	struct stat info;

	if (stat(path, &info) != 0) {
		return false;
	} else if(info.st_mode & S_IFDIR) {
		return true;
	}
	return false;
}

static bool getRegistryStringValue(HKEY hKey, const char * keyName, const char * valueName, char * value, DWORD & valueSize)
{
	DWORD result;
	DWORD type;

	if (!keyName || !valueName || !value) {
		return false;
	}

	result = RegOpenKeyEx(hKey, keyName, 0, KEY_QUERY_VALUE, &hKey);
	if (result != ERROR_SUCCESS) {
		return false;
	}

	result = RegQueryValueEx(hKey, valueName, NULL, &type, (LPBYTE)value, &valueSize);
	if (result != ERROR_SUCCESS) {
		RegCloseKey(hKey);
		return false;
	}

	if (type != REG_SZ) {
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);
	return true;
}

static bool getAppRegistryStringValue(const char * keyName, const char * valueName, char * value, DWORD & valueSize)
{
	bool gotValue = getRegistryStringValue(HKEY_CURRENT_USER, keyName, valueName, value, valueSize);
	if (gotValue) {
		return true;
	}
	gotValue = getRegistryStringValue(HKEY_LOCAL_MACHINE, keyName, valueName, value, valueSize);
	return gotValue;	
}

static bool getSteamPath(char * steamPath, DWORD & pathSize)
{
	return getAppRegistryStringValue("Software\\Valve\\Steam", "SteamPath", steamPath, pathSize);
}

static bool getInstalledSteamGamePathByAcf(const char * steamInstallPath, const char * appid, char * gamePath, unsigned int & gamePathSize)
{
	std::string steamGameAcf = steamInstallPath;
	steamGameAcf += "/SteamApps/appmanifest_";
	steamGameAcf += appid;
	steamGameAcf += ".acf";

	std::ifstream configfile(steamGameAcf.c_str());
	if (!configfile.is_open()) {
		return false;
	}
	std::string line;
	//TODO: use an acf parser
	while (std::getline(configfile, line)) {
		std::string gameInstallDir;
		bool hasDir;
#if __cplusplus >= 201103L
		std::smatch m;
		std::regex e("[^\"]*\"installdir[^\"]*\"[^\"]*\"([^\"]*)\"[^\"]*");
		hasDir = std::regex_search(line, m, e);
		gameInstallDir = m;
#else
		char gameInstallDirBuf[4096];
		int n;
		n = sscanf(line.c_str(), "%*[^\"]\"installdir\"%*[^\"]\"%4096[^\"]s\"%*s", gameInstallDirBuf);
		hasDir = n>0? true:false;
		gameInstallDir = gameInstallDirBuf;
#endif
		if (hasDir) {
			std::string gameInstallPath = steamInstallPath;
			gameInstallPath += "/SteamApps/common/";
			gameInstallPath += gameInstallDir;

			GetFullPathName(gameInstallPath.c_str(), gamePathSize, gamePath, NULL);

			if (directoryExists(gamePath)) {
				configfile.close();
				return true;
			}
		}
	}
	configfile.close();

	return false;

}

static bool getInstalledSteamGamePath(const char * appid, char * gamePath, unsigned int & gamePathSize)
{
	char steamPath[MAX_PATH];
	DWORD steamPathSize = sizeof(steamPath);
	bool hasPath;
	
	hasPath = getSteamPath(steamPath, steamPathSize);
	if (!hasPath) {
		return false;
	}

	std::string steamConfig = steamPath;
	steamConfig += "/config/config.vdf";

	std::ifstream configfile(steamConfig.c_str());
	if (!configfile.is_open()) {
		return false;
	}
	std::string line;
	//TODO: use a vdf parser
	while (std::getline(configfile, line)) {
		std::string steamInstallPath;
#if __cplusplus >= 201103L
		std::smatch m;
		std::regex e("[^\"]*\"BaseInstallFolder_[^\"]*\"[^\"]*\"([^\"]*)\"[^\"]*");
		hasPath = std::regex_search(line, m, e);
		steamInstallPath = m;
#else
		char steamInstallPathBuf[4096];
		int n;
		n = sscanf(line.c_str(), "%*[^\"]\"BaseInstallFolder_%*i\"%*[^\"]\"%4096[^\"]s\"%*s", steamInstallPathBuf);
		hasPath = n>0? true:false;
		steamInstallPath = steamInstallPathBuf;
#endif
		if (hasPath) {
			//TODO: decode path?
			hasPath = getInstalledSteamGamePathByAcf(steamInstallPath.c_str(), appid, gamePath, gamePathSize);
			if (hasPath) {
				configfile.close();
				return true;
			}
		}

	}
	configfile.close();

	return false;
}

bool getInstalledGamePath(int gameId, char * gamePath, unsigned int & gamePathSize)
{
	bool hasPath = false;

	switch (gameId) {
		case GAME_Q1:
			hasPath = getInstalledSteamGamePath("2310", gamePath, gamePathSize);
			break;
		case GAME_Q2:
			hasPath = getInstalledSteamGamePath("2320", gamePath, gamePathSize);
			break;
		case GAME_Q3:
			hasPath = getInstalledSteamGamePath("2200", gamePath, gamePathSize);
			break;
		case GAME_URT:
			break;
		case GAME_UFOAI:
			break;
		case GAME_QUETOO:
			break;
		case GAME_WARSOW:
			break;
		case GAME_NEXUIZ:
			hasPath = getInstalledSteamGamePath("96800", gamePath, gamePathSize);
			break;
		case GAME_TREMULOUS:
			break;
		case GAME_JA: //Jedi Knight - Jedi Academy
			hasPath = getInstalledSteamGamePath("6020", gamePath, gamePathSize);
			break;
		case GAME_REACTION:
			break;
		case GAME_ET:
			break;
		case GAME_QL:
			hasPath = getInstalledSteamGamePath("282440", gamePath, gamePathSize);
			break;
		case GAME_STVEF: //Star Trek Voyager : Elite Force
			break;
		case GAME_WOLF: //Return To Castle Wolfenstein RTCW
			hasPath = getInstalledSteamGamePath("9010", gamePath, gamePathSize);
			break;
//		case GAME_JK2: //Jedi Knight II - Jedi Outcast
//			hasPath = getInstalledSteamGamePath("6030", gamePath, gamePathSize);
//			break;
//		case GAME_HL: //Half-Life
//			hasPath = getInstalledSteamGamePath("70", gamePath, gamePathSize);
//			break;
//		case GAME_SOF2: //Soldier of Fortune II - Double Helix
//			break;
	}
	if (hasPath) {
		return true;
	}

	DWORD gamePathSizeD = gamePathSize;
	switch (gameId) {
		case GAME_Q1:
			break;
		case GAME_Q2:
			hasPath = getAppRegistryStringValue("Software\\Windows\\CurrentVersion\\App Paths\\Quake2_exe", "Path", gamePath, gamePathSizeD);
			break;
		case GAME_Q3:
			hasPath = getAppRegistryStringValue("Software\\Id\\Quake III Arena", "INSTALLPATH", gamePath, gamePathSizeD);
			break;
		case GAME_URT:
			break;
		case GAME_UFOAI:
			break;
		case GAME_QUETOO:
			break;
		case GAME_WARSOW:
			break;
		case GAME_NEXUIZ:
			break;
		case GAME_TREMULOUS:
			hasPath = getAppRegistryStringValue("Software\\Tremulous", "InstallDir", gamePath, gamePathSizeD);
			break;
		case GAME_JA:
			hasPath = getAppRegistryStringValue("Software\\LucasArts\\Star Wars Jedi Knight Jedi Academy\\1.0", "Install Path", gamePath, gamePathSizeD);
			break;
		case GAME_REACTION:
			break;
		case GAME_ET:
			hasPath = getAppRegistryStringValue("Software\\Activision\\Wolfenstein - Enemy Territory", "InstallPath", gamePath, gamePathSizeD);
			break;
		case GAME_QL:
			break;
		case GAME_STVEF:
			hasPath = getAppRegistryStringValue("Software\\Activision\\Star Trek: Voyager - Elite Force\\v1.0", "INSTALLPATH", gamePath, gamePathSizeD);
			break;
		case GAME_WOLF:
			hasPath = getAppRegistryStringValue("Software\\Activision\\Return to Castle Wolfenstein", "InstallPath", gamePath, gamePathSizeD);
			if (hasPath) {
				gamePathSize = static_cast<unsigned int>(gamePathSizeD);
				return true;
			}
			hasPath = getAppRegistryStringValue("Software\\Activision\\Return to Castle Wolfenstein - Game of The Year Edition", "InstallPath", gamePath, gamePathSizeD);
			break;
//		case GAME_JK2: //Jedi Knight II - Jedi Outcast
//			hasPath = getAppRegistryStringValue("Software\\LucasArts Entertainment Company LLC\\Star Wars JK II Jedi Outcast\\1.0", "Install Path", gamePath, gamePathSizeD);
//			break;
//		case GAME_HL: //Half-Life
//			hasPath = getAppRegistryStringValue("Software\\Valve\\Half-Life", "InstallPath", gamePath, gamePathSizeD);
//			break;
//		case GAME_SOF2: //Soldier of Fortune II - Double Helix
//			hasPath = getAppRegistryStringValue("Software\\Activision\\Soldier of Fortune II - Double Helix", "InstallPath", gamePath, gamePathSizeD);
//			break;
	}
	if (hasPath) {
		gamePathSize = static_cast<unsigned int>(gamePathSizeD);
		return true;
	}

	return false;
}
#else

bool getInstalledGamePath(int gameId, char * gamePath, unsigned int & gamePathSize)
{
	return false;
}

#endif