// Author: Daniels Pikurs
// Student Number: R00166279

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE_MAX 64
#define BOARD_SIZE_MIN 32
#define DICE_MAX 6
#define DICE_MIN 1
#define ARGUMENT_ERROR "You did not provide any values for snakes/ladders... 'int snakes' then 'int ladders' "

int board_size;
int ladder_count;
int snake_count;

typedef struct Square
{
    int square_number;
    struct Square *next;
    struct Square *transport;
} square;

typedef struct Ladder
{
    int top, bottom;
} ladder;

typedef struct Snake
{
    int head, tail;
} snake;

ladder *ladders;
snake *snakes;
square *player_square;
square *board_squares;
FILE *fptr;

int random_number(int min, int max)
{
    int x = rand() % ((max + 1) - min) + min;
    return x;
}

int roll_dice()
{
    return random_number(DICE_MIN, DICE_MAX);
}

char has_reached_last_square()
{
    if (player_square->next == NULL)
    {
        return 1;
    }
    return 0;
}

void perform_move(int rolled)
{
    for (int i = 0; i < rolled; i++)
    {
        if (has_reached_last_square())
        {
            return;
        }
        player_square = player_square->next;
    }
    if (has_reached_last_square())
    {
        return;
    }

    printf("\nMoved to square %i", player_square->square_number);
    fprintf(fptr, "\nMoved to square %i", player_square->square_number);
}

void player_turn()
{
    int rolled = roll_dice();
    printf("\nRolled %i", rolled);
    fprintf(fptr, "\nRolled %i", rolled);
    perform_move(rolled);

    if (has_reached_last_square())
    {
        return;
    }

    if (player_square->transport != NULL)
    {
        square *transport_destination = player_square->transport;

        if (transport_destination->square_number > player_square->square_number)
        {
            printf("\nStepped on a ladder moved to %i", transport_destination->square_number);
            fprintf(fptr, "\nStepped on a ladder moved to %i", transport_destination->square_number);
        }
        else
        {
            printf("\nStepped on a snake moved to %i", transport_destination->square_number);
            fprintf(fptr, "\nStepped on a snake moved to %i", transport_destination->square_number);
        }

        player_square = player_square->transport;
    }
}

int check_array_contains(int *array, int size, int value)
{
    for (int i = 0; i < size; i++)
    {
        if (array[i] == value)
        {
            return 1;
        }
    }
    return 0;
}

void generate_random_ladders(int *bottoms, int *tops)
{
    for (int i = 0; i < ladder_count; i++)
    {

        int ladder_bottom_square_index, random_length;
        // Temporary for While Loop
        int ladder_top_square_index = board_size + 1;

        // Top can't be outside Board
        while (ladder_top_square_index > board_size) 
        {   
            // Using 1 so that there isn't a ladder on the first square
            ladder_bottom_square_index = random_number(1, board_size); 

            // Check bottom not in bottom
            while (check_array_contains(bottoms, i, ladder_bottom_square_index))
            {
                ladder_bottom_square_index = random_number(1, board_size);
            }

            // Adds bottom to array
            bottoms[i] = ladder_bottom_square_index;

            random_length = random_number(1, 10);
            ladder_top_square_index = ladder_bottom_square_index + random_length;

            // Check top not in top
            while (check_array_contains(tops, i, ladder_top_square_index))
            {
                random_length = random_number(1, 10);
                ladder_top_square_index = ladder_bottom_square_index + random_length;
            }

            tops[i] = ladder_top_square_index;
        }

        ladders[i].bottom = ladder_bottom_square_index;
        ladders[i].top = ladder_top_square_index;
        printf("\nLadder bottom at %i and Ladder top at %i", ladder_bottom_square_index, ladder_top_square_index);
        fprintf(fptr, "\nLadder bottom at %i and Ladder top at %i", ladder_bottom_square_index, ladder_top_square_index);
    }
}

void generate_random_snakes(int *heads, int *tails, int *bottoms, int *tops)
{
    for (int i = 0; i < snake_count; i++)
    {

        int snake_tail_square_index, random_length;
        // Temporary for While Loop
        int snake_head_square_index = board_size + 1;

        while (snake_head_square_index > board_size)
        {
            snake_tail_square_index = random_number(1, board_size);

            // Check tail not in tail, bottom or top
            while (check_array_contains(tails, i, snake_tail_square_index) || check_array_contains(bottoms, ladder_count, snake_tail_square_index) || check_array_contains(tops, ladder_count, snake_tail_square_index))
            {
                snake_tail_square_index = random_number(1, board_size);
            }

            tails[i] = snake_tail_square_index; // Adds tail to array

            random_length = random_number(1, 10);
            snake_head_square_index = snake_tail_square_index + random_length;

            // Check head not in head, tail, top or bottom
            while (check_array_contains(tails, i, snake_head_square_index) || check_array_contains(heads, i, snake_head_square_index) || check_array_contains(bottoms, ladder_count, snake_head_square_index) || check_array_contains(tops, ladder_count, snake_head_square_index))
            {
                random_length = random_number(1, 10);
                snake_head_square_index = snake_tail_square_index + random_length;
            }
            // Adds head to array
            heads[i] = snake_head_square_index; 
        }

        snakes[i].head = snake_head_square_index;
        snakes[i].tail = snake_tail_square_index;
        printf("\nSnake head at %i and Snake tail at %i", snake_head_square_index, snake_tail_square_index);
        fprintf(fptr, "\nSnake head at %i and Snake tail at %i", snake_head_square_index, snake_tail_square_index);
    }
}

square *find_square_number(int number)
{
    for (int i = 0; i < board_size; i++)
    {
        if (board_squares[i].square_number == number)
        {
            return &board_squares[i];
        }
    }
    return NULL;
}

//Checks all snake/ladder ends and sets them to square transport
void set_square_transports()
{   
    //Sets all transports to NULL
    for (int i = 0; i < board_size; i++)
    {
        board_squares[i].transport = NULL;
    }

    for (int i = 0; i < ladder_count; i++)
    {
        int top = ladders[i].top;
        int bottom = ladders[i].bottom;

        square *top_square = find_square_number(top);
        square *bottom_square = find_square_number(bottom);

        bottom_square->transport = top_square;
    }

    for (int i = 0; i < snake_count; i++)
    {
        int tail = snakes[i].tail;
        int head = snakes[i].head;

        square *tail_square = find_square_number(tail);
        square *head_square = find_square_number(head);

        head_square->transport = tail_square;
    }
}

void initialize_snakes_ladders()
{
    ladders = malloc(ladder_count * sizeof(ladder));
    snakes = malloc(snake_count * sizeof(snake));
    int *ladder_bottoms = malloc(ladder_count * sizeof(int));
    int *ladder_tops = malloc(ladder_count * sizeof(int));

    int *snake_heads = malloc(snake_count * sizeof(int));
    int *snake_tails = malloc(snake_count * sizeof(int));

    generate_random_ladders(ladder_bottoms, ladder_tops);
    generate_random_snakes(snake_heads, snake_tails, ladder_bottoms, ladder_tops);

    set_square_transports();
}

void initialize_board()
{
    board_size = random_number(BOARD_SIZE_MIN, BOARD_SIZE_MAX);

    printf("BOARD SIZE %i", board_size);
    fprintf(fptr, "BOARD SIZE %i", board_size);

    board_squares = malloc(board_size * sizeof(square));

    int current_snake_count = 0;
    int current_ladder_count = 0;

    for (int i = 0; i < board_size; i++)
    {
        board_squares[i].square_number = i + 1;
        board_squares[i].next = &board_squares[i + 1];
    }
    board_squares[board_size - 1].next = NULL;
    player_square = &board_squares[0];

    initialize_snakes_ladders();
}

int main(int argc, char *argv[])
{
    fptr = fopen("snakes_ladders.txt", "a+");
    srand(time(0));
    if (argc < 2)
    {
        printf(ARGUMENT_ERROR);
        return 1;
    }

    snake_count = atoi(argv[1]);
    ladder_count = atoi(argv[2]);

    // snake_count = 4;
    // ladder_count = 5;

    initialize_board();

    while (player_square->next != NULL)
    {
        player_turn();
    }

    printf("\nLanded on the last square %i", player_square->square_number);
    fprintf(fptr, "\nLanded on the last square %i", player_square->square_number);

    printf("\nGame Over!");
    fprintf(fptr, "\nGame Over!\n---------------\n");
    fclose(fptr);
    return 0;
}
