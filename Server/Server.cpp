#include "../Common/Headers.h"

int main()
{
	// Socket koji osluskuje za nove konekcije.
	SOCKET listenSocket = INVALID_SOCKET;

	// Socket koji se koristi prilikom komunikacije sa klijentima.
	SOCKET clientSocket = INVALID_SOCKET;

	// Promenljiva u kojoj se cuvaju povratne vrednosti funkcija.
	int iResult;

	// Buffer koji se koristi prilikom slanja poruka.
	char dataBuffer[BUFFER_SIZE];

	// Inicijalizacija generatora nasumicnih brojeva.
	time_t t;
	srand((unsigned)time(&t));

	// Inicijalizacija glava listi.
	SUPPLIER *supplierHead = NULL;
	NODE *nodeHead = NULL;
	SECTION *sectionHead = NULL;
	POWER_SWITCH *powerSwitchHead = NULL;

	// Generisanje nasumicnih brojeva izvoda, cvorova, sekcija i prekidaca.
	int supplierQuantity = rand() % 5 + 2;
	int nodeQuantity = rand() % 10 + supplierQuantity;
	int powerSwitchQuantity = supplierQuantity - 1;
	int sectionQuantity = supplierQuantity + nodeQuantity + powerSwitchQuantity - 1;

	// Inicijalizacija komponenti i stvaranje listi.
	supplierHead = InitializeSupplierComponent(supplierHead, supplierQuantity);
	nodeHead = InitializeNodeComponent(nodeHead, nodeQuantity);
	sectionHead = InitializeSectionComponent(sectionHead, supplierQuantity, nodeQuantity, sectionQuantity);
	powerSwitchHead = InitializePowerSwitchComponent(powerSwitchHead, powerSwitchQuantity);

	// Inicijalizacija WSADATA strukture.
	if (InitializeWindowsSockets() == false)
	{
		return 1;
	}

	// Kreiranje i inicijalizacija server adresne strukture.
	sockaddr_in serverAddress;

	memset((char *)&serverAddress, 0, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
	serverAddress.sin_port = htons(SERVER_PORT);

	// Pravljenje novog socket-a.
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("Function socket failed with error: %ld.\n\n", WSAGetLastError());
		WSACleanup();

		return 1;
	}

	// Podesavanje TCP socket-a za osluskivanje. Bind-ovanje na adresu servera.
	iResult = bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

	if (iResult == SOCKET_ERROR)
	{
		printf("Function bind failed with error: %ld.\n\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();

		return 1;
	}

	// Postavljanje listen socket-a u neblokirajuci rezim.
	unsigned long mode = 1;

	if (ioctlsocket(listenSocket, FIONBIO, &mode) != 0)
	{
		printf("Function ioctlsocket failed with error: %ld.\n\n", WSAGetLastError());
	}

	// Postavljanje listen socket-a u rezim slusanja.
	iResult = listen(listenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		printf("Function listen failed with error: %ld.\n\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();

		return 1;
	}

	// Server ceka nove konekcije.
	printf("============================================================================\n");
	printf("Server socket is set to listening mode. Waiting for new connection requests.\n");
	printf("============================================================================\n\n");

	while (true)
	{
		// Proverava se da li se novi klijent povezao na server.
		iResult = ReadOrWrite(listenSocket, 0);

		if (iResult == 0)			// Slucaj kada se nista nije dogodilo.
		{
			continue;
		}
		else if (iResult == -1)		// Slucaj kada se desila greska.
		{
			return -1;
		}
		else
		{
			// Prihvatanje nove konekcije.
			clientSocket = accept(listenSocket, NULL, NULL);

			if (clientSocket == INVALID_SOCKET)
			{
				printf("Function accept failed with error : %d\n", WSAGetLastError());
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}

			// Primanje podataka od klijenta.
			if (Receive(clientSocket, supplierHead, nodeHead, sectionHead, dataBuffer, 4, 0) == 1)
			{
				return 1;
			}
		}
	}

	// Brisanje svih napravljenih listi.
	DeleteSupplierList(&supplierHead);
	DeleteNodeList(&nodeHead);
	DeleteSectionList(&sectionHead);
	DeletePowerSwitchList(&powerSwitchHead);

	// Gasenje konekcije.
	iResult = shutdown(clientSocket, SD_BOTH);

	if (iResult == SOCKET_ERROR)
	{
		printf("Function shutdown failed with error: %ld.\n\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();

		return 1;
	}

	// Zatvaranje socket-a.
	closesocket(listenSocket);
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}