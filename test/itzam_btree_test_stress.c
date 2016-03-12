/*
    Itzam/C (version 7.0) is an embedded database engine written in Standard C.
    
    It is part of the Drakontos Library of Interesting and Esoteric Oddities

    Copyright 2016 Scott Robert Ladd. All rights reserved.

    Itzam/C is user-supported open source software. It's continued development is dependent on
    financial support from the community. You can provide funding by visiting the Itzam/C
    website at:

        http://www.drakontos.com

    You license Itzam/C under the Simplified BSD License (FreeBSD License), the text of which 
    is available at the website above. 
*/

#include "../src/itzam.h"
#include "itzam_errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/*----------------------------------------------------------
 * embedded random number generator; ala Park and Miller
 */
static int32_t seed = 1325;

void init_test_prng(int32_t s)
{
    seed = s;
}

int32_t random_int32(int32_t limit)
{
    static const int32_t IA   = 16807;
    static const int32_t IM   = 2147483647;
    static const int32_t IQ   = 127773;
    static const int32_t IR   = 2836;
    static const int32_t MASK = 123459876;

    int32_t k;
    int32_t result;

    seed ^= MASK;
    k = seed / IQ;
    seed = IA * (seed - k * IQ) - IR * k;

    if (seed < 0L)
        seed += IM;

    result = (seed % limit);
    seed ^= MASK;

    return result;
}

/*----------------------------------------------------------
 *  Reports an itzam error
 */
void not_okay(itzam_state state)
{
    fprintf(stderr, "\nItzam problem: %s\n", STATE_MESSAGES[state]);
    exit(EXIT_FAILURE);
}

void error_handler(const char * function_name, itzam_error error)
{
    fprintf(stderr, "Itzam error in %s: %s\n", function_name, ERROR_STRINGS[error]);
    exit(EXIT_FAILURE);
}

/*----------------------------------------------------------
 *  Verifies that the database contains the expected records
 */
static itzam_bool verify(itzam_btree * btree, itzam_bool * key_flags, int maxkey)
{
    itzam_bool result = itzam_true;
    int32_t key, rec;

    printf(" -- verifying... ");
    fflush(stdout);

    for (key = 0; key < maxkey; ++key)
    {
        if (itzam_btree_find(btree,(const void *)(&key),(void *)(&rec)))
        {
            if (key_flags[key])
            {
                if (rec != key)
                {
                    printf("data does not match key %d\n", key);
                    result = itzam_false;
                }
            }
            else
            {
                printf("key %d found, and should not have been\n", key);
                result = itzam_false;
            }
        }
        else
        {
            if (key_flags[key])
            {
                printf("expected key %d not found\n", key);
                result = itzam_false;
            }
        }
    }

    if (result)
        printf("okay\n");

    return result;
}

/*----------------------------------------------------------
 * tests
 */
itzam_bool test_btree_stress()
{
    itzam_btree  btree;
    itzam_state  state;
    int32_t      key;
    itzam_int    n, i;
    itzam_int    add_count        = 0;
    itzam_int    rem_count        = 0;
    itzam_int    tran_count       = 0;
    itzam_int    roll_count       = 0;
    itzam_int    commit_count     = 0;
    itzam_bool   in_transaction   = itzam_false;
    itzam_bool * save_flags       = NULL;
    itzam_int    save_add_count   = 0;
    itzam_int    save_rem_count   = 0;
    char *       tran_message     = NULL;
    char *       filename         = "stress.itzam";
    itzam_int    maxkey           = 250000;
    itzam_int    test_size        = 1000000;
    int order                     = 25;
    itzam_bool   verified_run     = itzam_false;
    itzam_bool   verbose          = itzam_false;
    itzam_bool   use_transactions = itzam_true;
    itzam_bool * key_flags        = (itzam_bool *)malloc(maxkey * sizeof(itzam_bool));

    time_t start = time(NULL);

    for (n = 0; n < maxkey; ++n)
        key_flags[n] = itzam_false;

    // banner for this test
    printf("\nItzam/C B-Tree Test\nRandomized Stress Test\n");

    state = itzam_btree_create(&btree, filename, order, sizeof(int32_t), itzam_comparator_int32, error_handler);

    if (state != ITZAM_OKAY)
    {
        not_okay(state);
        return itzam_false;
    }

    save_flags = (itzam_bool *)malloc(sizeof(itzam_bool) * maxkey);

    for (n = 1; n <= test_size; ++ n)
    {
        if (use_transactions && !in_transaction)
        {
            if (random_int32(100) < 20)
            {
                state = itzam_btree_transaction_start(&btree);
                in_transaction = itzam_true;

                if (verbose) printf("transaction START\n");

                for (i = 0; i < maxkey; ++i)
                    save_flags[i] = key_flags[i];

                save_add_count = add_count;
                save_rem_count = rem_count;
                ++tran_count;
            }
        }

        key = (int32_t)random_int32((int32_t)maxkey);

        if (verbose) printf("%8d: test key = %8u: ", (int)n, key);

        state = itzam_btree_insert(&btree,(const void *)&key);

        switch (state)
        {
            case ITZAM_OKAY:
                if (key_flags[key])
                {
                    if (verbose) printf("key was not found, and should have been\n");
                    return itzam_false;
                }

                key_flags[key] = itzam_true;
                ++add_count;

                if (verbose) printf("written successfully");

                break;

            case ITZAM_DUPLICATE:
                if (!key_flags[key])
                {
                    if (verbose) printf("key was found, and should not have been\n");
                    return itzam_false;
                }

                state = itzam_btree_remove(&btree,(const void *)(&key));
                ++rem_count;

                if (state == ITZAM_OKAY)
                {
                    key_flags[key] = itzam_false;
                    if (verbose) printf("removed successfully");
                }
                else
                {
                    if (verbose) printf("unable to remove data\n");
                    return itzam_false;
                }

                break;

            default:
                not_okay(state);
                return itzam_false;
        }

        if (in_transaction)
        {
            int32_t choice = random_int32(100);

            if (choice < 35)
            {
                state = itzam_btree_transaction_commit(&btree);
                in_transaction = itzam_false;

                tran_message = "transaction COMMIT\n";
                ++commit_count;
            }
            else if (choice < 65)
            {
                state = itzam_btree_transaction_rollback(&btree);
                in_transaction = itzam_false;

                tran_message = "transaction ROLLBACK\n";

                for (i = 0; i < maxkey; ++i)
                    key_flags[i] = save_flags[i];

                add_count = save_add_count;
                rem_count = save_rem_count;

                ++roll_count;
            }
        }

        if (verified_run)
        {
            itzam_bool okay = verify(&btree,key_flags,maxkey);

            if (!okay)
                return itzam_false;
        }
        else
        {
            if (verbose) printf(" - done\n");
        }

        if (tran_message != NULL)
        {
            if (verbose)
                printf("%s\n",tran_message);

            tran_message = NULL;
        }
    }

    time_t elapsed = time(NULL) - start;

    int tests = add_count + rem_count;

    printf("\ntotal records   added: %u\n", (unsigned int)add_count);
    printf("total records removed: %u\n", (unsigned int)rem_count);

    if (use_transactions)
    {
        printf("         transactions: %u\n", (unsigned int)tran_count);
        printf("              commits: %u\n", (unsigned int)commit_count);
        printf("            rollbacks: %u\n", (unsigned int)roll_count);
    }

    printf("      database  count: %d\n", (int)btree.m_header->m_count);
    printf("      database ticker: %d\n", (int)btree.m_header->m_ticker);
    printf("       total run time: %u seconds\n", (unsigned int)elapsed);
    printf("operations per second: %f\n", ((double)tests / (double)elapsed));
    
    printf("\nTested with Itzam v%s\n\n",ITZAM_DISPLAY_VERSION);

    free(save_flags);

    state = itzam_btree_close(&btree);

    if (state != ITZAM_OKAY)
    {
        not_okay(state);
        return itzam_false;
    }

    free(key_flags);

    return itzam_true;
}

int main(int argc, char* argv[])
{
    int result = EXIT_FAILURE;

    itzam_set_default_error_handler(error_handler);

    init_test_prng((long)time(NULL));

    if (test_btree_stress())
        result = EXIT_SUCCESS;

    return result;
}

