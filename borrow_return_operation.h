#ifndef BORROW_RETURN_OPERATION_H
#define BORROW_RETURN_OPERATION_H

#include "utility.h" // For BorrowedBook, UserProfile, Book structs and common functions

// --- File Names ---
#define BORROWED_BOOKS_DATA_FILE "borrowedBooks.dat"
#define BORROWED_BOOKS_TEMP_FILE "borrowedBooksTemp.dat" // For deletion/update operations

// --- Function Prototypes ---

/**
 * @brief Adds a new borrowed book record to the file.
 * @param record BorrowedBook struct containing the details of the borrowed book.
 */
void addBorrowedBookRecord(BorrowedBook record);

/**
 * @brief Updates an existing borrowed book record (e.g., setting return date).
 * @param record BorrowedBook struct with updated details.
 * @return 1 if updated, 0 if record not found.
 */
int updateBorrowedBookRecord(BorrowedBook record);

/**
 * @brief Finds a specific borrowed book record by Book ID and User ID.
 * @param bookID The ID of the book.
 * @param userID The ID of the user.
 * @param record_out Pointer to a BorrowedBook struct to store the found data.
 * @return 1 if found, 0 otherwise.
 */
int findBorrowedBookRecord(const char *bookID, const char *userID, BorrowedBook *record_out);

/**
 * @brief Reads all borrowed book records from the file into an array.
 * @param records_array Array to store borrowed book records.
 * @param max_sz Maximum size of the array.
 * @return Number of records read.
 */
int viewAllBorrowedBooks(BorrowedBook records_array[], int max_sz);

/**
 * @brief Reads borrowed book records specific to a given User ID.
 * @param records_array Array to store borrowed book records.
 * @param userID The User ID to filter by.
 * @param max_sz Maximum size of the array.
 * @return Number of records read for the specific user.
 */
int viewUserBorrowedBooks(BorrowedBook records_array[], const char *userID, int max_sz);

/**
 * @brief Handles the process of a user borrowing a book.
 * Decreases book stock, creates a borrowed record, and applies public library rules.
 * @param libraryType PUBLIC_LIBRARY or SCHOOL_LIBRARY.
 */
void borrowBookProcess(int libraryType);

/**
 * @brief Handles the process of a user returning a book.
 * Increases book stock, updates borrowed record, calculates fines/forbidden status for public library.
 * @param libraryType PUBLIC_LIBRARY or SCHOOL_LIBRARY.
 */
void returnBookProcess(int libraryType);

/**
 * @brief Displays all currently borrowed books (those without a return date).
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void viewCurrentlyBorrowedBooks(int isUserView);

#endif // BORROW_RETURN_OPERATION_H
