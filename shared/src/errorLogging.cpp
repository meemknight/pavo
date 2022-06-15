#include "errorLogging.h"
#include "imgui.h"
#include <memory.h>
#include <string>

void ErrorLog::renderText()
{
	if (errorLog[0] != 0)
	{
		if (errorType == info)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3, 0.3, 0.7, 1.0));
			ImGui::Text(errorLog);
			ImGui::PopStyleColor(1);
		}
		else if (errorType == warn)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7, 0.6, 0.3, 1.0));
			ImGui::Text("Warn: %s", errorLog);
			ImGui::PopStyleColor(1);
		}
		else if (errorType == error)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7, 0.3, 0.3, 1.0));
			ImGui::Text("Error: %s", errorLog);
			ImGui::PopStyleColor(1);
		}

	}
}

void ErrorLog::clearError()
{
	memset(errorLog, 0, sizeof(errorLog));
}

void ErrorLog::setError(const char* e, ErrorType type)
{
	strcpy(errorLog, e);
	errorType = type;
}

#ifdef PAVO_UNIX
//gets last error as a string
std::string getLastErrorString()
{
	return strerror(errno);
}
#endif

#ifdef PAVO_WIN32

#define NOMINMAX
#include <Windows.h>

//https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
std::string getLastErrorString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
	{
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}


#endif