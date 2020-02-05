#ifndef PLAINSETTINGS_H
#define PLAINSETTINGS_H

#include "../settingsmanager.h"

class PlainSettings : public Settings
{
	friend class SettingsManager;
	public:
		~PlainSettings();

	std::vector<std::string> get(std::vector<std::string> keys);
	std::vector<std::string> getSet(std::string key);
	bool write(std::string key, std::string value, bool overwriteIfExists = true);
	bool exists(std::string key);
	private:
		PlainSettings(File file, bool isEncoded, Encoder* encoder);
		std::vector<std::string> sets;
		std::vector<std::string> reservedSets;
		std::vector<std::string> reservedLines;
		bool valid();
};

#endif // PLAINSETTINGS_H
