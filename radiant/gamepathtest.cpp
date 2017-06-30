
#include "gamepath.cpp"
#include <iomanip>

int main(int argc, char **argv)
{
	char gamePath[MAX_PATH];
	unsigned int gamePathSize = sizeof(gamePath);
	bool hasPath;

	for(int id = 0; id < GAME_COUNT; id++) {
		hasPath = getInstalledGamePath(id, gamePath, gamePathSize);
		if (hasPath) {
			std::cout << "Game id = " << std::setw(2) << id << ", path = " << gamePath << std::endl;
		} else {
			std::cout << "Game id = " << std::setw(2) << id << ", no path found." << std::endl;
		}
	}

	return 0;
}