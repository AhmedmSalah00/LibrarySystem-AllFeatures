#include "library_menus.h" // Include its own header
#include "utility.h"         // For common constants, structs, and utility functions
#include "admin_access.h"    // For admin login/management functions
#include "user_management.h" // For user management functions
#include "book_management.h" // For book catalog management functions
#include "borrow_return_operation.h" // For borrow/return functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Forward Declarations for Menu Functions ---
// These are declared here to ensure all functions are known before their first use,
// preventing "implicit declaration" warnings and "conflicting types" errors.
// These functions are DEFINED in this file.
void displayInitialLoginMenu();
void adminLibrarySelectionMenu();
void userLibrarySelectionMenu();
void publicLibraryAdminMenu();
void publicLibraryUserMenu();
void schoolLibraryAdminMenu();
void schoolLibraryUserMenu();

// Helper functions for common admin/user actions
// These functions are DEFINED in this file.
void manageAdminAccounts();
void manageLibraryUsers(int libraryType);
void manageBooks();
void handleBorrowedBooks(int libraryType);
void performBookSearch(int isUserView);


// --- Helper Functions for Menu Actions (Defined in this file) ---

/**
 * @brief Manages admin accounts (Add, Update, Delete, View).
 */
void manageAdminAccounts() {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- Admin Account Management ---\n\n");
        printf("V) View All Admin Accounts\n");
        printf("A) Add New Admin Account\n");
        printf("U) Update Admin Account Password\n");
        printf("D) Delete Admin Account\n");
        printf("ESC) Go Back to Admin Menu\n\n");
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'v': {
                AdminAccount admins[READ_ARRAY_SZ];
                int count = viewAdmins(admins, READ_ARRAY_SZ); // Call from admin_access.h
                printWelcomeMsg();
                printf("--- All Admin Accounts (Total: %d) ---\n\n", count);
                if (count == 0) {
                    printf("No admin accounts found.\n");
                } else {
                    printf("Usernames:\n");
                    for (int i = 0; i < count; i++) {
                        printf("%d. %s\n", i + 1, admins[i].userName);
                    }
                }
                printf("\nPress Enter to continue...\n");
                getch_portable();
                break;
            }
            case 'a': {
                AdminAccount newAdmin;
                char passV[MAX_PWD_SZ];
                printWelcomeMsg();
                printf("--- Add New Admin Account ---\n\n");
                printf("New User Name: ");
                getString(newAdmin.userName, MAX_USR_SZ);
                if (checkAdmin(newAdmin, 1) == 1) { // Call from admin_access.h
                    printf("Error: Username '%s' already exists.\n", newAdmin.userName);
                    printf("Press Enter to continue...\n");
                    getch_portable();
                    break;
                }
                printf("New Password: ");
                getPassword(newAdmin.password, MAX_PWD_SZ);
                printf("Re-type Password: ");
                getPassword(passV, MAX_PWD_SZ);
                if (strcmp(passV, newAdmin.password) == 0) {
                    addAdmin(newAdmin); // Call from admin_access.h
                    printf("\nAdmin account '%s' created successfully!\n", newAdmin.userName);
                } else {
                    printf("\nError: Passwords do not match.\n");
                }
                printf("Press Enter to continue...\n");
                getch_portable();
                break;
            }
            case 'u': {
                AdminAccount adminToUpdate;
                printWelcomeMsg();
                printf("--- Update Admin Account Password ---\n\n");
                printf("Enter Username to update: ");
                getString(adminToUpdate.userName, MAX_USR_SZ);
                if (checkAdmin(adminToUpdate, 1) == 0) { // Call from admin_access.h
                    printf("Error: Username '%s' not found.\n", adminToUpdate.userName);
                } else {
                    printf("Enter New Password: ");
                    getPassword(adminToUpdate.password, MAX_PWD_SZ);
                    if (updateAdmin(adminToUpdate) == 1) { // Call from admin_access.h
                        printf("\nPassword for '%s' updated successfully!\n", adminToUpdate.userName);
                    } else {
                        printf("\nError: Failed to update password.\n");
                    }
                }
                printf("Press Enter to continue...\n");
                getch_portable();
                break;
            }
            case 'd': {
                char userNameToDelete[MAX_USR_SZ];
                printWelcomeMsg();
                printf("--- Delete Admin Account ---\n\n");
                printf("Enter Username to delete: ");
                getString(userNameToDelete, MAX_USR_SZ);
                if (deleteAdmin(userNameToDelete) == 1) { // Call from admin_access.h
                    printf("\nAdmin account '%s' deleted successfully.\n", userNameToDelete);
                } else {
                    printf("\nError: Username '%s' not found.\n", userNameToDelete);
                }
                printf("Press Enter to continue...\n");
                getch_portable();
                break;
            }
            case ESC:
                return; // Go back to Admin Menu
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Manages library users (Add, Remove, View, Update fine/forbidden status for Public Library).
 * @param libraryType PUBLIC_LIBRARY or SCHOOL_LIBRARY.
 */
void manageLibraryUsers(int libraryType) {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- User Management ---\n\n");
        printf("V) View All Users\n");
        printf("A) Add New User\n");
        printf("D) Delete User\n");
        if (libraryType == PUBLIC_LIBRARY) {
            printf("C) Clear User Fine\n");
            printf("R) Remove User Forbidden Status\n");
        }
        printf("ESC) Go Back to Admin Menu\n\n");
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'v': {
                UserProfile users[READ_ARRAY_SZ * 5]; // Allocate more for viewing all
                int count = viewAllUsers(users, READ_ARRAY_SZ * 5); // Call from user_management.h
                printWelcomeMsg();
                printf("--- All User Accounts (Total: %d) ---\n\n", count);
                if (count == 0) {
                    printf("No user accounts found.\n");
                } else {
                    printf("+----------+--------------------+--------------------+--------------------+----------+--------------------+--------------------+\n");
                    printf("| User ID  | First Name         | Last Name          | Faculty/Type       | Fine     | Forbidden Until    | Password           |\n");
                    printf("+----------+--------------------+--------------------+--------------------+----------+--------------------+--------------------+\n");
                    for (int i = 0; i < count; i++) {
                        printf("| %-8s | %-18.18s | %-18.18s | %-18.18s | %-8d | %-18.18s | %-18.18s |\n",
                               users[i].userID, users[i].fName, users[i].lName, users[i].faculty,
                               users[i].fineAmount, users[i].forbiddenUntil, users[i].password);
                    }
                    printf("+----------+--------------------+--------------------+--------------------+----------+--------------------+--------------------+\n");
                }
                printf("\nPress Enter to continue...\n");
                getch_portable();
                break;
            }
            case 'a': {
                UserProfile newUser;
                char passV[MAX_PWD_SZ]; // For password verification
                printWelcomeMsg();
                printf("--- Add New User ---\n\n");

                // Prompt for First Name and Last Name first, as User ID generation depends on it
                printf("First Name: ");
                getString(newUser.fName, MAX_NAME);
                printf("Last Name: ");
                getString(newUser.lName, MAX_NAME);

                // Generate User ID based on first name
                generateUniqueUserID(newUser.fName, newUser.userID, MAX_ID); // Call from user_management.h
                printf("Generated User ID: %s\n", newUser.userID); // Show generated ID

                // Set faculty based on library type or prompt for school library
                if (libraryType == PUBLIC_LIBRARY) {
                    strcpy(newUser.faculty, "Public"); // Default for public library users
                } else { // SCHOOL_LIBRARY
                    printf("Faculty: ");
                    getString(newUser.faculty, MAX_NAME);
                }

                // Prompt for password
                printf("Password: ");
                getPassword(newUser.password, MAX_PWD_SZ);
                printf("Re-type Password: ");
                getPassword(passV, MAX_PWD_SZ);

                if (strcmp(passV, newUser.password) != 0) {
                    printf("\nError: Passwords do not match. User not added.\n");
                    printf("Press Enter to continue...\n");
                    getch_portable();
                    break;
                }

                newUser.fineAmount = 0;
                strcpy(newUser.forbiddenUntil, "N/A"); // Not forbidden initially

                addUser(newUser); // Call from user_management.h
                printf("\nUser '%s %s' (ID: %s) added successfully!\n", newUser.fName, newUser.lName, newUser.userID);
                printf("Press Enter to continue...\n");
                getch_portable();
                break;
            }
            case 'd': {
                char userIDToDelete[MAX_ID];
                printWelcomeMsg();
                printf("--- Delete User ---\n\n");
                printf("Enter User ID to delete: ");
                getString(userIDToDelete, MAX_ID);
                if (deleteUser(userIDToDelete) == 1) { // Call from user_management.h
                    printf("\nUser '%s' deleted successfully.\n", userIDToDelete);
                } else {
                    printf("\nError: User ID '%s' not found.\n", userIDToDelete);
                }
                printf("Press Enter to continue...\n");
                getch_portable();
                break;
            }
            case 'c': // Clear fine (Public Library only)
                if (libraryType == PUBLIC_LIBRARY) {
                    char userIDToClear[MAX_ID];
                    UserProfile userToUpdate;
                    printWelcomeMsg();
                    printf("--- Clear User Fine ---\n\n");
                    printf("Enter User ID to clear fine: ");
                    getString(userIDToClear, MAX_ID);
                    if (findUserByID(userIDToClear, &userToUpdate) == 1) { // Call from user_management.h
                        if (userToUpdate.fineAmount > 0) {
                            printf("Clearing fine of %d for user '%s %s'.\n", userToUpdate.fineAmount, userToUpdate.fName, userToUpdate.lName);
                            userToUpdate.fineAmount = 0;
                            updateUser(userToUpdate); // Call from user_management.h
                            printf("\nFine cleared successfully!\n");
                        } else {
                            printf("\nUser '%s' has no outstanding fine.\n", userIDToClear);
                        }
                    } else {
                        printf("\nError: User ID '%s' not found.\n", userIDToClear);
                    }
                    printf("Press Enter to continue...\n");
                    getch_portable();
                } else {
                    printf("\nInvalid option for School Library. Press Enter to continue...\n");
                    getch_portable();
                }
                break;
            case 'r': // Remove forbidden status (Public Library only)
                if (libraryType == PUBLIC_LIBRARY) {
                    char userIDToUnforbid[MAX_ID];
                    UserProfile userToUpdate;
                    printWelcomeMsg();
                    printf("--- Remove User Forbidden Status ---\n\n");
                    printf("Enter User ID to remove forbidden status: ");
                    getString(userIDToUnforbid, MAX_ID);
                    if (findUserByID(userIDToUnforbid, &userToUpdate) == 1) { // Call from user_management.h
                        if (strcmp(userToUpdate.forbiddenUntil, "N/A") != 0) {
                            printf("Removing forbidden status for user '%s %s' (was until %s).\n", userToUpdate.fName, userToUpdate.lName, userToUpdate.forbiddenUntil);
                            strcpy(userToUpdate.forbiddenUntil, "N/A");
                            updateUser(userToUpdate); // Call from user_management.h
                            printf("\nForbidden status removed successfully!\n");
                        } else {
                            printf("\nUser '%s' is not currently forbidden.\n", userIDToUnforbid);
                        }
                    } else {
                        printf("\nError: User ID '%s' not found.\n", userIDToUnforbid);
                    }
                    printf("Press Enter to continue...\n");
                    getch_portable();
                } else {
                    printf("\nInvalid option for School Library. Press Enter to continue...\n");
                    getch_portable();
                }
                break;
            case ESC:
                return; // Go back to Admin Menu
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Manages books (Add, Remove, View, Update, Count).
 */
void manageBooks() {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- Book Management ---\n\n");
        printf("V) View All Books\n");
        printf("A) Add New Book\n");
        printf("U) Update Book Details\n");
        printf("D) Delete Book\n");
        printf("C) Count Total Books\n");
        printf("ESC) Go Back to Admin Menu\n\n");
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'v': viewAllBooksInCatalog(FALSE); break; // Call from book_management.h
            case 'a': addBookToCatalog(); break; // Call from book_management.h
            case 'u': updateBookInCatalog(); break; // Call from book_management.h
            case 'd': deleteBookFromCatalog(); break; // Call from book_management.h
            case 'c': countTotalBooksInCatalog(); break; // Call from book_management.h
            case ESC:
                return; // Go back to Admin Menu
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Handles viewing currently borrowed books and managing borrow/return processes.
 * @param libraryType PUBLIC_LIBRARY or SCHOOL_LIBRARY.
 */
void handleBorrowedBooks(int libraryType) {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- Borrowed Books Management ---\n\n");
        printf("V) View Currently Borrowed Books\n");
        printf("ESC) Go Back to Admin Menu\n\n"); // Borrow/Return are user actions
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'v': viewCurrentlyBorrowedBooks(FALSE); break; // Call from borrow_return_operation.h
            case ESC:
                return; // Go back to Admin Menu
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Performs book search by title, author, or year.
 * @param isUserView Flag to determine if the display is for a logged-in user.
 */
void performBookSearch(int isUserView) {
    searchBooksInCatalog(isUserView); // Calls the function from book_management.h
}


// --- Main Library Selection Menu (Defined in this file) ---

/**
 * @brief Displays the initial menu for selecting login type (Admin or User).
 */
void displayInitialLoginMenu() {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- Welcome to Library Management System ---\n\n");
        printf("1) Login as Admin\n");
        printf("2) Login as User\n");
        printf("ESC) Exit Application\n\n");
        printf("Enter your choice: ");

        choice = getch_portable();
        if (isdigit(choice)) choice = choice - '0';

        switch (choice) {
            case 1:
                // Admin login must happen first
                if (checkFirstAdminLogin() == 1) { // Call from admin_access.h
                    createFirstAdminAccount(); // Call from admin_access.h
                } else {
                    adminLogin(); // Call from admin_access.h - This function will loop until successful login or exit
                }
                adminLibrarySelectionMenu(); // Defined in this file
                break;
            case 2:
                userLibrarySelectionMenu(); // Defined in this file
                break;
            case ESC:
                writeBooksToCSV(); // Call from loadCsvData.h (saves all current book data)
                printf("\nExiting Library Management System. Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid option. Please enter '1', '2', or ESC.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Displays the menu for selecting library type after Admin login.
 */
void adminLibrarySelectionMenu() {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- Admin: Select Library Type ---\n\n");
        printf("P) Public Library Admin\n");
        printf("S) School Library Admin\n");
        printf("ESC) Logout Admin\n\n");
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'p': publicLibraryAdminMenu(); break; // Defined in this file
            case 's': schoolLibraryAdminMenu(); break; // Defined in this file
            case ESC:
                printf("\nLogging out from Admin Mode...\n");
                printf("Press Enter to continue...\n");
                getch_portable();
                return; // Go back to initial login menu
            default:
                printf("\nInvalid option. Please enter 'P', 'S', or ESC.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Displays the menu for selecting library type after User login.
 */
void userLibrarySelectionMenu() {
    char choice;
    do {
        printWelcomeMsg();
        printf("--- User: Select Library Type ---\n\n");
        printf("P) Public Library User\n");
        printf("S) School Library User\n");
        printf("ESC) Go Back to Main Menu\n\n");
        printf("Enter your choice: ");

        choice = tolower(getch_portable());

        switch (choice) {
            case 'p': publicLibraryUserMenu(); break; // Defined in this file
            case 's': schoolLibraryUserMenu(); break; // Defined in this file
            case ESC:
                printf("\nReturning to main menu...\n");
                printf("Press Enter to continue...\n");
                getch_portable();
                return; // Go back to initial login menu
            default:
                printf("\nInvalid option. Please enter 'P', 'S', or ESC.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}


/**
 * @brief Displays the Admin menu for the Public Library mode.
 */
void publicLibraryAdminMenu() {
    char choice;
    // Admin login already handled by displayInitialLoginMenu()
    do {
        printWelcomeMsg();
        printf("--- Public Library Admin Menu ---\n\n");
        printf("1) Manage Admin Accounts\n");
        printf("2) Manage Library Users\n");
        printf("3) Manage Books (Add/Remove/View/Update/Count)\n");
        printf("4) View Borrowed Books\n");
        printf("5) Search Books\n");
        printf("ESC) Go Back to Library Selection\n\n");
        printf("Enter your choice: ");

        choice = getch_portable(); // Read as char, no tolower for numbers
        if (isdigit(choice)) choice = choice - '0'; // Convert char digit to int digit

        switch (choice) {
            case 1: manageAdminAccounts(); break; // Defined in this file
            case 2: manageLibraryUsers(PUBLIC_LIBRARY); break; // Defined in this file
            case 3: manageBooks(); break; // Defined in this file
            case 4: handleBorrowedBooks(PUBLIC_LIBRARY); break; // Defined in this file
            case 5: performBookSearch(FALSE); break; // Defined in this file
            case ESC:
                // Logout and return to library selection
                printf("\nReturning to Admin Library Selection...\n");
                printf("Press Enter to continue...\n");
                getch_portable();
                return;
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Displays the User menu for the Public Library mode.
 */
void publicLibraryUserMenu() {
    char userID[MAX_ID];
    char password[MAX_PWD_SZ]; // For user password input
    UserProfile userFound;

    // Loop for user login attempts
    while (1) {
        printWelcomeMsg();
        printf("--- Public Library User Login ---\n\n");
        printf("Enter your User ID: ");
        getString(userID, MAX_ID);
        printf("Enter your Password: ");
        getPassword(password, MAX_PWD_SZ);

        if (findUserByID(userID, &userFound) == 1) { // Call from user_management.h
            if (strcmp(password, userFound.password) == 0) { // Password verification
                currentLoggedInUser = userFound; // Set global current user
                char choice;
                do {
                    printUserWelcomeMsg(currentLoggedInUser); // Call from utility.h
                    printf("--- Public Library User Menu ---\n\n");
                    printf("1) View Available Books\n");
                    printf("2) Borrow a Book\n");
                    printf("3) Return a Book\n");
                    printf("4) Search Books\n");
                    printf("5) View My Borrowed Books\n");
                    printf("ESC) Logout\n\n");
                    printf("Enter your choice: ");

                    choice = getch_portable();
                    if (isdigit(choice)) choice = choice - '0';

                    switch (choice) {
                        case 1: viewAllBooksInCatalog(TRUE); break; // Call from book_management.h
                        case 2: borrowBookProcess(PUBLIC_LIBRARY); break; // Call from borrow_return_operation.h
                        case 3: returnBookProcess(PUBLIC_LIBRARY); break; // Call from borrow_return_operation.h
                        case 4: performBookSearch(TRUE); break; // Defined in this file
                        case 5: {
                            BorrowedBook userRecords[READ_ARRAY_SZ * 5];
                            int count = viewUserBorrowedBooks(userRecords, currentLoggedInUser.userID, READ_ARRAY_SZ * 5); // Call from borrow_return_operation.h
                            printUserWelcomeMsg(currentLoggedInUser); // Call from utility.h
                            printf("--- Your Borrowed Books (Total: %d) ---\n\n", count);
                            if (count == 0) {
                                printf("You have no borrowed books.\n");
                            } else {
                                printf("+----------+--------------------------------------------------+------------+------------+------------+\n");
                                printf("| Book ID  | Title                                            | Issue Date | Due Date   | Return Date|\n");
                                printf("+----------+--------------------------------------------------+------------+------------+------------+\n");
                                for (int i = 0; i < count; i++) {
                                    char bookTitle[MAX_TITLE] = "Unknown Title";
                                    int bookIdx = getBookIndexByID(userRecords[i].bookID); // Call from book_management.h
                                    if (bookIdx != -1) {
                                        strcpy(bookTitle, books_array[bookIdx].title);
                                    }
                                    printf("| %-8s | %-48.48s | %-10s | %-10s | %-10s |\n",
                                           userRecords[i].bookID, bookTitle, userRecords[i].issueDate,
                                           userRecords[i].dueDate, userRecords[i].returnDate);
                                }
                                printf("+----------+--------------------------------------------------+------------+------------+------------+\n");
                            }
                            printf("\nPress Enter to continue...\n");
                            getch_portable();
                            break;
                        }
                        case ESC:
                            // Logout and return to library selection
                            printf("\nLogging out from User Mode...\n");
                            printf("Press Enter to continue...\n");
                            getch_portable();
                            return; // Return to userLibrarySelectionMenu
                        default:
                            printf("\nInvalid option. Please try again.\n");
                            printf("Press Enter to acknowledge...\n");
                            getch_portable();
                    }
                } while (choice != ESC);
            } else {
                printf("Sorry! User ID or Password do not match.\n\n"); // Specific error message
                printf("Press Enter to re-login.\n");
                getch_portable();
                // Loop continues for retry
            }
        } else {
            printf("\nUser ID '%s' not found.\n", userID); // Specific error message
            printf("Press Enter to re-login.\n");
            getch_portable();
            // Loop continues for retry
        }
    }
}

/**
 * @brief Displays the Admin menu for the School Library mode.
 */
void schoolLibraryAdminMenu() {
    char choice;
    // Admin login already handled by displayInitialLoginMenu()
    do {
        printWelcomeMsg();
        printf("--- School Library Admin Menu ---\n\n");
        printf("1) Manage Admin Accounts\n");
        printf("2) Manage Library Users (Students)\n");
        printf("3) Manage Books (Add/Remove/View/Update/Count)\n");
        printf("4) View Borrowed Books\n");
        printf("5) Search Books\n");
        printf("ESC) Go Back to Library Selection\n\n");
        printf("Enter your choice: ");

        choice = getch_portable();
        if (isdigit(choice)) choice = choice - '0';

        switch (choice) {
            case 1: manageAdminAccounts(); break; // Defined in this file
            case 2: manageLibraryUsers(SCHOOL_LIBRARY); break; // Defined in this file
            case 3: manageBooks(); break; // Defined in this file
            case 4: handleBorrowedBooks(SCHOOL_LIBRARY); break; // Defined in this file
            case 5: performBookSearch(FALSE); break; // Defined in this file
            case ESC:
                // Logout and return to library selection
                printf("\nReturning to Admin Library Selection...\n");
                printf("Press Enter to continue...\n");
                getch_portable();
                return;
            default:
                printf("\nInvalid option. Please try again.\n");
                printf("Press Enter to acknowledge...\n");
                getch_portable();
        }
    } while (choice != ESC);
}

/**
 * @brief Displays the User menu for the School Library mode.
 */
void schoolLibraryUserMenu() {
    char userID[MAX_ID];
    char password[MAX_PWD_SZ]; // For user password input
    UserProfile userFound;

    // Loop for user login attempts
    while (1) {
        printWelcomeMsg();
        printf("--- School Library User Login ---\n\n");
        printf("Enter your User ID: ");
        getString(userID, MAX_ID);
        printf("Enter your Password: ");
        getPassword(password, MAX_PWD_SZ);

        if (findUserByID(userID, &userFound) == 1) { // Call from user_management.h
            if (strcmp(password, userFound.password) == 0) { // Password verification
                currentLoggedInUser = userFound; // Set global current user
                char choice;
                do {
                    printUserWelcomeMsg(currentLoggedInUser); // Call from utility.h
                    printf("--- School Library User Menu ---\n\n");
                    printf("1) View Available Books\n");
                    printf("2) Borrow a Book\n");
                    printf("3) Return a Book\n");
                    printf("4) Search Books\n");
                    printf("5) View My Borrowed Books\n");
                    printf("ESC) Logout\n\n");
                    printf("Enter your choice: ");

                    choice = getch_portable();
                    if (isdigit(choice)) choice = choice - '0';

                    switch (choice) {
                        case 1: viewAllBooksInCatalog(TRUE); break; // Call from book_management.h
                        case 2: borrowBookProcess(SCHOOL_LIBRARY); break; // Call from borrow_return_operation.h
                        case 3: returnBookProcess(SCHOOL_LIBRARY); break; // Call from borrow_return_operation.h
                        case 4: performBookSearch(TRUE); break; // Defined in this file
                        case 5: {
                            BorrowedBook userRecords[READ_ARRAY_SZ * 5];
                            int count = viewUserBorrowedBooks(userRecords, currentLoggedInUser.userID, READ_ARRAY_SZ * 5); // Call from borrow_return_operation.h
                            printUserWelcomeMsg(currentLoggedInUser); // Call from utility.h
                            printf("--- Your Borrowed Books (Total: %d) ---\n\n", count);
                            if (count == 0) {
                                printf("You have no borrowed books.\n");
                            } else {
                                printf("+----------+--------------------------------------------------+------------+------------+------------+\n");
                                printf("| Book ID  | Title                                            | Issue Date | Due Date   | Return Date|\n");
                                printf("+----------+--------------------------------------------------+------------+------------+------------+\n");
                                for (int i = 0; i < count; i++) {
                                    char bookTitle[MAX_TITLE] = "Unknown Title";
                                    int bookIdx = getBookIndexByID(userRecords[i].bookID); // Call from book_management.h
                                    if (bookIdx != -1) {
                                        strcpy(bookTitle, books_array[bookIdx].title);
                                    }
                                    printf("| %-8s | %-48.48s | %-10s | %-10s | %-10s |\n",
                                           userRecords[i].bookID, bookTitle, userRecords[i].issueDate,
                                           userRecords[i].dueDate, userRecords[i].returnDate);
                                }
                                printf("+----------+--------------------------------------------------+------------+------------+------------+\n");
                            }
                            printf("\nPress Enter to continue...\n");
                            getch_portable();
                            break;
                        }
                        case ESC:
                            // Logout and return to library selection
                            printf("\nLogging out from User Mode...\n");
                            printf("Press Enter to continue...\n");
                            getch_portable();
                            return; // Return to userLibrarySelectionMenu
                        default:
                            printf("\nInvalid option. Please try again.\n");
                            printf("Press Enter to acknowledge...\n");
                            getch_portable();
                    }
                } while (choice != ESC);
            } else {
                printf("Sorry! User ID or Password do not match.\n\n"); // Specific error message
                printf("Press Enter to re-login.\n");
                getch_portable();
                // Loop continues for retry
            }
        } else {
            printf("\nUser ID '%s' not found.\n", userID); // Specific error message
            printf("Press Enter to re-login.\n");
            getch_portable();
            // Loop continues for retry
        }
    }
}
