/*
 * Version: 1.3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

// Global constants
#define MAX_NUM_CHARACTER 200
#define MAX_NAME_LENGTH 100

// Global variables
char* g_characters_list[MAX_NUM_CHARACTER];
char* g_player_list[10];
char g_input[10];
char* g_end_pointer;
int g_number_of_characters = 0;
long g_number_players;

/**
 * Read File based on the chosen game passed.
 *
 * @param chosen_game chosen game, 1=League of Legends or 2=Valorant.
 */
void read_file(const long chosen_game) {
	// Reset global variables
	for (int character = 0; character < g_number_of_characters; ++character) {
		free(g_characters_list[character]);
	}
	g_number_of_characters = 0;

	// Selecting file
	char* file_name;
	if (chosen_game == 1)
		file_name = "champions.txt";
	else
		file_name = "agents.txt";

	// Open selected file
	FILE* characters_file = fopen(file_name, "r");
	if (!characters_file) {
		printf("\033[0;31mError opening file %s\033[0m\nFile can be downloaded from the github repository\n",
		       file_name);
		printf("<press enter to quit>");
		fgets(g_input, sizeof(g_input), stdin);
		exit(0);
	}

	// Writes file to list
	char line[MAX_NAME_LENGTH];
	while (fgets(line, sizeof(line), characters_file) != NULL) {
		// Remove newline character from each line
		line[strcspn(line, "\n")] = '\0';

		// Split line into first name and second name
		char* first_name = strtok(line, " ");
		char* second_name = strtok(NULL, " ");

		// Allocating memory for the name and copy it into the list
		char* character_name;
		if (second_name != NULL) {
			character_name = malloc(strlen(first_name) + strlen(second_name) + 2);
			sprintf(character_name, "%s %s", first_name, second_name);
		} else {
			character_name = malloc(strlen(first_name) + 1);
			sprintf(character_name, "%s", first_name);
		}

		// Adding character name to list
		g_characters_list[g_number_of_characters] = character_name;
		++g_number_of_characters;

		// Check if list size was reached
		if (g_number_of_characters >= MAX_NUM_CHARACTER) {
			printf("Maximum number of champions reached\n");
			break;
		}
	}
	// Close file
	fclose(characters_file);
}

/**
 * Prompts user to choose game
 * 1 = League of Legends
 * 2 = Valorant
 */
void choose_game() {
	while (1) {
		// Prompt user to select a game
		printf(
			"\nChoose a game\n"
			"1. League of Legends\n"
			"2. Valorant\n>> "
		);
		fgets(g_input, sizeof(g_input), stdin);
		const long game_choice = strtol(g_input, &g_end_pointer, 10);

		if (game_choice == 1 || game_choice == 2) {
			read_file(game_choice);
			break;
		}

		printf("\033[0;31m\nInvalid input\n\033[0m");
	}
}

/**
 * Split players into two teams based on the list sequence
 */
void split_teams() {
	// Get max number of players per team
	const long players_per_team = (g_number_players + 1) / 2;

	// Split teams between the first half and second half of the list
	printf("\n---Team 1---\n");
	for (int curr_player = 0; curr_player < players_per_team; ++curr_player) {
		printf("%s\n", g_player_list[curr_player]);
	}
	printf("\n---Team 2---\n");
	for (long curr_player = players_per_team; curr_player < g_number_players; ++curr_player) {
		printf("%s\n", g_player_list[curr_player]);
	}

	printf("\n<press enter to continue>\n");
	fgets(g_input, sizeof(g_input), stdin);
}

/**
 * Randomly shuffle players list
 */
void shuffle_teams() {
	// Set rand seed
	srand(clock());

	// Go through players
	for (long current_index = g_number_players - 1; current_index > 0; --current_index) {
		// Randomly pick a player index on list
		const long random_index = rand() % (current_index + 1);

		// Swap players
		char* tempPlayer = g_player_list[current_index];
		g_player_list[current_index] = g_player_list[random_index];
		g_player_list[random_index] = tempPlayer;
	}

	split_teams();
}

/**
 * Prompt user to enter number of players (1-10)
 */
void get_num_players() {
	// Getting number of players
	while (1) {
		// Getting user input
		printf("\nEnter number of players (1-10) >> ");
		fgets(g_input, sizeof(g_input), stdin);
		g_number_players = strtol(g_input, &g_end_pointer, 10);

		// Check if input is valid
		if (errno == ERANGE || *g_end_pointer != '\n' || g_number_players < 1 || g_number_players > 10) {
			printf("\033[0;31m\nInvalid input\n\033[0m");
		} else
			break;
	}
}

/**
 * Prompt user to enter players nicknames
 */
void get_players_nicknames() {
	printf("\n");
	// Getting players nicknames
	for (int player = 0; player < g_number_players; ++player) {
		char player_name[MAX_NAME_LENGTH];
		printf("Enter player %d nickname >> ", player + 1);
		scanf("%s", player_name);
		fgets(g_input, sizeof(g_input), stdin);

		// Allocating memory
		char* player_nickname = malloc(strlen(player_name) + 1);

		// Copying player nickname to list
		strcpy(player_nickname, player_name);
		g_player_list[player] = player_nickname;
	}

	// Prompt user to randomly separate teams or not
	printf(
		"\n1. Randomly split teams\n"
		"0. Continue\n>> "
	);
	fgets(g_input, sizeof(g_input), stdin);
	const long user_input = strtol(g_input, &g_end_pointer, 10);

	if (user_input == 1) {
		if (g_number_players % 2 == 0) {
			shuffle_teams();
		} else {
			printf("Teams will not be even\n");
			printf("\n<press enter to continue>\n");
			fgets(g_input, sizeof(g_input), stdin);
			shuffle_teams();
		}
	}
}

/**
 * Randomly select a characters from the character list and assign to players
 */
void choosing_characters() {
	// Set rand seed
	srand(clock());

	// Randomly choosing a champion from list and assigning it to player
	printf("\n");
	for (int player = 0; player < g_number_players; ++player) {
		const int selected_character = rand() % g_number_of_characters;
		printf("%s >> %s\n", g_player_list[player], g_characters_list[selected_character]);
	}

	printf("\n<press enter to continue>\n");
	fgets(g_input, sizeof(g_input), stdin);
}

int main() {
	// Prompt user to choose a game
	choose_game();

	// Prompt user to enter players information
	get_num_players();
	get_players_nicknames();

	// Randomly choosing character
	choosing_characters();

	// Main loop
	while (1) {
		// Interactive menu
		printf(
			"1. Re-roll\n"
			"2. Shuffle Teams\n"
			"3. Change number of players\n"
			"4. Change players nickname\n"
			"5. Change game\n"
			"0. Quit\n>> "
		);
		fgets(g_input, sizeof(g_input), stdin);
		const long user_input = strtol(g_input, &g_end_pointer, 10);

		if (errno == ERANGE || *g_end_pointer != '\n' || user_input < 0 || user_input > 5) {
			printf("\033[0;31m\nInvalid input\n\033[0m");
		} else {
			switch (user_input) {
			case 1:
				choosing_characters();
				break;
			case 2:
				shuffle_teams();
				break;
			case 3:
				get_num_players();
				get_players_nicknames();
				break;
			case 4:
				get_players_nicknames();
				break;
			case 5:
				choose_game();
				break;
			default:
				// Free allocated memory
				for (int character = 0; character < g_number_of_characters; ++character) {
					free(g_characters_list[character]);
				}
				for (int player = 0; player < g_number_players; ++player) {
					free(g_player_list[player]);
				}
				return 0;
			}
		}
	}
}
