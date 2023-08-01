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
	serverAddress.sin_port = htons(SERVER_PORT_2);

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

	// Node ceka nove konekcije.
	printf("==========================================================================\n");
	printf("Node socket is set to listening mode. Waiting for new connection requests.\n");
	printf("==========================================================================\n\n");

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
			if (Receive(clientSocket, NULL, NULL, NULL, dataBuffer, 4, 2) == 1)
			{
				return 1;
			}
		}
	}

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