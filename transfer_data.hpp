#ifndef TRANSFER_DATA_H_INCLUDED
#define TRANSFER_DATA_H_INCLUDED

struct transfer_data_client{
    bool guessed;
    int from;
    int to;
    int tries;
};

struct transfer_data_server{
    int guess;
};


#endif