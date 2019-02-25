/*
  This file is part of ethash.

  ethash is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ethash is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ethash.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file io.h
 * @author Lefteris Karapetsas <lefteris@ethdev.com>
 * @date 2015
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#ifdef __cplusplus
#define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h>
#include "endian.h"
//#include "ethash.h"

#ifdef __cplusplus
extern "C" {
#endif
// Maximum size for mutable part of DAG file name
// 6 is for "full-R", the suffix of the filename
// 10 is for maximum number of digits of a uint32_t (for REVISION)
// 1 is for - and 16 is for the first 16 hex digits for first 8 bytes of
// the seedhash and last 1 is for the null terminating character
// Reference: https://github.com/ethereum/wiki/wiki/Ethash-DAG
#define DAG_MUTABLE_NAME_MAX_SIZE (6 + 10 + 1 + 16 + 1)
/// Possible return values of @see ethash_io_prepare
enum ethash_io_rc {
	ETHASH_IO_FAIL = 0,           ///< There has been an IO failure
	ETHASH_IO_MEMO_SIZE_MISMATCH, ///< DAG with revision/hash match, but file size was wrong.
	ETHASH_IO_MEMO_MISMATCH,      ///< The DAG file did not exist or there was revision/hash mismatch
	ETHASH_IO_MEMO_MATCH,         ///< DAG file existed and revision/hash matched. No need to do anything
};

// small hack for windows. I don't feel I should use va_args and forward just
// to have this one function properly cross-platform abstracted
#if defined(_WIN32) && !defined(__GNUC__)
#define snprintf(...) sprintf_s(__VA_ARGS__)
#endif

/**
 * Logs a critical error in important parts of ethash. Should mostly help
 * figure out what kind of problem (I/O, memory e.t.c.) causes a NULL
 * ethash_full_t
 */
#ifdef ETHASH_PRINT_CRITICAL_OUTPUT
#define ETHASH_CRITICAL(...)							\
	do													\
	{													\
		printf("ETHASH CRITICAL ERROR: "__VA_ARGS__);	\
		printf("\n");									\
		fflush(stdout);									\
	} while (0)
#else
#define ETHASH_CRITICAL(...)          
#endif


/**
 * An fopen wrapper for no-warnings crossplatform fopen.
 *
 * Msvc compiler considers fopen to be insecure and suggests to use their
 * alternative. This is a wrapper for this alternative. Another way is to
 * #define _CRT_SECURE_NO_WARNINGS, but disabling all security warnings does
 * not sound like a good idea.
 *
 * @param file_name        The path to the file to open
 * @param mode             Opening mode. Check fopen()
 * @return                 The FILE* or NULL in failure
 */
FILE* ethash_fopen(char const* file_name, char const* mode);

/**
 * An fseek wrapper for crossplatform 64-bit seek.
 *
 * @param f            The file stream whose fd to get
 * @param offset       Number of bytes from @a origin
 * @param origin       Initial position
 * @return             Current offset or -1 to indicate an error
 */
int ethash_fseek(FILE* f, size_t offset, int origin);

/**
 * An strncat wrapper for no-warnings crossplatform strncat.
 *
 * Msvc compiler considers strncat to be insecure and suggests to use their
 * alternative. This is a wrapper for this alternative. Another way is to
 * #define _CRT_SECURE_NO_WARNINGS, but disabling all security warnings does
 * not sound like a good idea.
 *
 * @param des              Destination buffer
 * @param dest_size        Maximum size of the destination buffer. This is the
 *                         extra argument for the MSVC secure strncat
 * @param src              Souce buffer
 * @param count            Number of bytes to copy from source
 * @return                 If all is well returns the dest buffer. If there is an
 *                         error returns NULL
 */
char* ethash_strncat(char* dest, size_t dest_size, char const* src, size_t count);

/**
 * A cross-platform mkdir wrapper to create a directory or assert it's there
 *
 * @param dirname        The full path of the directory to create
 * @return               true if the directory was created or if it already
 *                       existed
 */
bool ethash_mkdir(char const* dirname);

/**
 * Get a file's size
 *
 * @param[in] f        The open file stream whose size to get
 * @param[out] size    Pass a size_t by reference to contain the file size
 * @return             true in success and false if there was a failure
 */
bool ethash_file_size(FILE* f, size_t* ret_size);

/**
 * Get a file descriptor number from a FILE stream
 *
 * @param f            The file stream whose fd to get
 * @return             Platform specific fd handler
 */
int ethash_fileno(FILE* f);

/**
 * Create the filename for the DAG.
 *
 * @param dirname            The directory name in which the DAG file should reside
 *                           If it does not end with a directory separator it is appended.
 * @param filename           The actual name of the file
 * @param filename_length    The length of the filename in bytes
 * @return                   A char* containing the full name. User must deallocate.
 */
char* ethash_io_create_filename(
	char const* dirname,
	char const* filename,
	size_t filename_length
);

#ifdef __cplusplus
}
#endif
