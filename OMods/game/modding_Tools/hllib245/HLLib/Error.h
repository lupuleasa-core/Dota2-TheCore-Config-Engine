/*
 * HLLib
 * Copyright (C) 2006-2010 Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdarg.h>

#include "stdafx.h"

#define ERROR_STRING_SIZE 4096

namespace HLLib
{
	class HLLIB_API CError
	{
	private:
		hlChar lpError[ERROR_STRING_SIZE];

		hlUInt uiSystemError;
		hlChar lpSystemError[ERROR_STRING_SIZE];

		hlChar lpShortFormattedError[ERROR_STRING_SIZE];
		hlChar lpLongFormattedError[ERROR_STRING_SIZE];

	public:
		CError();
		~CError();

		hlVoid Clear();

		const hlChar *GetErrorMessage() const;

		hlUInt GetSystemError() const;
		const hlChar *GetSystemErrorMessage() const;

		const hlChar *GetShortFormattedErrorMessage();
		const hlChar *GetLongFormattedErrorMessage();

		hlVoid SetErrorMessage(const hlChar *lpError);
		hlVoid SetErrorMessageFormated(const hlChar *lpFormat, ...);

		hlVoid SetSystemErrorMessage(const hlChar *lpError);
		hlVoid SetSystemErrorMessageFormated(const hlChar *lpFormat, ...);
	};
}

#endif
