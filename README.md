# Chess Program Specification

## Todo Currently
* get rid of castling blockers - this is meaningless since the rules for what squares must be empty and what square must not be controlled are different
* generate the control bitboards in legal_moves.c

## Things the Program Must Do
* store positions
* generate legal moves from a position
* generate bitboards for legal moves for each piece
* some sort of bitboard based implementation of en passant and castling
* evaluate positions
* store entire move tree somehow
* convert moves into algebreic notation

## Bitboard Design
* the most significant bit of the bitboard is a8, and the second most significan bit is b8
* for efficient accessing of moves, bitboards are stored per piece per square
* moves have a set of conditions where some bitboards either must:
  * be filled at specific bits: `bitboard & condition == condition &&` same for all others
  * be empty at specific bits: `bitboard & condition == 0 &&` same for all others
* moves also have a set of effects where some bitboards will:
  * be filled with a specific piece: `bitboard |= positions`
  * be emptied of a specific piece: `bitboard &= ~positions`

## Movetree Processing
* for simplicity the first version of the program will use overlong arrays and avoid dynamic memory allocation
* because nodes have children of the same type, pointer arrays will be needed
* these fill with null pointers that don't need memory until we actually add moves
* positions need to have approximately the following structure:
```C
typedef struct {
    /* actual position data */
    int legal_move_count

    char *algebaic_moves[MAX_LEGAL_MOVES];
    position *children[MAX_LEGAL_MOVES];
} position;
```

## Important Procedures
```C
void generate_bitboards(moveset *legal_moves);
```
Generates the bitboards at the beginning of the program, at a specified memory location
