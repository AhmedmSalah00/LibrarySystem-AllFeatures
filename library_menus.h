#ifndef LIBRARY_MENUS_H
#define LIBRARY_MENUS_H

#include "utility.h" // For common constants (PUBLIC_LIBRARY, SCHOOL_LIBRARY, ESC)

// --- Function Prototypes ---

/**
 * @brief Displays the initial menu for selecting login type (Admin or User).
 */
void displayInitialLoginMenu();

/**
 * @brief Displays the menu for selecting library type after Admin login.
 */
void adminLibrarySelectionMenu();

/**
 * @brief Displays the menu for selecting library type after User login.
 */
void userLibrarySelectionMenu();

/**
 * @brief Displays the Admin menu for the Public Library mode.
 */
void publicLibraryAdminMenu();

/**
 * @brief Displays the User menu for the Public Library mode.
 */
void publicLibraryUserMenu();

/**
 * @brief Displays the Admin menu for the School Library mode.
 */
void schoolLibraryAdminMenu();

/**
 * @brief Displays the User menu for the School Library mode.
 */
void schoolLibraryUserMenu();

// Helper functions for common admin/user actions (declared here for clarity, implemented in .c)
void manageAdminAccounts();
void manageLibraryUsers(int libraryType); // Handles Add/Remove/View Users
void manageBooks(); // Handles Add/Remove/View/Update/Count Books
void handleBorrowedBooks(int libraryType); // Handles View Borrowed Books
void performBookSearch(int isUserView); // Handles Search Books

#endif // LIBRARY_MENUS_H
