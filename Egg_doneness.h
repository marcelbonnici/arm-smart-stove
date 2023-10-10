/*A set of egg donenesses, from soft to hard boiled
Each doneness is stored as a C structure. */

struct Doneness{
    char name[17];
    double duration;
};
struct Doneness *doneness_ptr; //point to selected doneness
struct Doneness Soft={
    .name="Soft Boiled",
    .duration=2
};
struct Doneness Medium={
    .name="Medium Boiled",
    .duration=3
};
struct Doneness Hard={
    .name="Hard Boiled",
    .duration=6
};