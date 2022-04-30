# Chess program specification

## Things the program must do:
* store positions
* generate legal moves from a position
* generate bitboards for legal moves for each piece
* some sort of bitboard based implementation of en passant and castling
* evaluate positions
* store entire move tree somehow
* convert moves into algebreic notation

## Functions:
```C
int* generate_legal_moves(int* pos);
float evaluate_position(int* pos);
```

## Structs:

## Enums:
```c
enum bitboards {
    white_pawn,
    white_rook,
    white_knight,
    ...
    en_passant?
    white_castle_kingside,
    ...
};
```