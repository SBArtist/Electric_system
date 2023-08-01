#include "../Common/Headers.h"

int main()
{
	// Socket koji sluzi za komunikaciju sa serverom.
	SOCKET connectSocket = INVALID_SOCKET;

	// Promenljiva u kojoj se cuvaju povratne vrednosti funkcija.
	int iResult;

	// Buffer koji se koristi prilikom slanja poruka.
	char dataBuffer[BUFFER_SIZE];

	// Promenljiva u kojoj se cuva vrednost ID-a koji je korisnik uneo.
	int tempID;

	// Pomocni pokazivac za smestanje vrednosti ID-a u komunikacioni buffer (dataBuffer).
	int *id;

	// Inicijalizacija WSADATA strukture.
	if (InitializeWindowsSockets() == false)
	{
		return 1;
	}

	// Kreiranje i inicijalizacija server adresne strukture.
	sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
	serverAddress.sin_port = htons(SERVER_PORT);

	// Pravljenje socket-a za komunikaciju.
	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Function socket failed with error: %ld.\n", WSAGetLastError());
		WSACleanup();

		return 1;
	}

	// Povezivanje klijenta na server.
	iResult = connect(connectSocket, (SOCKADDR *)&serverAddress, sizeof(serverAddress));

	if (iResult == SOCKET_ERROR)
	{
		printf("Function connect failed with error: %ld.\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();

		return 1;
	}

	// Glavna petlja kod klijenta u okviru koje se vrsi komunikacija sa serverom.
	while (true)
	{
		// Korisnik, na pocetku, unosi ID izvoda.
		printf("Enter power supplier ID: ");
		scanf_s("%d", &tempID);
		getchar();

		// Smestanje specificiranog ID-a u komunikacioni buffer.
		id = (int *)(dataBuffer);
		id[0] = tempID;

		// Slanje podataka serveru.
		iResult = Send(connectSocket, dataBuffer, sizeof(int));

		if (iResult == -1)
		{
			printf("Function Send failed.\n\n");
			closesocket(connectSocket);
			WSACleanup();

			return 1;
		}

		printf("\nMessage successfully sent. Bytes sent: %ld.\n", iResult);

		// Primanje podataka od strane servera.
		iResult = Receive(connectSocket, NULL, NULL, NULL, dataBuffer, sizeof(double), 1);

		if (iResult == -1)
		{
			printf("Function Receive failed.\n\n");
			closesocket(connectSocket);
			WSACleanup();

			return 1;

		}

		// Na kraju, klijent odlucuje da li ce da nastavi sa radom, ili ce da prekine konekciju.
		printf("Press x to exit. Press any key to continue.\n");
		printf("=====================================================\n\n");

		// Logika za gasenje konekcije. Ukoliko klijent posalje karakter 'x', konekcija sa serverom se gasi.
		if (_getch() == 'x')
		{
			strcpy_s(dataBuffer, "x");

			iResult = Send(connectSocket, dataBuffer, sizeof(int));

			if (iResult == -1)
			{
				printf("Function Send failed.\n\n");
				closesocket(connectSocket);
				WSACleanup();

				return 1;

			}

			break;
		}
	}
	
	// Tmin: 0.6 [-1.1, 5] Tmax: 18.1 [17.2, 18.9]

	// Gasenje konekcije.
	iResult = shutdown(connectSocket, SD_BOTH);

	if (iResult == SOCKET_ERROR)
	{
		printf("Function shutdown failed with error: %ld.\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();

		return 1;
	}

	// Zatvaranje socket-a.
	closesocket(connectSocket);

	return 0;
}