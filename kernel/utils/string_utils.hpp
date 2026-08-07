#pragma once

#include "../memory/memory.hpp"
#include <stdarg.h>

constexpr int DEFAULT_BUFFER_ALLOCATION_SIZE = 16;
constexpr int DEFAULT_BUFFER_INCREMENT = 16;

class StringUtils {
public:
	inline static char scancode_to_ascii(unsigned char scancode) {
		// Simple map (AZERTY-ish as in your original code).
		// Unspecified entries default to 0.
		static const char ascii_map[128] = {
			0,   27,  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
			'-', '=', '\b', '\t', 'a',  'z',  'e',  'r',  't',  'y',  'u',  'i',
			'o', 'p', '[',  ']',  '\n', 0,    'q',  's',  'd',  'f',  'g',  'h',
			'j', 'k', 'l',  'm',  '\'', '`',  0,    '\\', 'w',  'x',  'c',  'v',
			'b', 'n', ';',  ',',  '.',  '/',  0,    '*',  0,    ' ',
		};

		if (scancode < 128) return ascii_map[scancode];
		return 0;
	}

	inline static int strcmp(const char *s1, const char *s2) {
		if (!s1 && !s2) return 0;
		if (!s1) return -1;
		if (!s2) return 1;

		int i = 0;
		while (s1[i] != '\0' && s2[i] != '\0') {
			if (s1[i] != s2[i])
				return (unsigned char)s1[i] - (unsigned char)s2[i];
			i++;
		}
		return (unsigned char)s1[i] - (unsigned char)s2[i];
	}

	// Splits in-place by `separator`. Returns argc, writes pointers into args[].
	inline static int split_by(char *str, const char separator, char **args, int max_args) {
		if (!str || !args || max_args <= 0) return 0;

		int arg_count = 0;
		bool in_word = false;

		for (int i = 0; str[i] != '\0'; i++) {
			if (str[i] == separator) {
				str[i] = '\0';
				in_word = false;
			} else if (str[i] != ' ' && str[i] != '\n' && str[i] != '\r' && str[i] != '\t') {
				if (!in_word) {
					if (arg_count < max_args) args[arg_count++] = &str[i];
					in_word = true;
				}
			}
		}
		return arg_count;
	}

	inline static int strlen(const char *str) {
		if (!str) return 0;
		int i = 0;
		while (str[i] != '\0') i++;
		return i;
	}

	// IMPORTANT: This allocates a new buffer. Caller owns it (and must kfree it).
	// Name it explicitly so it isn't mistaken for normal strcat().
	inline static const char *strcat_alloc(const char *str1, const char *str2) {
		if (!str1) str1 = "";
		if (!str2) str2 = "";

		int len1 = strlen(str1);
		int len2 = strlen(str2);

		char *dest = (char *)kmalloc((size_t)len1 + (size_t)len2 + 1);
		if (!dest) return "Out of memory\n";

		int idx = 0;
		for (int i = 0; i < len1; i++) dest[idx++] = str1[i];
		for (int i = 0; i < len2; i++) dest[idx++] = str2[i];
		dest[idx] = '\0';
		return dest;
	}

	inline static const char *iota(int val) {
		// Allocate numeric string. Caller owns result if it isn't a static literal.
		// Implemented via long long so INT_MIN works.
		long long v = (long long)val;
		if (v == 0) return "0";

		bool neg = false;
		if (v < 0) { neg = true; v = -v; }

		char map[10] = {'0','1','2','3','4','5','6','7','8','9'};

		int len = 0;
		long long tmp = v;
		while (tmp != 0) { tmp /= 10; len++; }
		if (neg) len++;

		char *str = (char *)kmalloc((size_t)len + 1);
		if (!str) return "";

		str[len] = '\0';

		int i = len - 1;
		while (v != 0 && i >= 0) {
			str[i--] = map[v % 10];
			v /= 10;
		}
		if (neg) str[0] = '-';

		return str;
	}

	// ---- Safe bounded append (prevents heap corruption) ----
	inline static void append(char *dest, size_t dest_capacity, const char *src) {
		if (!dest || dest_capacity == 0) return;
		if (!src) src = "";

		size_t dest_len = (size_t)strlen(dest);
		if (dest_len >= dest_capacity) {
			dest[dest_capacity - 1] = '\0';
			return;
		}

		size_t i = 0;
		while (src[i] != '\0' && (dest_len + i + 1) < dest_capacity) {
			dest[dest_len + i] = src[i];
			i++;
		}
		dest[dest_len + i] = '\0';
	}

	// Legacy unbounded append: keep ONLY if you are 100% sure dest has room.
	inline static void append(char *dest, const char *src) {
		if (!dest || !src) return;
		int dest_len = strlen(dest);
		int i = 0;
		while (src[i] != '\0') {
			dest[dest_len + i] = src[i];
			i++;
		}
		dest[dest_len + i] = '\0';
	}

	inline static char *grow_buffer(char *buf, size_t &capacity, int used, size_t needed) {
		if (needed <= capacity) return buf;

		size_t new_capacity = capacity;
		while (new_capacity < needed) new_capacity += DEFAULT_BUFFER_INCREMENT;

		char *new_buf = (char *)kmalloc(new_capacity);
		if (!new_buf) return nullptr;

		for (int i = 0; i < used; i++) new_buf[i] = buf[i];

		if (buf) kfree(buf);
		capacity = new_capacity;
		return new_buf;
	}

	inline static char *format(const char *fmt, ...) {
		if (!fmt) return nullptr;

		va_list args;
		va_start(args, fmt);

		size_t capacity = DEFAULT_BUFFER_ALLOCATION_SIZE;
		char *buf = (char *)kmalloc(capacity);
		if (!buf) { va_end(args); return nullptr; }

		int out = 0;

		for (int i = 0; fmt[i] != '\0'; i++) {
			if (fmt[i] == '%' && fmt[i + 1] != '\0') {
				i++;

				if (fmt[i] == '%') {
					buf = grow_buffer(buf, capacity, out, (size_t)out + 2);
					if (!buf) { va_end(args); return nullptr; }
					buf[out++] = '%';
				} else if (fmt[i] == 's') {
					const char *s = va_arg(args, const char *);
					if (!s) s = "";
					while (*s) {
						buf = grow_buffer(buf, capacity, out, (size_t)out + 2);
						if (!buf) { va_end(args); return nullptr; }
						buf[out++] = *s++;
					}
				} else if (fmt[i] == 'd') {
					int v = va_arg(args, int);
					const char *num = iota(v);
					while (*num) {
						buf = grow_buffer(buf, capacity, out, (size_t)out + 2);
						if (!buf) { va_end(args); return nullptr; }
						buf[out++] = *num++;
					}
				} else if (fmt[i] == 'c') {
					char c = (char)va_arg(args, int);
					buf = grow_buffer(buf, capacity, out, (size_t)out + 2);
					if (!buf) { va_end(args); return nullptr; }
					buf[out++] = c;
				} else {
					// unknown: output literally
					buf = grow_buffer(buf, capacity, out, (size_t)out + 2);
					if (!buf) { va_end(args); return nullptr; }
					buf[out++] = fmt[i];
				}
			} else {
				buf = grow_buffer(buf, capacity, out, (size_t)out + 2);
				if (!buf) { va_end(args); return nullptr; }
				buf[out++] = fmt[i];
			}
		}

		buf[out] = '\0';
		va_end(args);
		return buf;
	}

	static void strcpy(char *dest, const char *src) {
		if (!dest || !src) return;
		int i = 0;
		while (src[i] != '\0') { dest[i] = src[i]; i++; }
		dest[i] = '\0';
	}

	inline static int find(char *str, char c) {
		if (!str) return -1;
		for (int i = 0; str[i] != '\0'; i++) if (str[i] == c) return i;
		return -1;
	}

	inline static char *slice(char *str, int start, int end) {
		if (!str) return nullptr;
		int length = strlen(str);
		if (start >= end) return nullptr;
		if (start < 0 || end > length) return nullptr;

		int size = end - start;
		char *res = (char *)kmalloc((size_t)size + 1);
		if (!res) return nullptr;

		for (int i = 0; i < size; i++) res[i] = str[start + i];
		res[size] = '\0';
		return res;
	}

	inline static bool contains(const char *str, char c) {
		if (!str) return false;
		for (int i = 0; str[i] != '\0'; i++) if (str[i] == c) return true;
		return false;
	}
};
