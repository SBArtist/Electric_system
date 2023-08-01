#ifndef COMMON_H_
#define COMMON_H_

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include "conio.h"
#include <stdio.h>
#include <time.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "127.0.0.1"	// IP adresa servera.
#define SERVER_PORT 27016				// Prvi port.
#define SERVER_PORT_2 27017				// Drugi port.
#define BUFFER_SIZE 256					// Velicina komunikacionog buffer-a.
#define SERVER_SLEEP_TIME 50			// Vreme uspavljivanja servera.

typedef struct supplier			// Izvod - struktura.
{
	int id;						// ID izvoda.
	struct supplier *next;		// Pokazivac na sledeci u listi izvoda.

} SUPPLIER;

typedef struct node				// Cvor - struktura.
{
	int id;						// ID cvora.
	double consumption;			// Potrosnja cvora.
	SOCKET socket;				// Socket za cvor.
	struct node *next;			// Pokazivac na sledeci cvor u listi cvorova.

} NODE;

typedef struct section			// Sekcija - struktura.
{
	int id;						// ID sekcije.
	int node1;					// ID pocetnog cvora.
	int node2;					// ID krajnjeg cvora.
	struct section *next;		// Pokazivac na sledecu sekciju u listi sekcija.

} SECTION;

typedef struct power_switch     // Prekidac - struktura.
{
	int id;						// ID prekidaca.
	struct power_switch *next;	// Pokazivac na sledeci prekidac u listi prekidaca.

} POWER_SWITCH;

/* Funkcija koja pravi novi izvod i generise pocetak liste izvoda. */
SUPPLIER* CreateNewSupplier(int id, SUPPLIER *next);

/* Funkcija koja dodaje izvod na kraj liste izvoda. */
SUPPLIER* AddSupplierToTheEnd(SUPPLIER *head, int id);

/* Funkcija koja inicijalizuje izvod komponentu i pravi listu tog tipa. */
SUPPLIER* InitializeSupplierComponent(SUPPLIER *head, int quantity);

/* Funkcija koja brise listu izvoda. */
void DeleteSupplierList(SUPPLIER **head);

/* Funkcija koja pronalazi izvod u zavisnosti od specificiranog ID-a. */
bool FindSupplier(SUPPLIER *head, int id);

/* Funkcija koja pravi novi cvor i generise pocetak liste cvorova. */
NODE* CreateNewNode(int id, double consumption, SOCKET socket, NODE *next);

/* Funkcija koja dodaje cvor na kraj liste cvorova. */
NODE* AddNodeToTheEnd(NODE *head, int id, double consumption, SOCKET socket);

/* Funkcija koja inicijalizuje cvor komponentu i pravi listu tog tipa. */
NODE* InitializeNodeComponent(NODE *head, int quantity);

/* Funkcija koja brise listu cvorova. */
void DeleteNodeList(NODE **head);

/* Funkcija koja pravi novu sekciju i generise pocetak liste sekcija. */
SECTION* CreateNewSection(int id, int node1, int node2, SECTION *next);

/* Funkcija koja dodaje sekciju na kraj liste sekcija. */
SECTION* AddSectionToTheEnd(SECTION *head, int id, int node1, int node2);

/* Funkcija koja inicijalizuje sekciju komponentu i pravi listu tog tipa. */
SECTION* InitializeSectionComponent(SECTION *head, int supplierQuantity, int nodeQuantity, int sectionQuantity);

/* Funkcija koja brise listu sekcija. */
void DeleteSectionList(SECTION **head);

/* Funkcija koja sumira potrosnju za specificirani izvod. */
double SummarizeConsumption(SECTION *sectionHead, NODE *nodeHead, int id);

/* Funkcija koja upisuje potrosnju i socket cvora. */
void NodeConsumption(NODE *head, int id, double consumption, SOCKET socket);

/* Funkcija koja pravi novi prekidac i generise pocetak liste prekidaca. */
POWER_SWITCH* CreateNewPowerSwitch(int id, POWER_SWITCH *next);

/* Funkcija koja dodaje prekidac na kraj liste prekidaca. */
POWER_SWITCH* AddPowerSwitchToTheEnd(POWER_SWITCH *head, int id);

/* Funkcija koja inicijalizuje prekidac komponentu i pravi listu tog tipa. */
POWER_SWITCH* InitializePowerSwitchComponent(POWER_SWITCH *head, int quantity);

/* Funkcija koja brise listu prekidaca. */
void DeletePowerSwitchList(POWER_SWITCH **head);

/* Test funkcija koja (modifikovanjem) moze da ispisuje listu izvoda, cvorova, sekcija i prekidaca. */
void PrintList(SECTION *head);

/* Inicijalizacija WSADATA strukture koja ce primati podatke o implementaciji Windows socket-a. */
bool InitializeWindowsSockets();

/* Funkcija koja proverava da li na nekom socket-u ima odredjenih dogadjaja (citanje ili pisanje). */
int ReadOrWrite(SOCKET socket, int state);

/* Funkcija koja salje podatke specificiranom socket-u. */
int Send(SOCKET clientSocket, char *buff, int size);

/* Funkcija koja sluzi za primanje podataka. */
int Receive(SOCKET clientSocket, SUPPLIER *supplierHead, NODE *nodeHead, SECTION *sectionHead, char *buff, int size, int side);

#endif // COMMON_H_