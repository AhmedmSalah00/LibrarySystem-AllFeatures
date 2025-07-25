#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H

#include "utility.h" // For UserProfile struct and common constants/functions

// --- File Names ---
#define USER_DATA_FILE "UserData.dat"
#define USER_TEMP_FILE "UserTemp.dat" // For deletion operations

// --- Function Prototypes ---

/**
 * @brief Adds a new user profile to the file.
 * @param user UserProfile struct containing the new user's details.
 */
void addUser(UserProfile user);

/**
 * @brief Finds a user profile by ID.
 * @param userID The ID of the user to find.
 * @param user_out Pointer to a UserProfile struct to store the found data.
 * @return 1 if found, 0 otherwise.
 */
int findUserByID(const char *userID, UserProfile *user_out); // Corrected: Removed space

/**
 * @brief Updates an existing user profile in the file.
 * @param user UserProfile struct containing the updated user's details.
 * @return 1 if updated, 0 if user not found.
 */
int updateUser(UserProfile user);

/**
 * @brief Deletes a user profile from the file.
 * @param userID The ID of the user to delete.
 * @return 1 if deleted, 0 if user not found.
 */
int deleteUser(const char *userID);

/**
 * @brief Reads all user profiles from the file into an array.
 * @param users_array Array to store user profiles.
 * @param max_sz Maximum size of the array.
 * @return Number of users read.
 */
int viewAllUsers(UserProfile users_array[], int max_sz);

/**
 * @brief Generates a unique User ID based on the first name.
 * Finds the highest existing ID and increments it.
 * @param firstName The first name of the user.
 * @param newUserIDBuffer Buffer to store the generated ID.
 * @param bufferSize Size of the buffer.
 */
void generateUniqueUserID(const char *firstName, char *newUserIDBuffer, size_t bufferSize); // Corrected prototype arguments

#endif // USER_MANAGEMENT_H
