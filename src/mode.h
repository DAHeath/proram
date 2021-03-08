#ifndef PRORAM_MODE_H__
#define PRORAM_MODE_H__


/**
 * In the protocol, the parties P and V together play four roles:
 *
 * First, P plays the role of the `Input' party: here, she evaluates the
 * arithmetic circuit in cleartext and precomputes all wire values.
 * She uses these precomputed choices to select her OT inputs.
 *
 * Second, V plays the role of the `Verify' party. Here, he uses his
 * pseudorandom seed to generate his shares and his OT inputs.
 * He also accumulates a digest of all of the expected opened shares in the
 * circuit.
 *
 * Now, the parties perform OT.
 *
 * Third, P plays the role of the `Prove' party. Here, she uses her OT output
 * to compute a share on each wire.
 * She also accumulates a digest of all of the opened shares.
 *
 * Now, P commits to her digest.
 * Subsequently, V reveals his seed to V.
 *
 * Fourth, P plays the role of the `Check' party. Here, she uses V's seed to
 * reconstruct all messages that came from V. If there is any deviation, then
 * she aborts, since V clearly cheated.
 *
 * Now, P opens her commitment.
 * V checks that P's opened digest matches the digest he personally constructed.
 * If not, he aborts.
 * V accepts.
 *
 *
 * Crucially, in all four roles, the parties perform the same high level
 * circuit traversal; only the low-level handling of each gate differs from
 * role to role. Therefore, our general strategy is to structure procedures
 * that represent circuits as *templates*, where the template argument is the
 * role. This way, each circuit procedure shares code between the four
 * different "modes".
 */


enum class Mode {
  Input,
  Prove,
  Verify,
  Check,
};


#endif
