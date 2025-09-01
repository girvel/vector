#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>

// Define a cross-platform EXPORT macro for public API functions
#if defined _WIN32 || defined __CYGWIN__
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT __attribute__((visibility("default")))
#endif

#define MAX_LEN 4

typedef struct {
    int len;
    double items[MAX_LEN];
} vector;

static inline int get_index(char key) {
    if (key == 'x' || key == 'r') return 0;
    if (key == 'y' || key == 'g') return 1;
    if (key == 'z' || key == 'b') return 2;
    if (key == 'w' || key == 'a') return 3;
    return -1;
}

EXPORT void vector_unm_mut(vector *self) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] *= -1;
    }
}

EXPORT void vector_add_mut(vector *self, const vector *other) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] += other->items[i];
    }
}

EXPORT void vector_sub_mut(vector *self, const vector *other) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] -= other->items[i];
    }
}

EXPORT void vector_mul_mut(vector *self, double k) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] *= k;
    }
}

EXPORT void vector_div_mut(vector *self, double k) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] /= k;
    }
}

EXPORT void vector_mod_mut(vector *self, double k) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] = (int)self->items[i] % (int)k;
    }
}

EXPORT void vector_map_mut(vector *self, double (*f)(double)) {
    for (int i = 0; i < self->len; i++) {
        self->items[i] = f(self->items[i]);
    }
}

EXPORT bool vector_eq(const vector *self, const vector *other) {
    if (self->len != other->len) return false;
    for (int i = 0; i < self->len; i++) {
        if (self->items[i] != other->items[i]) return false;
    }
    return true;
}

EXPORT bool vector_lt(const vector *self, const vector *other) {
    for (int i = 0; i < self->len; i++) {
        if (self->items[i] >= other->items[i]) return false;
    }
    return true;
}

EXPORT bool vector_le(const vector *self, const vector *other) {
    for (int i = 0; i < self->len; i++) {
        if (self->items[i] > other->items[i]) return false;
    }
    return true;
}

EXPORT double vector_abs(const vector *self) {
    double result = 0;
    for (int i = 0; i < self->len; i++) {
        result += self->items[i] * self->items[i];
    }
    return sqrt(result);
}

EXPORT double vector_abs2(const vector *self) {
    double result = 0;
    for (int i = 0; i < self->len; i++) {
        result += fabs(self->items[i]);
    }
    return result;
}

EXPORT void vector_normalized_mut(vector *self) {
    double abs_val = vector_abs(self);
    if (abs_val > 0) {
        vector_div_mut(self, abs_val);
    }
}

EXPORT void vector_normalized2_mut(vector *self) {
    if (self->len != 2) return;

    if (fabs(self->items[0]) > fabs(self->items[1])) {
        self->items[0] = copysign(1, self->items[0]);
        self->items[1] = 0;
    } else if (self->items[1] != 0) {
        self->items[0] = 0;
        self->items[1] = copysign(1, self->items[1]);
    }
}

EXPORT void vector_swizzle(const vector *self, const char *swizzle_str, vector *result) {
    size_t swizzle_len = strlen(swizzle_str);
    result->len = swizzle_len;
    for (size_t i = 0; i < swizzle_len; i++) {
        result->items[i] = self->items[get_index(swizzle_str[i])];
    }
}

EXPORT const char* vector_name_from_direction(const vector *self) {
    if (self->len != 2) return NULL;
    if (self->items[0] == 0) {
        if (self->items[1] == 1) return "down";
        if (self->items[1] == -1) return "up";
    } else if (self->items[1] == 0) {
        if (self->items[0] == 1) return "right";
        if (self->items[0] == -1) return "left";
    }
    return NULL;
}

EXPORT bool vector_from_hex(const char *hex_str, vector *result) {
    size_t str_len = strlen(hex_str);
    if (str_len % 2 != 0 || str_len == 0 || str_len > (MAX_LEN * 2)) {
        return false;
    }

    errno = 0;
    char *endptr;
    long int all_value = strtol(hex_str, &endptr, 16);

    if (*endptr != '\0' || errno == ERANGE) {
        return false;
    }

    result->len = str_len / 2;
    for (int i = result->len - 1; i >= 0; i--) {
        result->items[i] = ((double) (all_value & 0xFF)) / 255.0;
        all_value >>= 8;
    }

    return true;
}
