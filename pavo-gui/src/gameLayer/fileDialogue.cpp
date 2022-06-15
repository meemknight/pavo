#include "fileDialogue.h"

void FileDialogue::start()
{
	fileDialog.SetTitle("Sellect program");

#ifdef PAVO_WIN32
	fileDialog.SetTypeFilters({".exe"});
#endif

#ifdef PAVO_UNIX

#endif

	fileDialog.Open();
}


bool FileDialogue::render()
{
	fileDialog.Display();
	return fileDialog.IsOpened();
}

std::string FileDialogue::getRezult()
{
	
	if (fileDialog.HasSelected())
	{
		return fileDialog.GetSelected().string();
	}
	
	return "";
}
