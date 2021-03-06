//---------------------------------------------------------
// Copyright 2015 Ontario Institute for Cancer Research
// Written by Jared Simpson (jared.simpson@oicr.on.ca)
//---------------------------------------------------------
//
// nanopolish_profile_hmm -- Profile Hidden Markov Model
//
#ifndef NANOPOLISH_PROFILE_HMM_H
#define NANOPOLISH_PROFILE_HMM_H

#include <stdint.h>
#include <vector>
#include <string>
#include "nanopolish_matrix.h"
#include "nanopolish_common.h"
#include "nanopolish_emissions.h"
#include "nanopolish_hmm_input_sequence.h"

//#define HMM_REVERSE_FIX 1
//#define DEBUG_FILL 1

//
// High level algorithms
//

// Calculate the probability of the nanopore events given a sequence
float profile_hmm_score(const HMMInputSequence& sequence, const HMMInputData& data, const uint32_t flags = 0);
float profile_hmm_score(const HMMInputSequence& sequence, const std::vector<HMMInputData>& data, const uint32_t flags = 0);

// Run viterbi to align events to kmers
std::vector<HMMAlignmentState> profile_hmm_align(const HMMInputSequence& sequence, const HMMInputData& data, const uint32_t flags = 0);

// Print the alignment between the read-strand and a sequence
void print_alignment(const std::string& name,
                     uint32_t seq_id,
                     uint32_t read_id,
                     const HMMInputSequence& consensus, 
                     const HMMInputData& data,
                     const std::vector<HMMAlignmentState>& alignment);

//
// Forward algorithm
//

// Initialize the forward algorithm
void profile_hmm_forward_initialize(FloatMatrix& fm);

// Terminate the forward algorithm
float profile_hmm_forward_terminate(const FloatMatrix& fm, uint32_t row);

//
// Viterbi
// 

// initialize viterbi
void profile_hmm_viterbi_initialize(FloatMatrix& m);

//
// Training
//
void profile_hmm_update_training(const HMMInputSequence& sequence, 
                                 const HMMInputData& data);


// Convenience enum for keeping track of the states in the profile HMM
enum ProfileState
{
    PS_KMER_SKIP = 0,
    PS_BAD_EVENT,
    PS_MATCH,
    PS_NUM_STATES = 3,
    PS_PRE_SOFT // intentionally after PS_NUM_STATES
};

enum HMMMovementType
{
    HMT_FROM_SAME_M = 0,
    HMT_FROM_PREV_M,
    HMT_FROM_SAME_B,
    HMT_FROM_PREV_B,
    HMT_FROM_PREV_K,
    HMT_FROM_SOFT,
    HMT_NUM_MOVEMENT_TYPES
};
typedef struct { float x[HMT_NUM_MOVEMENT_TYPES]; } HMMUpdateScores;

// Flags to modify the behaviour of the HMM
enum HMMAlignmentFlags
{
    HAF_ALLOW_PRE_CLIP = 1, // allow events to go unmatched before the aligning region
    HAF_ALLOW_POST_CLIP = 2 // allow events to go unmatched after the aligning region
};

// Convert an enumerated state into a symbol
inline char ps2char(ProfileState ps) { return "KBMNS"[ps]; }

// Pre-computed transitions from the previous block
// into the current block of states. Log-scaled.
struct BlockTransitions
{
    // Transition from m state (match event to k-mer)
    float lp_mm_self;
    float lp_mb;
    float lp_mk;
    float lp_mm_next;

    // Transitions from b state (bad event that should be ignored)
    float lp_bb;
    float lp_bk;
    float lp_bm_next; // movement to next k-mer
    float lp_bm_self; // movement to k-mer that we came from

    // Transitions from k state (no observation from k-mer)
    float lp_kk;
    float lp_km;
};

//
#include "nanopolish_profile_hmm.inl"

#endif
