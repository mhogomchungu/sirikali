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

#include "lxqtwallet.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <sys/mman.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <gcrypt.h>
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#define VERSION 200
#define VERSION_SIZE sizeof( short )
/*
 * below string MUST BE 11 bytes long
 */
#define MAGIC_STRING "lxqt_wallet"
#define MAGIC_STRING_SIZE 11
#define MAGIC_STRING_BUFFER_SIZE 16
#define PASSWORD_SIZE 32
#define BLOCK_SIZE 16
#define IV_SIZE 16
#define SALT_SIZE 16
#define FILE_BLOCK_SIZE 1024

#define PBKDF2_ITERATIONS 10000

#define NODE_HEADER_SIZE ( 2 * sizeof( u_int32_t ) )

#define WALLET_EXTENSION ".lwt"

struct lxqt_wallet_struct
{
    char *application_name;
    char *wallet_name;
    char key[ PASSWORD_SIZE ];
    char salt[ SALT_SIZE ];
    char *wallet_data;
    u_int64_t wallet_data_size;
    u_int64_t wallet_data_entry_count;
    int wallet_modified;
};

/*
 * Encrypted file documentation.
 *
 * A newly created file or an empty one takes 64 bytes.
 *
 * The first 16 bytes are used for pbkdf2 salt.
 * This salt is obtained from "/dev/urandom" and will not change when the wallet is updated.
 *
 * The second 16 bytes are used to store AES Initialization Vector.
 * The IV is initially obtained from "/dev/urandom".
 * The IV is stored unencrypted and will change on every wallet update.
 *
 * Everything from 32nd byte onward is store encrypted.
 *
 * The third 16 bytes are "magic string" bytes.
 * The first 11 bytes are used to store a known data aka "magic string" to be used to check if decryption key is correct or not.
 * The remaining 5 bytes are used to store file version number.
 *
 * The fourth 16 bytes are used to store information about the contents of the load.
 * The first 8 bytes are a u_int64_t data type and are used to store the load size
 * The second 8 bytes are a u_int64_t data type and are used to store the number of entries in the wallet.
 *
 * The load starts at 64th byte.
 *
 * The file is encrypted using CBC mode of 256 bit AES and hence may be padded to a file size larger than file contents to
 * accomodate CBC mode demanding data sizes that are divisible by 16.
 *
 * Key-Pair entries are stored as singly linked list nodes in an array.
 * Interesting video on why traditional linked lists are bad: http://www.youtube.com/watch?v=YQs6IC-vgmo
 *
 * A node of a linked list has 4 properties.
 * First 4 bytes of the node are a u_int32_t data type and are used to store the size of the key.
 * Second 4 bytes of the node are a u_int32_t data type and used to stores the size of the value.
 * The 8th byte of the node will be the beginning of the key.
 * The 8th byte of the node plus the size of the key will be the beginning of the value.
 *
 * The sum of the two 4 bytes plus the length of the key plus the length of the value will
 * point to the next node in the list.
 *
 * An empty node takes 8 bytes.A key is not allowed to be empty necessitating it having at least one character
 * making the minimum allowed size for the node to be 9 bytes.
 *
 * The size of the key in the node is managed by a u_int32_t data type.
 * The size of the value in the node is managed by a u_int32_t data type.
 * The above two data types means a node can occupy upto 8 bytes + 8 GiB of memory.
 *
 */

static char *_wallet_full_path(char *path_buffer, u_int32_t path_buffer_size, const char *wallet_name, const char *application_name);

static void _create_application_wallet_path(const char *application_name);

static gcry_error_t _create_key(const char salt[ SALT_SIZE ], char output_key[ PASSWORD_SIZE ], const char *input_key, u_int32_t input_key_length);

static gcry_error_t _create_temp_key(char *output_key, u_int32_t output_key_size, const char *input_key, u_int32_t input_key_length);

static void _get_iv_from_wallet_header(char iv[ IV_SIZE ], int fd);

static void _get_salt_from_wallet_header(char salt[ SALT_SIZE ], int fd);

static void _get_volume_info(char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ], int fd);

static void _get_random_data(char *buffer, size_t buffer_size);

static void _create_magic_string_header(char magic_string[ MAGIC_STRING_BUFFER_SIZE ]);

static int _wallet_is_compatible(const char *);

static int _password_match(const char *buffer);

static int _volume_version(const char *buffer);

static void _get_load_information(lxqt_wallet_t, const char *buffer);

static lxqt_wallet_error _lxqt_wallet_open(const char *password, u_int32_t password_length,
        const char *wallet_name, const char *application_name, char *buffer,
        int *ffd, struct lxqt_wallet_struct **ww, gcry_cipher_hd_t *h);

int lxqt_wallet_library_version(void)
{
    return VERSION;
}

char *_lxqt_wallet_get_wallet_data(lxqt_wallet_t wallet)
{
    if (wallet == NULL)
    {
        return NULL;
    }
    else
    {
        return wallet->wallet_data;
    }
}

static int _failed(gcry_error_t r)
{
    return r != GPG_ERR_NO_ERROR;
}

static int _passed(gcry_error_t r)
{
    return r == GPG_ERR_NO_ERROR;
}

inline static void  _get_header_components(u_int32_t *first, u_int32_t *second, const char *str)
{
    memcpy(first, str, sizeof(u_int32_t));
    memcpy(second, str + sizeof(u_int32_t), sizeof(u_int32_t));
}

u_int64_t lxqt_wallet_wallet_size(lxqt_wallet_t wallet)
{
    if (wallet == NULL)
    {
        return 0;
    }
    else
    {
        return wallet->wallet_data_size;
    }
}

u_int64_t lxqt_wallet_wallet_entry_count(lxqt_wallet_t wallet)
{
    if (wallet == NULL)
    {
        return 0;
    }
    else
    {
        return wallet->wallet_data_entry_count;
    }
}

static lxqt_wallet_error _exit_create(lxqt_wallet_error r, gcry_cipher_hd_t handle)
{
    if (handle != 0)
    {
        gcry_cipher_close(handle);
    }
    return r;
}

static lxqt_wallet_error lxqt_wallet_create_1(gcry_cipher_hd_t *h, const char *password,
        u_int32_t password_length, char *key, char *iv,
        char *salt)
{
    gcry_error_t r;

    gcry_cipher_hd_t handle;

    if (gcry_control(GCRYCTL_INITIALIZATION_FINISHED_P) == 0)
    {
        gcry_check_version(NULL);
        gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
    }

    r = gcry_cipher_open(h, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0);

    if (_failed(r))
    {
        return lxqt_wallet_gcry_cipher_open_failed;
    }

    handle = *h;

    _get_random_data(salt, SALT_SIZE);

    r = _create_key(salt, key, password, password_length);

    if (_failed(r))
    {
        return lxqt_wallet_failed_to_create_key_hash;
    }

    r = gcry_cipher_setkey(handle, key, PASSWORD_SIZE);

    if (_failed(r))
    {
        return lxqt_wallet_gcry_cipher_setkey_failed;
    }

    _get_random_data(iv, IV_SIZE);

    r = gcry_cipher_setiv(handle, iv, IV_SIZE);

    if (_failed(r))
    {
        return lxqt_wallet_gcry_cipher_setiv_failed;
    }
    else
    {
        return r;
    }
}

lxqt_wallet_error lxqt_wallet_create(const char *password, u_int32_t password_length,
                                     const char *wallet_name, const char *application_name)
{
    int fd;
    char path[ PATH_MAX ];
    char iv[ IV_SIZE ];
    char key[ PASSWORD_SIZE ];
    char salt[ SALT_SIZE ];
    char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ] = { '\0' };

    gcry_cipher_hd_t handle = 0;
    gcry_error_t r;

    if (password == NULL || wallet_name == NULL || application_name == NULL)
    {
        return _exit_create(lxqt_wallet_invalid_argument, handle);
    }
    if (lxqt_wallet_exists(wallet_name, application_name) == 0)
    {
        return _exit_create(lxqt_wallet_wallet_exists, handle);
    }

    r = lxqt_wallet_create_1(&handle, password, password_length, key, iv, salt);

    if (_failed(r))
    {
        return _exit_create(lxqt_wallet_gcry_cipher_encrypt_failed, handle);
    }
    else
    {
        _create_magic_string_header(buffer);

        r = gcry_cipher_encrypt(handle, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE, NULL, 0);

        _create_application_wallet_path(application_name);

        _wallet_full_path(path, PATH_MAX, wallet_name, application_name);

        fd = open(path, O_WRONLY | O_CREAT, 0600);

        if (fd == -1)
        {
            return _exit_create(lxqt_wallet_failed_to_open_file, handle);
        }
        else
        {
            /*
             * first 16 bytes are for PBKDF2 salt
             */
            write(fd, salt, SALT_SIZE);
            /*
             * second 16 bytes are for AES IV
             */
            write(fd, iv, IV_SIZE);
            /*
             * third 16 bytes are for the magic string
             */
            write(fd, buffer, MAGIC_STRING_BUFFER_SIZE);
            /*
             * fourth 16 bytes block that holds information about data load sizes
             */
            write(fd, buffer + MAGIC_STRING_BUFFER_SIZE, BLOCK_SIZE);

            close(fd);
            return _exit_create(lxqt_wallet_no_error, handle);
        }
    }
}

lxqt_wallet_error lxqt_wallet_create_encrypted_file(const char *password, u_int32_t password_length,
        const char *source, const char *destination, int(*function)(int, void *), void *v)
{
    gcry_error_t r;
    int fd_dest;
    int fd_src;
    char iv[ IV_SIZE ];
    char key[ PASSWORD_SIZE ];
    char salt[ SALT_SIZE ];
    char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ] = { '\0' };
    char file_buffer[ FILE_BLOCK_SIZE ];
    u_int64_t size;
    u_int64_t i;
    u_int64_t j;
    u_int64_t l;
    int k;
    gcry_cipher_hd_t handle = 0;

    struct stat st;

    if (password == NULL || source == NULL || destination == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }

    if (stat(destination, &st) == 0)
    {
        return lxqt_wallet_failed_to_open_file;
    }

    r = lxqt_wallet_create_1(&handle, password, password_length, key, iv, salt);

    if (_failed(r))
    {
        return _exit_create(lxqt_wallet_gcry_cipher_encrypt_failed, handle);
    }
    else
    {
        fd_dest = open(destination, O_WRONLY | O_CREAT, 0600);
        if (fd_dest == -1)
        {
            return _exit_create(lxqt_wallet_failed_to_open_file, handle);
        }
        fd_src = open(source, O_RDONLY);
        if (fd_src == -1)
        {
            close(fd_dest);
            return _exit_create(lxqt_wallet_failed_to_open_file, handle);
        }
        /*
         * first 16 bytes are for PBKDF2 salt
         */
        write(fd_dest, salt, SALT_SIZE);
        /*
         * second 16 bytes are for AES IV
         */
        write(fd_dest, iv, IV_SIZE);

        fstat(fd_src, &st);
	size = (u_int64_t)st.st_size;

        _create_magic_string_header(buffer);

        memcpy(buffer + MAGIC_STRING_BUFFER_SIZE, &size, sizeof(u_int64_t));

        gcry_cipher_encrypt(handle, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE, NULL, 0);

        /*
         * write third 16 byte and fourth 16 to the header
         */
        write(fd_dest, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE);

        i = 0;
        j = 0;
        l = 0;

        while (1)
        {
            k = (int)read(fd_src, file_buffer, FILE_BLOCK_SIZE);
            if (k == 0)
            {
                break;
            }
            r = gcry_cipher_encrypt(handle, file_buffer, FILE_BLOCK_SIZE, NULL, 0);
            write(fd_dest, file_buffer, FILE_BLOCK_SIZE);
            if (k < FILE_BLOCK_SIZE)
            {
                break;
            }
            i += FILE_BLOCK_SIZE;
            j = (i * 100 / size);
            if (j > l)
            {
                if (function((int)j, v))
                {
                    break;
                }
                l = j;
            }
        }

        function(100, v);

        close(fd_dest);
        close(fd_src);
        return  _exit_create(lxqt_wallet_no_error, handle);
    }
}

lxqt_wallet_error lxqt_wallet_change_wallet_password(lxqt_wallet_t wallet, const char *new_key, u_int32_t new_key_size)
{
    char key[ PASSWORD_SIZE ];
    gcry_error_t r;

    if (wallet == NULL || new_key == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }
    else
    {
        r = _create_key(wallet->salt, key, new_key, new_key_size);
        if (_failed(r))
        {
            return lxqt_wallet_failed_to_create_key_hash;
        }
        else
        {
            memcpy(wallet->key, key, PASSWORD_SIZE);
            wallet->wallet_modified = 1;
            return lxqt_wallet_no_error;
        }
    }
}

static lxqt_wallet_error _exit_open(lxqt_wallet_error st,
                                    struct lxqt_wallet_struct *w, gcry_cipher_hd_t handle, int fd)
{
    if (handle != 0)
    {
        gcry_cipher_close(handle);
    }
    if (fd != -1)
    {
        close(fd);
    }
    if (w != NULL)
    {
        free(w->wallet_name);
        free(w->application_name);
        free(w);
    }
    return st;
}

static lxqt_wallet_error _lxqt_wallet_open_0(gcry_cipher_hd_t *h, struct lxqt_wallet_struct *w,
        const char *password, u_int32_t password_length, int fd, char *buffer)
{
    gcry_error_t r;
    gcry_cipher_hd_t handle;
    char iv[ IV_SIZE ];

    if (gcry_control(GCRYCTL_INITIALIZATION_FINISHED_P) == 0)
    {
        gcry_check_version(NULL);
        gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
    }

    r = gcry_cipher_open(h, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0);

    handle = *h;

    if (_failed(r))
    {
        return lxqt_wallet_gcry_cipher_open_failed;
    }

    _get_salt_from_wallet_header(w->salt, fd);

    r = _create_key(w->salt, w->key, password, password_length);

    if (_failed(r))
    {
        return lxqt_wallet_failed_to_create_key_hash;
    }

    r = gcry_cipher_setkey(handle, w->key, PASSWORD_SIZE);

    if (_failed(r))
    {
        return lxqt_wallet_gcry_cipher_setkey_failed;
    }

    _get_iv_from_wallet_header(iv, fd);

    r = gcry_cipher_setiv(handle, iv, IV_SIZE);

    if (_failed(r))
    {
        return lxqt_wallet_gcry_cipher_setiv_failed;
    }
    else
    {
        _get_volume_info(buffer, fd);
        return gcry_cipher_decrypt(handle, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE, NULL, 0);
    }
}

lxqt_wallet_error lxqt_wallet_create_decrypted_file(const char *password, u_int32_t password_length,
        const char *source, const char *destination, int(*function)(int, void *), void *v)
{
    gcry_error_t r;

    int fd_dest;
    int fd_src;

    struct stat st;

    char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ] = { '\0' };
    char file_buffer[ FILE_BLOCK_SIZE ];

    u_int64_t size;
    u_int64_t i;
    u_int64_t j;
    u_int64_t l;
    u_int64_t n;
    u_int64_t t;

    gcry_cipher_hd_t handle = 0;

    struct lxqt_wallet_struct *w;

    if (password == NULL || source == NULL || destination == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }

    if (stat(destination, &st) == 0)
    {
        return lxqt_wallet_failed_to_open_file;
    }

    w = malloc(sizeof(struct lxqt_wallet_struct));

    if (w == NULL)
    {
        return _exit_open(lxqt_wallet_failed_to_allocate_memory, NULL, handle, -1);
    }

    memset(w, '\0', sizeof(struct lxqt_wallet_struct));

    fd_src = open(source, O_RDONLY);
    if (fd_src == -1)
    {
        return _exit_open(lxqt_wallet_failed_to_open_file, w, handle, -1);
    }

    r = _lxqt_wallet_open_0(&handle, w, password, password_length, fd_src, buffer);

    if (_failed(r))
    {
        close(fd_src);
        return _exit_open(lxqt_wallet_failed_to_open_file, w, handle, -1);
    }

    if (_password_match(buffer) && _wallet_is_compatible(buffer))
    {
        fd_dest = open(destination, O_WRONLY | O_CREAT, 0600);
        if (fd_dest == -1)
        {
            close(fd_src);
            return _exit_open(lxqt_wallet_failed_to_open_file, w, handle, -1);
        }

        _get_load_information(w, buffer);

        size = w->wallet_data_size;

        i = 0;
        j = 0;
        l = 0;

        n = size / FILE_BLOCK_SIZE;

        for (t = 0; t < n; t++)
        {
            read(fd_src, file_buffer, FILE_BLOCK_SIZE);
            gcry_cipher_decrypt(handle, file_buffer, FILE_BLOCK_SIZE, NULL, 0);
            write(fd_dest, file_buffer, FILE_BLOCK_SIZE);
            i += FILE_BLOCK_SIZE;
            j = (i * 100 / size);
            if (j > l)
            {
                if (function((int)j, v))
                {
                    break;
                }
                l = j;
            }
        }

        size = size - i;

        if (size > 0)
        {
            read(fd_src, file_buffer, FILE_BLOCK_SIZE);
            gcry_cipher_decrypt(handle, file_buffer, FILE_BLOCK_SIZE, NULL, 0);
            write(fd_dest, file_buffer, size);
        }

        close(fd_src);
        close(fd_dest);

        function(100, v);

        return _exit_open(lxqt_wallet_no_error, w, handle, -1);
    }
    else
    {
        close(fd_src);
        return _exit_open(lxqt_wallet_wrong_password, w, handle, -1);
    }
}

static lxqt_wallet_error _lxqt_wallet_open(const char *password, u_int32_t password_length,
        const char *wallet_name, const char *application_name, char *buffer,
        int *ffd, struct lxqt_wallet_struct **ww, gcry_cipher_hd_t *h)
{
    gcry_error_t r;
    gcry_cipher_hd_t handle = 0;

    char path[ PATH_MAX ];

    int fd;

    size_t len;

    struct lxqt_wallet_struct *w;

    _wallet_full_path(path, PATH_MAX, wallet_name, application_name);

    fd = open(path, O_RDONLY);

    if (fd == -1)
    {
        return _exit_open(lxqt_wallet_failed_to_open_file, NULL, handle, fd);
    }

    w = malloc(sizeof(struct lxqt_wallet_struct));

    if (w == NULL)
    {
        return _exit_open(lxqt_wallet_failed_to_allocate_memory, NULL, handle, fd);
    }

    memset(w, '\0', sizeof(struct lxqt_wallet_struct));

    len = strlen(wallet_name);

    w->wallet_name = malloc(sizeof(char) * (len + 1));

    if (w->wallet_name == NULL)
    {
        return _exit_open(lxqt_wallet_failed_to_allocate_memory, w, handle, fd);
    }
    else
    {
        memcpy(w->wallet_name, wallet_name, len + 1);
    }

    len = strlen(application_name);

    w->application_name = malloc(sizeof(char) * (len + 1));

    if (w->application_name == NULL)
    {
        return _exit_open(lxqt_wallet_failed_to_allocate_memory, w, handle, fd);
    }
    else
    {
        memcpy(w->application_name, application_name, len + 1);
    }

    r = _lxqt_wallet_open_0(&handle, w, password, password_length, fd, buffer);

    if (_failed(r))
    {
        return _exit_open(lxqt_wallet_gcry_cipher_decrypt_failed, w, handle, fd);
    }
    else
    {
        *ww = w;
        *ffd = fd;
        *h = handle;
        return lxqt_wallet_no_error;
    }
}

lxqt_wallet_error lxqt_wallet_open(lxqt_wallet_t *wallet, const char *password, u_int32_t password_length,
                                   const char *wallet_name, const char *application_name)
{
    struct stat st;
    u_int64_t len;
    char *e;

    int fd;
    struct lxqt_wallet_struct *w = 0;

    gcry_cipher_hd_t handle = 0;

    char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ];

    gcry_error_t r;

    if (wallet_name == NULL || application_name == NULL || wallet == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }

    r = _lxqt_wallet_open(password, password_length, wallet_name, application_name, buffer, &fd, &w, &handle);

    if (r != lxqt_wallet_no_error)
    {
        return r;
    }

    if (_password_match(buffer))
    {
        if (_wallet_is_compatible(buffer))
        {
            fstat(fd, &st);

            len = (u_int64_t)(st.st_size - (SALT_SIZE + IV_SIZE + MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE));

            if ((int64_t)len <= 0)
            {
                /*
                 * empty wallet
                 */
                *wallet = w;
                return _exit_open(lxqt_wallet_no_error, NULL, handle, fd);
            }
            else
            {
                _get_load_information(w, buffer);
                if( w->wallet_data_size > len ){
                    /*
                     * Wallet is corrupt somehow,lets clear it.
                    */
                    w->wallet_data_size = 0;
                    w->wallet_data_entry_count = 0;
                    w->wallet_modified = 1;
                }
                e = malloc(len);

                if (e != NULL)
                {
                    mlock(e, len);
                    read(fd, e, len);
                    r = gcry_cipher_decrypt(handle, e, len, NULL, 0);
                    if (_passed(r))
                    {
                        w->wallet_data = e;
                        *wallet = w;
                        return _exit_open(lxqt_wallet_no_error, NULL, handle, fd);
                    }
                    else
                    {
                        free(e);
                        return _exit_open(lxqt_wallet_gcry_cipher_decrypt_failed, w, handle, fd);
                    }
                }
                else
                {
                    return _exit_open(lxqt_wallet_failed_to_allocate_memory, w, handle, fd);
                }
            }
        }
        else
        {
            return _exit_open(lxqt_wallet_incompatible_wallet, w, handle, fd);
        }
    }
    else
    {
        return _exit_open(lxqt_wallet_wrong_password, w, handle, fd);
    }
}

int lxqt_wallet_volume_version(const char *wallet_name, const char *application_name, const char *password, u_int32_t password_length)
{
    int fd;
    int version;
    struct lxqt_wallet_struct *w = 0;

    gcry_cipher_hd_t handle = 0;

    char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ];

    gcry_error_t r;

    if (wallet_name == NULL || application_name == NULL)
    {
        return -1;
    }
    else
    {
        r = _lxqt_wallet_open(password, password_length, wallet_name, application_name, buffer, &fd, &w, &handle);

        if (r != lxqt_wallet_no_error)
        {
            return -1;
        }
        else
        {
            if (_password_match(buffer))
            {
                version = _volume_version(buffer);
                _exit_open(lxqt_wallet_no_error, w, handle, fd);
                return version;
            }
            else
            {
                _exit_open(lxqt_wallet_wrong_password, w, handle, fd);
                return -1;
            }
        }
    }
}

int lxqt_wallet_read_key_value(lxqt_wallet_t wallet, const char *key, u_int32_t key_size, lxqt_wallet_key_values_t *key_value)
{
    const char *e;
    const char *z;

    u_int64_t k = 0;
    u_int64_t i = 0;

    u_int32_t key_len;
    u_int32_t key_value_len;

    if (key == NULL || wallet == NULL || key_value == NULL)
    {
    }
    else
    {
        e = wallet->wallet_data;
        z = e;
        k = wallet->wallet_data_size;

        while (i < k)
        {
            _get_header_components(&key_len, &key_value_len, e);

            if (key_len == key_size && memcmp(key, e + NODE_HEADER_SIZE, key_size) == 0)
            {
                key_value->key            = e + NODE_HEADER_SIZE;
                key_value->key_size       = key_len;
                key_value->key_value      = e + NODE_HEADER_SIZE + key_len;
                key_value->key_value_size = key_value_len;
                return 1;
            }
            else
            {
                i = i + NODE_HEADER_SIZE + key_len + key_value_len;
                e = z + i;
            }
        }
    }

    return 0;
}

int lxqt_wallet_wallet_has_key(lxqt_wallet_t wallet, const char *key, u_int32_t key_size)
{
    lxqt_wallet_key_values_t key_value;
    return lxqt_wallet_read_key_value(wallet, key, key_size, &key_value);
}

int lxqt_wallet_wallet_has_value(lxqt_wallet_t wallet, const char *value, u_int32_t value_size, lxqt_wallet_key_values_t *key_value)
{
    const char *e;
    const char *z;

    u_int64_t k = 0;
    u_int64_t i = 0;

    u_int32_t key_len;
    u_int32_t key_value_len;

    if (key_value == NULL || wallet == NULL)
    {
        return 0;
    }
    else
    {
        e = wallet->wallet_data;
        z = e;
        k = wallet->wallet_data_size;

        while (i < k)
        {
            _get_header_components(&key_len, &key_value_len, e);

            if (key_value_len == value_size && memcmp(value, e + NODE_HEADER_SIZE + key_len, value_size) == 0)
            {
                key_value->key            = e + NODE_HEADER_SIZE;
                key_value->key_size       = key_len;
                key_value->key_value      = e + NODE_HEADER_SIZE + key_len;
                key_value->key_value_size = key_value_len;
                return 1;
            }
            else
            {
                i = i + NODE_HEADER_SIZE + key_len + key_value_len;
                e = z + i;
            }
        }

        return 0;
    }
}

lxqt_wallet_error lxqt_wallet_add_key(lxqt_wallet_t wallet, const char *key, u_int32_t key_size,
                                      const char *value, u_int32_t key_value_length)
{
    char *e;
    char *f;

    u_int64_t len;

    if (key == NULL || wallet == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }
    else
    {
        if (key_size == 0)
        {
            return lxqt_wallet_invalid_argument;
        }
        else
        {
            if (value == NULL || key_value_length == 0)
            {
                key_value_length = 0;
                value = "";
            }

            len = NODE_HEADER_SIZE + key_size + key_value_length;
            f = realloc(wallet->wallet_data, wallet->wallet_data_size + len);

            if (f != NULL)
            {
                mlock(f, wallet->wallet_data_size + len);
                e = f + wallet->wallet_data_size;

                memcpy(e, &key_size, sizeof(u_int32_t));
                memcpy(e + sizeof(u_int32_t), &key_value_length, sizeof(u_int32_t));
                memcpy(e + NODE_HEADER_SIZE, key, key_size);
                memcpy(e + NODE_HEADER_SIZE + key_size, value, key_value_length);

                wallet->wallet_data_size += len;
                wallet->wallet_modified = 1;
                wallet->wallet_data = f;
                wallet->wallet_data_entry_count++;

                return lxqt_wallet_no_error;
            }
            else
            {
                return lxqt_wallet_failed_to_allocate_memory;
            }
        }
    }
}

int lxqt_wallet_iter_read_value(lxqt_wallet_t wallet, lxqt_wallet_iterator_t *iter)
{
    u_int32_t key_len;
    u_int32_t key_value_len;

    const char *e;

    if (wallet == NULL || iter->iter_pos >= wallet->wallet_data_size)
    {
        return 0;
    }
    else
    {
        e = wallet->wallet_data + iter->iter_pos;

        _get_header_components(&key_len, &key_value_len, e);

        iter->entry.key             = e + NODE_HEADER_SIZE;
        iter->entry.key_size        = key_len;
        iter->entry.key_value       = e + NODE_HEADER_SIZE + key_len;
        iter->entry.key_value_size  = key_value_len;

        iter->iter_pos += NODE_HEADER_SIZE + key_len + key_value_len;

        return 1;
    }
}

int lxqt_wallet_read_value_at(lxqt_wallet_t wallet, u_int64_t pos, lxqt_wallet_key_values_t *key_value)
{
    char *e;
    char *z;

    u_int64_t k = 0;
    u_int64_t i = 0;

    u_int32_t key_len = 0;
    u_int32_t key_value_len = 0;

    if (wallet == NULL || wallet->wallet_data_entry_count == 0 || pos > wallet->wallet_data_entry_count)
    {
        return 0;
    }
    else
    {
        e = wallet->wallet_data;
        z = e;
        while (1)
        {
            _get_header_components(&key_len, &key_value_len, e);

            if (k == pos)
            {
                key_value->key            = e + NODE_HEADER_SIZE;
                key_value->key_size       = key_len;
                key_value->key_value      = e + NODE_HEADER_SIZE + key_len;
                key_value->key_value_size = key_value_len;
                break;
            }
            else
            {
                i = i + NODE_HEADER_SIZE + key_len + key_value_len;
                e = z + i;
                k++;
            }
        }
        return 1;
    }
}

lxqt_wallet_error lxqt_wallet_delete_key(lxqt_wallet_t wallet, const char *key, u_int32_t key_size)
{
    char *e;
    char *z;

    u_int64_t k = 0;
    u_int64_t i = 0;

    u_int32_t key_len;
    u_int32_t key_value_len;

    u_int64_t block_size;

    if (key == NULL || wallet == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }
    else
    {
        e = wallet->wallet_data;
        z = e;
        k = wallet->wallet_data_size;

        while (i < k)
        {
            _get_header_components(&key_len, &key_value_len, e);

            if (key_len == key_size && memcmp(key, e + NODE_HEADER_SIZE, key_size) == 0)
            {

                if (wallet->wallet_data_entry_count == 1)
                {
                    memset(wallet->wallet_data, '\0', wallet->wallet_data_size);
                    free(wallet->wallet_data);
                    wallet->wallet_data_size = 0;
                    wallet->wallet_modified = 1;
                    wallet->wallet_data = NULL;
                    wallet->wallet_data_entry_count = 0;
                }
                else
                {
                    block_size = NODE_HEADER_SIZE + key_len + key_value_len;

                    memmove(e, e + block_size, wallet->wallet_data_size - (i + block_size));

                    memset(z + wallet->wallet_data_size - block_size, '\0', block_size);

                    wallet->wallet_data_size -= block_size;
                    wallet->wallet_modified = 1;
                    wallet->wallet_data_entry_count--;
                }

                break;
            }
            else
            {
                i = i + NODE_HEADER_SIZE + key_len + key_value_len;
                e = z + i;
            }
        }
    }

    return lxqt_wallet_no_error;
}

lxqt_wallet_error lxqt_wallet_delete_wallet(const char *wallet_name, const char *application_name)
{
    char path[ PATH_MAX ];
    _wallet_full_path(path, PATH_MAX, wallet_name, application_name);
    unlink(path);
    return lxqt_wallet_no_error;
}

static lxqt_wallet_error _close_exit(lxqt_wallet_error err, lxqt_wallet_t *w, gcry_cipher_hd_t handle)
{
    lxqt_wallet_t wallet = *w;
    *w = NULL;

    if (handle != 0)
    {
        gcry_cipher_close(handle);
    }

    if (wallet->wallet_data_size > 0)
    {
        memset(wallet->wallet_data, '\0', wallet->wallet_data_size);
        munlock(wallet->wallet_data, wallet->wallet_data_size);
        free(wallet->wallet_data);
    }
    free(wallet->wallet_name);
    free(wallet->application_name);
    free(wallet);
    return err;
}

lxqt_wallet_error lxqt_wallet_close(lxqt_wallet_t *w)
{
    gcry_cipher_hd_t handle;
    int fd;
    char iv[ IV_SIZE ];
    char path[ PATH_MAX ];
    char path_1[ PATH_MAX+16 ];
    char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ];

    lxqt_wallet_t wallet;

    u_int64_t k;
    char *e;

    gcry_error_t r;

    if (w == NULL || *w == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }

    wallet = *w;

    if (wallet->wallet_modified == 0)
    {
        return _close_exit(lxqt_wallet_no_error, w, 0);
    }

    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

    r = gcry_cipher_open(&handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0);

    if (_failed(r))
    {
        return _close_exit(lxqt_wallet_gcry_cipher_open_failed, w, 0);
    }

    r = gcry_cipher_setkey(handle, wallet->key, PASSWORD_SIZE);

    if (_failed(r))
    {
        return _close_exit(lxqt_wallet_gcry_cipher_setkey_failed, w, handle);
    }

    _get_random_data(iv, IV_SIZE);

    r = gcry_cipher_setiv(handle, iv, IV_SIZE);

    if (_failed(r))
    {
        return _close_exit(lxqt_wallet_gcry_cipher_setiv_failed, w, handle);
    }

    _create_magic_string_header(buffer);

    memcpy(buffer + MAGIC_STRING_BUFFER_SIZE, &wallet->wallet_data_size, sizeof(u_int64_t));
    memcpy(buffer + MAGIC_STRING_BUFFER_SIZE + sizeof(u_int64_t), &wallet->wallet_data_entry_count, sizeof(u_int64_t));

    r = gcry_cipher_encrypt(handle, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE, NULL, 0);

    if (_failed(r))
    {
        return _close_exit(lxqt_wallet_gcry_cipher_encrypt_failed, w, handle);
    }

    _wallet_full_path(path, sizeof (path), wallet->wallet_name, wallet->application_name);
    snprintf(path_1, sizeof (path_1), "%s.tmp", path);

    k = wallet->wallet_data_size;

    if (k == 0)
    {
        fd = open(path_1, O_WRONLY | O_CREAT, 0600);
        if (fd == -1)
        {
            return _close_exit(lxqt_wallet_gcry_cipher_open_failed, w, handle);
        }
        else
        {
            write(fd, wallet->salt, SALT_SIZE);
            write(fd, iv, IV_SIZE);
            write(fd, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE);
            close(fd);
            rename(path_1, path);
            return _close_exit(lxqt_wallet_no_error, w, handle);
        }
    }
    else
    {
        while (k % 32 != 0)
        {
            k++;
        }

        e = realloc(wallet->wallet_data, k);

        if (e != NULL)
        {
            wallet->wallet_data = e;
            r = gcry_cipher_encrypt(handle, wallet->wallet_data, k, NULL, 0);
            if (_failed(r))
            {
                return _close_exit(lxqt_wallet_gcry_cipher_encrypt_failed, w, handle);
            }
            else
            {
                fd = open(path_1, O_WRONLY | O_CREAT, 0600);
                if (fd == -1)
                {
                    return _close_exit(lxqt_wallet_gcry_cipher_open_failed, w, handle);
                }
                else
                {
                    write(fd, wallet->salt, SALT_SIZE);
                    write(fd, iv, IV_SIZE);
                    write(fd, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE);
                    write(fd, wallet->wallet_data, k);
                    close(fd);
                    rename(path_1, path);
                    return _close_exit(lxqt_wallet_no_error, w, handle);
                }
            }
        }
        else
        {
            return _close_exit(lxqt_wallet_failed_to_allocate_memory, w, handle);
        }
    }
}

char **lxqt_wallet_wallet_list(const char *application_name, int *size)
{
    char path[ PATH_MAX ];
    char **result = NULL;
    char **result_1;
    char *e;

    int count = 0;
    u_int32_t len;
    struct dirent *entry;
    DIR *dir;

    if (application_name == NULL || size == NULL)
    {
        return NULL;
    }

    lxqt_wallet_application_wallet_path(path, PATH_MAX, application_name);

    dir = opendir(path);

    if (dir == NULL)
    {
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        len = (u_int32_t)(strlen(entry->d_name) - strlen(WALLET_EXTENSION));
        if ((int32_t)len > 0)
        {
            result_1 = realloc(result, sizeof(char *) * ((size_t)count + 1));
            if (result_1 != NULL)
            {
                e = malloc(len + 1);
                if (e != NULL)
                {
                    memcpy(e, entry->d_name, len);
                    *(e + len) = '\0';
                    result_1[ count ] = e;
                    result = result_1;
                    count++;
                }
            }
        }
    }

    *size = count;
    closedir(dir);

    return result;
}

int lxqt_wallet_exists(const char *wallet_name, const char *application_name)
{
    struct stat st;
    char path[ PATH_MAX ];
    if (wallet_name == NULL || application_name == NULL)
    {
        return lxqt_wallet_invalid_argument;
    }
    else
    {
        _wallet_full_path(path, PATH_MAX, wallet_name, application_name);
        return stat(path, &st);
    }
}

void lxqt_wallet_application_wallet_path(char *path, u_int32_t path_buffer_size, const char *application_name)
{
    struct passwd *pass = getpwuid(getuid());
    snprintf(path, path_buffer_size, "%s/.config/lxqt/wallets/%s/", pass->pw_dir, application_name);
}

static char *_wallet_full_path(char *path_buffer, u_int32_t path_buffer_size, const char *wallet_name, const char *application_name)
{
    char path_1[ PATH_MAX ];
    lxqt_wallet_application_wallet_path(path_1, sizeof (path_1), application_name);
    snprintf(path_buffer, path_buffer_size, "%s/%s%s", path_1, wallet_name, WALLET_EXTENSION);
    return path_buffer;
}

static void _create_application_wallet_path(const char *application_name)
{
    char path[ PATH_MAX ];
    char *e;

    lxqt_wallet_application_wallet_path(path, PATH_MAX, application_name);

    for (e = path + 1; *e != '\0'; e++)
    {
        if (*e == '/')
        {
            *e = '\0';
            mkdir(path, 0755);
            *e = '/';
        }
    }
}

static gcry_error_t _create_temp_key(char *output_key, u_int32_t output_key_size, const char *input_key, u_int32_t input_key_length)
{
    gcry_md_hd_t md;
    unsigned char *digest;

    gcry_error_t r = gcry_md_open(&md, GCRY_MD_SHA256, GCRY_MD_FLAG_SECURE);

    if (_passed(r))
    {
        gcry_md_write(md, input_key, input_key_length);
        gcry_md_final(md);
        digest = gcry_md_read(md, 0);
        if (digest == NULL)
        {
            r = !GPG_ERR_NO_ERROR;
        }
        else
        {
            memcpy(output_key, digest, output_key_size);
        }
        gcry_md_close(md);
    }

    return r;
}

/*
 * gcry_kdf_derive() doesnt seem to work with empty passphrases,to work around it,we create a temporary passphrases
 * based on provided passphrase and then feed the temporary key to gcry_kdf_derive()
 */
static gcry_error_t _create_key(const char salt[ SALT_SIZE ],
                                char output_key[ PASSWORD_SIZE ], const char *input_key, u_int32_t input_key_length)
{
    char temp_key[ PASSWORD_SIZE ];
    gcry_error_t r = _create_temp_key(temp_key, PASSWORD_SIZE, input_key, input_key_length);

    if (_passed(r))
    {
        return gcry_kdf_derive(temp_key, PASSWORD_SIZE, GCRY_KDF_PBKDF2, GCRY_MD_SHA256,
                               salt, SALT_SIZE, PBKDF2_ITERATIONS, PASSWORD_SIZE, output_key);
    }
    else
    {
        return r;
    }
}

static void _get_iv_from_wallet_header(char iv[ IV_SIZE ], int fd)
{
    lseek(fd, SALT_SIZE, SEEK_SET);
    read(fd, iv, IV_SIZE);
}

static void _get_salt_from_wallet_header(char salt[ SALT_SIZE ], int fd)
{
    lseek(fd, 0, SEEK_SET);
    read(fd, salt, SALT_SIZE);
}

static void _get_volume_info(char buffer[ MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ], int fd)
{
    lseek(fd, IV_SIZE + SALT_SIZE, SEEK_SET);
    read(fd, buffer, MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE);
}

static void _get_load_information(lxqt_wallet_t w, const char *buffer)
{
    buffer = buffer + MAGIC_STRING_BUFFER_SIZE;
    memcpy(&w->wallet_data_size, buffer, sizeof(u_int64_t));
    memcpy(&w->wallet_data_entry_count, buffer + sizeof(u_int64_t), sizeof(u_int64_t));
}

static void _get_random_data(char *buffer, size_t buffer_size)
{
    int fd;
    fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1)
    {
        read(fd, buffer, buffer_size);
        close(fd);
    }
    else
    {
        gcry_create_nonce(buffer, buffer_size);
    }
}

static int _password_match(const char *buffer)
{
    return memcmp(buffer, MAGIC_STRING, MAGIC_STRING_SIZE) == 0;
}

static void _create_magic_string_header(char magic_string[ MAGIC_STRING_BUFFER_SIZE ])
{
    u_int16_t version = VERSION;
    /*
     * write 11 bytes of magic string
     */
    memcpy(magic_string, MAGIC_STRING, MAGIC_STRING_SIZE);
    /*
     * write version information in the remaining 5 bytes of the 16 byte buffer
     */
    memcpy(magic_string + MAGIC_STRING_SIZE, &version, sizeof(u_int16_t));
}

static int _wallet_is_compatible(const char *buffer)
{
    u_int16_t version;
    memcpy(&version, buffer + MAGIC_STRING_SIZE, sizeof(u_int16_t));
    /*
     * This source file should be able to guarantee it can open volumes that have the same major version number
     */
    return version >= VERSION && version < (VERSION + 100);
}

static int _volume_version(const char *buffer)
{
    u_int16_t version;
    memcpy(&version, buffer + MAGIC_STRING_SIZE, sizeof(u_int16_t));
    return (int)version;
}
