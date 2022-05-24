# Chess Program Specification

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
  * be filled at specific bits: `bitboard & condition != 0 &&` same for all others
  * be empty at specific bits: `bitboard | condition == 0 &&` same for all others
* moves also have a set of effects where some bitboards will:
  * be filled with a specific piece: `bitboard |= positions`
  * be emptied of a specific piece: `bitboard &= ~positions`

## Important Procedures
```C
void generate_bitboards(moves * legal_moves);
```
Generates the bitboards at the beginning of the program, at a specified memory location