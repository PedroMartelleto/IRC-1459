#pragma once

/**
 * @brief The server forms the backbone of IRC, providing a point to which
   clients may connect to to talk to each other, and a point for other
   servers to connect to, forming an IRC network.  The only network
   configuration allowed for IRC servers is that of a spanning tree [see
   Fig. 1] where each server acts as a central node for the rest of the
   net it sees.


                           [ Server 15 ]  [ Server 13 ] [ Server 14]
                                 /                \         /
                                /                  \       /
        [ Server 11 ] ------ [ Server 1 ]       [ Server 12]
                              /        \          /
                             /          \        /
                  [ Server 2 ]          [ Server 3 ]
                    /       \                      \
                   /         \                      \
           [ Server 4 ]    [ Server 5 ]         [ Server 6 ]
            /    |    \                           /
           /     |     \                         /
          /      |      \____                   /
         /       |           \                 /
 [ Server 7 ] [ Server 8 ] [ Server 9 ]   [ Server 10 ]

                                  :
                               [ etc. ]
                                  :
 * 
 */
class Server {


};