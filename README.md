About FileSettings Plain, this is what you can and can't do:
The format for values is: key=value
To make a set, put the setname between square brackets: [setname]
And the set with an empty line (double newline char) For example:
[myset]
key1=value
key2=value

[myset2]
etc.

If you don't create a set the key will be part of the global values.

DON'T PUT THE GLOBAL VALUES ANYWHERE ELSE THAN AT THE BEGINNING OF THE FILE
DON'T LET ANY GLOBAL VALUE END IN A NUMBER! (If this is problematic consider putting them in a set [numbervalues] it's all the same in the end anyway)
NEVER USE NUMBERS IN SETNAMES.
DOT (.) AND EQUALS (=) CHARACTERS ARE RESERVED AND CAN NEVER BE USED IN A KEY OR VALUE OR SET

You can create subsets. These you must close with a '~' character:
[myset]
key1=value
[subset]
keysubset=value
~

[myset2]
etc.

You can write and read to the settingsfile with the corresponding functions. Write your keys in this format: myset.subset.key
For example:
std::vector<std::string> settingsList = settings->get({"myset.subset.key", "otherset.key");

#################################################################################################################################################################################
Example of a plainsettingsfile:
globalkey=value
globalkey2=value

[firstset]
entities=2
[subset1]
hp=10
mana=2
name=X
~
[subset2]
hp=20
mana=0
name=y
~

[secondset]
key=value