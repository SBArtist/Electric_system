#include "Headers.h"

SUPPLIER* CreateNewSupplier(int id, SUPPLIER *next)
{
	SUPPLIER *newSupplier = (SUPPLIER *)malloc(sizeof(SUPPLIER));	// Alociranje memorije za glavu liste.

	if (newSupplier == NULL)
	{
		printf("Request for memory allocation failed.\n\n");
		exit(0);
	}

	newSupplier->id = id;											// Prosledjivanje podataka.
	newSupplier->next = next;

	return newSupplier;
}

SUPPLIER* AddSupplierToTheEnd(SUPPLIER *head, int id)
{
	SUPPLIER *current = head;

	while (current->next != NULL)
	{
		current = current->next;
	}

	SUPPLIER *newSupplier = CreateNewSupplier(id, NULL);

	current->next = newSupplier;

	return head;
}

SUPPLIER* InitializeSupplierComponent(SUPPLIER *head, int quantity)
{
	int id = 0;

	head = CreateNewSupplier(id, NULL);

	for (int i = 1; i < quantity; i++)			// Pravljenje liste.
	{
		id = i * 4;

		head = AddSupplierToTheEnd(head, id);
	}

	return head;
}

void DeleteSupplierList(SUPPLIER **head)
{
	SUPPLIER *current = *head;
	SUPPLIER *next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	*head = NULL;
}

bool FindSupplier(SUPPLIER *head, int id)
{
	bool found = false;
	SUPPLIER *current = head;

	// Pretraga traje sve dok se ne pronadje specificiran ID, odnosno dok se ne dodje do kraja liste.
	while (found == false && current != NULL)
	{
		if (current->id == id)
		{
			found = true;
		}
		else
		{
			current = current->next;
		}
	}

	return found;
}

NODE* CreateNewNode(int id, double consumption, SOCKET socket, NODE *next)
{
	NODE *newNode = (NODE *)malloc(sizeof(NODE));

	if (newNode == NULL)
	{
		printf("Request for memory allocation failed.\n\n");
		exit(0);
	}

	newNode->id = id;
	newNode->consumption = consumption;
	newNode->socket = socket;
	newNode->next = next;

	return newNode;
}

NODE* AddNodeToTheEnd(NODE *head, int id, double consumption, SOCKET socket)
{
	NODE *current = head;

	while (current->next != NULL)
	{
		current = current->next;
	}

	NODE *newNode = CreateNewNode(id, consumption, socket, NULL);

	current->next = newNode;

	return head;
}

NODE* InitializeNodeComponent(NODE *head, int quantity)
{
	time_t t;
	srand((unsigned)time(&t));

	int id = 1;
	double consumption = rand() % 500 + 10;
	SOCKET socket = INVALID_SOCKET;

	head = CreateNewNode(id, consumption, socket, NULL);

	for (int i = 1; i < quantity; i++)
	{
		id += 4;
		consumption = ((double)(rand() % 500)) / (double)3;
		head = AddNodeToTheEnd(head, id, consumption, socket);
	}

	return head;
}

void DeleteNodeList(NODE **head)
{
	NODE *current = *head;
	NODE *next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	*head = NULL;
}

SECTION* CreateNewSection(int id, int node1, int node2, SECTION *next)
{
	SECTION *newSection = (SECTION *)malloc(sizeof(SECTION));

	if (newSection == NULL)
	{
		printf("Request for memory allocation failed.\n\n");
		exit(0);
	}

	newSection->id = id;
	newSection->node1 = node1;
	newSection->node2 = node2;
	newSection->next = next;

	return newSection;
}

SECTION* AddSectionToTheEnd(SECTION *head, int id, int node1, int node2)
{
	SECTION *current = head;

	while (current->next != NULL)
	{
		current = current->next;
	}

	SECTION *newSection = CreateNewSection(id, node1, node2, NULL);

	current->next = newSection;

	return head;
}

SECTION* InitializeSectionComponent(SECTION *head, int supplierQuantity, int nodeQuantity, int sectionQuantity)
{
	// Odredjivanje broja cvorova po izvodu. Priblizno ravnomerna raspodela.
	div_t nodesPerSupplier = div(nodeQuantity, supplierQuantity);
	int quotient = nodesPerSupplier.quot;
	int remainder = nodesPerSupplier.rem;

	int currentSupplierID = 0;				// Inicijalne vrednosti ID-eva komponenti.
	int currentNodeID = 1;
	int currentSectionID = 2;
	int currentPowerSwitchID = 3;

	int previousNodeID = 0;					// Prethodne vrednosti ID-eva komponenti.
	int previousPowerSwitchID = 0;

	// Brojac konekcija na prekidacu.
	int connectionCounter = 0;

	previousNodeID = currentNodeID;
	previousPowerSwitchID = currentPowerSwitchID;

	// Pravljenje sekcija. Iteriranje do broja izvoda.
	for (int i = 0; i < supplierQuantity; i++)
	{
		if (i == 0)		// Slucaj kada se cvorovi konektuju na prvi izvod.
		{
			head = CreateNewSection(currentSectionID, currentSupplierID, currentNodeID, NULL);

			currentSupplierID += 4;		// Uvecavanje trenutnih vrednosti ID-eva. 
			currentNodeID += 4;
			currentSectionID += 4;

			for (int j = 0; j <= quotient + remainder - 1; j++)		// Ukoliko se ne moze ostvariti ravnomerna raspodela ostatak cvorova povezuje se na prvi izvod.
			{
				if (j == quotient + remainder - 1)					// Dodavanje sekcije koja spaja cvor i prekidac.
				{
					head = AddSectionToTheEnd(head, currentSectionID, previousNodeID, currentPowerSwitchID);

					connectionCounter++;
					currentSectionID += 4;
				}
				else    // Dodavanje sekcije koja spaja cvor sa cvorom.
				{
					head = AddSectionToTheEnd(head, currentSectionID, previousNodeID, currentNodeID);

					previousNodeID = currentNodeID;
					currentNodeID += 4;
					currentSectionID += 4;
				}
			}
		}
		else    // Slucaj kada se cvorovi povezuju na ostale izvode.
		{
			for (int j = 0; j <= quotient; j++)
			{
				if (j == quotient)
				{
					if (connectionCounter < 2)		// Ukoliko je broj konekcija na prekidacu manji od dva, na prekidac se konektuje jos jedan cvor.
					{
						head = AddSectionToTheEnd(head, currentSectionID, previousNodeID, previousPowerSwitchID);

						if (i < supplierQuantity - 1)	// Ukoliko ima jos nepovezanih izvoda trenutni krajnji cvor se povezuje na dva prekidaca.
						{
							head = AddSectionToTheEnd(head, currentSectionID + 4, previousNodeID, currentPowerSwitchID + 4);
						}

						connectionCounter = 1;
						currentPowerSwitchID += 4;
						currentSectionID += 4;
						previousPowerSwitchID = currentPowerSwitchID;
					}
					else             // Dodavanje sekcije koja spaja cvor i prekidac.
					{
						head = AddSectionToTheEnd(head, currentSectionID, previousNodeID, currentPowerSwitchID);

						connectionCounter++;
						currentSectionID += 4;
					}
				}
				else if (j == 0)	// Dodavanje sekcije koja spaja izvod i cvor.
				{
					head = AddSectionToTheEnd(head, currentSectionID, currentSupplierID, currentNodeID);

					previousNodeID = currentNodeID;
					currentSupplierID += 4;
					currentNodeID += 4;
					currentSectionID += 4;
				}
				else                // Dodavanje sekcije koja spaja cvor sa cvorom.
				{
					head = AddSectionToTheEnd(head, currentSectionID, previousNodeID, currentNodeID);

					previousNodeID = currentNodeID;
					currentNodeID += 4;
					currentSectionID += 4;
				}
			}
		}
	}

	return head;
}

void DeleteSectionList(SECTION **head)
{
	SECTION *current = *head;
	SECTION *next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	*head = NULL;
}

double SummarizeConsumption(SECTION *sectionHead, NODE *nodeHead, int id)
{
	// Socket koji sluzi za komunikaciju sa serverom.
	SOCKET connectSocket = INVALID_SOCKET;

	// Promenljiva u kojoj se cuvaju povratne vrednosti funkcija.
	int iResult;

	// Buffer koji se koristi prilikom slanja poruka.
	char dataBuffer[BUFFER_SIZE];

	// Pomocni pokazivac za smestanje vrednosti ID-a u komunikacioni buffer (dataBuffer).
	int *Id;

	SECTION *current = sectionHead;

	double sum = 0;						// Sumirana potrosnja. Povratna vrednost.
	double consumption = 0;				// Potrosnja nekog cvora.
	double *val;

	// Inicijalizacija WSADATA strukture.
	if (InitializeWindowsSockets() == false)
	{
		return 1;
	}

	// Kreiranje i inicijalizacija server adresne strukture.
	sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
	serverAddress.sin_port = htons(SERVER_PORT_2);

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

	// Glavna petlja. Prolazak kroz listu sekcija.
	while (current != NULL)
	{
		if (current->node1 == id)				// Pronalazenje izvoda sa specificiranim ID-em.
		{
			while (current->node2 % 4 != 3)		// Prolazak kroz listu sve dok se ne dodje do prekidaca.
			{
				Id = (int *)(dataBuffer);		// Smestanje ID-a u dataBuffer.
				Id[0] = current->node2;

				// Slanje ID-a.
				iResult = Send(connectSocket, dataBuffer, sizeof(int));

				if (iResult == -1)
				{
					printf("Function Send failed.\n\n");
					closesocket(connectSocket);
					WSACleanup();

					return 1;
				}

				// Dobijanje informacija o potrosnji.
				iResult = Receive(connectSocket, NULL, NULL, NULL, dataBuffer, sizeof(double), 3);

				val = (double *)dataBuffer;

				consumption = *val;

				NodeConsumption(nodeHead, current->node2, consumption, connectSocket);

				sum += consumption;

				current = current->next;
			}

			// Zatvaranje konekcije.
			strcpy_s(dataBuffer, "x");

			iResult = Send(connectSocket, dataBuffer, sizeof(int));

			if (iResult == -1)
			{
				printf("Function Send failed.\n\n");
				closesocket(connectSocket);
				WSACleanup();

				return 1;

			}

			iResult = shutdown(connectSocket, SD_BOTH);

			if (iResult == SOCKET_ERROR)
			{
				printf("Function shutdown failed with error: %ld.\n", WSAGetLastError());
				closesocket(connectSocket);
				WSACleanup();

				return 1;
			}

			closesocket(connectSocket);

			break;
		}
		else
		{
			current = current->next;
		}
	}

	return sum;
}

void NodeConsumption(NODE *head, int id, double consumption, SOCKET socket)
{
	NODE *current = head;
	int ID = -1;

	while (ID != id)				// Pronalazenje prosledjenog ID-a.
	{
		if (current->id == id)		// Provera jednakosti ID-eva.
		{
			current->consumption = consumption;		// Belezenje informacija o potrosnji i socket-u.
			current->socket = socket;
			ID = current->id;
		}
		else
		{
			current = current->next;
		}
	}
}

POWER_SWITCH* CreateNewPowerSwitch(int id, POWER_SWITCH *next)
{
	POWER_SWITCH *newPowerSwitch = (POWER_SWITCH *)malloc(sizeof(POWER_SWITCH));

	if (newPowerSwitch == NULL)
	{
		printf("Request for memory allocation failed.\n\n");
		exit(0);
	}

	newPowerSwitch->id = id;
	newPowerSwitch->next = next;

	return newPowerSwitch;
}

POWER_SWITCH* AddPowerSwitchToTheEnd(POWER_SWITCH *head, int id)
{
	POWER_SWITCH *current = head;

	while (current->next != NULL)
	{
		current = current->next;
	}

	POWER_SWITCH *newPowerSwitch = CreateNewPowerSwitch(id, NULL);

	current->next = newPowerSwitch;

	return head;
}

POWER_SWITCH* InitializePowerSwitchComponent(POWER_SWITCH *head, int quantity)
{
	int id = 3;

	head = CreateNewPowerSwitch(id, NULL);

	for (int i = 1; i < quantity; i++)
	{
		id += 4;
		head = AddPowerSwitchToTheEnd(head, id);
	}

	return head;
}

void DeletePowerSwitchList(POWER_SWITCH **head)
{
	POWER_SWITCH *current = *head;
	POWER_SWITCH *next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}

	*head = NULL;
}

void PrintList(SECTION *head)
{
	SECTION *current = head;

	while (current != NULL)
	{
		printf("Section ID: %d\tNode 1 ID: %d\tNode 2 ID:%d\n\n", current->id, current->node1, current->node2);
		current = current->next;
	}
}

bool InitializeWindowsSockets()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());

		return false;
	}

	return true;
}

int ReadOrWrite(SOCKET socket, int state)
{
	fd_set fds;
	timeval timeVal;

	int iResult;

	timeVal.tv_sec = 0;
	timeVal.tv_usec = 0;

	FD_ZERO(&fds);

	FD_SET(socket, &fds);

	if (state == 0)			// Slucaj: citanje.
	{
		iResult = select(0, &fds, NULL, NULL, &timeVal);
	}
	else if (state == 1)	// Slucaj: pisanje.
	{
		iResult = select(0, NULL, &fds, NULL, &timeVal);
	}
	else
	{
		printf("Invalid state.\n\n");
		return -1;
	}

	if (iResult == SOCKET_ERROR)
	{
		printf("Function select failed with error: %ld.\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();

		return 1;
	}
	else if (iResult == 0)
	{
		Sleep(SERVER_SLEEP_TIME);
	}

	return iResult;
}

int Send(SOCKET clientSocket, char *buff, int size)
{
	int count = 0;
	int selectResult = 0;
	int iResult = 0;

	while (size - count != 0)		// Podaci se salju sve dok ne budu svi poslati.
	{
		do
		{
			selectResult = ReadOrWrite(clientSocket, 1);	// Ceka se na dogadjaj (pisanje).

		} while (selectResult < 1);

		// Slanje podataka.
		iResult = send(clientSocket, buff + count, size - count, 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("Function send failed with error: %ld.\n\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();

			return -1;
		}

		count += iResult;
	}

	return count;
}

int Receive(SOCKET clientSocket, SUPPLIER *supplierHead, NODE *nodeHead, SECTION *sectionHead, char *buff, int size, int side)
{
	int selectResult;		// Povratna vrednost select() funkcije.
	int iResult = 0;
	int count = 0;
	int *id;				// Pomocni pokazivac za smestanje ID-a u komunikacioni buffer.
	double result = 0;		// Promenljiva u kojoj se skladiste podaci o sumi potrosnje.
	double *val;			// Pomocni pokazivac za smestanje potrosnje u komunikacioni buffer.

	while (true)
	{
		while (size - count != 0)
		{
			do
			{
				selectResult = ReadOrWrite(clientSocket, 0);

			} while (selectResult < 1);

			iResult = recv(clientSocket, buff + count, size - count, 0);
			count += iResult;
		}

		if (buff[0] == 'x')		// Logika za zavrsetak komunikacije.
		{
			iResult = 0;

			break;
		}

		if (size - count == 0 && side == 0)		// Logika za obradu podataka kada server primi podatke od klijenta.
		{
			id = (int *)buff;
			printf("Request for consumption summarization for power supplier with ID: %d.\n\n", *id);
			count = 0;

			if (FindSupplier(supplierHead, *id) == true)					// Pronalazenje izvoda.
			{
				val = (double *)buff;

				result = SummarizeConsumption(sectionHead, nodeHead, *id);	// Sumiranje potrosnje.

				val[0] = result;

				Send(clientSocket, buff, sizeof(double));					// Slanje sumirane potrosnje klijentu.
			}
			else    // U slucaju da je unet nepostojeci ID izvoda, server vraca klijentu negativnu vrednost (-1) potrosnje.
			{
				memset(buff, 0, size);
				val = (double *)buff;
				val[0] = -1;

				Send(clientSocket, buff, sizeof(double));
			}
		}
		else if (size - count == 0 && side == 1)	// Logika obrade podataka kada klijent primi podatke od servera.
		{
			val = (double *)buff;

			if (*val == -1)							// Negativna vrednost potrosnje (-1) podrazumeva da izvod sa specificiranim ID-em ne postoji.
			{
				printf("\n-----------------------------------------------------\n");
				printf("              MESSAGE FROM SERVER\n");
				printf("-----------------------------------------------------\n");
				printf("\nPower supplier with the specified ID does not exit.\n\n");
				printf("-----------------------------------------------------\n\n");
			}
			else                                    // U suprotnom ispisuje se ukupna potrosnja na tom izvodu.
			{
				printf("\n-----------------------------------------------------\n");
				printf("              MESSAGE FROM SERVER\n");
				printf("-----------------------------------------------------\n");
				printf("\nConsumption sum: %lf.\n\n", *val);
				printf("-----------------------------------------------------\n\n");
			}

			count = 0;

			break;
		}
		else if (size - count == 0 && side == 2)	// Logika obrade podataka kada node primi zahtev od servera.
		{
			id = (int *)buff;
			printf("Request for consumption value for node with the ID: %d.\n\n", *id);
			count = 0;

			result = ((double)(rand() % 500)) / (double)3;

			val = (double *)buff;
			val[0] = result;

			Send(clientSocket, buff, sizeof(double));
		}
		else if (size - count == 0 && side == 3)	// Logika obrade podataka kada server primi podatke od node-a.
		{
			val = (double *)buff;

			count = 0;

			break;
		}
	}

	// Zatvaranje konekcije sa klijentom.
	if (iResult == 0)	
	{
		printf("Connection with client closed.\n");
		printf("=====================================================================\n\n");
		closesocket(clientSocket);
	}

	return iResult;
}