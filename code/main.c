#include <stdio.h>
#include <stdlib.h>
#include "move_bitboards.h"

int main()
{
    moves * legal_moves = malloc(
        sizeof(moves) *
        NUMBER_OF_CHESSMEN *
        NUMBER_OF_SQUARES
    );

    generate_bitboards(legal_moves);

    printf(
        "%d\n",
        legal_moves[
            NUMBER_OF_SQUARES * white_rook +
            0
        ].move_count
    );
    
    free(legal_moves);
    return 0;
}