typedef struct {
    size_t length;
    size_t used;
    int *data;
} arraylist_t;

int al_init(arraylist_t *, size_t);
void al_destroy(arraylist_t *);
int al_append(arraylist_t *, int);
int al_remove(arraylist_t *, int *);