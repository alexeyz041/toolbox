
#include <string>
#include <unordered_map>

class Settings {
public:
	Settings();
	~Settings();

public:
	std::string get(const char *key);
	int load(const char *fnm);

private:
	std::unordered_map<std::string, std::string> m_Settings;
};
