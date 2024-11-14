#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// to differentiate student types
typedef enum {DOMESTIC, INTERNATIONAL} StudentType;

// struct both students
struct Student {
    char first_name[50];
    char last_name[50];
    char month_of_birth[4];
    int year_of_birth;
    int day_of_birth;
    double gpa;
    int toefl; // -1 means they don't have toefl
    StudentType type;
};

// function prototypes
int count_lines(const char *filename);
void merge(struct Student students[], int left, int mid, int right);
void merge_sort(struct Student students[], int left, int right);
void write_to_file(const char *output_file, const struct Student students[], int count, int option);
int compare(const struct Student *a, const struct Student *b);
void read_students(const char *input_file, struct Student students[], int count);

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

    struct Student students[lines];
    read_students(input_file, students, lines);

    merge_sort(students, 0, lines - 1);
    write_to_file(output_file, students, lines, option);

    return 0;
}

void read_students(const char *input_file, struct Student students[], int count) {
    FILE *file = fopen(input_file, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", input_file);
        exit(-1);
    }

    char month_str[4];
    char type_str;
    for (int i = 0; i < count; i++) {
        int result = fscanf(file, "%s %s %3s-%d-%d %lf %c",
                            students[i].first_name,
                            students[i].last_name,
                            month_str,
                            &students[i].day_of_birth,
                            &students[i].year_of_birth,
                            &students[i].gpa,
                            &type_str);

        if (result != 7 && result != 8) {
            printf("Error reading data for input file, please match specified format: <fname> <lname> "
                   "<birth month/day/year> <gpa> <tofel> <dom/inter>\n");
            exit(-1);
        }

        if (type_str == 'I') {
            fscanf(file, "%d", &students[i].toefl);
            students[i].type = INTERNATIONAL;
        } else {
            students[i].toefl = -1;
            students[i].type = DOMESTIC;
        }

        // Store the month string directly
        strncpy(students[i].month_of_birth, month_str, 3);
        students[i].month_of_birth[3] = '\0'; // Ensure null-termination

    }

    fclose(file);
}


void merge(struct Student students[], int left, int mid, int right) {
    int n1 = mid - left + 1; // size of the left half
    int n2 = right - mid;    // size of the right half

    // allocate mem for the temp arrays
    struct Student *L = malloc(sizeof(struct Student) * n1);
    struct Student *R = malloc(sizeof(struct Student) * n2);

    // copy data into the temp arrays
    for (int i = 0; i < n1; i++)
        L[i] = students[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = students[mid + 1 + j];

    // merge the two halves back into the original array
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if(compare(&L[i], &R[j]) > 0) {
            students[k++] = L[i++];
        } else {
            students[k++] = R[j++];
        }
    }
    while (i < n1) {
        students[k++] = L[i++];
    }
    while (j < n2) {
        students[k++] = R[j++];
    }
    free(L);
    free(R);
}

void merge_sort(struct Student students[], int left, int right) {
    if(left < right) {
        const int mid = left + (right - left) / 2;        // calculate midpoint
        merge_sort(students, left, mid);        // sort left half
        merge_sort(students, mid + 1, right);    // sort right half
        merge(students, left, mid, right);           // merge 2 halves
    }
}

void write_to_file(const char *output_file, const struct Student students[], int count, int option) {
    FILE *file = fopen(output_file, "w");
    if (file == NULL) {
        printf("Error opening file %s\n", output_file);
        exit(-1);
    }

    for (int i = 0; i < count; i++) {
        int write = 0;

        switch (option) {
            case 1: // Domestic students only
                if (students[i].type == DOMESTIC) {
                    write = 1;
                }
            break;
            case 2: // International students only
                if (students[i].type == INTERNATIONAL) {
                    write = 1;
                }
            break;
            case 3: // Both domestic and international students
                write = 1;
            break;
            default:
                printf("Error, try again");
        }

        if (write) {
            fprintf(file, "%s %s %s-%02d-%d %.2f",
                    students[i].first_name,
                    students[i].last_name,
                    students[i].month_of_birth,
                    students[i].day_of_birth,
                    students[i].year_of_birth,
                    students[i].gpa);
            if (students[i].type == INTERNATIONAL) {
                fprintf(file, " %d I\n", students[i].toefl);
            } else {
                fprintf(file, " D\n");
            }
        }
    }

    fclose(file);
}

int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");

    if(file == NULL) {
        printf("Error opening file\n");
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

int compare(const struct Student *a, const struct Student *b) {
    int cmp;

    // compare year of birth
    if(a->year_of_birth != b->year_of_birth) {
        return b->year_of_birth - a->year_of_birth;
    }

    // compare month of birth
    cmp = strcmp(b->month_of_birth, a->month_of_birth);
    if(cmp != 0) return cmp;

    // compare day of birth
    if(a->day_of_birth != b->day_of_birth) {
        return b->day_of_birth - a->day_of_birth;
    }

    // compare last names
    cmp = strcmp(a->last_name, b->last_name);
    if(cmp != 0) return cmp;

    // compare first names
    cmp = strcmp(a->first_name, b->first_name);
    if(cmp != 0) return cmp;

    if(a->toefl != b->toefl) {
        return b->toefl - a->toefl;
    } else if(a->toefl != 0) {
        return -1;
    } else if(b->toefl != 0) {
        return -1;
    }

    // domestic > international
    if(a->type == DOMESTIC && b->type == INTERNATIONAL) {
        return 1;
    }

    // international < domestic
    return (a->type == INTERNATIONAL && b->type == DOMESTIC );
}
