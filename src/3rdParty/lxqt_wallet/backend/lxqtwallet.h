/*
 * copyright: 2013
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LXQTWALLET_H
#define LXQTWALLET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

    /*
     * NOTE: For documentation on how to use this API,look at the end of this header file.
     *
     * NOTE: This API takes anything of any size,most arguments takes "char *" or "const char *" for convenience
     *       since most typical use case will be storage of strings and these argument types will aleviate this use case
     *       from casting if the API was using "void *" or "const void *" as some would expect.
     */

    typedef struct lxqt_wallet_struct *lxqt_wallet_t ;

    /*
     * error values
     */
    typedef enum
    {
        lxqt_wallet_no_error = 0,
        lxqt_wallet_wrong_password,
        lxqt_wallet_wallet_exists,
        lxqt_wallet_gcry_cipher_open_failed,
        lxqt_wallet_gcry_cipher_setkey_failed,
        lxqt_wallet_gcry_cipher_setiv_failed,
        lxqt_wallet_gcry_cipher_encrypt_failed,
        lxqt_wallet_gcry_cipher_decrypt_failed,
        lxqt_wallet_failed_to_open_file,
        lxqt_wallet_failed_to_allocate_memory,
        lxqt_wallet_invalid_argument,
        lxqt_wallet_incompatible_wallet,
        lxqt_wallet_failed_to_create_key_hash,
        lxqt_wallet_libgcrypt_version_mismatch
    } lxqt_wallet_error;

    /*
     * key can not be NULL,
     * a NULL value or a non NULL value of size 0 will be taken as an empty value.
     */
    lxqt_wallet_error lxqt_wallet_add_key(lxqt_wallet_t, const char *key, u_int32_t key_size, const char *key_value, u_int32_t key_value_length) ;

    /*
     * open "wallet_name" wallet of application "application_name" using a password of size password_length.
     *
     * The rest of the API except lxqt_wallet_create() are undefined if this function returns a non zero number
     */
    lxqt_wallet_error lxqt_wallet_open(lxqt_wallet_t *, const char *password, u_int32_t password_length,
                                       const char *wallet_name, const char *application_name) ;

    /*
     * create a new wallet named "wallet_name" owned by application "application_name" using a password "password" of size "password_length".
     */
    lxqt_wallet_error lxqt_wallet_create(const char *password, u_int32_t password_length, const char *wallet_name, const char *application_name) ;

    /*
     * give a list of all wallets that belong to a program
     * Returned value is a NULL terminated array of strings with names of program wallets.
     * size argument will contain the number of wallets managed by the program
     * The caller of this function is expected to free() the returned value.
     * The caller of this function is expected to free() all individual entries in the returned array
     */
    char **lxqt_wallet_wallet_list(const char *application_name, int *size) ;

    /*
     * return the version of the library used to create the volume.
     * -1 is returned on error
     */
    int lxqt_wallet_volume_version(const char *wallet_name, const char *application_name, const char *password, u_int32_t password_length) ;

    /*
     * return the version of this library.
     * return value will be something like 200 for version 2.0.0
     */
    int lxqt_wallet_library_version(void) ;
    /*
     * delete a key.
     */
    lxqt_wallet_error lxqt_wallet_delete_key(lxqt_wallet_t, const char *key, u_int32_t key_size) ;

    /*
     * delete a wallet named "wallet_name" of an application named "application_name" exists
     */
    lxqt_wallet_error lxqt_wallet_delete_wallet(const char *wallet_name, const char *application_name) ;

    /*
     * close a wallet handle.
     */
    lxqt_wallet_error lxqt_wallet_close(lxqt_wallet_t *) ;

    /*
     * Check if a wallet named "wallet_name" of an application named "application_name" exists
     * returns 0 if the wallet exist
     */
    int lxqt_wallet_exists(const char *wallet_name, const char *application_name) ;

    /*
     * returns a path to where the wallet file is stored.
     * on return path_buffer will contain something like "/home/$USER/.local/application_name/wallets"
     */
    void lxqt_wallet_application_wallet_path(char *path_buffer, u_int32_t path_buffer_size, const char *application_name) ;

    /*
     * returns the amount of memory managed data consumes.
     */
    u_int64_t lxqt_wallet_wallet_size(lxqt_wallet_t) ;

    /*
     * returns the number of elements in the wallet
     */
    u_int64_t lxqt_wallet_wallet_entry_count(lxqt_wallet_t) ;

    typedef struct
    {
        const char *key ;
        u_int32_t key_size ;
        const char *key_value ;
        u_int32_t key_value_size ;
    } lxqt_wallet_key_values_t ;

    typedef struct
    {
        u_int64_t iter_pos ;
        lxqt_wallet_key_values_t entry ;
    } lxqt_wallet_iterator_t ;

    /*
     * iterate over the internal data structure and return an entry at the current interator position.
     * Any operation that modifies the internal data structure invalidates the iterator.
     *
     * 0 is returned when the end of the list is reached or on error
     * 1 is returned otherwise and the info at the current iterator position is returned through the "lxqt_wallet_key_values_t"
     * structure in the iterator.
     *
     * Loot at the example at the end of this header file to see how to use this function.
     */
    int lxqt_wallet_iter_read_value(lxqt_wallet_t, lxqt_wallet_iterator_t *) ;

    /*
     * 1 is returned if a matching key was found and key_value structure was filled up.
     * 0 is returned if a matching key was not found.
     * Content of the key_value returned are undefined after an entry is added or removed from the list
     */
    int lxqt_wallet_read_key_value(lxqt_wallet_t, const char *key, u_int32_t key_size, lxqt_wallet_key_values_t *key_value) ;

    /*
     * returns 1 if a wallet has a key and 0 otherwise
     */
    int lxqt_wallet_wallet_has_key(lxqt_wallet_t, const char *key, u_int32_t key_size) ;

    /*
     * returns 1 if value was found in one of the entries.
     * returns 0 otherwise.
     * If an entry is found,key_value argument will be filled up
     * Content of the key_value returned are undefined after an entry is added or removed from the list
     */
    int lxqt_wallet_wallet_has_value(lxqt_wallet_t, const char *value, u_int32_t value_size, lxqt_wallet_key_values_t *key_value) ;

    /*
     * change the wallet password
     */
    lxqt_wallet_error lxqt_wallet_change_wallet_password(lxqt_wallet_t, const char *new_password, u_int32_t new_password_size) ;

    /*
     * get a file given by argument "source" and create an encrypted version of the file given by argument "destination" using
     * a password "password" of length "password_length"
     *
     * function is an argument pointer to a function that takes an interger and returns an integer.The input argument will
     * show progress in percentage.If the return value of the function is non zero,the process will terminate.
     */
    lxqt_wallet_error lxqt_wallet_create_encrypted_file(const char *password, u_int32_t password_length,
            const char *source, const char *destination, int(*function)(int, void *), void *) ;

    /*
     * get an encrypted file given by argument "source" and create an un encrypted version of the file given by argument "destination" using
     * a password "password" of length "password_length"
     *
     * function is an argument pointer to a function that takes an interger and returns an integer.The input argument will
     * show progress in percentage.If the return value of the function is non zero,the process will terminate.
     */
    lxqt_wallet_error lxqt_wallet_create_decrypted_file(const char *password, u_int32_t password_length,
            const char *source, const char *destination, int(*function)(int, void *), void *) ;
    /*
     * undocumented API
     */
    char *_lxqt_wallet_get_wallet_data(lxqt_wallet_t wallet) ;

    /*
     * below is a complete program that tests the library functionality,it may be useful as a
     * source of how the use the API
     */
#if 0


#include "lxqtwallet.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

    /*
     * This source file shows how the library can be used
     */

    static const char *wallet_name    = "wallet_name" ;
    static const char *application_name = "application_name" ;

#define stringsAreEqual( x,y ) strcmp( x,y ) == 0

    int main(int argc, char *argv[])
    {
        lxqt_wallet_t wallet ;
        lxqt_wallet_error r = lxqt_wallet_no_error ;

        const char *f ;
        const char *z ;
        const char *command ;
        char **p ;

        lxqt_wallet_key_values_t value ;
        lxqt_wallet_iterator_t iter ;

        int a ;
        int b ;

        if (argc  < 2)
        {
            printf("wrong number of arguments\n") ;
            return lxqt_wallet_invalid_argument ;
        }

        command = argv[ 1 ] ;

        if (stringsAreEqual(command, "create"))
        {
            /*
             * create a new wallet
             * additional arguments: password
             * eg ./wallet create xxx
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_create(f, strlen(f), wallet_name, application_name) ;
            }
        }
        else if (stringsAreEqual(command, "add"))
        {
            /*
             * add an entry to the wallet
             * additional arguments: password key key_value
             * eg ./wallet add xxx alicia abc
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_open(&wallet, f, strlen(f), wallet_name, application_name) ;
            }

            if (r != lxqt_wallet_no_error)
            {
                if (r == lxqt_wallet_wrong_password)
                {
                    puts("wrong password") ;
                }
                else
                {
                    puts("general error has occured") ;
                }
            }
            else
            {
                f = argv[ 3 ] ;
                z = argv[ 4 ] ;
                r = lxqt_wallet_add_key(wallet, f, strlen(f) + 1, z, strlen(z) + 1) ;

                lxqt_wallet_close(&wallet) ;
            }
        }
        else if (stringsAreEqual(command, "read"))
        {
            /*
             * read an value in a wallet through its key
             * additional arguments: password key key_value
             * eg ./wallet read xxx alicia
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_open(&wallet, f, strlen(f), wallet_name, application_name) ;
            }

            if (r != lxqt_wallet_no_error)
            {
                if (r == lxqt_wallet_wrong_password)
                {
                    puts("wrong password") ;
                }
                else
                {
                    puts("general error has occured") ;
                }
            }
            else
            {
                f = argv[ 3 ] ;
                if (lxqt_wallet_read_key_value(wallet, f, strlen(f) + 1, &value))
                {
                    printf("key=%s:value=%s\n", value.key, value.key_value) ;
                }
                else
                {
                    printf("key=%s:value=(NULL)\n", f) ;
                }

                lxqt_wallet_close(&wallet) ;
            }
        }
        else if (stringsAreEqual(command, "print"))
        {
            /*
             * print all entries in the wallet
             * additional arguments: password
             * eg ./wallet print xxx
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_open(&wallet, f, strlen(f), wallet_name, application_name) ;
            }

            if (r == lxqt_wallet_no_error)
            {

                iter.iter_pos = 0 ;

                while (lxqt_wallet_iter_read_value(wallet, &iter))
                {
                    printf("key=%s\tkey value=\"%s\"\n", iter.entry.key, iter.entry.key_value) ;
                }

                lxqt_wallet_close(&wallet) ;
            }
            else
            {
                if (r == lxqt_wallet_wrong_password)
                {
                    puts("wrong password") ;
                }
                else
                {
                    puts("general error has occured") ;
                }
            }
        }
        else if (stringsAreEqual(command, "delete"))
        {
            /*
             * delete a key from a wallet
             * additional arguments: password key
             * eg ./wallet delete xxx alicia
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_open(&wallet, f, strlen(f), wallet_name, application_name) ;
            }

            if (r == lxqt_wallet_no_error)
            {
                f = argv[ 3 ] ;
                r = lxqt_wallet_delete_key(wallet, f, strlen(f) + 1) ;
                lxqt_wallet_close(&wallet) ;
            }
            else
            {
                if (r == lxqt_wallet_wrong_password)
                {
                    puts("wrong password") ;
                }
                else
                {
                    puts("general error has occured") ;
                }
            }
        }
        else if (stringsAreEqual(command, "change"))
        {
            /*
             * replace wallet key
             * additional arguments: old_password new_password
             * eg ./wallet replace xxx zzz
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_open(&wallet, f, strlen(f), wallet_name, application_name) ;
            }

            if (r == lxqt_wallet_no_error)
            {
                f = argv[ 3 ] ;
                r = lxqt_wallet_change_wallet_password(wallet, f, strlen(f)) ;
                lxqt_wallet_close(&wallet) ;
            }
            else
            {
                if (r == lxqt_wallet_wrong_password)
                {
                    puts("wrong password") ;
                }
                else
                {
                    puts("general error has occured") ;
                }
            }
        }
        else if (stringsAreEqual(command, "value"))
        {
            /*
             * returns a key with a particular value
             * additional arguments: password value
             * eg ./wallet value xxx zzz
             */
            if (argc < 3)
            {
                r = lxqt_wallet_invalid_argument ;
            }
            else
            {
                f = argv[ 2 ] ;
                r = lxqt_wallet_open(&wallet, f, strlen(f), wallet_name, application_name) ;
            }

            if (r == lxqt_wallet_no_error)
            {
                f = argv[ 3 ] ;
                if (lxqt_wallet_wallet_has_value(wallet, f, strlen(f) + 1, &value))
                {
                    printf("key=\"%s\"\nkey size=%d\n", value.key, value.key_size) ;
                }
                else
                {
                    printf("key=\"\"\n") ;
                }
                lxqt_wallet_close(&wallet) ;
            }
            else
            {
                if (r == lxqt_wallet_wrong_password)
                {
                    puts("wrong password") ;
                }
                else
                {
                    puts("general error has occured") ;
                }
            }
        }
        else if (stringsAreEqual(command, "version"))
        {
            /*
             * return the version of the library used to create the volume
             * additional argument: password
             * example ./wallet version zzz
             */
            if (argc < 3)
            {
                printf("wrong number of arguments\n") ;
            }
            else
            {
                f = argv[ 2 ] ;
                printf("%d\n", lxqt_wallet_volume_version(wallet_name, application_name, f, strlen(f))) ;
            }
        }
        else
        {
            puts("unknown option") ;
        }
        return r ;
    }


#endif

#ifdef __cplusplus
}
#endif

#endif
