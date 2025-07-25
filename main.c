// Standard library files
#include <stdio.h>
#include <stdlib.h> // For exit(), malloc, free, realloc
#include <string.h> // For strcmp, strcpy, strlen, strcspn
#include <ctype.h>  // For tolower, isdigit

// Custom header files
#include "utility.h"     // Contains common utilities and global declarations
#include "loadCsvData.h" // Contains CSV loading/saving functions
#include "library_menus.h" // Contains all menu navigation functions

// --- Main Function ---
int main() {
    // Load books from CSV at startup
    bookCount = readBooksFromCSV();
    if (bookCount == FAIL) {
        printf("Error loading book data. Starting with empty book list.\n");
        bookCount = 0;
        if (books_array != NULL) {
            free(books_array);
            books_array = NULL;
        }
    }

    // Start the initial login menu
    displayInitialLoginMenu();

    // Free dynamically allocated memory for books before exiting
    if (books_array != NULL) {
        writeBooksToCSV(); // Ensure books are saved before exiting
        free(books_array);
        books_array = NULL;
    }

    return 0;
}

