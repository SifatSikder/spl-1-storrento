#ifndef BENCODE_H
#define BENCODE_H

#include <stddef.h>
#include <stdbool.h>
#include <openssl/sha.h>

#define INIT_CAP 32
#define FNV_OFF 2166136261UL // FNV1a 32 bit offset
#define FNV_PRIME 16777619UL // FNVI1a 32 bit prime

typedef enum
{
	BE_INT,
	BE_STR,
	BE_LIST,
	BE_DICT,
} be_type;

typedef struct be_node
{
	be_type type;
	unsigned char *key;
	void *val;
} be_node;

typedef struct be_string
{
	size_t len;
	unsigned char *str;
} be_string;

typedef struct be_list
{
	be_node node;
	struct be_list *next;
	// So that we can add elements easily and preserve order
	struct be_list *last;
} be_list;

typedef struct be_dict
{
	size_t capacity;
	size_t length;
	// Please check if this is set to true before doing anything with
	// the actual hash
	bool has_info_hash;
	// The info_hash (SHA1 hash of the info dictionary)
	unsigned char info_hash[SHA_DIGEST_LENGTH];
	struct be_node *entries;
} be_dict;


be_dict *decode_file (const char *file);

void *decode (unsigned char **buffer, size_t *len, be_type *type);

be_dict *dict_create (void);

void dict_destroy (be_dict *dict);

void *dict_get (be_dict *dict, unsigned char *key, be_type *type);

unsigned char *dict_set (be_dict *dict, unsigned char *key, void *val,be_type type);

void dict_val_print (unsigned char *key, void *val, be_type type);

void dict_dump (be_dict *dict);

void hex_dump(unsigned char *str, size_t len);

#endif /* BENCODE_H */
