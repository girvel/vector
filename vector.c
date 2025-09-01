#include <stdbool.h>

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

EXPORT bool vector_eq(const vector *self, const vector *other) {
    if (self->len != other->len) {
        return false;
    }

    for (int i = 0; i < self->len; i++) {
        if (self->items[i] != other->items[i]) {
            return false;
        }
    }

    return true;
}
