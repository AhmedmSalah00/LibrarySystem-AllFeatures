#ifndef ADMIN_ACCESS_H
#define ADMIN_ACCESS_H

#include "utility.h" // For AdminAccount struct and common constants/functions

// --- File Names ---
#define ADMIN_DATA_FILE "adminAccess.dat"
#define ADMIN_TEMP_FILE "tempFile.dat" // For deletion operations

// --- Function Prototypes ---

/**
 * @brief Checks if an admin account exists based on username and password (flag=0)
 * or just username (flag=1).
 * @param p AdminAccount struct containing username and password to check.
 * @param flag 0 for full check, 1 for username only check.
 * @return 1 if found, 0 otherwise.
 */
int checkAdmin(AdminAccount p, int flag);

/**
 * @brief Reads all admin accounts from the file into an array.
 * @param p Array to store admin accounts.
 * @param sz Maximum size of the array.
 * @return Number of accounts read.
 */
int viewAdmins(AdminAccount p[], int sz);

/**
 * @brief Adds a new admin account to the file.
 * @param p AdminAccount struct containing the new admin's details.
 */
void addAdmin(AdminAccount p);

/**
 * @brief Updates an existing admin account's password.
 * @param p AdminAccount struct containing the username and new password.
 * @return 1 if updated, 0 if username not found.
 */
int updateAdmin(AdminAccount p);

/**
 * @brief Deletes an admin account from the file.
 * @param userName The username of the account to delete.
 * @return 1 if deleted, 0 if username not found.
 */
int deleteAdmin(char *userName);

/**
 * @brief Checks if it's the first time an admin is logging in (i.e., no admin accounts exist).
 * @return 1 if no admin accounts, 0 otherwise.
 */
int checkFirstAdminLogin();

/**
 * @brief Guides the user through creating the first admin account.
 */
void createFirstAdminAccount();

/**
 * @brief Handles admin login.
 */
void adminLogin();

#endif // ADMIN_ACCESS_H
