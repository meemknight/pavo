#include "fileDialogue.h"

void FileDialogue::start()
{
	fileDialog.SetTitle("Sellect program");
	fileDialog.SetTypeFilters({".exe"});
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
