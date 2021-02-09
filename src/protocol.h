#ifndef PROTOCOL_H___
#define PROTOCOL_H___

#include "link.h"


template <typename F>
void verifier(F, Link&);



template <typename FInput, typename FProve, typename FCheck>
void prover(FInput, FProve, FCheck, Link&);


#include "protocol.hh"


#endif
