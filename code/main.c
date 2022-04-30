#include <stdio.h>
#include "main.h"
#include "move_bitboards.h"

int main()
{
    struct moves legal_moves[NUMBER_OF_CHESSMEN];
    generate_bitboards(&legal_moves);
    printf("%d", legal_moves[white_rook].move_count);

    return 0;
}