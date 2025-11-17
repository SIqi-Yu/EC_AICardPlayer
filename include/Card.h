// include/Card.h
#pragma once

// Basic card representation: rank 2..14, suit 0..3
struct Card {
    int rank; // 2..14 (11=J,12=Q,13=K,14=A)
    int suit; // 0..3  (e.g., 0=Clubs,1=Diamonds,2=Hearts,3=Spades)
};
