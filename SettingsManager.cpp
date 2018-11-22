#include "settingsmanager.h"
#include "settings/plainsettings.h"

Settings* SettingsManager::getSettings(std::string fileName, bool isEncoded, Settings::type settingsType, Encoder* encoder)
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
			case Settings::PLAIN:
				return new PlainSettings(file, isEncoded, encoder);
			default:
				return new PlainSettings(file, isEncoded, encoder);
		}
	} catch (std::string error)
	{
		throw error;
	}
}

Settings::Settings(File file, bool isEncoded, Encoder* encoder) : file(file), encoded(isEncoded), encoder(encoder)
{
	this->file.open();
	if (isEncoded)
		this->file.decode(encoder->getEncodeAlgorithm(), encoder->getDecodeAlgorithm());
}

Settings::~Settings()
{
	if (file.isOpen() && encoded)
		file.encode(encoder->getEncodeAlgorithm());
	if(encoder != nullptr)
		delete encoder;
}
