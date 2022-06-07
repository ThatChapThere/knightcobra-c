#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "move_bitboards.h"
#include "legal_moves.h"

int main()
{
    time_t start_time = clock();

    /* rules of chess - left 1d for performance */
    moveset_type *legal_moves = malloc(
        sizeof(moveset_type) *
        NUMBER_OF_CHESSMEN *
        NUMBER_OF_SQUARES
    );
    
    generate_bitboards(legal_moves);
    
    /* loop through squares */
    for(int i = 0; i < 64; i++)
    {
        /* board to be displayed */
        char show_legal_moves[64];

        /* fill with spaces */
        for(int j = 0; j < 64; j++)
            show_legal_moves[j] = '_';
        
        /* set start square */
        show_legal_moves[i] = 'X';
        
        /* find all queen moves from this square */
        moveset_type moves = legal_moves[
            white_pawn * NUMBER_OF_SQUARES +
            i
        ];
        
        /* loop through queen moves from square i */
        for(int j = 0; j < moves.move_count; j++)
        {
            move_type currentmove = moves.moves[j];
            /* loop through effects of move j */
            for(int k = 0; k < currentmove.effect_count; k++)
            {
                /* ignore pieces being added */
                if(!currentmove.effects[k].fill) continue;

                /* this is okay since bitboard_type is a typedef of an integer - which is copied by assignment */
                bitboard_type effect = currentmove.effects[k].squares;

                int most_sig_bit = 63;
                /* eg. for h1: effect = 1, the while loop does nothing, most_sig_bit is correctly 63 */
                while(effect >>= 1) most_sig_bit--;
                show_legal_moves[most_sig_bit] = 'o';
            }
        }

        /* display moveset from square i */
        for(int j = 0; j < 64; j++)
        {
            if(j % 8){}else printf("\n");
            printf("%c", show_legal_moves[j]);
        }

        printf("\n");
    }
    
    free(legal_moves);

    time_t end_time = clock();

    printf("%d ms\n", (int) difftime(end_time, start_time));

    getchar();
    return 0;
}