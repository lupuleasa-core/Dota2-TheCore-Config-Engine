/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "Error.h"

using namespace HLLib;

CError::CError()
{
	*this->lpError = '\0';
	this->uiSystemError = 0;
	*this->lpSystemError = '\0';
	*this->lpShortFormattedError = '\0';
	*this->lpLongFormattedError = '\0';
}

CError::~CError()
{

}

hlVoid CError::Clear()
{

}

const hlChar *CError::GetErrorMessage() const
{
	return this->lpError;
}

hlUInt CError::GetSystemError() const
{
	return this->uiSystemError;
}

const hlChar *CError::GetSystemErrorMessage() const
{
	return this->lpSystemError;
}

const hlChar *CError::GetShortFormattedErrorMessage()
{
	if(this->uiSystemError == 0)
	{
		if(*this->lpError)
		{
			sprintf(this->lpShortFormattedError, "Error: %s", this->lpError);
		}
		else
		{
			strcpy(this->lpShortFormattedError, "<No error reported.>");
		}
	}
	else
	{
		sprintf(this->lpShortFormattedError, "Error (0x%.8x): %s %s", this->uiSystemError, this->lpError, this->lpSystemError);
	}

	return this->lpShortFormattedError;
}

const hlChar *CError::GetLongFormattedErrorMessage()
{
	if(this->uiSystemError == 0)
	{
		if(*this->lpError)
		{
			sprintf(this->lpLongFormattedError, "Error:\n%s", this->lpError);
		}
		else
		{
			strcpy(this->lpLongFormattedError, "<No error reported.>");
		}
	}
	else
	{
		sprintf(this->lpLongFormattedError, "Error:\n%s\n\nSystem Error (0x%.8x):\n%s", this->lpError, this->uiSystemError, this->lpSystemError);
	}

	return this->lpLongFormattedError;
}

hlVoid CError::SetErrorMessage(const hlChar *lpError)
{
	this->SetErrorMessageFormated("%s", lpError);
}

hlVoid CError::SetErrorMessageFormated(const hlChar *lpFormat, ...)
{
	va_list ArgumentList;
	va_start(ArgumentList, lpFormat);
	vsprintf(this->lpError, lpFormat, ArgumentList);
	va_end(ArgumentList);

	this->uiSystemError = 0;
	*this->lpSystemError = '\0';
}

hlVoid CError::SetSystemErrorMessage(const hlChar *lpError)
{
	this->SetSystemErrorMessageFormated("%s", lpError);
}

hlVoid CError::SetSystemErrorMessageFormated(const hlChar *lpFormat, ...)
{
	va_list ArgumentList;
	va_start(ArgumentList, lpFormat);
	vsprintf(this->lpError, lpFormat, ArgumentList);
	va_end(ArgumentList);

#ifdef _WIN32
	this->uiSystemError = GetLastError();

	LPVOID lpMessage;

	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, this->uiSystemError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMessage, 0, NULL))
	{
		strcpy(this->lpSystemError, (hlChar *)lpMessage);

		LocalFree(lpMessage);
#else
	this->uiSystemError = (hlUInt)errno;

	hlChar *lpMessage = strerror(errno);

	if(lpMessage != 0)
	{
		strcpy(this->lpSystemError, lpMessage);
#endif

		hlUInt uiLength = (hlUInt)strlen(this->lpSystemError);

		while(uiLength >= 0 && isspace(this->lpSystemError[uiLength - 1]))
		{
			uiLength--;
		}
		this->lpSystemError[uiLength] = '\0';
	}
	else
	{
		strcpy(this->lpSystemError, "<Unable to retrieve system error message string.>");
	}
}
