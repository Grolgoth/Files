#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "File.h"
#include "encoder.h"

class Settings
{
	friend class SettingsManager;
	public:
		virtual ~Settings();
		virtual std::vector<std::string> get(std::vector<std::string> keys) = 0;
		virtual std::vector<std::string> getSet(std::string key) = 0;
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
		enum type {
			XML = 1,
			JSON,
			PLAIN = 0
		};
		SettingsManager(std::string fileName, bool isEncoded, SettingsManager::type settingsType, Encoder* encoder);
		~SettingsManager();
		std::vector<std::string> get(std::vector<std::string> keys) {return settings->get(keys);}
		std::vector<std::string> getSet(std::string key) {return settings->getSet(key);}
		bool write(std::string key, std::string value, bool overwriteIfExists = true) {return settings->write(key, value, overwriteIfExists);}
		bool exists(std::string key) {return settings->exists(key);}
	private:
		Settings* settings = nullptr;
};

#endif // SETTINGSMANAGER_H
