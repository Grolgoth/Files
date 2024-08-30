#include "settingsmanager.h"
#include "settings/plainsettings.h"

SettingsManager::SettingsManager(std::string fileName, bool isEncoded, SettingsManager::type settingsType, Encoder* encoder)
{
	try
	{
		File file(fileName, false);
		if (!file.exists())
			file.create();
		if (isEncoded && encoder == nullptr)
			throw "Error initiating Settings object: cannot create encoded Settings without an valid Encoder pointer";
		switch(settingsType)
		{
			case SettingsManager::PLAIN:
				settings = new PlainSettings(file, isEncoded, encoder);
				break;
			default:
				settings = new PlainSettings(file, isEncoded, encoder);
		}
	} catch (std::string error)
	{
		throw error;
	}
}

SettingsManager::~SettingsManager()
{
	if (settings != nullptr)
		delete settings;
}

Settings::Settings(File file, bool isEncoded, Encoder* encoder) : file(file), encoded(isEncoded), encoder(encoder)
{
	this->file.open();
	if (isEncoded)
	{
		this->file.decode(encoder->getEncodeAlgorithm(), encoder->getDecodeAlgorithm());
		this->file.close();
		this->file.open();
	}
}

Settings::~Settings()
{
	if (file.isOpen() && encoded)
		file.encode(encoder->getEncodeAlgorithm());
	if(encoder != nullptr)
		delete encoder;
}
