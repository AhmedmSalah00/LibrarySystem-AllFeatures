#include "loadCsvData.h" // Include its own header
#include "utility.h"     // For UserProfile, Book, MAX_ID, etc. (indirectly via loadCsvData.h now)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Declare static functions here, so they are only visible within this file
// Removed 'static' keyword from here as they are no longer declared in the header
int loadStudentValuesFromCSV(char *line, char *sFields[STU_COL_NUM]);
int loadBookValuesFromCSV(char *line, char *lFields[BK_COL_NUM]);


/**
 * @brief Reads student profile from StudentData.csv based on student ID.
 * @param stuId Student ID to search for.
 * @param st Pointer to UserProfile struct to store found data.
 * @return PASS if found, FAIL otherwise.
 */
// Changed StuProfile to UserProfile in definition
int getStudentProfileFromCSV(char stuId[MAX_ID], UserProfile *st) {
    FILE *fps = fopen(STU_DATA_CSV_FILE, "r");
    if (fps == NULL) {
        printf("Error: Could not open StudentData.csv. Please ensure it exists.\n");
        return FAIL;
    }

    char inBuffer[BUFFER_SZ];
    char *sFields[STU_COL_NUM];
    long rowNum = 0L;

    while (fgets(inBuffer, BUFFER_SZ - 1, fps) != NULL) {
        if (++rowNum == 1) continue; // Skip header

        if (strlen(inBuffer) == 0) continue; // Skip empty lines

        if (loadStudentValuesFromCSV(inBuffer, sFields) == FAIL) {
            // printf("Warning: Error parsing student line %ld in StudentData.csv. Skipping.\n", rowNum);
            continue;
        }

        if (strcmp(sFields[STU_ID_COL], stuId) == 0) {
            strcpy(st->userID, sFields[STU_ID_COL]); // Map to userID
            strcpy(st->fName, sFields[STU_FNAME_COL]);
            strcpy(st->lName, sFields[STU_LNAME_COL]);
            strcpy(st->faculty, sFields[STU_FACULTY_COL]);
            // Initialize new fields for UserProfile if reading from old CSV
            st->fineAmount = 0;
            strcpy(st->forbiddenUntil, "N/A");
            fclose(fps);
            return PASS;
        }
    }
    fclose(fps);
    return FAIL;
}

/**
 * @brief Parses a line from StudentData.csv into an array of string pointers.
 * Handles quoted fields and delimiters.
 * @param line The input line string.
 * @param sFields Array of char pointers to store field values.
 * @return PASS if successful, FAIL otherwise.
 */
int loadStudentValuesFromCSV(char *line, char *sFields[STU_COL_NUM]) {
    char delim = ',';
    if (line == NULL)
        return FAIL;

    line[strcspn(line, "\r\n")] = '\0';

    char *cptr = line;
    int fld = 0;
    int inquote = FALSE;
    char ch;

    sFields[fld] = cptr;
    while ((ch = *cptr) != '\0' && fld < STU_COL_NUM) {
        if (ch == '"') {
            if (!inquote) {
                sFields[fld] = cptr + 1;
            } else {
                *cptr = '\0';
            }
            inquote = !inquote;
        } else if (ch == delim && !inquote) {
            *cptr = '\0';
            sFields[++fld] = cptr + 1;
        }
        cptr++;
    }
    if (fld != STU_COL_NUM - 1) {
        return FAIL;
    }
    return PASS;
}

/**
 * @brief Reads all book data from BookData.csv into the global `books_array`.
 * Dynamically allocates memory for `books_array`.
 * @return Number of books read, or FAIL if error.
 */
int readBooksFromCSV() {
    FILE *fpl = fopen(BK_DATA_CSV_FILE, "r");
    if (fpl == NULL) {
        printf("Error: Could not open BookData.csv. Please ensure it exists.\n");
        return FAIL; // Return failure if the file does not exist
    }

    int numRows = getRowCountFromCSV(fpl);
    if (numRows <= 1) {
        fclose(fpl);
        return 0; // No books to read
    }
    int actualBookCount = numRows - 1;

    if (books_array != NULL) {
        free(books_array);
        books_array = NULL;
    }

    books_array = (Book *)malloc(actualBookCount * sizeof(Book));
    if (books_array == NULL) {
        printf("Error: Memory allocation failed for books.\n");
        fclose(fpl);
        return FAIL;
    }

    rewind(fpl);

    char inBuffer[BUFFER_SZ];
    char *lFields[BK_COL_NUM];
    int rowNum = 0;
    int currentBookIndex = 0;

    while (fgets(inBuffer, BUFFER_SZ - 1, fpl) != NULL) {
        if (++rowNum == 1) continue; // Skip header

        if (strlen(inBuffer) == 0) continue; // Skip empty lines

        if (loadBookValuesFromCSV(inBuffer, lFields) == FAIL) {
            printf("Warning: Error parsing line %d in BookData.csv. Skipping.\n", rowNum);
            continue;
        }

        if (currentBookIndex < actualBookCount) {
            strcpy(books_array[currentBookIndex].bookID, lFields[BOOK_ID_COL]);
            strcpy(books_array[currentBookIndex].title, lFields[TITLE_COL]);
            strcpy(books_array[currentBookIndex].author, lFields[AUTHOR_COL]);
            strcpy(books_array[currentBookIndex].genre, lFields[GENRE_COL]);
            strcpy(books_array[currentBookIndex].year, lFields[YEAR_COL]);
            strcpy(books_array[currentBookIndex].stockCount, lFields[STK_CNT_COL]);
            strcpy(books_array[currentBookIndex].shelfNo, lFields[SHELF_NO_COL]);
            currentBookIndex++;
        }
    }

    fclose(fpl);
    return currentBookIndex; // Return the number of books read
}

/**
 * @brief Parses a line from BookData.csv into an array of string pointers.
 * Handles quoted fields and delimiters.
 * @param line The input line string.
 * @param lFields Array of char pointers to store field values.
 * @return PASS if successful, FAIL otherwise.
 */
int loadBookValuesFromCSV(char *line, char *lFields[BK_COL_NUM]) {
    char delim = ',';
    if (line == NULL)
        return FAIL;

    line[strcspn(line, "\r\n")] = '\0';

    char *cptr = line;
    int fld = 0;
    int inquote = FALSE;
    char ch;

    lFields[fld] = cptr;
    while ((ch = *cptr) != '\0' && fld < BK_COL_NUM) {
        if (ch == '"') {
            if (!inquote) {
                lFields[fld] = cptr + 1;
            } else {
                *cptr = '\0';
            }
            inquote = !inquote;
        } else if (ch == delim && !inquote) {
            *cptr = '\0';
            lFields[++fld] = cptr + 1;
        }
        cptr++;
    }
    if (fld != BK_COL_NUM - 1) {
        return FAIL;
    }
    return PASS;
}

/**
 * @brief Counts the number of rows in a CSV file.
 * @param fpl File pointer to the CSV file.
 * @return Number of rows.
 */
int getRowCountFromCSV(FILE *fpl) {
    int n = 0;
    char tBuffer[BUFFER_SZ];
    long currentPos = ftell(fpl);
    rewind(fpl);

    while (fgets(tBuffer, BUFFER_SZ - 1, fpl) != NULL) {
        n++;
    }
    fseek(fpl, currentPos, SEEK_SET);
    return n;
}

/**
 * @brief Writes all current book data from `books_array` back to BookData.csv.
 */
void writeBooksToCSV() {
    FILE *fpl = fopen(TMP_BK_DATA_CSV_FILE, "w");
    if (fpl == NULL) {
        printf("Error: Could not open temporary book data file for writing.\n");
        return;
    }

    fprintf(fpl, "BookID,Title,Author,Genre,Year of Publication,Stock Count,Shelf Number\n");

    for (int i = 0; i < bookCount; i++) {
        fprintf(fpl, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n",
                books_array[i].bookID, books_array[i].title, books_array[i].author,
                books_array[i].genre, books_array[i].year, books_array[i].stockCount,
                books_array[i].shelfNo);
    }

    fclose(fpl);

    remove(BK_DATA_CSV_FILE); // Delete the old file
    rename(TMP_BK_DATA_CSV_FILE, BK_DATA_CSV_FILE); // Rename the temp file to the original
    printf("\nBook data saved successfully.\n");
}

