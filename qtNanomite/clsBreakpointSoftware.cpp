/*
 * 	This file is part of Nanomite.
 *
 *    Nanomite is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Nanomite is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Nanomite.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "clsMemManager.h"
#include "clsBreakpointSoftware.h"
#include "clsDebugger\clsDebugger.h"
#include "clsMemoryProtector.h"

bool clsBreakpointSoftware::wSoftwareBP(DWORD processID, DWORD64 breakpointOffset, DWORD breakpointSize, BYTE **dataBackup, DWORD breakpointDataType)
{
	SIZE_T	bytesWritten			= NULL,
			bytesRead				= NULL;
	BYTE	*breakpointData			= (PBYTE)clsMemManager::CAlloc(breakpointSize),
			*breakpointDataBackup	= (PBYTE)clsMemManager::CAlloc(breakpointSize);
	DWORD	oldProtection			= NULL,
			newProtection			= PAGE_READWRITE;
	HANDLE	processHandle			= clsDebugger::GetProcessHandleByPID(processID);
	bool	returnValue				= false,
			worked					= false;

	if(breakpointDataType == BP_SW_LONGINT3)
	{
		breakpointData[0] = 0xCD;
		breakpointData[1] = 0x03;
	}
	else if(breakpointDataType == BP_SW_UD2)
	{
		breakpointData[0] = 0x0F;
		breakpointData[1] = 0x0B;
	}
	else if(breakpointDataType == BP_SW_HLT)
	{
		memset(breakpointData, 0xF4, breakpointSize);
	}
	else
	{
		memset(breakpointData, 0xCC, breakpointSize);
	}

	memset(breakpointDataBackup, NULL, breakpointSize);

	
	clsMemoryProtector tempMemoryProtector(processHandle, PAGE_READWRITE, breakpointSize, breakpointOffset, &worked);
	if(worked && ReadProcessMemory(processHandle, (LPVOID)breakpointOffset, (LPVOID)breakpointDataBackup, breakpointSize, &bytesRead))
	{
		if(*dataBackup == NULL)
			*dataBackup = breakpointDataBackup;
		else
			clsMemManager::CFree(breakpointDataBackup);

		if(WriteProcessMemory(processHandle, (LPVOID)breakpointOffset, (LPVOID)breakpointData, breakpointSize, &bytesWritten))
			returnValue = true;
	}
	else
	{
		clsMemManager::CFree(breakpointDataBackup);
	}

	clsMemManager::CFree(breakpointData);
	return returnValue;
}

bool clsBreakpointSoftware::dSoftwareBP(DWORD processID, DWORD64 breakpointOffset, DWORD breakpointSize, BYTE *orgBreakpointData)
{
	if(breakpointOffset == 0 || orgBreakpointData == NULL)
		return false;

	SIZE_T	bytesWritten	= NULL;
	HANDLE	processHandle	= clsDebugger::GetProcessHandleByPID(processID);
	DWORD	oldProtection	= NULL,
			newProtection	= PAGE_READWRITE;
	bool	worked			= false;


	clsMemoryProtector tempMemoryProtector(processHandle, PAGE_READWRITE, breakpointSize, breakpointOffset, &worked);
	if(worked && WriteProcessMemory(processHandle, (LPVOID)breakpointOffset, (LPVOID)orgBreakpointData, breakpointSize, &bytesWritten))
	{
		return true;
	}

	return false;
}