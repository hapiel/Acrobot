#include "utilsAcrobot.h"

// Function definition
int splitString(const String &str, char delimiter, String *arr, int maxItems)
{
  int itemCount = 0;  // Number of items found
  int startIndex = 0; // Start index of the current substring

  // Loop through each character in the string
  for (int i = 0; i < str.length(); i++)
  {
    // Check if current character is the delimiter
    if (str.charAt(i) == delimiter)
    {
      // Extract substring from startIndex to current index
      arr[itemCount++] = str.substring(startIndex, i);
      startIndex = i + 1; // Update startIndex for the next substring
    }

    // Check if maximum items reached
    if (itemCount == maxItems - 1)
    {
      // Extract the rest of the string and exit loop
      arr[itemCount++] = str.substring(startIndex);
      break;
    }
  }

  // If there are characters left after the last delimiter, extract them
  if (startIndex < str.length())
  {
    arr[itemCount++] = str.substring(startIndex);
  }

  // Return the number of items found
  return itemCount;
}