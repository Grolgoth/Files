#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "File.h"
#include "encoder.h"

class Settings
{
	friend class SettingsManager;
	public:
		enum type {
			XML = 1,
			JSON,
			PLAIN = 0
		};
		virtual ~Settings();
		virtual std::vector<std::string> get(std::vector<std::string> keys) = 0;
		virtual bool write(std::string key, std::string value, bool overwriteIfExists = true) = 0;
		virtual bool exists(std::string key) = 0;
	protected:
		Settings(File file, bool isEncoded, Encoder* encoder);
		File file;
		bool encoded;
		Encoder* encoder;
		std::string error;
};

class SettingsManager
{
	public:
		static Settings* getSettings(std::string fileName, bool isEncoded, Settings::type settingsType, Encoder* encoder);
	private:
		SettingsManager(){}
};

#endif // SETTINGSMANAGER_H
