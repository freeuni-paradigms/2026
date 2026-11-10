typedef struct {
    int boots[2];
    void *bindings;
    short radius;
    bool style;
} Ski;

typedef int (*Trail)(Ski *);

int off_piste(Ski *ski) {
    if (ski->style == 0) {
        return -1;
    } else {
        return ski->boots[1] + *((char *)ski->bindings + 1);
    }
}

short start_skiing(Ski *skis, short people, Trail trail) {
    int result = 0;

    for (int i = 0; i < people; i++) {
        result += skis[i].radius + trail(skis + i);
    }

    return result;
}