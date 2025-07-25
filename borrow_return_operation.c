#include "borrow_return_operation.h" // Include its own header
#include "utility.h"                 // For BorrowedBook, UserProfile, Book, date functions, etc.
#include "book_management.h"         // For getBookIndexByID
#include "user_management.h"         // For findUserByID, updateUser
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Adds a new borrowed book record to the file.
 * @param record BorrowedBook struct containing the details of the borrowed book.
 */
void addBorrowedBookRecord(BorrowedBook record) {
    FILE *fp = fopen(BORROWED_BOOKS_DATA_FILE, "ab+"); // Open in append binary mode
    if (fp == NULL) {
        printf("Error: Could not open borrowed books data file '%s' for adding.\n", BORROWED_BOOKS_DATA_FILE);
        return;
    }
    fwrite(&record, sizeof(BorrowedBook), 1, fp);
    fclose(fp);
}

/**
 * @brief Updates an existing borrowed book record (e.g., setting return date).
 * @param record BorrowedBook struct with updated details.
 * @return 1 if updated, 0 if record not found.
 */
int updateBorrowedBookRecord(BorrowedBook record) {
    FILE *fp = fopen(BORROWED_BOOKS_DATA_FILE, "rb+"); // Open in read/write binary mode
    if (fp == NULL) {
        printf("Error: Could not open borrowed books data file '%s' for updating.\n", BORROWED_BOOKS_DATA_FILE);
        return 0;
    }

    BorrowedBook current_record;
    int flag = 0;
    long currentPos;

    while (fread(&current_record, sizeof(BorrowedBook), 1, fp) == 1) {
        // Find the specific record by bookID AND userID, and ensure it's not already returned
        if (strcmp(current_record.bookID, record.bookID) == 0 &&
            strcmp(current_record.userID, record.userID) == 0 &&
            strcmp(current_record.returnDate, "N/A") == 0) { // Only update if not yet returned
            currentPos = ftell(fp);
            fseek(fp, currentPos - sizeof(BorrowedBook), SEEK_SET);
            fwrite(&record, sizeof(BorrowedBook), 1, fp); // Overwrite with updated data
            flag = 1;
            break;
        }
    }
    fclose(fp);
    return flag;
}

/**
 * @brief Finds a specific borrowed book record by Book ID and User ID.
 * @param bookID The ID of the book.
 * @param userID The ID of the user.
 * @param record_out Pointer to a BorrowedBook struct to store the found data.
 * @return 1 if found, 0 otherwise.
 */
int findBorrowedBookRecord(const char *bookID, const char *userID, BorrowedBook *record_out) {
    FILE *fp = fopen(BORROWED_BOOKS_DATA_FILE, "rb");
    if (fp == NULL) return 0;

    BorrowedBook current_record;
    int found = 0;
    while (fread(&current_record, sizeof(BorrowedBook), 1, fp) == 1) {
        if (strcmp(current_record.bookID, bookID) == 0 &&
            strcmp(current_record.userID, userID) == 0 &&
            strcmp(current_record.returnDate, "N/A") == 0) { // Only find active borrowed records
            *record_out = current_record;
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

/**
 * @brief Reads all borrowed book records from the file into an array.
 * @param records_array Array to store borrowed book records.
 * @param max_sz Maximum size of the array.
 * @return Number of records read.
 */
int viewAllBorrowedBooks(BorrowedBook records_array[], int max_sz) {
    FILE *fp = fopen(BORROWED_BOOKS_DATA_FILE, "rb");
    if (fp == NULL) return 0;

    BorrowedBook current_record;
    int i = 0;
    while (fread(&current_record, sizeof(BorrowedBook), 1, fp) == 1 && i < max_sz) {
        records_array[i] = current_record;
        i++;
    }
    fclose(fp);
    return i;
}

/**
 * @brief Reads borrowed book records specific to a given User ID.
 * @param records_array Array to store borrowed book records.
 * @param userID The User ID to filter by.
 * @param max_sz Maximum size of the array.
 * @return Number of records read for the specific user.
 */
int viewUserBorrowedBooks(BorrowedBook records_array[], const char *userID, int max_sz) {
    FILE *fp = fopen(BORROWED_BOOKS_DATA_FILE, "rb");
    if (fp == NULL) return 0;

    BorrowedBook current_record;
    int i = 0;
    while (fread(&current_record, sizeof(BorrowedBook), 1, fp) == 1 && i < max_sz) {
        if (strcmp(current_record.userID, userID) == 0) {
            records_array[i] = current_record;
            i++;
        }
    }
    fclose(fp);
    return i;
}


/**
 * @brief Handles the process of a user borrowing a book.
 * Decreases book stock, creates a borrowed record, and applies public library rules.
 * @param libraryType PUBLIC_LIBRARY or SCHOOL_LIBRARY.
 */
void borrowBookProcess(int libraryType) {
    char bookID[MAX_ID];
    int bookIndex;
    int currentStock;
    char tempStr[MAX_ID];
    char issueDate[DATE_SZ];
    char dueDate[DATE_SZ];
    BorrowedBook newBorrowedRecord;
    UserProfile user = currentLoggedInUser; // Get current user's profile

    if (libraryType == PUBLIC_LIBRARY) {
        // Check if user is forbidden
        if (strcmp(user.forbiddenUntil, "N/A") != 0) {
            if (compareDates("CURRENT_DATE", user.forbiddenUntil) < 0) { // Current date is before forbidden until date
                printUserWelcomeMsg(user);
                printf("You are currently forbidden from borrowing books until %s due to a late return.\n\n", user.forbiddenUntil);
                printf("Press Enter to continue...\n");
                getch_portable();
                return; // Exit borrow process
            } else {
                // Forbidden period has passed, reset forbidden status
                strcpy(user.forbiddenUntil, "N/A");
                updateUser(user); // Update user profile in file
                currentLoggedInUser = user; // Update global variable
            }
        }
        // Check for outstanding fines
        if (user.fineAmount > 0) {
            printUserWelcomeMsg(user);
            printf("You have an outstanding fine of %d. Please clear your fine before borrowing.\n\n", user.fineAmount);
            printf("Press Enter to continue...\n");
            getch_portable();
            return; // Exit borrow process
        }
    }

    printUserWelcomeMsg(user);
    printf("--- Borrow a Book ---\n\n");
    printf("Enter Book ID to borrow: ");
    getString(bookID, MAX_ID);

    bookIndex = getBookIndexByID(bookID);

    if (bookIndex == -1) {
        printf("Error: Book ID '%s' does not exist.\n", bookID);
    } else {
        currentStock = stringToInt(books_array[bookIndex].stockCount);
        if (currentStock <= 0) {
            printf("Sorry! Book '%s' is out of stock.\n", books_array[bookIndex].title);
        } else {
            // Check if user already borrowed this specific book and hasn't returned it
            if (findBorrowedBookRecord(bookID, user.userID, &newBorrowedRecord) == 1) {
                printf("You have already borrowed '%s' (ID: %s) and have not returned it yet.\n",
                       books_array[bookIndex].title, bookID);
                printf("Please return the current copy before borrowing again.\n");
                printf("Press Enter to continue...\n");
                getch_portable();
                return;
            }

            strcpy(newBorrowedRecord.bookID, bookID);
            strcpy(newBorrowedRecord.userID, user.userID);

            getCurrentDate(issueDate, DATE_SZ);
            strcpy(newBorrowedRecord.issueDate, issueDate);

            // Set due date: 7 days for Public Library, 14 days for School Library (example)
            int daysToBorrow = (libraryType == PUBLIC_LIBRARY) ? 7 : 14;
            addDaysToDate(issueDate, daysToBorrow, dueDate, DATE_SZ);
            strcpy(newBorrowedRecord.dueDate, dueDate);

            strcpy(newBorrowedRecord.returnDate, "N/A"); // Not returned yet

            addBorrowedBookRecord(newBorrowedRecord);

            // Decrease stock count
            intToString(tempStr, currentStock - 1);
            strcpy(books_array[bookIndex].stockCount, tempStr);

            printf("\nBook '%s' issued successfully to you (ID: %s)!\n", books_array[bookIndex].title, user.userID);
            printf("Please return by: %s\n", newBorrowedRecord.dueDate);
        }
    }

    printf("\nPress Enter to continue...\n");
    getch_portable();
}

/**
 * @brief Handles the process of a user returning a book.
 * Increases book stock, updates borrowed record, calculates fines/forbidden status for public library.
 * @param libraryType PUBLIC_LIBRARY or SCHOOL_LIBRARY.
 */
void returnBookProcess(int libraryType) {
    char bookID[MAX_ID];
    int bookIndex;
    int currentStock;
    char tempStr[MAX_ID];
    char returnDate[DATE_SZ];
    BorrowedBook returnedRecord;
    UserProfile user = currentLoggedInUser; // Get current user's profile

    printUserWelcomeMsg(user);
    printf("--- Return a Book ---\n\n");
    printf("Enter Book ID to return: ");
    getString(bookID, MAX_ID);

    // Find the active borrowed record for this book and user
    if (findBorrowedBookRecord(bookID, user.userID, &returnedRecord) == 0) {
        printf("Error: You have not borrowed book ID '%s' or it has already been returned.\n", bookID);
        printf("Press Enter to continue...\n");
        getch_portable();
        return;
    }

    bookIndex = getBookIndexByID(bookID);
    if (bookIndex == -1) {
        // This should ideally not happen if findBorrowedBookRecord succeeded
        printf("Internal Error: Book '%s' not found in catalog. Cannot update stock.\n", bookID);
        printf("Press Enter to continue...\n");
        getch_portable();
        return;
    }

    getCurrentDate(returnDate, DATE_SZ);
    strcpy(returnedRecord.returnDate, returnDate); // Set the actual return date

    if (updateBorrowedBookRecord(returnedRecord) == 1) {
        // Increase stock count
        currentStock = stringToInt(books_array[bookIndex].stockCount);
        intToString(tempStr, currentStock + 1);
        strcpy(books_array[bookIndex].stockCount, tempStr);

        printf("\nBook '%s' returned successfully by you (ID: %s)!\n", books_array[bookIndex].title, user.userID);

        // --- Public Library Specific Rules ---
        if (libraryType == PUBLIC_LIBRARY) {
            int dateComparison = compareDates(returnDate, returnedRecord.dueDate);
            if (dateComparison > 0) { // Return date is AFTER due date (late)
                long daysLate = daysBetweenDates(returnedRecord.dueDate, returnDate);
                int fine = daysLate * FINE_PER_DAY;

                printf("Warning: Book returned %ld day(s) late. A fine of %d will be applied.\n", daysLate, fine);
                user.fineAmount += fine; // Add to existing fine
                strcpy(user.forbiddenUntil, "N/A"); // Reset forbidden status on return, it's about new late returns

                // Apply forbidden status for late returns
                char forbiddenDate[DATE_SZ];
                addDaysToDate(returnDate, FORBIDDEN_DAYS, forbiddenDate, DATE_SZ);
                strcpy(user.forbiddenUntil, forbiddenDate);
                printf("You are forbidden from borrowing new books until %s.\n", user.forbiddenUntil);

            } else {
                printf("Book returned on time.\n");
                // If fine was previously outstanding, it needs to be cleared by admin, not automatically here.
                // Forbidden status is only applied on late return.
            }
        }
        updateUser(user); // Save updated user profile (fine/forbidden status)
        currentLoggedInUser = user; // Update global variable
    } else {
        printf("Error: Failed to update borrowed record. Book might not have been borrowed by you.\n");
    }

    printf("\nPress Enter to continue...\n");
    getch_portable();
}

/**
 * @brief Displays all currently borrowed books (those without a return date).
 * This function is typically for admin view.
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void viewCurrentlyBorrowedBooks(int isUserView) {
    BorrowedBook records[READ_ARRAY_SZ * 5]; // Allocate more space for viewing all
    int data_sz = viewAllBorrowedBooks(records, READ_ARRAY_SZ * 5); // Get all records

    if (isUserView) {
        printUserWelcomeMsg(currentLoggedInUser);
    } else {
        printWelcomeMsg();
    }
    printf("--- Currently Borrowed Books ---\n\n");

    printf("+-----------+----------+--------------------------------------------------+------------+------------+------------+\n");
    printf("| User ID   | Book ID  | Title                                            | Issue Date | Due Date   | Return Date|\n");
    printf("+-----------+----------+--------------------------------------------------+------------+------------+------------+\n");

    int foundActive = 0;
    for (int i = 0; i < data_sz; i++) {
        // Only display books that have not been returned yet
        if (strcmp(records[i].returnDate, "N/A") == 0) {
            // Get book title for display (requires looking up in books_array)
            char bookTitle[MAX_TITLE] = "Unknown Title";
            int bookIdx = getBookIndexByID(records[i].bookID);
            if (bookIdx != -1) {
                strcpy(bookTitle, books_array[bookIdx].title);
            }

            printf("| %-9s | %-8s | %-48.48s | %-10s | %-10s | %-10s |\n",
                   records[i].userID, records[i].bookID, bookTitle,
                   records[i].issueDate, records[i].dueDate, records[i].returnDate);
            foundActive++;
        }
    }
    printf("+-----------+----------+--------------------------------------------------+------------+------------+------------+\n\n");

    if (foundActive == 0) {
        printf("No books are currently borrowed.\n");
    } else {
        printf("Total %d books currently borrowed.\n", foundActive);
    }

    printf("\nPress Enter to go back.\n");
    getch_portable();
}
