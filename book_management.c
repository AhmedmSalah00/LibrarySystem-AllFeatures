#include "book_management.h" // Include its own header
#include "utility.h"         // For Book, getString, printWelcomeMsg, printUserWelcomeMsg, etc.
#include "loadCsvData.h"     // For readBooksFromCSV, writeBooksToCSV
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For tolower

/**
 * @brief Gets the index of a book in the global `books_array` by its ID.
 * @param bookID The ID of the book to search for.
 * @return The index of the book if found, -1 otherwise.
 */
int getBookIndexByID(const char *bookID) {
    for (int i = 0; i < bookCount; i++) {
        if (strcmp(bookID, books_array[i].bookID) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Adds a new book to the library's book catalog.
 * Prompts the user for book details and adds it to the in-memory array.
 * The changes will be saved to CSV when the application exits.
 */
void addBookToCatalog() {
    Book newBook;
    printWelcomeMsg(); // Use general welcome message for admin-like actions

    printf("--- Add New Book ---\n\n");

    // Automatically generate Book ID
    int newBookID = bookCount + 1;
    snprintf(newBook.bookID, MAX_ID, "BK%03d", newBookID); // Format as BK001, BK002, etc.
    printf("Generated Book ID: %s\n", newBook.bookID); // Show generated ID

    printf("Enter Title: ");
    getString(newBook.title, MAX_TITLE);
    printf("Enter Author: ");
    getString(newBook.author, MAX_NAME);
    printf("Enter Genre: ");
    getString(newBook.genre, MAX_NAME);
    printf("Enter Year of Publication: ");
    getString(newBook.year, MAX_YR);
    printf("Enter Shelf Number: ");
    getString(newBook.shelfNo, MAX_ID);
    printf("Enter Stock Count: ");
    getString(newBook.stockCount, MAX_ID);

    // Basic validation for stock count
    if (stringToInt(newBook.stockCount) < 0) {
        printf("\nError: Stock Count cannot be negative. Book not added.\n");
        printf("Press Enter to continue...\n");
        getch_portable();
        return; // Exit without adding
    }

    // Reallocate memory for the new book
    Book *temp_books = (Book *)realloc(books_array, (bookCount + 1) * sizeof(Book));
    if (temp_books == NULL) {
        printf("\nError: Failed to allocate memory. Book not added.\n");
        printf("Press Enter to continue...\n");
        getch_portable();
        return;
    }
    books_array = temp_books;

    // Add the new book to the end of the array
    books_array[bookCount] = newBook; // Copy the entire struct
    bookCount++;

    // Call writeBooksToCSV immediately after adding a book
    writeBooksToCSV();

    printf("\nBook '%s' (ID: %s) added successfully!\n", newBook.title, newBook.bookID);
    printf("Press Enter to add another book or ESC to go back to Menu.\n");
    if (getch_portable() == ESC) {
        // Return to calling menu (handled by library_menus.c)
    } else {
        addBookToCatalog(); // Loop to add another
    }
}

/**
 * @brief Updates details of an existing book in the library's catalog.
 * Prompts the user for the book ID and then allows updating various fields.
 */
void updateBookInCatalog() {
    char bookID[MAX_ID];
    int index;
    Book temp_book_data; // Use a temporary struct for new values

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
        getString(temp_book_data.title, MAX_TITLE);
        if (strlen(temp_book_data.title) > 0) strcpy(books_array[index].title, temp_book_data.title);

        printf("New Author: ");
        getString(temp_book_data.author, MAX_NAME);
        if (strlen(temp_book_data.author) > 0) strcpy(books_array[index].author, temp_book_data.author);

        printf("New Genre: ");
        getString(temp_book_data.genre, MAX_NAME);
        if (strlen(temp_book_data.genre) > 0) strcpy(books_array[index].genre, temp_book_data.genre);

        printf("New Year: ");
        getString(temp_book_data.year, MAX_YR);
        if (strlen(temp_book_data.year) > 0) strcpy(books_array[index].year, temp_book_data.year);

        printf("New Shelf No: ");
        getString(temp_book_data.shelfNo, MAX_ID);
        if (strlen(temp_book_data.shelfNo) > 0) strcpy(books_array[index].shelfNo, temp_book_data.shelfNo);

        printf("New Stock Count: ");
        getString(temp_book_data.stockCount, MAX_ID);
        if (strlen(temp_book_data.stockCount) > 0) {
            if (stringToInt(temp_book_data.stockCount) >= 0) { // Validate non-negative
                strcpy(books_array[index].stockCount, temp_book_data.stockCount);
            } else {
                printf("Invalid stock count. Must be a non-negative number. Stock not updated.\n");
            }
        }

        printf("\nBook details updated successfully!\n");
        // Call writeBooksToCSV immediately after updating a book
        writeBooksToCSV();
    } else {
        printf("\nError: Book ID '%s' not found.\n", bookID);
    }

    printf("\nPress Enter to update another book or ESC to go back to Menu.\n");
    if (getch_portable() == ESC) {
        // Return to calling menu
    } else {
        updateBookInCatalog(); // Loop to update another
    }
}

/**
 * @brief Deletes a book from the library's catalog.
 * Prompts the user for the book ID and confirms deletion.
 */
void deleteBookFromCatalog() {
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
            // Shift elements to the left to remove the book
            for (int i = indexToRemove; i < bookCount - 1; i++) {
                books_array[i] = books_array[i + 1];
            }
            bookCount--;

            // Reallocate memory to shrink the array
            // If bookCount becomes 0, realloc might return NULL, which is fine as books_array will be NULL
            Book *temp_books = (Book *)realloc(books_array, bookCount * sizeof(Book));
            if (bookCount > 0 && temp_books == NULL) {
                printf("\nError: Failed to reallocate memory after deletion. Data might be inconsistent.\n");
            } else {
                books_array = temp_books;
                printf("\nBook '%s' (ID: %s) deleted successfully.\n", bookID, bookID);
                // Call writeBooksToCSV immediately after deleting a book
                writeBooksToCSV();
            }
        } else {
            printf("\nBook deletion cancelled.\n");
        }
    } else {
        printf("\nError: Book ID '%s' not found.\n", bookID);
    }

    printf("\nPress Enter to delete another book or ESC to go back to Menu.\n");
    if (getch_portable() == ESC) {
        // Return to calling menu
    } else {
        deleteBookFromCatalog(); // Loop to delete another
    }
}

/**
 * @brief Displays books by page from the global `books_array`.
 * This function handles the visual formatting of the book table.
 * @param sIndex Starting index for display.
 * @param lIndex Ending index for display.
 * @param total Total number of books.
 * @param isUserView Flag to determine if the display is for a logged-in user (influences header).
 */
void displayBooksByPage(int sIndex, int lIndex, int total, int isUserView) {
    if (isUserView) {
        // This function is called from library_menus.c, which includes utility.h
        // and has access to currentLoggedInUser.
        printUserWelcomeMsg(currentLoggedInUser);
    } else {
        printWelcomeMsg(); // For admin view or general public view
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
 * @brief Allows viewing all books in the catalog with pagination.
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void viewAllBooksInCatalog(int isUserView) {
    if (bookCount == 0) {
        if (isUserView) printUserWelcomeMsg(currentLoggedInUser); else printWelcomeMsg();
        printf("No books available in the library.\n\n");
        printf("Press Enter to go back.\n");
        getch_portable();
        return; // Return to calling menu
    }

    int pageSize = READ_ARRAY_SZ; // Using READ_ARRAY_SZ as page size
    int lIndex = 0;
    int hIndex = (pageSize > bookCount) ? bookCount : pageSize;
    char choice;

    do {
        displayBooksByPage(lIndex, hIndex, bookCount, isUserView);
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
            break; // Exit loop
        } else {
            printf("\nInvalid input. Press 'B', 'N', or 'ESC'.\n");
            printf("Press Enter to acknowledge...\n");
            getch_portable();
        }
    } while (choice != ESC);

    // Return to calling menu (handled by library_menus.c)
}

/**
 * @brief Searches for a book by title, author, or publication year.
 * Displays matching books.
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void searchBooksInCatalog(int isUserView) {
    char searchTerm[MAX_TITLE]; // Can be title, author, or year
    char searchFieldChoice;
    int foundCount = 0;

    if (isUserView) {
        printUserWelcomeMsg(currentLoggedInUser);
    } else {
        printWelcomeMsg();
    }
    printf("--- Search Books ---\n\n");
    printf("Search by:\n");
    printf("T) Title\n");
    printf("A) Author\n");
    printf("Y) Publication Year\n");
    printf("Enter your choice: ");
    searchFieldChoice = tolower(getch_portable());
    printf("\n"); // Newline after choice

    printf("Enter search term: ");
    getString(searchTerm, MAX_TITLE);

    if (strlen(searchTerm) == 0) {
        printf("\nSearch term cannot be empty.\n");
        printf("Press Enter to continue...\n");
        getch_portable();
        return; // Return to calling menu
    }

    // Convert search term to lowercase for case-insensitive search (except for year)
    char lowerSearchTerm[MAX_TITLE];
    if (searchFieldChoice != 'y') { // Don't convert year to lowercase
        for (int i = 0; searchTerm[i]; i++) {
            lowerSearchTerm[i] = tolower(searchTerm[i]);
        }
        lowerSearchTerm[strlen(searchTerm)] = '\0';
    } else {
        strcpy(lowerSearchTerm, searchTerm); // For year, use as is
    }


    printf("\n--- Search Results for '%s' ---\n\n", searchTerm);
    printf("+----------+--------------------------------------------------+--------------------+---------------+------+---------+-------+\n");
    printf("| Book ID  | Title                                            | Author             | Genre         | Year | ShelfNo | Stock |\n");
    printf("+----------+--------------------------------------------------+--------------------+---------------+------+---------+-------+\n");

    for (int i = 0; i < bookCount; i++) {
        char lowerTitle[MAX_TITLE];
        char lowerAuthor[MAX_NAME];

        // Convert current book's title and author to lowercase for comparison
        for (int j = 0; books_array[i].title[j]; j++) {
            lowerTitle[j] = tolower(books_array[i].title[j]);
        }
        lowerTitle[strlen(books_array[i].title)] = '\0';

        for (int j = 0; books_array[i].author[j]; j++) {
            lowerAuthor[j] = tolower(books_array[i].author[j]);
        }
        lowerAuthor[strlen(books_array[i].author)] = '\0';

        int match = 0;
        switch (searchFieldChoice) {
            case 't': // Search by title
                if (strstr(lowerTitle, lowerSearchTerm) != NULL) match = 1;
                break;
            case 'a': // Search by author
                if (strstr(lowerAuthor, lowerSearchTerm) != NULL) match = 1;
                break;
            case 'y': // Search by year
                if (strcmp(books_array[i].year, searchTerm) == 0) match = 1;
                break;
            default:
                printf("\nInvalid search field choice. Displaying all books.\n");
                match = 1; // Show all if invalid choice
                break;
        }

        if (match) {
            printf("| %-8s | %-48.48s | %-18.18s | %-13.13s | %-4s | %-7s | %-5s |\n",
                   books_array[i].bookID, books_array[i].title, books_array[i].author,
                   books_array[i].genre, books_array[i].year, books_array[i].shelfNo,
                   books_array[i].stockCount);
            foundCount++;
        }
    }
    printf("+----------+--------------------------------------------------+--------------------+---------------+------+---------+-------+\n\n");

    if (foundCount == 0) {
        printf("No books found matching '%s'.\n", searchTerm);
    } else {
        printf("Found %d book(s) matching '%s'.\n", foundCount, searchTerm);
    }

    printf("\nPress Enter to go back.\n");
    getch_portable();
    // Return to calling menu (handled by library_menus.c)
}

/**
 * @brief Counts and displays the total number of books in the catalog.
 */
void countTotalBooksInCatalog() {
    printWelcomeMsg();
    printf("--- Total Books in Library ---\n\n");
    printf("There are %d books currently in the catalog.\n\n", bookCount);
    printf("Press Enter to go back.\n");
    getch_portable();
    // Return to calling menu (handled by library_menus.c)
}
