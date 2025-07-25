#include "userMenu.h" // Include its own header
#include "utility.h"  // For general utilities and global variables
#include "loadCsvData.h" // For getStudentProfileFromCSV, writeBooksToCSV
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Displays the student menu.
 */
void studentMenu() {
    char choice;
    do {
        printStudentWelcomeMsg(currentStudentProfile);
        printf("Student Menu:\n\n");
        printf("A) View All Books\n");
        printf("B) Search Books\n");
        printf("C) Add New Book\n"); // Now accessible directly
        printf("D) Update Book Details\n"); // Now accessible directly
        printf("E) Delete Book\n"); // Now accessible directly
        printf("ESC) Log Out and Exit Application\n\n");
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'a': viewAllBooks(TRUE); break; // TRUE indicates it's a student view context
            case 'b': searchBooks(TRUE); break; // TRUE indicates it's a student view context
            case 'c': addBook(); break;
            case 'd': updateBook(); break;
            case 'e': deleteBook(); break;
            case ESC:
                writeBooksToCSV(); // Save books before exiting
                exit(0);
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to continue...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Gets the index of a book in the `books_array` by its ID.
 * @param bookID The ID of the book to search for.
 * @return The index of the book if found, -1 otherwise.
 */
int getBookIndexByID(char bookID[MAX_ID]) {
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(bookID, books_array[i].bookID) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Displays books by page.
 * @param sIndex Starting index for display.
 * @param lIndex Ending index for display.
 * @param total Total number of books.
 * @param isStudentView Flag to determine welcome message (student).
 */
void viewBooksByPage(int sIndex, int lIndex, int total, int isStudentView) {
    if (isStudentView) {
        printStudentWelcomeMsg(currentStudentProfile);
    } else {
        printWelcomeMsg(); // Fallback for other contexts if needed
    }

    printf("Showing books from %d to %d out of %d.\n\n", sIndex + 1, lIndex, total);

    // Table Header
    printf("+----------+--------------------------------------------------+--------------------+---------------+------+---------+-------+\n");
    printf("| Book ID  | Title                                            | Author             | Genre         | Year | ShelfNo | Stock |\n");
    printf("+----------+--------------------------------------------------+--------------------+---------------+------+---------+-------+\n");

    for (int i = sIndex; i < lIndex; i++) {
        printf("| %-8s | %-48.48s | %-18.18s | %-13.13s | %-4s | %-7s | %-5s |\n",
               books_array[i].bookID, books_array[i].title, books_array[i].author,
               books_array[i].genre, books_array[i].year, books_array[i].shelfNo,
               books_array[i].stockCount);
    }
    printf("+----------+--------------------------------------------------+--------------------+---------------+------+---------+-------+\n\n");

    printf("Press 'B' for Previous, 'N' for Next, 'ESC' to go back to menu.\n");
}

/**
 * @brief Allows viewing all books with pagination.
 * @param isStudentView Flag to determine welcome message (student).
 */
void viewAllBooks(int isStudentView) {
    if (bookCount == 0) {
        if (isStudentView) printStudentWelcomeMsg(currentStudentProfile); else printWelcomeMsg();
        printf("No books available in the library.\n\n");
        printf("Press Enter to go back.\n");
        getch_portable();
        studentMenu();
        return;
    }

    int pageSize = READ_ARRAY_SZ; // Using READ_ARRAY_SZ as page size
    int lIndex = 0;
    int hIndex = (pageSize > bookCount) ? bookCount : pageSize;
    char choice;

    do {
        viewBooksByPage(lIndex, hIndex, bookCount, isStudentView);
        choice = tolower(getch_portable());

        if (choice == 'b') { // Previous page
            if (lIndex > 0) {
                lIndex -= pageSize;
                if (lIndex < 0) lIndex = 0;
                hIndex = lIndex + pageSize;
                if (hIndex > bookCount) hIndex = bookCount;
            }
        } else if (choice == 'n') { // Next page
            if (hIndex < bookCount) {
                lIndex += pageSize;
                hIndex += pageSize;
                if (hIndex > bookCount) hIndex = bookCount;
            }
        } else if (choice == ESC) {
            break;
        } else {
            printf("\nInvalid input. Press 'B', 'N', or 'ESC'.\n");
            printf("Press Enter to acknowledge...\n");
            getch_portable();
        }
    } while (choice != ESC);

    studentMenu();
}

/**
 * @brief Adds a new book to the library.
 */
void addBook() {
    Book b;
    printWelcomeMsg();

    printf("--- Add New Book ---\n\n");
    printf("Book ID (e.g., BK001): ");
    getString(b.bookID, MAX_ID);

    if (getBookIndexByID(b.bookID) < 0) { // Check if ID already exists
        printf("Title: ");
        getString(b.title, MAX_TITLE);
        printf("Author: ");
        getString(b.author, MAX_NAME);
        printf("Genre: ");
        getString(b.genre, MAX_NAME);
        printf("Year of Publication: ");
        getString(b.year, MAX_YR);
        printf("Shelf Number: ");
        getString(b.shelfNo, MAX_ID);
        printf("Stock Count: ");
        getString(b.stockCount, MAX_ID);

        // Reallocate memory for the new book
        Book *temp_books = (Book *)realloc(books_array, (bookCount + 1) * sizeof(Book));
        if (temp_books == NULL) {
            printf("\nError: Failed to allocate memory. Book not added.\n");
            printf("Press Enter to continue...\n");
            getch_portable();
            studentMenu();
            return;
        }
        books_array = temp_books;

        // Add the new book to the end of the array
        strcpy(books_array[bookCount].bookID, b.bookID);
        strcpy(books_array[bookCount].title, b.title);
        strcpy(books_array[bookCount].author, b.author);
        strcpy(books_array[bookCount].genre, b.genre);
        strcpy(books_array[bookCount].year, b.year);
        strcpy(books_array[bookCount].stockCount, b.stockCount);
        strcpy(books_array[bookCount].shelfNo, b.shelfNo);
        bookCount++;

        printf("\nBook '%s' added successfully!\n", b.title);
    } else {
        printf("\nError: Book ID '%s' already exists. Please use a unique ID.\n", b.bookID);
    }

    printf("\nPress Enter to add another book or ESC to go back to Menu.\n");
    if (getch_portable() == ESC) {
        studentMenu();
    } else {
        addBook();
    }
}

/**
 * @brief Updates details of an existing book.
 */
void updateBook() {
    char bookID[MAX_ID];
    int index;
    Book b_temp; // Use a temporary struct for new values

    printWelcomeMsg();
    printf("--- Update Book Details ---\n\n");
    printf("Enter Book ID to update: ");
    getString(bookID, MAX_ID);

    index = getBookIndexByID(bookID);
    if (index >= 0) {
        printf("\n--- Current Details for Book ID: %s ---\n", bookID);
        printf("Title: %s\n", books_array[index].title);
        printf("Author: %s\n", books_array[index].author);
        printf("Genre: %s\n", books_array[index].genre);
        printf("Year: %s\n", books_array[index].year);
        printf("Shelf No: %s\n", books_array[index].shelfNo);
        printf("Stock Count: %s\n\n", books_array[index].stockCount);

        printf("--- Enter New Details (leave blank to keep current) ---\n");
        printf("New Title: ");
        getString(b_temp.title, MAX_TITLE);
        if (strlen(b_temp.title) > 0) strcpy(books_array[index].title, b_temp.title);

        printf("New Author: ");
        getString(b_temp.author, MAX_NAME);
        if (strlen(b_temp.author) > 0) strcpy(books_array[index].author, b_temp.author);

        printf("New Genre: ");
        getString(b_temp.genre, MAX_NAME);
        if (strlen(b_temp.genre) > 0) strcpy(books_array[index].genre, b_temp.genre);

        printf("New Year: ");
        getString(b_temp.year, MAX_YR);
        if (strlen(b_temp.year) > 0) strcpy(books_array[index].year, b_temp.year);

        printf("New Shelf No: ");
        getString(b_temp.shelfNo, MAX_ID);
        if (strlen(b_temp.shelfNo) > 0) strcpy(books_array[index].shelfNo, b_temp.shelfNo);

        printf("New Stock Count: ");
        getString(b_temp.stockCount, MAX_ID);
        if (strlen(b_temp.stockCount) > 0) {
            if (stringToInt(b_temp.stockCount) >= 0) {
                strcpy(books_array[index].stockCount, b_temp.stockCount);
            } else {
                printf("Invalid stock count. Must be a non-negative number. Stock not updated.\n");
            }
        }

        printf("\nBook details updated successfully!\n");
    } else {
        printf("\nError: Book ID '%s' not found.\n", bookID);
    }

    printf("\nPress Enter to update another book or ESC to go back to Menu.\n");
    if (getch_portable() == ESC) {
        studentMenu();
    } else {
        updateBook();
    }
}

/**
 * @brief Deletes a book from the library.
 */
void deleteBook() {
    char bookID[MAX_ID];
    int indexToRemove;

    printWelcomeMsg();
    printf("--- Delete Book ---\n\n");
    printf("Enter Book ID to delete: ");
    getString(bookID, MAX_ID);

    indexToRemove = getBookIndexByID(bookID);
    if (indexToRemove >= 0) {
        printf("\nAre you sure you want to delete '%s' (ID: %s)? (Y/N): ",
               books_array[indexToRemove].title, books_array[indexToRemove].bookID);
        char confirm = tolower(getch_portable());

        if (confirm == 'y') {
            for (int i = indexToRemove; i < bookCount - 1; i++) {
                books_array[i] = books_array[i + 1];
            }
            bookCount--;

            Book *temp_books = (Book *)realloc(books_array, bookCount * sizeof(Book));
            if (bookCount > 0 && temp_books == NULL) {
                printf("\nError: Failed to reallocate memory after deletion. Data might be inconsistent.\n");
            } else {
                books_array = temp_books;
                printf("\nBook '%s' (ID: %s) deleted successfully.\n", bookID, bookID);
            }
        } else {
            printf("\nBook deletion cancelled.\n");
        }
    } else {
        printf("\nError: Book ID '%s' not found.\n", bookID);
    }

    printf("\nPress Enter to delete another book or ESC to go back to Menu.\n");
    if (getch_portable() == ESC) {
        studentMenu();
    } else {
        deleteBook();
    }
}

/**
 * @brief Searches for a book by ID and displays its details.
 * @param isStudentView Flag to determine welcome message (student).
 */
void searchBooks(int isStudentView) {
    char bookID[MAX_ID];
    int index;

    if (isStudentView) {
        printStudentWelcomeMsg(currentStudentProfile);
    } else {
        printWelcomeMsg();
    }
    printf("--- Search Book ---\n\n");
    printf("Enter Book ID to search: ");
    getString(bookID, MAX_ID);

    index = getBookIndexByID(bookID);

    if (index >= 0) {
        printf("\nDetails for Book ID: %s\n", bookID);
        printf("-------------------------------------------------\n");
        printf("Title: %s\n", books_array[index].title);
        printf("Author: %s\n", books_array[index].author);
        printf("Genre: %s\n", books_array[index].genre);
        printf("Year: %s\n", books_array[index].year);
        printf("Shelf Number: %s\n", books_array[index].shelfNo);
        printf("Stock Count: %s\n", books_array[index].stockCount);
    } else {
        printf("\nSorry! Book with ID '%s' not found.\n", bookID);
    }

    printf("\nPress Enter to go back.\n");
    getch_portable();
    studentMenu();
}
