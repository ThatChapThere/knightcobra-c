# Chess program specification

## Things the program must do:
* the most significant bit of the bitboard is a8, and the second most significan bit is b8
* store positions
* generate legal moves from a position
* generate bitboards for legal moves for each piece
* some sort of bitboard based implementation of en passant and castling
* evaluate positions
* store entire move tree somehow
* convert moves into algebreic notation

## Bitboard design
* for efficient accessing of moves, bitboards are stored per piece per square
* moves have a set of conditions where some bitboards either must:
  * be filled at specific bits: `bitboard & condition != 0 &&` same for all others
  * be empty at specific bits: `bitboard | condition == 0 &&` same for all others
* moves also have a set of effects where some bitboards will:
  * be filled with a specific piece: `bitboard |= positions`
  * be emptied of a specific piece: `bitboard &= ~positions`


## Functions:
```C
void generate_bitboards(struct moves * legal_moves);
```

## Structs:
```C
move_condition
move_effect
move
moves
```

## Enums:
```C
piece_type {white_pawn ... black_control}
```