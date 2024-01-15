#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include"conio2.h"
#include <fstream>
#include <time.h>

void rollDice(int* dice1, int* dice2) {

	*dice1 = (rand() % 6) + 1;
	*dice2 = (rand() % 6) + 1;
}

void printDice(int dice1, int dice2) {
	char buffer[50]; // Assuming a maximum of 50 characters per line is sufficient

	// Convert the dice values to strings
	sprintf(buffer, "Dice 1: %d\n", dice1);
	gotoxy(1, 14);
	cputs(buffer);
	gotoxy(1, 15);
	sprintf(buffer, "Dice 2: %d\n", dice2);
	cputs(buffer);
}

void displayBoard(char board[6][24]) {
	// go back to menu
	system("cls");


	for (int i = 0; i < 6; i++) {
		for (int j = 12; j < 24; j++) {
			putchar(board[i][j]);
		}
		putchar('\n');
	}

	for (int i = 5; i >= 0; i--) {
		for (int j = 11; j >= 0; j--) {
			putchar(board[i][j]);
		}
		putchar('\n');
	}

}

void updateBoard(char board[6][24], int newField, int feildCurrent, char player) {
	for (int i = 6; i >= 0; i--) {
		if (board[i][feildCurrent - 1] == player) {
			board[i][feildCurrent - 1] = ' ';
			break;
		}
	}

	for (int i = 0; i < 6; i++) {
		if (board[i][newField - 1] == ' ') {
			board[i][newField - 1] = player;
			break;
		}
	}
}

int firstMove() {
	system("cls");
	int playerW, playerB;
	do {
		rollDice(&playerW, &playerB);
		printf("White rolls: %d, Black rolls: %d\n", playerW, playerB);
		if (playerW > playerB) {
			printf("White starts the game.\n");
			return 1;
		}
		else if (playerW < playerB) {
			printf("Black starts the game.\n");
			return 2;
		}
		else {
			printf("Both players rolled the same number, rolling again...\n");
		}
	} while (playerW == playerB);
	return 0;
}

void saveGameState(char board[6][24], const char* filename) {
	FILE* outFile = fopen(filename, "w");
	if (!outFile) {
		fprintf(stderr, "Error opening file for writing.\n");
		return;
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 12; j < 24; j++) {
			fputc(board[i][j], outFile);
		}
		fputc('\n', outFile);
	}
	for (int i = 5; i >= 0; i--) {
		for (int j = 11; j >= 0; j--) {
			fputc(board[i][j], outFile);
		}
		fputc('\n', outFile);
	}
	fclose(outFile);
	cputs("Game saved");
}

void loadGame(char board[6][24], const char* filename) {
	FILE* inFile = fopen(filename, "r");
	if (!inFile) {
		fprintf(stderr, "Error opening file for reading.\n");
		return;
	}
	char c;
	for (int i = 0; i < 6; i++) {
		for (int j = 12; j < 24; j++) {
			c = fgetc(inFile);
			if (c == EOF) {
				fclose(inFile);
				return;
			} if (c != '\n') board[i][j] = c;
			else j--;
		} fgetc(inFile); // Skip the newline
	}
	for (int i = 5; i >= 0; i--) {
		for (int j = 11; j >= 0; j--) {
			c = fgetc(inFile);
			if (c == EOF) {
				fclose(inFile);
				return;
			} if (c != '\n') board[i][j] = c;
			else j++;
		} fgetc(inFile);
	} fclose(inFile);
}

void captureOppPawn(char board[6][24], int newField, int fieldCurrent, char player) {
	char opponent = (player == 'W') ? 'B' : 'W';
	if (board[0][newField - 1] == opponent) {
		board[0][newField - 1] = ' ';
	}
	updateBoard(board, newField, fieldCurrent, player);
}

int* checkOptions(char b[6][24], int field, int d1, int d2, char pl) {
	static int r[2];
	r[0] = 0;
	r[1] = 0;
	int nf = field - 1;
	if (pl == 'W') {
		if (b[0][nf] == 'W') {
			if ((b[0][nf - d1] == ' ') || (b[0][nf - d1] == 'W') && (d1 != 0)) r[0] = field - d1;
			if ((b[0][nf - d2] == ' ') || (b[0][nf - d2] == 'W') && (d2 != 0)) r[1] = field - d2;
		}
	}
	else if (pl == 'B') {
		if (b[0][nf] == 'B') {
			if ((b[0][nf + d1] == ' ') || (b[0][nf + d1] == 'B') && (d1 != 0)) r[0] = field + d1;
			if ((b[0][nf + d2] == ' ') || (b[0][nf + d2] == 'B') && (d2 != 0)) r[1] = field + d2;
		}
		else {
			cputs("Can't choose this field");
		}
	} return r;
}

void checkDice(int field, int ch, int* d1, int* d2) {
	if (ch == field - *d1 || ch == field + *d1) *d1 = 0;
	else if (ch == field - *d2 || ch == field + *d2) *d2 = 0;
}

void makeMove(char board[6][24], int curtPl, int* d1, int* d2) {
	int f, MoveExist = 0;
	char b[50];
	char Pl = (curtPl == 1) ? 'W' : 'B';
	int* p;
	do {
		sprintf(b, "P: %c\n", Pl);
		gotoxy(1, 17);
		cputs(b);

		cputs("Field: ");
		scanf("%d", &f);


		if (f == 0) {
			cputs("No move\n");
			return;
		}
		if (f <= 24 && f >= 1) {
			p = checkOptions(board, f, *d1, *d2, Pl);
			cputs("Result: ");
			for (int i = 0; i < 2; i++) {
				printf("%d ", *(p + i));
				if (*(p + i) != 0) MoveExist = 1;
			}
			if (!MoveExist) cputs("No moves. Choose again.\n");
		}
		else if (f == 777) saveGameState(board, "filename.txt");
		else cputs("Invalid field.\n");
	} while (!MoveExist);

	int ch;
	cputs("\nMove: ");
	scanf("%d", &ch);
	updateBoard(board, ch, f, Pl);
	checkDice(f, ch, d1, d2);
}

void startGame(char board[6][24], bool& continuePlaying) {
	int currentPlayer = firstMove();
	cputs("Number of Rounds? ");
	int rounds;
	rounds = getch() - '0';
	system("cls");
	while (rounds >= 0) {
		while (continuePlaying) {
			displayBoard(board);
			int dice1, dice2;
			rollDice(&dice1, &dice2);
			printDice(dice1, dice2);

			int nmMoves = 2;
			if (dice1 == dice2) nmMoves = 4;
			while (nmMoves > 0) {
				printDice(dice1, dice2);
				makeMove(board, currentPlayer, &dice1, &dice2);
				displayBoard(board);
				nmMoves--;
			}
			currentPlayer = (currentPlayer == 1) ? 2 : 1;
		}
		rounds--;
	}
}

void initializeBoard(char board[6][24]) {

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 24; j++) {
			board[i][j] = ' ';
		}
	}

	for (int i = 0; i < 2; i++) board[i][23] = 'W'; // 24th point
	for (int i = 0; i < 5; i++) board[i][12] = 'W'; // 13th point
	for (int i = 0; i < 3; i++) board[i][7] = 'W'; // 8th point
	for (int i = 0; i < 5; i++) board[i][5] = 'W'; // 6th point

	for (int i = 0; i < 2; i++) board[i][0] = 'B'; // 24th point (mirrored)
	for (int i = 0; i < 5; i++) board[i][11] = 'B'; // 13th point (mirrored)
	for (int i = 0; i < 3; i++) board[i][16] = 'B'; // 8th point (mirrored)
	for (int i = 0; i < 5; i++) board[i][18] = 'B'; // 6th point (mirrored)
}

void printAuthors() {
	textcolor(LIGHTGRAY);
	gotoxy(2, 1);
	cputs("Author: Fatkhullakh\n");
	cputs("Surname: Turakhonov\n");
	cputs("Index Number: 192066\n");

	gotoxy(2, 5);
	cputs("Menu:\n");
	cputs("1. Start Game\n");
	cputs("2. Load Game\n");
	cputs("3. Quit\n");
	cputs("Save Game - 777\n");
	cputs("Enter your choice: ");
}

int main() {
	srand(static_cast<unsigned int>(time(nullptr)));
	char board[6][24] = {};

	initializeBoard(board);

#ifndef __cplusplus
	Conio2_Init();
#endif

	printAuthors();

	int ch;
	bool contGame = true;
	while (contGame) {
		ch = getch() - '0';
		switch (ch) {
		case 1:
			startGame(board, contGame);
			break;
		case 2:
			loadGame(board, "filename.txt");
			startGame(board, contGame);
			break;
		case 3:
			contGame = false;
			break;
		default:
			gotoxy(2, 12);
			cputs("Invalid choice.");
			break;
		}
	}

	return 0;
}