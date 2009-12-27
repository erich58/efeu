#ifndef CUBE_HANDLE_H
#define CUBE_HANDLE_H

#include <EFEU/mdmat.h>

typedef struct CubeHandle {
	int fd;
	int stat;
	char *cmd;
	char *arg;
	mdmat *md;
	StrBuf *buf_in;
	StrBuf *buf_out;
	IO *out;
	size_t line;
	size_t last_pos;
	size_t save_limit;
	size_t max_lines;
	size_t max_byte;
	int headline;
	int out_eof;
	const char *key;
	const char *seg;
} CubeHandle;

void utf8_put (const char *str, IO *out);
void utf8_save (const char *str, StrBuf *buf);
char *trim_arg (char *arg);

void label_beg (void);
size_t label_pos (void);
void label_add (size_t pos, mdaxis *x, size_t n);
void label_xadd (size_t pos, mdaxis *x, size_t n);
char *label_get (void);

CubeHandle *CubeHandle_init (CubeHandle *handle, int fd);
void CubeHandle_close (CubeHandle *handle);
char *CubeHandle_read (CubeHandle *handle);
void CubeHandle_unread (CubeHandle *handle);
void CubeHandle_beg (CubeHandle *handle, const char *key, const char *seg);
void CubeHandle_send (CubeHandle *handle);
void CubeHandle_error (CubeHandle *handle, const char *fmt, ...);
void CubeHandle_string (CubeHandle *handle, const char *str);
void CubeHandle_label (CubeHandle *handle, const char *str);
void CubeHandle_stridx (CubeHandle *handle, StrPool *sbuf, size_t pos);

void CubeHandle_set (CubeHandle *handle, char *args);
void CubeHandle_axis (CubeHandle *handle, char *args);
void CubeHandle_index (CubeHandle *handle, char *args);
void CubeHandle_print (CubeHandle *handle, char *arg);
void CubeHandle_head (CubeHandle *handle, int ext);
void CubeHandle_data (CubeHandle *handle, char *arg);
void CubeHandle_xdata (CubeHandle *handle, char *arg);
void CubeHandle_tdata (CubeHandle *handle, char *arg);
void CubeHandle_choice (CubeHandle *handle, char *arg);

char *test_key (const char *key, char *cmd);
void process (int fd);
void list_commands (IO *out);

void MapTab_load (IO *io);
void MapTab_clean (void);
void MapTab_add (const char *name, const char *path, const char *arg);
void MapTab_list (CubeHandle *handle);
mdmat *MapTab_get (const char *name);

#endif
