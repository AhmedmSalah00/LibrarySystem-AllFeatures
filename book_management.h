#ifndef BOOK_MANAGEMENT_H
#define BOOK_MANAGEMENT_H

#include "utility.h" // For Book struct and common constants/functions
#include "loadCsvData.h" // For readBooksFromCSV, writeBooksToCSV

// --- Function Prototypes ---

/**
 * @brief Gets the index of a book in the global `books_array` by its ID.
 * @param bookID The ID of the book to search for.
 * @return The index of the book if found, -1 otherwise.
 */
int getBookIndexByID(const char *bookID);

/**
 * @brief Adds a new book to the library's book catalog.
 * Prompts the user for book details and adds it to the in-memory array.
 * The changes will be saved to CSV when the application exits.
 */
void addBookToCatalog();

/**
 * @brief Updates details of an existing book in the library's catalog.
 * Prompts the user for the book ID and then allows updating various fields.
 */
void updateBookInCatalog();

/**
 * @brief Deletes a book from the library's catalog.
 * Prompts the user for the book ID and confirms deletion.
 */
void deleteBookFromCatalog();

/**
 * @brief Displays books by page from the global `books_array`.
 * This function handles the visual formatting of the book table.
 * @param sIndex Starting index for display.
 * @param lIndex Ending index for display.
 * @param total Total number of books.
 * @param isUserView Flag to determine if the display is for a logged-in user (influences header).
 */
void displayBooksByPage(int sIndex, int lIndex, int total, int isUserView);

/**
 * @brief Allows viewing all books in the catalog with pagination.
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void viewAllBooksInCatalog(int isUserView);

/**
 * @brief Searches for a book by title, author, or publication year.
 * Displays matching books.
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void searchBooksInCatalog(int isUserView);

/**
 * @brief Counts and displays the total number of books in the catalog.
 */
void countTotalBooksInCatalog();

#endif // BOOK_MANAGEMENT_H
