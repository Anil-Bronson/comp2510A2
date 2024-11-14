#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// to differentiate student types
typedef enum {DOMESTIC, INTERNATIONAL} StudentType;

// struct for domestic students
struct DomesticStudent {
    char first_name[50];
    char last_name[50];
    char month_of_birth_str[4]; // Month as a string
    int year_of_birth;
    int day_of_birth;
    double gpa;
};

// struct for international students
struct InternationalStudent {
    char first_name[50];
    char last_name[50];
    char month_of_birth_str[4]; // Month as a string
    int year_of_birth;
    int day_of_birth;
    double gpa;
    int toefl; // TOEFL score
};

// function prototypes
int count_lines(const char *filename);
void merge(void *students, int left, int mid, int right, int student_size, int (*compare)(const void*, const void*));
void merge_sort(void *students, int left, int right, int student_size, int (*compare)(const void*, const void*));
void write_to_file(const char *output_file, const void *students, int count, StudentType type, int student_size);
int compare_domestic(const void *a, const void *b);
int compare_international(const void *a, const void *b);
void read_domestic_students(const char *input_file, struct DomesticStudent students[], int count);
void read_international_students(const char *input_file, struct InternationalStudent students[], int count);

int main(int argc, char const *argv[]) {

    if(argc != 4) {
        printf("Usage %s <input file> <output file> <option>\n", argv[0]);
        return -1;
    }

    // get the inputs from the command line
    const char *input_file = argv[1];
    const char *output_file = argv[2];
    const int option = atoi(argv[3]);

    // option error checking
    if(option < 1 || option > 3 ) {
        printf("Error, option must be between 1 and 3\n");
    }

    int lines = count_lines(input_file);

    // check to make sure the file is not empty
    if(lines < 1) {
        printf("Input file empty\n");
    }

    if (option == 1) {
        struct DomesticStudent students[lines];
        read_domestic_students(input_file, students, lines);
        merge_sort(students, 0, lines - 1, sizeof(struct DomesticStudent), compare_domestic);
        write_to_file(output_file, students, lines, DOMESTIC, sizeof(struct DomesticStudent));
    } else if (option == 2) {
        struct InternationalStudent students[lines];
        read_international_students(input_file, students, lines);
        merge_sort(students, 0, lines - 1, sizeof(struct InternationalStudent), compare_international);
        write_to_file(output_file, students, lines, INTERNATIONAL, sizeof(struct InternationalStudent));
    } else if (option == 3) {
        // Handle both types of students
        struct DomesticStudent domestic_students[lines];
        struct InternationalStudent international_students[lines];
        read_domestic_students(input_file, domestic_students, lines);
        read_international_students(input_file, international_students, lines);
        merge_sort(domestic_students, 0, lines - 1, sizeof(struct DomesticStudent), compare_domestic);
        merge_sort(international_students, 0, lines - 1, sizeof(struct InternationalStudent), compare_international);
        write_to_file(output_file, domestic_students, lines, DOMESTIC, sizeof(struct DomesticStudent));
        write_to_file(output_file, international_students, lines, INTERNATIONAL, sizeof(struct InternationalStudent));
    }

    return 0;
}
int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");

    if(file == NULL) {
        printf("Error opening file %s\n", filename);
        return -1;
    }

    int count = 0;
    char c;
    while((c = fgetc(file)) != EOF) {
        if(c == '\n') {
            count++;
        }
    }

    fclose(file);
    return count;
}

void read_domestic_students(const char *input_file, struct DomesticStudent students[], int count) {
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", input_file);
        exit(-1);
    }

    char month_str[4];
    for (int i = 0; i < count; i++) {
        fscanf(file, "%s %s %3s-%d-%d %lf",
               students[i].first_name,
               students[i].last_name,
               month_str,
               &students[i].day_of_birth,
               &students[i].year_of_birth,
               &students[i].gpa);

        // Store the month string directly
        strncpy(students[i].month_of_birth_str, month_str, 3);
        students[i].month_of_birth_str[3] = '\0'; // Ensure null-termination
    }

    fclose(file);
}

void read_international_students(const char *input_file, struct InternationalStudent students[], int count) {
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", input_file);
        exit(-1);
    }

    char month_str[4];
    for (int i = 0; i < count; i++) {
        fscanf(file, "%s %s %3s-%d-%d %lf %d",
               students[i].first_name,
               students[i].last_name,
               month_str,
               &students[i].day_of_birth,
               &students[i].year_of_birth,
               &students[i].gpa,
               &students[i].toefl);

        // Store the month string directly
        strncpy(students[i].month_of_birth_str, month_str, 3);
        students[i].month_of_birth_str[3] = '\0'; // Ensure null-termination
    }

    fclose(file);
}

void merge(void *students, int left, int mid, int right, int student_size, int (*compare)(const void*, const void*)) {
    int n1 = mid - left + 1; // size of the left half
    int n2 = right - mid;    // size of the right half

    // allocate memory for the temp arrays
    void *L = malloc(student_size * n1);
    void *R = malloc(student_size * n2);

    // copy data into the temp arrays
    memcpy(L, (char*)students + left * student_size, student_size * n1);
    memcpy(R, (char*)students + (mid + 1) * student_size, student_size * n2);

    // merge the two halves back into the original array
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if(compare((char*)L + i * student_size, (char*)R + j * student_size) > 0) {
            memcpy((char*)students + k * student_size, (char*)L + i * student_size, student_size);
            i++;
        } else {
            memcpy((char*)students + k * student_size, (char*)R + j * student_size, student_size);
            j++;
        }
        k++;
    }
    while (i < n1) {
        memcpy((char*)students + k * student_size, (char*)L + i * student_size, student_size);
        i++;
        k++;
    }
    while (j < n2) {
        memcpy((char*)students + k * student_size, (char*)R + j * student_size, student_size);
        j++;
        k++;
    }
    free(L);
    free(R);
}

void merge_sort(void *students, int left, int right, int student_size, int (*compare)(const void*, const void*)) {
    if(left < right) {
        int mid = left + (right - left) / 2;        // calculate midpoint
        merge_sort(students, left, mid, student_size, compare);        // sort left half
        merge_sort(students, mid + 1, right, student_size, compare);    // sort right half
        merge(students, left, mid, right, student_size, compare);           // merge 2 halves
    }
}

void write_to_file(const char *output_file, const void *students, int count, StudentType type, int student_size) {
    FILE *file = fopen(output_file, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", output_file);
        exit(-1);
    }

    if (type == DOMESTIC) {
        const struct DomesticStudent *domestic_students = (const struct DomesticStudent *)students;
        for (int i = 0; i < count; i++) {
            fprintf(file, "First Name: %s\n", domestic_students[i].first_name);
            fprintf(file, "Last Name: %s\n", domestic_students[i].last_name);
            fprintf(file, "Date of Birth: %s-%02d-%d\n", domestic_students[i].month_of_birth_str, domestic_students[i].day_of_birth, domestic_students[i].year_of_birth);
            fprintf(file, "GPA: %.2f\n", domestic_students[i].gpa);
            fprintf(file, "Student Type: Domestic\n\n");
        }
    } else if (type == INTERNATIONAL) {
        const struct InternationalStudent *international_students = (const struct InternationalStudent *)students;
        for (int i = 0; i < count; i++) {
            fprintf(file, "First Name: %s\n", international_students[i].first_name);
            fprintf(file, "Last Name: %s\n", international_students[i].last_name);
            fprintf(file, "Date of Birth: %s-%02d-%d\n", international_students[i].month_of_birth_str, international_students[i].day_of_birth, international_students[i].year_of_birth);
            fprintf(file, "GPA: %.2f\n", international_students[i].gpa);
            fprintf(file, "TOEFL Score: %d\n", international_students[i].toefl);
            fprintf(file, "Student Type: International\n\n");
        }
    }

    fclose(file);
}

int compare_domestic(const void *a, const void *b) {
    const struct DomesticStudent *student_a = (const struct DomesticStudent *)a;
    const struct DomesticStudent *student_b = (const struct DomesticStudent *)b;

    // compare year of birth
    if(student_a->year_of_birth != student_b->year_of_birth) {
        return student_a->year_of_birth - student_b->year_of_birth;
    }

    // compare month of birth
    if(student_a->month_of_birth_str != student_b->month_of_birth_str) {
        return strcmp(student_a->month_of_birth_str, student_b->month_of_birth_str);
    }

    // compare day of birth
    if(student_a->day_of_birth != student_b->day_of_birth) {
        return student_a->day_of_birth - student_b->day_of_birth;
    }

    // compare last names
    int cmp = strcmp(student_a->last_name, student_b->last_name);
    if(cmp != 0) return cmp;

    // compare first names
    return strcmp(student_a->first_name, student_b->first_name);
}

int compare_international(const void *a, const void *b) {
    const struct InternationalStudent *student_a = (const struct InternationalStudent *)a;
    const struct InternationalStudent *student_b = (const struct InternationalStudent *)b;

    // compare year of birth
    if(student_a->year_of_birth != student_b->year_of_birth) {
        return student_a->year_of_birth - student_b->year_of_birth;
    }

    // compare month of birth
    if(student_a->month_of_birth_str != student_b->month_of_birth_str) {
        return strcmp(student_a->month_of_birth_str, student_b->month_of_birth_str);
    }

    // compare day of birth
    if(student_a->day_of_birth != student_b->day_of_birth) {
        return student_a->day_of_birth - student_b->day_of_birth;
    }

    // compare last names
    int cmp = strcmp(student_a->last_name, student_b->last_name);
    if(cmp != 0) return cmp;

    // compare first names
    return strcmp(student_a->first_name, student_b->first_name);
}
