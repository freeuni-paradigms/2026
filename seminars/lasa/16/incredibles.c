typedef struct {
    int violet;
    char *dash[2];
    char superboy[4];
} superhero;

static superhero **pixar(superhero *syndrome);

static superhero * theincredibles(short *frozone, superhero elastigirl){
    frozone += elastigirl.superboy[*frozone];                             // line 1
    ((superhero *)((superhero *) elastigirl.dash[0])->dash)->violet = 400;  // line 2 
    return *pixar(&elastigirl) + 10;                                        // line 3
}
