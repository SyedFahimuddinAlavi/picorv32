#include "indcpa.h"
#include "kem.h"
#include "params.h"
#include "randombytes.h"
#include "symmetric.h"
#include "verify.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

//#define DISABLE_BENCH_MARKING_L1

#ifndef DISABLE_BENCH_MARKING_L1
#define time(cycles)\
{\
	__asm__ volatile ("rdcycle %0" : "=r"(cycles));\
}
#endif // DISABLE_BENCH_MARKING_L1

/*************************************************
* Name:        PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair_derand
*
* Description: Generates public and private key
*              for CCA-secure Kyber key encapsulation mechanism
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
*                (an already allocated array of KYBER_SECRETKEYBYTES bytes)
*              - uint8_t *coins: pointer to input randomness
*                (an already allocated array filled with 2*KYBER_SYMBYTES random bytes)
**
* Returns 0 (success)
**************************************************/
int PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair_derand(uint8_t *pk,
        uint8_t *sk,
        const uint8_t *coins) {
    PQCLEAN_KYBER1024_CLEAN_indcpa_keypair_derand(pk, sk, coins);
    memcpy(sk + KYBER_INDCPA_SECRETKEYBYTES, pk, KYBER_PUBLICKEYBYTES);
    hash_h(sk + KYBER_SECRETKEYBYTES - 2 * KYBER_SYMBYTES, pk, KYBER_PUBLICKEYBYTES);
    /* Value z for pseudo-random output on reject */
    memcpy(sk + KYBER_SECRETKEYBYTES - KYBER_SYMBYTES, coins + KYBER_SYMBYTES, KYBER_SYMBYTES);
    return 0;
}

/*************************************************
* Name:        PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair
*
* Description: Generates public and private key
*              for CCA-secure Kyber key encapsulation mechanism
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
*                (an already allocated array of KYBER_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair(uint8_t *pk,
        uint8_t *sk) {
    uint8_t coins[2 * KYBER_SYMBYTES];
    randombytes(coins, 2 * KYBER_SYMBYTES);
    PQCLEAN_KYBER1024_CLEAN_crypto_kem_keypair_derand(pk, sk, coins);
    return 0;
}

/*************************************************
* Name:        PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc_derand
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - uint8_t *ct: pointer to output cipher text
*                (an already allocated array of KYBER_CIPHERTEXTBYTES bytes)
*              - uint8_t *ss: pointer to output shared secret
*                (an already allocated array of KYBER_SSBYTES bytes)
*              - const uint8_t *pk: pointer to input public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*              - const uint8_t *coins: pointer to input randomness
*                (an already allocated array filled with KYBER_SYMBYTES random bytes)
**
* Returns 0 (success)
**************************************************/
int PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc_derand(uint8_t *ct,
        uint8_t *ss,
        const uint8_t *pk,
        const uint8_t *coins) {
    uint8_t buf[2 * KYBER_SYMBYTES];
    /* Will contain key, coins */
    uint8_t kr[2 * KYBER_SYMBYTES];
 #ifndef DISABLE_BENCH_MARKING_L1
    long            Begin_Time=0,
                End_Time=0;
 #endif // DISABLE_BENCH_MARKING_L1

    memcpy(buf, coins, KYBER_SYMBYTES);

    /* Multitarget countermeasure for coins + contributory KEM */
 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    hash_h(buf + KYBER_SYMBYTES, pk, KYBER_PUBLICKEYBYTES);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "hash_h cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    hash_g(kr, buf, 2 * KYBER_SYMBYTES);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "hash_g cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    /* coins are in kr+KYBER_SYMBYTES */
    PQCLEAN_KYBER1024_CLEAN_indcpa_enc(ct, buf, pk, kr + KYBER_SYMBYTES);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "PQCLEAN_KYBER1024_CLEAN_indcpa_enc cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1
    memcpy(ss, kr, KYBER_SYMBYTES);
    return 0;
}

/*************************************************
* Name:        PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc
*
* Description: Generates cipher text and shared
*              secret for given public key
*
* Arguments:   - uint8_t *ct: pointer to output cipher text
*                (an already allocated array of KYBER_CIPHERTEXTBYTES bytes)
*              - uint8_t *ss: pointer to output shared secret
*                (an already allocated array of KYBER_SSBYTES bytes)
*              - const uint8_t *pk: pointer to input public key
*                (an already allocated array of KYBER_PUBLICKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc(uint8_t *ct,
        uint8_t *ss,
        const uint8_t *pk) {
    uint8_t coins[KYBER_SYMBYTES];
    randombytes(coins, KYBER_SYMBYTES);
    PQCLEAN_KYBER1024_CLEAN_crypto_kem_enc_derand(ct, ss, pk, coins);
    return 0;
}

/*************************************************
* Name:        PQCLEAN_KYBER1024_CLEAN_crypto_kem_dec
*
* Description: Generates shared secret for given
*              cipher text and private key
*
* Arguments:   - uint8_t *ss: pointer to output shared secret
*                (an already allocated array of KYBER_SSBYTES bytes)
*              - const uint8_t *ct: pointer to input cipher text
*                (an already allocated array of KYBER_CIPHERTEXTBYTES bytes)
*              - const uint8_t *sk: pointer to input private key
*                (an already allocated array of KYBER_SECRETKEYBYTES bytes)
*
* Returns 0.
*
* On failure, ss will contain a pseudo-random value.
**************************************************/
int PQCLEAN_KYBER1024_CLEAN_crypto_kem_dec(uint8_t *ss,
        const uint8_t *ct,
        const uint8_t *sk) {
    int fail;
    uint8_t buf[2 * KYBER_SYMBYTES];
    /* Will contain key, coins */
    uint8_t kr[2 * KYBER_SYMBYTES];
    uint8_t cmp[KYBER_CIPHERTEXTBYTES + KYBER_SYMBYTES];
    const uint8_t *pk = sk + KYBER_INDCPA_SECRETKEYBYTES;
 #ifndef DISABLE_BENCH_MARKING_L1
    long            Begin_Time=0,
                End_Time=0;
 #endif // DISABLE_BENCH_MARKING_L1
 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    PQCLEAN_KYBER1024_CLEAN_indcpa_dec(buf, ct, sk);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "PQCLEAN_KYBER1024_CLEAN_indcpa_dec cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    /* Multitarget countermeasure for coins + contributory KEM */
    memcpy(buf + KYBER_SYMBYTES, sk + KYBER_SECRETKEYBYTES - 2 * KYBER_SYMBYTES, KYBER_SYMBYTES);
    hash_g(kr, buf, 2 * KYBER_SYMBYTES);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "memcpy and hash_g cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    /* coins are in kr+KYBER_SYMBYTES */
    PQCLEAN_KYBER1024_CLEAN_indcpa_enc(cmp, buf, pk, kr + KYBER_SYMBYTES);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "PQCLEAN_KYBER1024_CLEAN_indcpa_enc cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    fail = PQCLEAN_KYBER1024_CLEAN_verify(ct, cmp, KYBER_CIPHERTEXTBYTES);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "PQCLEAN_KYBER1024_CLEAN_verify cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    /* Compute rejection key */
    rkprf(ss, sk + KYBER_SECRETKEYBYTES - KYBER_SYMBYTES, ct);
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "rkprf cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1

 #ifndef DISABLE_BENCH_MARKING_L1
    time (Begin_Time);
 #endif // DISABLE_BENCH_MARKING_L1
    /* Copy true key to return buffer if fail is false */
    PQCLEAN_KYBER1024_CLEAN_cmov(ss, kr, KYBER_SYMBYTES, (uint8_t) (1 - fail));
#ifndef DISABLE_BENCH_MARKING_L1
    time (End_Time);
    fprintf(stdout, "PQCLEAN_KYBER1024_CLEAN_cmov cycles = %ld, begin:%ld, end:%ld\n", End_Time - Begin_Time,Begin_Time,End_Time);
#endif // DISABLE_BENCH_MARKING_L1
    return 0;
}
