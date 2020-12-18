OPENQASM 2.0;

include "test.inc";

creg c[10];

gate u3(theta,phi,lambda) q { U(theta,phi,lambda) q; }
